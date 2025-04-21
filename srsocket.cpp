#include <QTimerEvent>
#include <QCryptographicHash>
#include <QtGui>
#include "srsocket.h"
#include "common.h"
#include "common2.h"
#include "pointmon.h"
#include "alarmsummary.h"
#include "srview.h"
#include "epsene.h"

#define REALTIME
extern CComMon *g_CommMon;
extern CComMon2 *g_CommMon2;
extern CAlarmSummary *g_AlarmSum;
extern QString g_Ip;
extern QString g_Password;
extern int g_Port;
extern bool g_DebugDisplay;
extern CSrView	*g_pMainView;
extern QString GraphicId;
extern bool   m_bTmsInitOk;

CSrSocket::CSrSocket() :
    QObject()
{
    m_bConnected = false;
    m_bInitOK = false;
    m_nState = PROT_IDLE;
    m_timerIdCommand = 0;

    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(&m_tcpSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
         this, SLOT(displayError(QAbstractSocket::SocketError)));

    m_timerIdAlive = 0;
    QJsonValue val(1000);
    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    argObject.insert(QString("Id"), QJsonValue(QString("Touch")));
    argObject.insert(QString("Desc"), QJsonValue(QString("Touch")));
    argObject.insert(QString("AccessLevel"),QJsonValue(QString("Admin")));
    m_LoginKeyCommand = MakeCommand(QString("*Login"), argObject, vValue);

    connectToHost();
    m_timerIdReal = startTimer(3000);
    m_bChangeWriteMode = false;
    m_sStartPage = "";
}

CSrSocket::~CSrSocket()
{
    while(!m_listCommand.isEmpty())
    {
        QByteArray *cmd;
        cmd = m_listCommand.takeFirst();
        delete cmd;
    }
    m_tcpSocket.disconnectFromHost();
}

void CSrSocket::connected()
{
    m_bConnected = true;
    mGraphicInitOk = mPointInitOk = false;
    m_bTmsInitOk = false;
    if(g_DebugDisplay)
    {
        qDebug() << "connected!!!";
        QDateTime time = QDateTime::currentDateTime();
        qDebug() << "time:" << time;
        qDebug() << "connected:msec" << time.toMSecsSinceEpoch();
    }
    m_SendData = m_LoginKeyCommand;
    writeData(false);

    QString cmd = "Tag_GroupList";
//    QJsonValue vObject(QJsonValue::Null);
//    QJsonObject obj;
    QByteArray ar = MakeCommand(cmd, QJsonObject(), QJsonValue(), true);
    m_SendData = ar;
    writeData(false);
}

void CSrSocket::writeData(bool waitRes)
{
    m_tcpSocket.write(m_SendData);
    m_waitLen = 0;
    m_RcvBuffer.clear();
    if(waitRes)
        m_nState = PROT_WAIT_RES;
   if(g_DebugDisplay)
    {
        QDateTime time = QDateTime::currentDateTime();
        qDebug() << "time:" << time;
        qDebug() << "time:" << time.toMSecsSinceEpoch();
        qDebug() << "send : " << m_SendData;
    }
}

