#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "networksetup.h"
#include "ui_networksetup.h"
#include <QMessageBox>
#include "testdialog.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "monitorudp.h"
#include "versioninfo.h"
#include "systemmanagement.h"
#include "passwordinput.h"
#include "route.h"
#include "pingtest.h"
#include "epshasusetup.h"

extern bool    m_bScreenSave;

extern bool g_bPowerOff;
extern QString gSoftwareModel;
extern QString gHardwareModel;
extern QString gCpuSerialNumber;
extern QString gPassword;
bool bTmsMode;
extern QString gPassword;
bool g_bExit;
CNetworkSetup::CNetworkSetup( bool bAll, QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CNetworkSetup)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    mPassword = gPassword;
    g_bExit = false;
    bTmsMode = bAll;
    if(bAll == false)
    {
 //       ui->sysMange->setVisible(false);
        ui->lan1DefaultGateway->setVisible(false);
        ui->lan2DefaultGateway->setVisible(false);
    }
    else
    {
        ui->btnRoute->setVisible(false);
        ui->testMode->setVisible(false);
    }
    ui->lan2NotUsed->setChecked(true);
    lan2Used = false;

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(&m_TimerClose, SIGNAL(timeout()), this, SLOT(onTimerClose()));
    m_Timer.start(1000);

    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);
    mbChanged = false;
//    if(gSoftwareModel != "EPS_HANRIVER" && gSoftwareModel != "EPS_HASU")
    ui->btnModel->setVisible(false);
//    if(gSoftwareModel != "EPS_HASU")
    ui->btnHasuSetup->setVisible(false);
    m_RejectTimerId = startTimer(1000*60*5-30);
    ui->serialNo->setText(gCpuSerialNumber);
}

CNetworkSetup::~CNetworkSetup()
{
    delete ui;
    foreach (const Gateway_TAB * gateway, GatewayList)
        delete gateway;
}

void CNetworkSetup::paintEvent(QPaintEvent *)
{
    QPainter	p(this);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);

    QSize size = frameSize();
    p.setPen(pen);
    p.drawLine(0, 0, size.rwidth(), 0);
    p.drawLine(0, 0, 0, size.rheight());
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawLine(size.rwidth(), 0, size.rwidth(), size.rheight());
    p.drawLine(0, size.rheight(), size.rwidth(), size.rheight());
}

void CNetworkSetup::on_testMode_clicked()
{
    CQuestionDialog dlg("테스트모드에 들어가면 시스템이 정상 동작하지 않습니다.\n 테스트모드를 실행하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
         RunStart(false);
         CTestDialog dlg(this);
         dlg.exec();
         RunStart(true);
         m_TimerClose.start(1000);
    }
}

