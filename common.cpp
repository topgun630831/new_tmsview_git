#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include <QScrollBar>
#include <QTextEdit>
#include "common.h"
#include "ui_common.h"
#include "commonselect.h"
#include "srsocket.h"

extern CSrSocket *g_pSrSocket;
extern QString GraphicId;
CComMon *g_CommMon;

CComMon::CComMon(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CComMon)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    g_CommMon = this;
    m_bHexDisp = false;
#if 0
    QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical {
        width: 50px; /* Set the desired width */
        height: 50px; /* Set the desired height */
    /*    border: 1px solid black;
        background: black;*/
    }
    QScrollBar:vertical { width: 50px;}
    QScrollBar:horizontal { height: 50px;}
#endif
    QTableWidget *table = ui->tableFrame;
    QScrollBar *scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 50px;height: 50px;}QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { width: 50px;  height: 50px;}");
    scroll  = table->horizontalScrollBar();
    scroll->setStyleSheet("QScrollBar:horizontal { height: 50px;}");
    table->horizontalHeader()->resizeSection(0, 100);
    table->horizontalHeader()->resizeSection(1, 140);
    table->horizontalHeader()->resizeSection(2, 80);
    table->horizontalHeader()->resizeSection(3, 1520);
    //table->setWordWrap(true);

    g_pSrSocket->sendCommand((QWidget*)this, QString("Monitor_List"), GraphicId);
    mbStarted = false;
    m_nHexStart = 0x20;
    connect(this, SIGNAL(sigSelect()), this, SLOT(on_btnSelect_clicked()));
}

CComMon::~CComMon()
{
    g_CommMon = 0;
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

void CComMon::paintEvent(QPaintEvent *)
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

void CComMon::on_btnSelect_clicked()
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
        SendCommand("Monitor_Select");
        ui->btnMonStart->setText(tr("감시 정지"));
        InsertRow(QString(""), QString(""), QString(""), QString(tr("감시 시작")));
        mbStarted = true;
        ui->btnMonStart->setChecked(true);

/*        if(mbStarted == true)
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
            mbStarted = false;
        }*/
    }
}

void CComMon::onRead(QString& cmd, QJsonObject& jobject)
{
    if(cmd == "Monitor_List")
    {
        QJsonArray array = jobject["Result"].toArray();
        foreach (const QJsonValue& value, array)
        {
            QJsonObject commObj = value.toObject();
            COMM_SELECT *com = new COMM_SELECT;
            com->Driver = commObj["Title"].toString();
            com->Device = commObj["SubTitle"].toString();
            com->Port = commObj["Port"].toString();
            mCommList.append(com);
            if(mPortList.contains(com->Port) == false)
            {
                mPortList.append(com->Port);
            }
        }
        emit sigSelect();
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
//        time = time.mid(6,time.length()-6);  // 10번째이후 복사
        time = time.mid(3,5);  // mm:ss
        QString kind = KindStr(obj["Kind"].toString());
        QString sFrame = obj["Frame"].toString();
        QString sDesc = obj["FrameDesc"].toString();
        bool bFrame = (sFrame.length() == 0);
        bool bDesc = (sDesc.length() == 0);
        bool bNoSpace = false;
        int nNoSpaceCount = 0;
        QByteArray aFrame;
        aFrame.append(sFrame);
        aFrame = aFrame.fromBase64( aFrame);
        sFrame = "";
        for(int i = 0; i < aFrame.size(); i++)
        {
            char ch = aFrame[i];
            if(m_bHexDisp == false && m_nHexStart == 0x21 && ch == 0x20)
            {
                sFrame.append("^");
            }
            else
            if(m_bHexDisp || (ch < m_nHexStart || ch > 0x7e))   // ~ 까지는 문자로 표현
            {
                QString str = QString("<%1>").arg((unsigned char)ch,2,16,QChar('0'));
                sFrame += str;
            }
            else
                sFrame.append(ch);
            if(ch == 0x20)
                nNoSpaceCount = 0;
            else
            if(++nNoSpaceCount > 90)
                bNoSpace = true;

        }
/*         sFrame += "(";
        sFrame += obj["FrameDesc"].toString();
        sFrame +=  "}";
        qDebug() << "sFrame:" << sFrame << "\n   Desc:" << obj["FrameDesc"].toString();
 */
        if(bFrame)
            sFrame = sDesc;
        if(bFrame == false && bDesc == false)
        {
            sFrame += "\n[";
            sFrame += sDesc;
            sFrame += "]";
        }
        if(bNoSpace || m_bHexDisp || m_nHexStart == 0x21)
        {
            int len = sFrame.length();
            qDebug() << "len:" << len;
            if(len > 100)
            {
                int cnt = len / 100;
                for(int i = 0; i < cnt; i++)
                {
                    sFrame.insert((i+1)*100, tr("\n"));
                }
            }
        }
//            InsertRow(QString(""), QString("부가정보"), QString(""), sDesc);
        QString num = QString("%1").arg(aFrame.length(),5,10);
        InsertRow(time, kind, num, sFrame);
        int cnt = ui->tableFrame->rowCount();
        if(cnt > 300)
        {
            while((cnt = ui->tableFrame->rowCount()) > 300)
                ui->tableFrame->removeRow(cnt-1);
        }
    }
}

void CComMon::on_btnClear_clicked()
{
    while(ui->tableFrame->rowCount())
        ui->tableFrame->removeRow(0);
}

void CComMon::SendCommand(const char *command)
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

void CComMon::on_btnMonStart_clicked()
{
    if(mbStarted == false)
    {
        SendCommand("Monitor_Select");
        ui->btnMonStart->setText(tr("감시 정지"));
        InsertRow(QString(""), QString(""), QString(""), QString(tr("감시 시작")));
        mbStarted = true;
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
        InsertRow(QString(""), QString(""), QString(""), QString(tr("감시 정지")));
        mbStarted = false;
    }
}

void CComMon::on_btnReset_clicked()
{
    SendCommand("Monitor_Reset");
}

void CComMon::InsertRow(QString s1, QString s2, QString s3, QString s4)
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
    table->resizeRowsToContents();
}

QString CComMon::KindStr(QString kind)
{
    QString str;
    if(kind == "ReadSend")
        str = "Send";
    else
    if(kind == "ReadRecv")
        str = "Recv";
    else
    if(kind == "WriteSend")
        str = "Send";
    else
    if(kind == "WriteRecv ")
        str = "Recv";
    else
    if(kind == "Timeout")
        str = "Timeout";
    else
    if(kind == "Closed")
        str = "Closed";
    else
    if(kind == "ConnectFail")
        str = "Fail";
    else
    if(kind == "SocketError")
        str = "Error(S)";
    else
    if(kind == "FrameError")
        str = "Error(F)";
    else
    if(kind == "Connect ")
        str = "Connect";
    else
    if(kind == "Disconnect ")
        str = "Didconnect";
    else
    if(kind == "Event")
        str = "Event";
    else
        str = kind;
    return str;
}

void CComMon::on_checkHex_clicked()
{
    m_bHexDisp = ui->checkHex->isChecked();
}

void CComMon::on_checkSpace_clicked()
{
    if(ui->checkSpace->isChecked())
        m_nHexStart = 0x21;
    else
        m_nHexStart = 0x20;
}
