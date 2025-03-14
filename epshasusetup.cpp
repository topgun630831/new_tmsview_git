#include "epshasusetup.h"
#include "ui_epshasusetup.h"
#include "monitorudp.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "epsene.h"
extern QString gUseAck;

extern bool g_DebugDisplay;
extern QSqlDatabase gDb;
extern bool    m_bScreenSave;
extern EpsEnE *g_pEps;

EpsHasuSetup::EpsHasuSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpsHasuSetup)
{
    ui->setupUi(this);

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
    QSqlQuery query2("SELECT Ip,Port FROM Port WHERE Name='Ts'");
    if(query2.next())
    {
        ui->serverIp->setText(query2.value(0).toString());
        ui->serverPort->setText(query2.value(1).toString());
    }
    gDb.close();


    dbname =  "/tmp/share.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        if(g_DebugDisplay)
            qDebug() << "QSqlError : " << err.databaseText();
    }
    QSqlQuery query3("SELECT Item, Value FROM Share WHERE Item='EPS_UseAck'");
    if(query3.next())
    {
        QString item = query3.value(0).toString();
        if(item == "EPS_UseAck")
        {
            double val = query3.value(1).toDouble();
            if(val == 1)
                gUseAck = "1";
            else
                gUseAck = "0";
        }
    }
    gDb.close();


    QString text = "Ack 사용안함";
    if(gUseAck == "1")
        text = "Ack 사용함";
    ui->useAck->setText(text);
    startTimer(1000);
}

EpsHasuSetup::~EpsHasuSetup()
{
    delete ui;
}

void EpsHasuSetup::on_close_clicked()
{
    QDialog::accept();
}

void EpsHasuSetup::on_serverSave_clicked()
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
        str = QString("Update Port SET Ip='%1',Port='%2',Kind='TcpClient'  Where Name='Ts'").arg(ui->serverIp->text()).arg(ui->serverPort->text());
        QSqlQuery query2(str);
        query2.exec();

        gDb.close();
        RunStart(false);
        RunStart(true);
        QDialog::accept();
    }
}

void EpsHasuSetup::RunStart(bool start)
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

void EpsHasuSetup::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}

void EpsHasuSetup::on_btnAck_clicked()
{
    CQuestionDialog dlg("Ack 사용을 변경하시겠습니까?");
    QString text;
    if(gUseAck == "1")
        text = "Ack 사용함";
    if(dlg.exec() == QDialog::Accepted)
    {
        bool bAck;
        if(gUseAck == "0")
        {
            gUseAck = "1";
            text = "Ack 사용함";
        }
        else
        {
            gUseAck = "0";
            text = "Ack 사용안함";
        }
        ui->useAck->setText(text);
        g_pEps->ShareDataSetSend();
    }
}
