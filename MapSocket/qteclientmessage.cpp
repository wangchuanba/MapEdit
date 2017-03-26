#include "qteclientmessage.h"
#include "QTEDefine.h"
#include <QBuffer>

QTEClientMessage::QTEClientMessage(QObject *parent) :
    QObject(parent)
{
    m_Head = _TCPCMD_TAGHEAD;
    m_Size = m_Cmd = m_Sum = 0;
    m_Data.clear();;
    m_Tail = _TCPCMD_TAGTAIL;
}

void QTEClientMessage::translate()
{
    m_Size = m_Data.length() + 0x10;
    QByteArray qbaVerify;
    qbaVerify << m_Size << m_Cmd << m_Data;
    m_Sum = 0;
    // 校验码等错误 会导致服务器不回复消息
    // 如果不添加quint8 0x0112+0x0088=0x009a 单字节到二字节进位的位置看不懂
    for(int i = 0; i < qbaVerify.length(); i++)
        m_Sum += quint8(qbaVerify.at(i));
    //real verify
    //m_Sum = qChecksum(qbaVerify.data(), qbaVerify.length());
}

QDebug operator<<(QDebug dbg, const QTEClientMessage &c)
{
    dbg.nospace() << hex << c.head() << "|" <<
                     hex << c.size() << "|" <<
                     hex << c.cmd() << "|" <<
                     dec << c.data().size() << "|" <<
                     hex << c.sum() << "|" <<
                     hex << c.tail();
    return dbg.space();
}

quint16 QTEClientParser::parseBlockSize(const QByteArray &netData)
{
    QByteArray l = netData.left(4);
    quint16 b0 = 0, b1 = 0;
    l >> b0 >> b1;
    return b1;
}

void QTEClientParser::parse(QTEClientMessage &getter, const QByteArray &netData)
{
    QByteArray l = netData;
    quint16 b0 = 0, b1 = 0, b2 = 0, b5 = 0;
    quint32 b6 = 0;
    QByteArray b4;
    l >> b0 >> b1 >> b2;
    b4.resize(b1-0x10);
    l >> b4 >> b5 >> b6;
    getter.setHead(b0);
    getter.setSize(b1);
    getter.setCmd(b2);
    getter.setData(b4);
    getter.setSum(b5);
    getter.setTail(b6);
}

void QTEClientParser::pack(QByteArray &netData, const QTEClientMessage &setter)
{
    netData << setter.head();
    netData << setter.size();
    netData << setter.cmd();
    netData << setter.data();
    netData << setter.sum();
    netData << setter.tail();
}

void QTEClientData::packHeartBeatData(QByteArray &l, const QTEHeartBeat &t)
{
    l << t.m_tipJ;
    l += t.m_content.toAscii();
}

void QTEClientData::parseHeartBeatRspData(QTEHeartBeatRsp &t, const QByteArray &l)
{
    QByteArray _l = l;
    _l >> t.m_tipJJ;


    QVariant v(_l);
    QString str = v.toString();

    t.m_content = str;

    pline() << t.m_tipJJ << t.m_content;

}