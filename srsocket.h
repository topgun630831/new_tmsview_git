#ifndef SRSOCKET_H
#define SRSOCKET_H

#include <QObject>
#include <QHostAddress>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

#include <QMap>
#include "mondialog.h"

enum ProtolState
{
    PROT_IDLE,
    PROT_WAIT_RES,
    PROT_ERR_TIMEOUT,
    PROT_ERR_RCV,
    PROT_ERR_SEND,
    PROT_WAIT_QUIT
};

enum Owner
{
    ownRealtime,
    ownLogReport,
    ownSetup,
    ownLoadControl
};

class CSrSocket : public QObject
{
    Q_OBJECT
public:
//    explicit CSrSocket(QObject *parent, Owner owner=ownRealtime);
    explicit CSrSocket();
    ~CSrSocket();
    void connectToHost();
    void disconnectHost();
    void sendCommand(QByteArray data);
    QByteArray MakeCommand(QString cmd, QJsonObject argObject, QJsonValue vValue, bool bVal=false);
    void changeWriteMode(QString password);
    int m_LoginKey;
//    void sendCommand(QWidget *owner, QString cmd, QJsonObject argObject);
    void sendCommand(QWidget *owner, QString cmd, QString id, QJsonValue argValue = QJsonValue(QJsonValue::Null));
    void sendCommand(QWidget *owner,  QString cmd, QString id, QByteArray ar);
    QString             m_sStartPage;
    QStringList         mGroupList;
    QStringList         mDeviceList;
    bool                mGraphicInitOk;
    bool                mPointInitOk;
    bool                m_bConnected;

private:
    QTcpSocket          m_tcpSocket;
    int                 m_timerIdReal;
    int                 m_timerIdCommand;
    int                 m_timerIdAlive;
    QByteArray          m_RcvBuffer;	// 수신된 문자를 저장할 버퍼
    QByteArray          m_SendData;     // Read data 요구
    QByteArray          m_LoginKeyCommand;     // Real Command
    QByteArray          m_ProjectInfoCommand;     // Real Command
    int                 m_waitLen;
    ProtolState         m_nState;
    Owner               m_nOwner;
    QList<QByteArray*>   m_listCommand;
    QByteArray makeWriteMode();
    bool                m_bChangeWriteMode;
    QString             m_sPassword;
    QMap<QString, QWidget*> commandMap;
    QByteArray          mGraphicCommand;
    bool                m_bInitOK;

protected:
    void timerEvent(QTimerEvent *);
    void writeData(bool waitRes=true);

//signals:
//    void sigData(QJsonArray);
//    void sigTimeOut();
//    void sigConnect();

public slots:
    void connected();
    void readyRead();
    void disconnected();
    void displayError(QAbstractSocket::SocketError socketError);

};

#endif // SRSOCKET_H
