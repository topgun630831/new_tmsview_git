#include "monview.h"
#include "monitorudp.h"
#include "ui_monview.h"

extern bool    m_bScreenSave;

MonView::MonView(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::MonView)
{
    ui->setupUi(this);

    ui->listWidget->setStyleSheet("QListWidget { "
                               "margin:20px;"
                               "padding:20px;}");
    ui->listWidget->setSpacing(5);
    m_Pause = false;
    StartMsgIndex = -500;

    QJsonObject argObject;
    QJsonValue vValue(-500);
    CMonitorUdp::Instance()->sendCommand(this, QString("Msg_Get"), argObject, vValue);
    startTimer(1000);
}

MonView::~MonView()
{
    delete ui;
}


void MonView::on_update_clicked()
{
    ui->listWidget->clear();
    QJsonObject argObject;
    QJsonValue vValue(-500);
    CMonitorUdp::Instance()->sendCommand(this, QString("Msg_Get"), argObject, vValue);
}

void MonView::onRead(QString& cmd, QJsonObject& robject)
{
    qDebug() << "MonView::onRead : " << cmd;
    QJsonObject jobject = robject["Result"].toObject();
    if(cmd == "Msg_Get")
    {
        QJsonArray array = jobject["Msgs"].toArray();
        qDebug() << "size : " << array.size();
        if(array.size() != 0)
        {
            StartMsgIndex = jobject["StartMsgIndex"].toInt();
            foreach (const QJsonValue& value, array)
            {
                QString str = value.toString();
                ui->listWidget->addItem(str);
            }
            ui->listWidget->scrollToItem(ui->listWidget->item(ui->listWidget->count() - 1), QAbstractItemView::PositionAtBottom);
        }
    }
}

void MonView::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
    if(m_Pause == true)
        return;
    QJsonObject argObject;
    QJsonValue vValue(StartMsgIndex);
    CMonitorUdp::Instance()->sendCommand(this, QString("Msg_Get"), argObject, vValue);
}

void MonView::on_pause_clicked(bool checked)
{
    m_Pause = checked;
}
