#include "testsetup.h"
#include "ui_testsetup.h"
#include "monitorudp.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "monitorudp.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
extern bool g_DebugDisplay;

extern QSqlDatabase gDb;
TestSetup::TestSetup(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TestSetup)
{
    ui->setupUi(this);
    ui->lan2NotUsed->setChecked(true);
    lan2Used = false;

    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);
    ui->stackedWidget->setCurrentIndex(0);
    QString dbname =  "/data/project.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        if(g_DebugDisplay)
            qDebug() << "QSqlError : " << err.databaseText();
    }
    QSqlQuery query("SELECT SiteCode FROM Ext_Eps");
    if(query.next())
        ui->siteCode->setText(query.value(0).toString());
    QSqlQuery query2("SELECT Ip FROM Port WHERE Name='Ts'");
    if(query2.next())
        ui->serverIp->setText(query2.value(0).toString());
    gDb.close();
}

TestSetup::~TestSetup()
{
    delete ui;
}

void TestSetup::on_save_2_clicked()
{
    bool lan1Dhcp = ui->lan1Dhcp->isChecked();
    bool lan2Dhcp = ui->lan2Dhcp->isChecked();

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

    CQuestionDialog dlg(tr("저장 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
        CMonitorUdp::Instance()->sendCommand(0,  QString("Network_Set"), argObject, vObject);
}

bool TestSetup::IpValidate(QString &ip, QString msg)
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

void TestSetup::on_lan1DefaultGateway_clicked()
{
    ui->lan2DefaultGateway->setChecked(false);
}

void TestSetup::on_lan2DefaultGateway_clicked()
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


void TestSetup::onRead(QString& cmd, QJsonObject& robject)
{
    QJsonObject jobject = robject["Result"].toObject();
    if(cmd == "Network_Get")
    {
//        ui->productName->setText(jobject["Model"].toString());
        ui->serialNo->setText(QString("%1").arg((int)jobject["SerialNumber"].toDouble(),10,10,QChar('0')));
        if(jobject["Dns"].isNull() == false)
            ui->dnsPrimary->setText(jobject["Dns"].toString());
        if(jobject["Dns2"].isNull() == false)
            ui->dnsSecondary->setText(jobject["Dns2"].toString());
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
                    {
                        ui->lan2Static->setChecked(true);
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
    }
}

void TestSetup::on_server_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void TestSetup::on_network_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void TestSetup::on_serverSave_clicked()
{
    CQuestionDialog dlg("변경된 내용을 적용하시겠습니까?\n 적용하면 프로그램이 재기동합니다.");
    if(dlg.exec() == QDialog::Accepted)
    {
        QString dbname =  "/data/project.db";
        gDb.setDatabaseName(dbname);
        gDb.open();

        QString str = QString("Update Ext_Eps SET SiteCode='%1'").arg(ui->siteCode->text());
        QSqlQuery query(str);
        query.exec();
        str = QString("Update Port SET Ip='%1' Where Name='Ts'").arg(ui->serverIp->text());
        QSqlQuery query2(str);
        query2.exec();

        gDb.close();
        RunStart(false);
        RunStart(true);
        QDialog::accept();
    }
}

void TestSetup::RunStart(bool start)
{
    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);

    QString proc = "Sun";
    bool SaveStatus=false;
    argObject.insert(QString("Proc"),QJsonValue(proc));
    argObject.insert(QString("Enabled"),QJsonValue(start));
    argObject.insert(QString("SaveStatus"),QJsonValue(SaveStatus));

    CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Set"), argObject, vObject);
}