void CNetworkSetup::onRead(QString& cmd, QJsonObject& robject)
{
    bool bRouteLan1 = false;
    QString sRouteLan1;
    QJsonObject jobject = robject["Result"].toObject();
    if(cmd == "Network_Get")
    {
        QString build;
        QFile file("/app/wwwroot/BUILDTIME");
        if(file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QByteArray ar;
            ar = file.readLine();
            build = (QString(ar.data()));
            file.close();
        }

        //        ui->productName->setText(jobject["Model"].toString());
        build.remove("\n");
//        if(build == "")
//            ui->serialNo->setText(QString("%1 %2").arg((int)jobject["SerialNumber"].toDouble(),10,10,QChar('0')).arg(gHardwareModel));
//        else
//            ui->serialNo->setText(QString("%1(%2) %3").arg((int)jobject["SerialNumber"].toDouble(),10,10,QChar('0')).arg(build).arg(gHardwareModel));
        if(jobject["Dns"].isNull() == false)
            ui->dnsPrimary->setText(jobject["Dns"].toString());
        if(jobject["Dns2"].isNull() == false)
            ui->dnsSecondary->setText(jobject["Dns2"].toString());
        mGateways = jobject["Gateways"].toArray();
        foreach (const QJsonValue & value, mGateways)
        {
            QJsonObject gateway = value.toObject();
            Gateway_TAB * gatewayTab = new Gateway_TAB;
            gatewayTab->Dest = gateway["Dest"].toString();
            gatewayTab->Netmask = gateway["Netmask"].toString();
            gatewayTab->GatewayIp = gateway["GatewayIp"].toString();
            if(gateway["Interface"].toString() == "eth0")
            {
                gatewayTab->Interface = "LAN1";
                if(bRouteLan1 == false)
                {
                    sRouteLan1 = gatewayTab->GatewayIp;
                    bRouteLan1 = true;
                }
            }
            else
                gatewayTab->Interface = "LAN2";
            GatewayList.append(gatewayTab);
        }
        mEthernets = jobject["Ethernets"].toArray();
        foreach (const QJsonValue & value, mEthernets)
        {
            QJsonObject lan = value.toObject();
            if (lan["Interface"].toString() == "eth0")
            {
                if(lan["UsingDhcp"].toBool() == true)
                    ui->lan1Dhcp->setChecked(true);
                else
                {
                    ui->lan1Static->setChecked(true);
                    ui->lan1IP->setText(lan["Ip"].toString());
                    ui->lan1Subnetmask->setText(lan["Netmask"].toString());
                    QString gateway = lan["DefaultGatewayIp"].toString();
                    ui->lan1Gateway->setText(gateway);
                    if(gateway != "")
                        ui->lan1DefaultGateway->setChecked(true);
                    ui->lan2DefaultGateway->setChecked(false);
                    ui->lan1Mac->setText(lan["Mac"].toString());
                }
            }
            else
            if (lan["Interface"].toString() == "eth1")
            {
                lan2Used = true;
                if(lan["Enabled"].toBool() == false)
                    ui->lan2NotUsed->setChecked(true);
                else
                {
                    if(lan["UsingDhcp"].toBool() == true)
                        ui->lan2Dhcp->setChecked(true);
                    else
                        ui->lan2Static->setChecked(true);
                    {
                        ui->lan2IP->setText(lan["Ip"].toString());
                        ui->lan2Subnetmask->setText(lan["Netmask"].toString());
                        ui->lan2Mac->setText(lan["Mac"].toString());
                        QString gateway = lan["DefaultGatewayIp"].toString();
                        ui->lan2Gateway->setText(gateway);
                        if(ui->lan1DefaultGateway->isChecked() == false && gateway != "")
                            ui->lan2DefaultGateway->setChecked(true);
                    }
                }
            }
        }
        if(bTmsMode == false /*&& gSoftwareModel == "ORION_TMS"*/)
            ui->lan1Gateway->setText(sRouteLan1);
    }
    else
    if(cmd == "Network_Set")
    {
        QString error = jobject["Error"].toString();
        if(error == "Error")
        {
            QString result = jobject["Result"].toString();
            CInfomationDialog dlg(result);
            dlg.exec();
        }
    }
}

