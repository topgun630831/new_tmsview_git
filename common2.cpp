#include <QScrollBar>
#include "common2.h"
#include "ui_common2.h"
#include "commonselect.h"
#include "srsocket.h"

extern CSrSocket *g_pSrSocket;
CComMon2 *g_CommMon2;

CComMon2::CComMon2(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CComMon2)
{
    ui->setupUi(this);

    g_CommMon2 = this;
    m_bHexDisp = false;

    QTableWidget *table = ui->tableFrame;
    QScrollBar *scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;}");

    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->horizontalHeader()->resizeSection(0, 60);
    table->horizontalHeader()->resizeSection(1, 60);
    table->horizontalHeader()->resizeSection(2, 80);
    table->horizontalHeader()->resizeSection(3, 730);

    g_pSrSocket->sendCommand((QWidget*)this, QString("Monitor_List"), QString(""));
}

CComMon2::~CComMon2()
{
    g_CommMon2 = 0;
    /*
    QJsonObject tmpObject;
    QJsonValue vValue(QJsonValue::Null);
    QString cmd = "Monitor_Reject";
    tmpObject.insert(QString("Args"),vValue);
    tmpObject.insert(QString("Command"),QJsonValue(cmd));
    */
//    g_pSrSocket->sendCommand(0, cmd, tmpObject);
    QString cmd = "Monitor_Reject";
    g_pSrSocket->sendCommand(0, cmd, QString(""));
    delete ui;
    foreach (const COMM_SELECT * com, mCommList)
        delete com;
}

void CComMon2::on_btnSelect_clicked()
{
    CComMonSelect dlg(mPortList, mCommList);
    if(dlg.exec() == QDialog::Accepted)
    {
        mDriver = dlg.mDriver;
        mDevice = dlg.mDevice;
        mPort = dlg.mPort;
        mbPort = dlg.mbPort;
        QString txt;
        if(mbPort)
            txt = mPort;
        else
            txt = QString("%1:%2[%3]").arg(mDriver).arg(mDevice).arg(mPort);
        ui->label->setText(txt);
        ui->btnMonStart->setEnabled(true);

        ui->btnReset->setEnabled(true);
    }
}

void CComMon2::onRead(QString& cmd, QJsonObject& jobject)
{
//    qDebug() << "CComMon2::onRead : " << cmd;
    if(cmd == "Monitor_List")
    {
        QJsonArray array = jobject["Result"].toArray();
//        qDebug() << "size : " << array.size();
        foreach (const QJsonValue& value, array)
        {
            QJsonObject commObj = value.toObject();
            COMM_SELECT *com = new COMM_SELECT;
            com->Driver = commObj["Title"].toString();
            com->Device = commObj["SubTitle"].toString();
            com->Port = commObj["Port"].toString();
            mCommList.append(com);
//            qDebug() << "mCommList : " << com->Driver << "," << com->Device << "," << com->Port;
            if(mPortList.contains(com->Port) == false)
            {
                mPortList.append(com->Port);
//                qDebug() << "Add PortList : " << com->Port;
            }
        }
    }
    else
    if(cmd == "@Monitor_Event")
    {
        QJsonObject obj = jobject["Result"].toObject();

        QJsonObject stat = obj["Stat"].toObject();

        ui->readCnt->display(stat["R"].toDouble());
        ui->writeCnt->display(stat["W"].toDouble());
        ui->readError->display(stat["RE"].toDouble());
        ui->writeError->display(stat["WE"].toDouble());
        ui->continuError->display(stat["CE"].toDouble());
        ui->connectCount->display(stat["C"].toDouble());
        ui->totalRead->display(stat["RB"].toDouble());
        ui->totalWrite->display(stat["WR"].toDouble());

        QString time = obj["Time"].toString();
        time = time.mid(6,time.length()-6);  // 10번째이후 복사
        QString kind = KindStr(obj["Kind"].toString());
        QString sFrame = obj["Frame"].toString();
        QByteArray aFrame;
        aFrame.append(sFrame);
        aFrame = aFrame.fromBase64( aFrame);
        sFrame = "";
        foreach (const char ch, aFrame)
        {
            if(m_bHexDisp || (ch < 0x20 || ch > 'z'))
            {
                QString str = QString("<%1>").arg((unsigned char)ch,2,16,QChar('0'));
                sFrame += str;
            }
            else
                sFrame.append(ch);
        }
        sFrame += "\n(";
        sFrame += obj["FrameDesc"].toString();
        sFrame +=  "}";
        QString num = QString("%1").arg(aFrame.length(),5,10);
        InsertRow(time, kind, num, sFrame);
        int cnt = ui->tableFrame->rowCount();
        if(cnt > 100)
        {
            for(int i = 0; i < cnt-100; i++)
                ui->tableFrame->removeRow(100);
        }
    }
}