void CSrSocket::readyRead()
{
    QByteArray		tmpBuffer;

    qint32 len = m_tcpSocket.bytesAvailable();
    if(len == 0)
    {
        disconnectHost();
        return;
    }
    if(m_RcvBuffer.length()+len < 10)
            return;
    tmpBuffer = m_tcpSocket.read(len);

    if(g_DebugDisplay)
    {
        QDateTime time = QDateTime::currentDateTime();
        qDebug() << "time:" << time;
        qDebug() << "time:" << time.toMSecsSinceEpoch();
        qDebug() << "len : " << len << "\nrecv : " << tmpBuffer;
    }
    if(m_timerIdCommand != 0)
        killTimer(m_timerIdCommand);
    m_timerIdCommand = 0;
    //m_timerIdCommand = startTimer(3000);
    qint32 offset = 0;
    while(1)
    {
        QByteArray cnt;
        if(m_waitLen == 0)
        {
            cnt = tmpBuffer.mid(offset+4, 6);
            QString cntstr(cnt);
            m_waitLen = cntstr.toInt();
            m_RcvBuffer = tmpBuffer.mid(offset+10, m_waitLen);  // 10번째이후 복사
            offset += m_waitLen + 10;
        }
        else
        {
            int copyLen;
            if((m_RcvBuffer.size()+tmpBuffer.size()) > m_waitLen)
                copyLen = m_waitLen - m_RcvBuffer.size();
            else
                copyLen = tmpBuffer.size();
            QByteArray ar = tmpBuffer.mid(offset, copyLen);
            m_RcvBuffer.append(ar);
            offset += copyLen;
        }

        if(m_RcvBuffer.size() < m_waitLen)
        {
            QString str;
            str.sprintf("more Data req  rev : %d, wait :%d", m_RcvBuffer.size(), m_waitLen);
            if(g_DebugDisplay)
                qDebug() << str;
            return;
        }

        QJsonParseError err;
        QString errstr;
        QJsonDocument tmpDoc = QJsonDocument::fromJson(m_RcvBuffer,&err);
        if(err.error != QJsonParseError::NoError)
        {
            errstr = err.errorString();
            qDebug() << "\n\n\nError : " << errstr;
            m_nState = PROT_IDLE;
            return;
        }

        QJsonObject jobject = tmpDoc.object();
        QString cmd = jobject["Command"].toString();
        QJsonObject result = jobject["Result"].toObject();
        QString Id = result["Id"].toString();
        if(g_DebugDisplay)
            qDebug() << "readyRead command : " << cmd << " len : " << m_RcvBuffer.length();
        //    QDateTime time = QDateTime::currentDateTime();
        //    qDebug() << "time:" << time;
        //    qDebug() << "time:" << time.toMSecsSinceEpoch();
        QString key = cmd;
        if(cmd != "ShareData_Get")
        {
            if(Id == "")
                key += GraphicId;
            else
                key += Id;
        }
        if(commandMap.contains(key))
        {
            QWidget *owner = commandMap[key];
            commandMap.remove(key);
            if (qobject_cast<CMonDialog*>(owner))
                ((CMonDialog*)owner)->onRead(cmd, jobject);
            else
                qDebug() << "CMonDialog 상속클래스가 아닙니다";
        }
        else
        {
            if(g_DebugDisplay)
                qDebug() << "command map not contains : " << cmd << "key:" << key << "cnt:" << commandMap.count();

            if(g_CommMon != 0 && cmd == "@Monitor_Event")
                g_CommMon->onRead(cmd, jobject);
            else
            if(g_CommMon2 != 0 && cmd == "@Monitor_Event")
                g_CommMon2->onRead(cmd, jobject);
            else
            if(g_AlarmSum != 0 && cmd == "@AlarmEvent")
                g_AlarmSum->onRead(cmd, jobject);
            else
            if(g_pMainView != 0 && cmd == "@WriteEvent")
                g_pMainView->onRead(cmd, jobject);

        }
        if(cmd == "Project_GetInfo")
        {
            QJsonObject jObj = jobject["Result"].toObject();
            QJsonArray ar = jObj["StartPage"].toArray();
            foreach (const QJsonValue& val, ar)
            {
                QJsonObject startPage = val.toObject();
                if(startPage["Item"].toString() == "TouchGraphic")
                {
                    m_sStartPage = startPage["Value"].toString();
                    m_bInitOK = true;
                    break;
                }
            }
        }
        if(cmd == "Tag_GroupList")
        {
            QJsonObject jObj = jobject["Result"].toObject();
            QJsonArray groupList = jObj["GroupList"].toArray();
            QJsonArray deviceList = jObj["DeviceList"].toArray();
            mGroupList.clear();
            mDeviceList.clear();
            foreach (const QJsonValue& value, groupList)
            {
                mGroupList.append(value.toString());
//                qDebug() << "Group:" << value.toString();
            }
            foreach (const QJsonValue& value, deviceList)
                mDeviceList.append(value.toString());
        }
//        if(m_bInitOK)
        {
            m_nState = PROT_IDLE;
            if(!m_listCommand.isEmpty())
            {
                QByteArray *cmd;
                cmd = m_listCommand.takeFirst();
                m_SendData = *cmd;
                delete cmd;
                m_tcpSocket.write(m_SendData);
                m_waitLen = 0;
                m_RcvBuffer.clear();
                m_nState = PROT_WAIT_RES;

                if(g_DebugDisplay)
                    qDebug() << "\nsend : " << m_SendData;
            }
        }
        m_waitLen = 0;
        m_RcvBuffer.clear();
        if(len <= offset)
            return;
    }
}

void CSrSocket::disconnected()
{
    m_bConnected = false;
//    QDateTime time = QDateTime::currentDateTime();
//    qDebug() << "time:" << time;
//    qDebug() << "time:" << time.toMSecsSinceEpoch();
//    qDebug() << "disconnected!!!";
//    emit sigTimeOut();
    m_tcpSocket.disconnectFromHost();
    if(m_timerIdCommand != 0)
        killTimer(m_timerIdCommand);
    m_timerIdCommand = 0;
    commandMap.clear();
}

void CSrSocket::displayError(QAbstractSocket::SocketError socketError)
 {
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
//         QMessageBox::information(tr("Fortune Client"),
  //                                tr("The host was not found. Please check the "
    //                                 "host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
 //        QMessageBox::information(this, tr("Fortune Client"),
   //                               tr("The connection was refused by the peer. "
     //                                "Make sure the fortune server is running, "
       //                              "and check that the host name and port "
         //                            "settings are correct."));
         break;
     default:
         ;
//         QMessageBox::information(this, tr("Fortune Client"),
  //                                tr("The following error occurred: %1.")
    //                              .arg(m_ptcpSocket->errorString()));
     }
}