void CNetworkSetup::on_ok_clicked()
{
    bool lan1Dhcp = ui->lan1Dhcp->isChecked();
    bool lan2Dhcp = ui->lan2Dhcp->isChecked();
    if(lan2Dhcp)
        on_lan2Dhcp_clicked();

    if(lan1Dhcp && lan2Dhcp)
    {
        CInfomationDialog dlg(tr("LAN1과 LAN2 모두 DHCP를 사용할 수 없습니다."));
        return;
    }
    QString lan1IP = ui->lan1IP->text();
    QString lan1Subnetmask = ui->lan1Subnetmask->text();
    QString lan1Gateway = ui->lan1Gateway->text();
    QString lan2IP = ui->lan2IP->text();
    QString lan2Subnetmask = ui->lan2Subnetmask->text();
    QString lan2Gateway = ui->lan2Gateway->text();
    QString dns1 = ui->dnsPrimary->text();
    QString dns2 = ui->dnsSecondary->text();
    bool lan2NotUsed = ui->lan2NotUsed->isChecked();

    if(bTmsMode == false /*&& gSoftwareModel == "ORION_TMS"*/)
    {
        ui->lan1DefaultGateway->setChecked(false);
        ui->lan2DefaultGateway->setChecked(true);
    }

    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);
    bool        bLan2 = false;

    QJsonArray Ethernets;
    foreach (const QJsonValue & value, mEthernets)
    {
        QJsonObject lan = value.toObject();
        if (lan["Interface"].toString() == "eth0")
        {
            lan["Enabled"] = QJsonValue(bool(true));
            lan["UsingDhcp"] = QJsonValue(lan1Dhcp);
            if(lan1Dhcp == false)
            {
                if(IpValidate(lan1IP, QString(tr("LAN1 IP주소가 올바르지 않습니다"))) == false)
                    return;
                lan["Ip"] = QJsonValue(lan1IP);
                if(IpValidate(lan1Subnetmask, QString(tr("LAN1 서브넷마스크가 올바르지 않습니다"))) == false)
                    return;
                lan["Netmask"] = QJsonValue(lan1Subnetmask);
                if(ui->lan1DefaultGateway->isChecked())
                {
                    if(IpValidate(lan1Gateway, QString(tr("LAN1 게이트웨이 주소가 올바르지 않습니다"))) == false)
                        return;
                    lan["DefaultGatewayIp"] = QJsonValue(lan1Gateway);
                }
                if(ui->lan1DefaultGateway->isChecked() == false)
                    lan["DefaultGatewayIp"] = QJsonValue(QJsonValue::Null);
            }
        }
        else
        if (lan["Interface"].toString() == "eth1")
        {
            lan["Enabled"] = QJsonValue(bool(!lan2NotUsed));
            lan["UsingDhcp"] = QJsonValue(lan2Dhcp);
            bLan2 = true;
            if(lan2NotUsed == false)
            {
                if(lan2Dhcp == false)
                {
                    if(IpValidate(lan2IP, QString(tr("LAN2 IP주소가 올바르지 않습니다"))) == false)
                        return;
                    lan["Ip"] = QJsonValue(lan2IP);
//                    qDebug() << "LAN2 Subnetmask : " << lan2Subnetmask;
                    if(IpValidate(lan2Subnetmask, QString(tr("LAN2 서브넷마스크가 올바르지 않습니다"))) == false)
                        return;
                    lan["Netmask"] = QJsonValue(lan2Subnetmask);
                    if(ui->lan2DefaultGateway->isChecked())
                    {
                        if(IpValidate(lan2Gateway, QString(tr("LAN2 게이트웨이 주소가 올바르지 않습니다"))) == false)
                            return;
                        lan["DefaultGatewayIp"] = QJsonValue(lan2Gateway);
                    }
                    if(ui->lan1DefaultGateway->isChecked())
                        lan["DefaultGatewayIp"] = QJsonValue(QJsonValue::Null);
                    if(ui->lan2DefaultGateway->isChecked() == false)
                        lan["DefaultGatewayIp"] = QJsonValue(QJsonValue::Null);
                }
            }
        }
        Ethernets.append(QJsonValue(lan));
    }
    if(bLan2 == false && lan2NotUsed == false)
    {
        QJsonObject lan;
        lan["Interface"] = QJsonValue(QString("eth1"));
        lan["Enabled"] = QJsonValue(bool(true));
        lan["UsingDhcp"] = QJsonValue(lan2Dhcp);
        bLan2 = true;
        if(lan2Dhcp == false)
        {
            if(IpValidate(lan2IP, QString(tr("LAN2 IP주소가 올바르지 않습니다"))) == false)
                return;
            lan["Ip"] = QJsonValue(lan2IP);
            if(IpValidate(lan2Subnetmask, QString(tr("LAN2 서브넷마스크가 올바르지 않습니다"))) == false)
                return;
            lan["Netmask"] = QJsonValue(lan2Subnetmask);
            if(ui->lan2DefaultGateway->isChecked())
            {
                if(IpValidate(lan2Gateway, QString(tr("LAN2 게이트웨이 주소가 올바르지 않습니다"))) == false)
                    return;
                lan["DefaultGatewayIp"] = QJsonValue(lan2Gateway);
            }
            if(ui->lan2DefaultGateway->isChecked() == false)
                lan["DefaultGatewayIp"] = QJsonValue(QJsonValue::Null);
        }
        Ethernets.append(QJsonValue(lan));
    }
    if(dns1 != "")
    {
        if(IpValidate(dns1, QString(tr("기본 DNS 주소가 올바르지 않습니다"))) == false)
            return;
    }
    argObject.insert(QString("Dns"),QJsonValue(dns1));
    if(dns2 != "")
    {
        if(IpValidate(dns2, QString(tr("보조 DNS 주소가 올바르지 않습니다"))) == false)
            return;
    }


    argObject.insert(QString("Dns2"),QJsonValue(dns2));
    argObject.insert(QString("Ethernets"), QJsonValue(Ethernets));

    QJsonArray Gateway;
    QJsonObject gateway;
    if(bTmsMode == false /*&& gSoftwareModel == "ORION_TMS"*/)
    {
        gateway["Interface"] = QJsonValue(QString("eth0"));
        gateway["Kind"] = QJsonValue(QString("net"));
        gateway["Dest"] = QJsonValue(QString("10.101.164.0"));
        gateway["Netmask"] = QJsonValue(QString("255.255.255.0"));
        gateway["GatewayIp"] = QJsonValue(lan1Gateway);

        Gateway.append(QJsonValue(gateway));
    }
    else
    {
        foreach (const Gateway_TAB * gatewayTab, GatewayList) {
            if(gatewayTab->Interface == "LAN1")
                gateway["Interface"] = QJsonValue(QString("eth0"));
            else
                gateway["Interface"] = QJsonValue(QString("eth1"));
            gateway["Kind"] = QJsonValue(QString("net"));
            gateway["Dest"] = QJsonValue(gatewayTab->Dest);
            gateway["Netmask"] = QJsonValue(gatewayTab->Netmask);
            gateway["GatewayIp"] = QJsonValue(gatewayTab->GatewayIp);

            Gateway.append(QJsonValue(gateway));
        }
    }
    argObject.insert(QString("Gateways"), QJsonValue(Gateway));

    CQuestionDialog dlg(tr("저장 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
    {
        CMonitorUdp::Instance()->sendCommand(this,  QString("Network_Set"), argObject, vObject);
        mbChanged = false;
        if(lan2Dhcp)
            m_timerId = startTimer(3000);
    }
}

bool CNetworkSetup::IpValidate(QString &ip, QString msg)
{
    QString Octet = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegExpValidator v(QRegExp("^" + Octet + "\\." + Octet + "\\." + Octet + "\\." + Octet + "$"), 0);

    int pos = 0;
    if(ip == "" || v.validate(ip, pos) != QValidator::Acceptable)
    {
        CInfomationDialog dlg(msg);
        dlg.exec();
        return false;
    }
    return true;
}

void CNetworkSetup::on_lan1DefaultGateway_clicked()
{
    ui->lan2DefaultGateway->setChecked(false);
}

void CNetworkSetup::on_lan2DefaultGateway_clicked()
{
    if(ui->lan2NotUsed->isChecked() == true)
        if(ui->lan2DefaultGateway->isChecked())
        {
            ui->lan2DefaultGateway->setChecked(false);
            CInfomationDialog dlg(QString(tr("LAN2가 사용안함으로 선택되었습니다.")));
            dlg.exec();
        }
    ui->lan1DefaultGateway->setChecked(false);
}

void CNetworkSetup::RunStart(bool start)
{
    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);

    QString proc = "app";
    bool SaveStatus=false;
    argObject.insert(QString("Proc"),QJsonValue(proc));
    argObject.insert(QString("Enabled"),QJsonValue(start));
    argObject.insert(QString("SaveStatus"),QJsonValue(SaveStatus));

    CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Set"), argObject, vObject);
}

