#ifndef QTEMESSAGE_H
#define QTEMESSAGE_H

#include "QTEDefine.h"

#define _TCPCMD_TAGHEAD                   0xEECC
#define _TCPCMD_TAGTAIL                   0xCCEE

#define _TCPCMD_HEART                     0x0000
#define _TCPCMD_HEART_RSP                     0x8000

#define _TCP_BLOCKDATA_SIZE                    0x400
#define _TCP_RECVBUFF_SIZE                      0x800


typedef struct tagHeartBeat
{
    quint8 m_tipJ;
    QString m_content;
}QTEHeartBeat;

typedef struct tagHeartBeatRsp
{
    quint16 m_tipJJ;
    QString m_content;
}QTEHeartBeatRsp;

class QTEClientMessage : public QObject
{
    Q_OBJECT
public:
    explicit QTEClientMessage(QObject *parent = 0);

    const quint16& head() const { return m_Head; }
    void setHead(quint16 head) { m_Head = head; }
    const quint16& size() const { return m_Size; }
    void setSize(quint16 size) { m_Size = size; }
    const quint16& cmd() const { return m_Cmd; }
    void setCmd(quint16 cmd) { m_Cmd = cmd; }
    const QByteArray& data() const { return m_Data; }
    void setData(QByteArray& data) { m_Data = data; }
    const quint16& sum() const { return m_Sum; }
    void setSum(quint16 sum) { m_Sum = sum; }
    const quint32& tail() const { return m_Tail; }
    void setTail(quint32 tail) { m_Tail = tail; }
    void translate();

signals:

public slots:

private:
    quint16 m_Head;
    quint16 m_Size;
    quint16 m_Cmd;
    QByteArray m_Data;
    quint16 m_Sum;
    quint32 m_Tail;
};

QDebug operator<< (QDebug dbg, const QTEClientMessage &c);

class QTEClientParser : public QObject
{
public:
    explicit QTEClientParser(QObject *parent = 0) : QObject(parent) {}

    static quint16 parseBlockSize(const QByteArray &netData);
    static void parse(QTEClientMessage& getter, const QByteArray &netData);
    static void pack(QByteArray& netData, const QTEClientMessage& setter);

private:
};

class QTEClientData : public QObject
{
    Q_OBJECT
public:
    explicit QTEClientData(QObject *parent = 0) : QObject(parent) {}

    template <typename T>
    static void pack(QByteArray& l, quint16 cmd, const T& t)
    {
        switch(cmd)
        {
        case _TCPCMD_HEART:
            packHeartBeatData(l, (QTEHeartBeat&)t);
            break;
        default:
            pline() << "pack unknown data" << hex << cmd;
            break;
        }
    }

    template <typename T>
    static void parse(T& t, quint16 cmd, const QByteArray& l)
    {
        switch(cmd)
        {
        case _TCPCMD_HEART_RSP:
            parseHeartBeatRspData((QTEHeartBeatRsp&)t, l);
            break;
        default:
            pline() << "parse unknown data" << hex << cmd;
            break;
        }
    }

    static void packHeartBeatData(QByteArray& l, const QTEHeartBeat& t);
    static void parseHeartBeatRspData(QTEHeartBeatRsp& t, const QByteArray& l);


signals:
public slots:
protected:
private:
};

#endif // QTEMESSAGE_H