#ifndef NETWORKSETUP_H
#define NETWORKSETUP_H

#include "mondialog.h"
#include <QJsonArray>
#include <QList>
#include <QTimer>

namespace Ui {
class CNetworkSetup;
}

struct Gateway_TAB {
    QString Interface;
    QString Kind;
    QString Dest;
    QString Netmask;
    QString GatewayIp;
};

class CNetworkSetup : public CMonDialog
{
    Q_OBJECT

public:
    explicit CNetworkSetup(bool bAll=true, QWidget *parent = 0);
    ~CNetworkSetup();
    void onRead(QString &cmd, QJsonObject&);
    static bool IpValidate(QString &ip, QString msg);

private slots:
    void on_testMode_clicked();
    void on_ok_clicked();
    void on_lan1DefaultGateway_clicked();
    void on_lan2DefaultGateway_clicked();
    void on_versionInfo_clicked();
    void on_sysMange_clicked();
    void on_btnRoute_clicked();
    void on_cancel_clicked();
    void on_btnModel_clicked();
    void on_btnPingTest_clicked();
    void on_lan2Dhcp_clicked();
    void onTimer();
    void onTimerClose();

    void on_btnHasuSetup_clicked();

protected:
    void timerEvent(QTimerEvent *);

private:
    Ui::CNetworkSetup *ui;
    QJsonArray mGateways;
    QJsonArray mEthernets;
    bool        lan2Used;
    QList<Gateway_TAB *> GatewayList;
    bool mbChanged;
    int         m_timerId;
    void RunStart(bool start);
    bool passwordCheck();
    void	paintEvent(QPaintEvent*);
    QString mPassword;
    QTimer              m_Timer;
    QTimer              m_TimerClose;
};

#endif // NETWORKSETUP_H