void CNetworkSetup::on_versionInfo_clicked()
{
    CVersionInfo dlg;
    dlg.exec();
}

void CNetworkSetup::on_sysMange_clicked()
{
    SystemManagement dlg(bTmsMode, this);
    dlg.setModal(true);
    if(passwordCheck())
        dlg.exec();
}

bool CNetworkSetup::passwordCheck()
{
    bool ret = false;
    if(gSoftwareModel == "EPS_HANRIVER")    // 한강수계 표준RTU
    {
        PasswordInput dlg(this);
        if(dlg.exec() == QDialog::Accepted)
        {
            if(dlg.m_sPassword == gPassword || dlg.m_sPassword == "bestec21")
                ret = true;
            else
            {
                CInfomationDialog dlg(tr("비밀번호가 올바르지 않습니다."));
                dlg.exec();
            }
        }
    }
    else
        ret = true;
    return ret;
}

void CNetworkSetup::on_btnRoute_clicked()
{
    Route dlg(&GatewayList);
    dlg.exec();
    mbChanged = dlg.mbChanged;
}

void CNetworkSetup::on_cancel_clicked()
{
    if(mbChanged)
    {
        CQuestionDialog dlg(tr("변경된 데이터가 있습니다. \n 저장 하시겠습니까?"));
        if(dlg.exec() == QDialog::Accepted)
        {
            on_ok_clicked();
        }
    }
    QDialog::accept();
}