void CSrSocket::timerEvent(QTimerEvent *event)
{

    if (event->timerId() == m_timerIdReal)
    {
        if(m_tcpSocket.state() == QAbstractSocket::ConnectedState)
        {
 /*           if(m_bLogin == true && m_nState == PROT_IDLE)
            {
               // qint64 ret = m_tcpSocket.write(m_SendData);
                m_SendData = m_RealCommand;
                writeData();
                //m_tcpSocket.write(m_RealCommand);
                //m_waitLen = 0;
                //m_RcvBuffer.clear();
                //m_nState = PROT_WAIT_RES;
                return;
            }

        */
        }
        else
          connectToHost();
    }
    else
    if (event->timerId() == m_timerIdCommand)
    {
//        emit sigTimeOut();
        m_tcpSocket.disconnectFromHost();
        killTimer(m_timerIdCommand);
        m_timerIdCommand = 0;
    }
    else
    if (event->timerId() == m_timerIdAlive)
    {
        m_SendData = m_LoginKeyCommand;
        writeData();
    }
}

void CSrSocket::connectToHost()
{
    m_tcpSocket.abort();
    disconnectHost();
    m_tcpSocket.connectToHost(g_Ip, g_Port);
}

void CSrSocket::disconnectHost()
{
/*    QDateTime time = QDateTime::currentDateTime();
    qDebug() << "time:" << time;
    qDebug() << "time:" << time.toMSecsSinceEpoch();
    qDebug() << "\ndisconnectHost";
  */  m_tcpSocket.disconnectFromHost();
    if(m_timerIdCommand != 0)
    killTimer(m_timerIdCommand);
    m_timerIdCommand = 0;
}

void CSrSocket::sendCommand(QByteArray data)
{
    QByteArray *cmd = new QByteArray;
    *cmd = data;
    m_listCommand.append(cmd);
    if(m_timerIdCommand != 0)
        killTimer(m_timerIdCommand);
//    if(m_nOwner != ownRealtime)
    m_timerIdCommand = startTimer(3000);
    if(m_timerIdAlive != 0)
    {
        killTimer(m_timerIdAlive);
        m_timerIdAlive = startTimer(5000);
    }
    m_SendData = data;
    if(m_tcpSocket.state() == QAbstractSocket::UnconnectedState)
        connectToHost();
    else
    {
        if(m_nState == PROT_IDLE)
        {
            QByteArray *cmd;
            cmd = m_listCommand.takeFirst();
            m_SendData = *cmd;
            delete cmd;
            writeData();
        }
    }
}

QByteArray CSrSocket::MakeCommand(QString cmd, QJsonObject argObject, QJsonValue vValue, bool bVal)
{
    QString nid = "0";
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;

    tmpObject.insert(QString("Command"),QJsonValue(cmd));
    tmpObject.insert(QString("Id"),QJsonValue(nid));

    if(vValue.isNull() && bVal == false)
        tmpObject.insert(QString("Args"),argObject);
    else
        tmpObject.insert(QString("Args"),vValue);

    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);

    data.insert(0, QString("BEST%1").arg(data.size(),6,10,QChar('0')));
    return data;
}

void CSrSocket::changeWriteMode(QString password)
{
    m_sPassword = password;
    if(m_tcpSocket.state() == QAbstractSocket::ConnectedState)
    {
        if(m_nState == PROT_IDLE)
        {
            m_SendData = makeWriteMode();
            writeData();
        }
        else
        {
            QByteArray *cmd = new QByteArray;
            *cmd = makeWriteMode();
            m_listCommand.append(cmd);
        }
        return;
    }

    m_bChangeWriteMode = true;
    connectToHost();
}

QByteArray CSrSocket::makeWriteMode()
{
    QByteArray data;

    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);
    QString password = QString("%1:%2").arg(m_LoginKey).arg(m_sPassword);
    QByteArray bytepass = QCryptographicHash::hash(password.toLocal8Bit(), QCryptographicHash::Sha1);
    QString hashPassword;
    for(int i = 0; i < bytepass.length(); i++)
        hashPassword += QString("%1").arg((unsigned char)bytepass.at(i),2,16,QChar('0'));
    argObject.insert(QString("WritePassword"), QJsonValue(hashPassword));
    argObject.insert(QString("WriteEnable"), QJsonValue(bool(true)));
    return(MakeCommand("Login", argObject,vObject));
}

void CSrSocket::sendCommand(QWidget *owner, QString cmd, QString id, QJsonValue argValue)
{
    if(m_bConnected == false)
        return;
    QString nid = "0";
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;
    QJsonValue vValue(QJsonValue::Null);

    tmpObject.insert(QString("Command"),QJsonValue(cmd));
    tmpObject.insert(QString("Id"),QJsonValue(nid));
    tmpObject.insert(QString("Args"),argValue);
    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);
    data.insert(0, QString("BEST%1").arg(data.size(),6,10,QChar('0')));
    QString key = cmd + id;
    if(owner != 0)
        commandMap[key] = owner;
    sendCommand(data);
}

void CSrSocket::sendCommand(QWidget *owner,  QString cmd, QString id, QByteArray ar)
{
    if(m_bConnected == false)
        return;
    QString key = cmd + id;
    if(owner != 0)
        commandMap[key] = owner;
    sendCommand(ar);
}
