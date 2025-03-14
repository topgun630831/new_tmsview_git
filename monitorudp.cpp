#include "monitorudp.h"
#include "networksetup.h"
#include "versioninfo.h"
#include "srview.h"
#include "myapplication.h"
#include "infomationdialog.h"
#include <QEvent>

extern CMyApplication *gApp;
extern CSrView	*g_pMainView;
extern QString g_Ip;

extern bool g_DebugDisplay;

CMonitorUdp* CMonitorUdp::m_pInstance = 0;

CMonitorUdp::CMonitorUdp(QObject *parent) :
    QObject(parent)
{
        socket = new QUdpSocket(this);

    m_IpAddr.setAddress(g_Ip);
    m_Port = 11001;
//    socket->bind(m_Port);

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

CMonitorUdp* CMonitorUdp::Instance()
{
    if(!m_pInstance)
        m_pInstance = new CMonitorUdp;

    return m_pInstance;
}

void CMonitorUdp::readyRead()
{
    if(g_DebugDisplay)
        qDebug() << "CMonitorUdp::readyRead()";
    while(socket->hasPendingDatagrams())
    {
        // when data comes in
        QByteArray buffer;
        buffer.resize(socket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;

        // qint64 QUdpSocket::readDatagram(char * data, qint64 maxSize,
        //                 QHostAddress * address = 0, quint16 * port = 0)
        // Receives a datagram no larger than maxSize bytes and stores it in data.
        // The sender's host address and port is stored in *address and *port
        // (unless the pointers are 0).

        socket->readDatagram(buffer.data(), buffer.size(),
                             &sender, &senderPort);

        if(g_DebugDisplay)
        {
            qDebug() << "Message from: " << sender.toString();
            qDebug() << "Message port: " << senderPort;
            qDebug() << "Message: " << buffer;
        }

        QByteArray cnt;
        if(buffer[9] == ' ')
            cnt = buffer.mid(4, 5);
        else
            cnt = buffer.mid(4, 6);
        QString cntstr(cnt);
        QByteArray rcvBuffer = buffer.mid(10,buffer.length()-10);  // 10번째이후 복사

        QJsonParseError err;
        QString errstr;
        QJsonDocument tmpDoc = QJsonDocument::fromJson(rcvBuffer,&err);
        if(err.error != QJsonParseError::NoError)
        {
            errstr = err.errorString();
            qDebug() << "\n\n\nError : " << errstr;
            CInfomationDialog dlg(errstr);
            dlg.exec();
            return;
        }

        QJsonObject jobject = tmpDoc.object();
        QString error = jobject["Error"].toString();
        if(error == "Error")
        {
            errstr = jobject["Result"].toString();
            CInfomationDialog dlg(errstr);
            dlg.exec();
        }
        QString cmd = jobject["Command"].toString();
        if(commandMap.contains(cmd))
        {
            QWidget *owner = commandMap[cmd];
            commandMap.remove(cmd);
            if (qobject_cast<CMonDialog*>(owner))
            {
                ((CMonDialog*)owner)->onRead(cmd, jobject);
            }
        }
 #if 1
        else
        if(cmd == "@Event_TouchClick")
        {
            qDebug() << "Event_TouchClick";
            QJsonObject value = jobject["Result"].toObject();
            double x = value["X"].toDouble();
            double y = value["Y"].toDouble();
            qDebug() << "Pos " << x << "," << y;;
            QPoint pos(x,y);
            QMouseEvent event(QEvent::MouseButtonPress,
                              pos,
                              Qt::LeftButton,
                              Qt::LeftButton,//Qt::MouseButtons buttons
                              0 //Qt::KeyboardModifiers modifiers)
                              );
//            g_pMainView->mousePressEvent(&event);


            QWidget *act = QApplication::topLevelAt((int)x,(int)y);
            if(act == 0)
                return;
            while(1)
            {            QWidget *act2 = act->childAt((int)x,(int)y);
                        if(act2 != 0)
                            act = act2;
                        else
                            break;
            }
//            QWidget *receiver = QApplication::widgetAt(pos);
//            QWidget *receiver = QApplication::widgetAt(pos);

            if (act) {
//                QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton,
                                                     QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, act->mapFromGlobal(pos), Qt::LeftButton,
                    Qt::LeftButton, Qt::NoModifier);
                QCoreApplication::postEvent(act, event);
//                QMouseEvent *event2 = new QMouseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton,
                                                      QMouseEvent *event2 = new QMouseEvent(QEvent::MouseButtonRelease, act->mapFromGlobal(pos), Qt::LeftButton,
                    Qt::LeftButton, Qt::NoModifier);
                QCoreApplication::postEvent(act, event2);
            }
/*
                        QMouseEvent *mEvnPress;
            QMouseEvent *mEvnRelease;
            mEvnPress = new QMouseEvent(QEvent::MouseButtonPress, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            mEvnRelease = new QMouseEvent(QEvent::MouseButtonRelease, pos, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
            QCoreApplication::postEvent(receiver,mEvnPress);
            QCoreApplication::postEvent(receiver,mEvnRelease);
            break;
            */
//            gApp->sendEvent(act, &event);
//            gApp->sendEvent(act, &event);
//            QWidget *act2 = act->childAt((int)x,(int)y);
//            if(act2 != 0)
//                gApp->sendEvent(act2, &event);
//            QApplication::sendEvent(g_pMainView, &event);
        }
#endif
        else
        if(g_DebugDisplay)
            qDebug() << "command map not contains";
    }
}

void CMonitorUdp::sendCommand(QWidget *owner, QString cmd, QJsonObject argObject, QJsonValue vValue)
{
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;

    tmpObject.insert(QString("Command"),QJsonValue(cmd));

    tmpObject.insert(QString("Id"),QJsonValue(QString("Touch")));
    tmpObject.insert(QString("AccessLevel"),QJsonValue(QString("Admin")));
    if(vValue.isNull())
        tmpObject.insert(QString("Args"),argObject);
    else
        tmpObject.insert(QString("Args"),vValue);

    QJsonArray tmpArray;
    QJsonValue tmpValue(tmpObject);
    tmpArray.append(tmpValue);
    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);
    data.insert(0, QString("BEST%1 ").arg(data.size(),5,10,QChar('0')));
    socket->writeDatagram(data, m_IpAddr, m_Port);
    if(g_DebugDisplay)
    {
        qDebug() << "Send Message to: " << m_IpAddr.toString();
        qDebug() << "Send Message port: " << m_Port;
        qDebug() << "Send Message: " << data;
    }
    if(owner != 0)
        commandMap[cmd] = owner;
}