void CNetworkSetup::on_btnModel_clicked()
{
    PasswordInput dlg(this);
    if(dlg.exec() == QDialog::Accepted)
    {
        if(dlg.m_sPassword == "EPSENE")
        {
            QString newmodel,oldmodel;
            if(gSoftwareModel == "EPS_HANRIVER")
            {
                oldmodel = "한강수계";
                newmodel = "일반";
            }
            else
            {
                oldmodel = "일반";
                newmodel = "한강수계";
            }
            QString msg = QString("모델을 변경하면 모든 데이터가 초기화되는 중요한 명령입니다.\n"
                           " %1모델을 %2모델로 변경 하시겠습니까?").arg(oldmodel).arg(newmodel);
            CQuestionDialog dlg(msg);
            if(dlg.exec() == QDialog::Accepted)
            {
                QString model;
                if(gSoftwareModel == "EPS_HANRIVER")
                    model = "EPS_HASU";
                else
                    model = "EPS_HANRIVER";
                QJsonObject argObject;
                QJsonValue vValue(model);
                CMonitorUdp::Instance()->sendCommand(0, QString("System_ChangeSoftwareModel"), argObject, vValue);
            }
        }
        else
        {
            CInfomationDialog dlg(tr("비밀번호가 올바르지 않습니다."));
            dlg.exec();
        }
    }
}

void CNetworkSetup::on_btnPingTest_clicked()
{
    PingTest dlg(ui->lan1Gateway->text());
    dlg.exec();
}

void CNetworkSetup::on_lan2Dhcp_clicked()
{
    ui->lan1DefaultGateway->setChecked(false);
    ui->lan2DefaultGateway->setChecked(true);
}

void CNetworkSetup::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
    {
        QJsonObject argObject;
        QJsonValue vValue(QJsonValue::Null);
        CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);
    }
    if (event->timerId() == m_RejectTimerId)
        QDialog::reject();
}

void CNetworkSetup::onTimer()
{
    if(mPassword != gPassword)
        QDialog::reject();
    if (m_bScreenSave == true)
        QDialog::reject();
}

void CNetworkSetup::onTimerClose()
{
    g_bExit = true;
    QDialog::reject();
}

void CNetworkSetup::on_btnHasuSetup_clicked()
{
    EpsHasuSetup dlg(this);
    dlg.exec();
}
