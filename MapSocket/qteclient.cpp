#include "qteclient.h"
#include "QTEDefine.h"
#include "qteclientmessage.h"

#define MAX_HEARDBEAT 10

QTEClient::QTEClient(QObject *parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(socketStateChanged(QAbstractSocket::SocketState)) );
    // connected
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()) );
    // disconnected
    connect(this, SIGNAL(disconnected()), this, SLOT(socketDisconnect()) );
    // domain
    connect(this, SIGNAL(hostFound()), this, SLOT(domainHostFound()));
    // error
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketErrorOccured(QAbstractSocket::SocketError)) );

    connect(this, SIGNAL(readyRead()), this, SLOT(readyReadData()));

    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(updateProgress(qint64)));





    setSocketOption(QAbstractSocket::LowDelayOption, 0);
    setSocketOption(QAbstractSocket::KeepAliveOption, 0);
    setReadBufferSize(_TCP_RECVBUFF_SIZE);

    m_heartCount = 0;

    m_PORT = 0;

    timer = new QTimer(this);
    timer->setSingleShot(false);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendHeatBeatMessage()));
}

QTEClient::~QTEClient()
{
}

void QTEClient::setServHostPort(QString ip, quint32 p)
{
    m_host = ip;
    m_PORT = p;
}

void QTEClient::SendConnectMessage()
{
    pline() << isValid() << isOpen();
    if(isValid())
        return;

    connectToHost(QHostAddress(m_host), m_PORT);
}


void QTEClient::SendDisConnectFromHost()
{
    if(!isValid())
        return;


    disconnectFromHost();
    close();
}

void QTEClient::domainHostFound()
{
    pline();
}



void QTEClient::socketStateChanged(QAbstractSocket::SocketState eSocketState)
{
    pline() << eSocketState;
    switch(eSocketState)
    {
    case QAbstractSocket::HostLookupState:
    case QAbstractSocket::ConnectingState:
        break;
    case QAbstractSocket::ConnectedState:
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::UnconnectedState:

        break;
    default:
        break;
    }
}


void QTEClient::socketErrorOccured(QAbstractSocket::SocketError e)
{
    //在错误状态下重新连接其他热点，直到确定连接类型，写入配置文件
    pline() << e;
    switch(e)
    {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        emit signalConnectFail();
        break;
    default:
        break;
    }
}


void QTEClient::socketConnected()
{
    pline() << peerName() << peerAddress().toString() << peerPort();
    //这个步骤，socket重建，资源重新开始
    m_heartCount = 0;    
    //TODO:心跳检测重连会不会引发这条消息？
    //如果连接还未成功开始发送心跳包，
    //QNativeSocketEngine::write() was not called in QAbstractSocket::ConnectedState
    timer->start(30 * 1000);
    emit signalConnectSucc();

}

/**
 * @brief HNClient::socketDisconnect
 * 功能接口
 */
void QTEClient::socketDisconnect()
{
    pline();
    m_heartCount = MAX_HEARDBEAT + 1;
    timer->stop();
}

void QTEClient::updateProgress(qint64 bytes)
{
    //pline() << bytes;
}



void QTEClient::sendHeatBeatMessage()
{
    //断链判断 如果断链 TODO:
    if(m_heartCount > MAX_HEARDBEAT)
    {
#if 1
        //重连策略 30 * 2 s
        static int curp = 0;
        if(curp >= 2)
        {
            curp = 0;
            connectToHost(QHostAddress(m_host), m_PORT);
            return;
        }
        curp++;
#else
        //此处设置重连策略 30s 150s 300s 600s
        static int p[4] = {1, 5, 10, 20};
        static int curp = 0;
        static int curpos = 0;
        if(curp >= p[curpos])
        {
            curp = 0;
            curpos = (curpos + 1) % 4;
            connectToHost(QHostAddress(m_host), m_PORT);
            return;
        }
        curp++;
#endif
        return;
    }
    pline() << "HeartBeat Count:" << m_heartCount;
    m_heartCount++;
#if 1
    QTEHeartBeat t;
    t.m_content = "this is a heartbeat";
    quint16 _tcpcmd = _TCPCMD_HEART;

    QByteArray d;
    QTEClientData::pack(d, _tcpcmd, t);

    QTEClientMessage qMsg;
    qMsg.setCmd(_TCPCMD_HEART);
    qMsg.setData(d);
    qMsg.translate();
    pline() << qMsg;

    QByteArray b;
    QTEClientParser::pack(b, qMsg);
    write(b);
#else
    QTEHeartBeat t;
    t.m_content = "this is a heartbeat";
    quint16 _tcpcmd = _TCPCMD_HEART;
    sendMessage(_tcpcmd, t);
#endif
}

void QTEClient::readyReadData()
{
    static QByteArray m_blockOnNet;
    m_blockOnNet += readAll();
    //TODO:假设具备判断已经接受完全的装备
    do{
        quint16 nBlockLen = QTEClientParser::parseBlockSize(m_blockOnNet);

        pline() << m_blockOnNet.size() << "..." << nBlockLen;

        if(m_blockOnNet.length() < nBlockLen)
        {
            return;
        }
        else if(m_blockOnNet.length() > nBlockLen)
        {
            //还没有处理完毕，数据已经接收到，异步信号处理出现这种异常
            pline() << "Stick package" << m_blockOnNet.length() << nBlockLen;
            QByteArray netData;
            netData.resize(nBlockLen);
            m_blockOnNet >> netData;
            //TODO:如果异步调用这个函数绘出现什么问题？正常情况，同步获取数据，异步处理；检测异步获取并且处理会有什么状况
            dispatchRecvedMessage(netData);
            continue;
        }
        dispatchRecvedMessage(m_blockOnNet);
        break;
    }while(1);

    m_blockOnNet.clear();
}

void QTEClient::dispatchRecvedMessage(QByteArray &blockOnNet)
{
    QTEClientMessage qMsg;
    QTEClientParser::parse(qMsg, blockOnNet);
    pline() << qMsg;
    switch(qMsg.cmd())
    {
    case _TCPCMD_HEART_RSP:
        recvHeatBeatResultMessage(qMsg);
        break;
    default:
        pline() << "receive unknown command:" << hex << qMsg.cmd();
        break;
    }
}

void QTEClient::recvHeatBeatResultMessage(QTEClientMessage &qMsg)
{
    QTEHeartBeatRsp rsp;
    QTEClientData::parse(rsp, qMsg.cmd(), qMsg.data());
    m_heartCount = 0;
    pline() << rsp.m_content;
}