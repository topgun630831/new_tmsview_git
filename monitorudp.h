#ifndef MONITORUDP_H
#define MONITORUDP_H

#include <QObject>
#include <QUdpSocket>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMap>

class CMonitorUdp : public QObject
{
    Q_OBJECT
public:
    explicit CMonitorUdp(QObject *parent = 0);
    static CMonitorUdp* Instance();
    void sendCommand(QWidget *owner, QString cmd, QJsonObject argObject, QJsonValue vValue);

signals:
public slots:
    void readyRead();

private:
    static CMonitorUdp*     m_pInstance;
    QUdpSocket              *socket;
    QHostAddress            m_IpAddr;
    quint16                 m_Port;
    QMap<QString, QWidget*> commandMap;
};

#endif // MONITORUDP_H