void CComMon2::on_btnHexDisp_clicked()
{
    m_bHexDisp = ui->btnHexDisp->isChecked();
}

void CComMon2::on_btnClear_clicked()
{
    while(ui->tableFrame->rowCount())
        ui->tableFrame->removeRow(0);
}

void CComMon2::SendCommand(const char *command)
{
    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    QString cmd = command;
    if(mbPort == false)
    {
        argObject.insert(QString("Title"),mDriver);
        argObject.insert(QString("SubTitle"),mDevice);
        argObject.insert(QString("Port"),mPort);
    }
    else
    {
        argObject.insert(QString("Port"),mPort);
        argObject.insert(QString("Title"),QJsonValue(vValue));
        argObject.insert(QString("SubTitle"),QJsonValue(vValue));
    }
    QJsonValue argValue(argObject);
    g_pSrSocket->sendCommand(0, cmd, QString(""), argValue);
}

void CComMon2::on_btnMonStart_clicked()
{
    if(ui->btnMonStart->isChecked() == true)
    {
        SendCommand("Monitor_Select");
        ui->btnMonStart->setText(tr("감시 정지"));
        InsertRow(QString(""), QString(""), QString(""), QString(tr("감시 시작")));
    }
    else
    {
        QJsonObject tmpObject;
        QJsonValue vValue(QJsonValue::Null);
        QString cmd = "Monitor_Reject";
        tmpObject.insert(QString("Command"),QJsonValue(cmd));
        tmpObject.insert(QString("Args"),vValue);
        g_pSrSocket->sendCommand((QWidget*)this, cmd, QString(""));
        ui->btnMonStart->setText(tr("감시 시작"));
        ui->btnMonStart->setChecked(false);
        InsertRow(QString(""), QString(""), QString(""), QString(tr("감시 정지")));
    }
}

void CComMon2::on_btnReset_clicked()
{
    SendCommand("Monitor_Reset");
}

void CComMon2::InsertRow(QString s1, QString s2, QString s3, QString s4)
{
    QTableWidget *table = ui->tableFrame;
    table->insertRow(0);
    QTableWidgetItem *item = new QTableWidgetItem(s1);
    table->setItem(0, 0, item);
    item = new QTableWidgetItem(s2);
    table->setItem(0, 1, item);
    item = new QTableWidgetItem(s3);
    table->setItem(0, 2, item);

    item = new QTableWidgetItem(s4);
    table->setItem(0, 3, item);
/*
    QTextEdit *edit = new QTextEdit();
    edit->setText(s4);
    ui->tableFrame->setCellWidget(0, 3, edit);*/
}

QString CComMon2::KindStr(QString kind)
{
    QString str;
    if(kind == "ReadSend")
        str = "RS";
    else
    if(kind == "ReadRecv")
        str = "RR";
    else
    if(kind == "WriteSend")
        str = "WR";
    else
    if(kind == "WriteRecv ")
        str = "WR";
    else
    if(kind == "Timeout")
        str = "T";
    else
    if(kind == "Closed")
        str = "C";
    else
    if(kind == "ConnectFail")
        str = "CF";
    else
    if(kind == "SocketError")
        str = "SE";
    else
    if(kind == "FrameError")
        str = "FE";
    else
    if(kind == "Connect ")
        str = "C";
    else
    if(kind == "Disconnect ")
        str = "D";
    else
    if(kind == "Event")
        str = "E";
    else
        str = kind;
    return str;
}
