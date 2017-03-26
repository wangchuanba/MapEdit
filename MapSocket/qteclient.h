/**************************************************
 **************************************************/
#ifndef QTECLIENT_H
#define QTECLIENT_H

#include <QTcpSocket>
#include <QHostInfo>
#include "qteclientmessage.h"
#include <QTimer>
#include <QThread>
#include "QTEDefine.h"

#define QTE_Q_TCP_SOCKET 0
#define QTE_Q_SOCKET 1
#define QTE_Q_THREAD 0

class QTEClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit QTEClient(QObject *parent = 0);
    virtual ~QTEClient();

    void setServHostPort(QString ip, quint32 p);
    void SendConnectMessage();
    void SendDisConnectFromHost();

signals:
    void signalConnectSucc();
    void signalConnectFail();//

public slots:
    //服务器需要解析收到的命令，而此处不需要，所以客户端和服务器代码分开编写。
    void sendHeatBeatMessage();

private:
    qint8 m_heartCount;
    QString m_host;
    quint32 m_PORT;
    QTimer* timer;
    //TODO:public for debug
    template <typename T>
    void sendMessage(quint16 cmd, const T &t)
    {
        QTEClientMessage qMsg;
        qMsg.setCmd(cmd);
        QByteArray d;
        QTEClientData::pack(d, cmd, t);
        qMsg.setData(d);
        qMsg.translate();
        pline() << qMsg;
        QByteArray b;
        QTEClientParser::pack(b, qMsg);
        write(b);
    }
private slots:
    void domainHostFound();
    void socketStateChanged(QAbstractSocket::SocketState);
    void socketErrorOccured(QAbstractSocket::SocketError);
    void socketConnected();
    void socketDisconnect();
    void updateProgress(qint64);
private slots:
    void readyReadData();
    void dispatchRecvedMessage(QByteArray& blockOnNet);
    void recvHeatBeatResultMessage(QTEClientMessage&);
};


#endif // QTECLIENT_H