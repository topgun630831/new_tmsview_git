#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "history.h"
#include "ui_history.h"
#include "calendardlg.h"
#include "srsocket.h"
#include "infomationdialog.h"
#include <QDebug>

extern CSrSocket *g_pSrSocket;
extern bool g_DebugDisplay;
extern QString gSoftwareModel;
extern QString GraphicId;

int ColumnWidth[HISTORY_MAX][9] = {
    {           //Alarm
        120,     //No
        250,    //Date
        200,    //Name
        460,    //Desc
        160,    //Status
        160,    //PreStatus
        220,    //Value
        150     //AckUser
    },
    {           //Control
        120,     //No
        300,    //Date
        170,     //AckUser
        200,    //Name
        470,    //Desc
        160,    //Value
        550     //Result
    },
    {           //Event
        120,     //No
        360,    //Date
        240,    //Name
        500,    //Desc
        320,    //Value
    },
    {           //Config
        120,     //No
        240,    //Date
        240,    //User
        240,    //Kind
        160,    //Item
        480,    //Before
        480,    //After
    },
    {           //System
        100,     //No
        350,    //Date
        240,    //User
        320,    //Kind
        850,    //Message
    },
    {           //Eps
        100,    //No
        126,    //DateTime
        1200    //Msg
    },
    {           //TMS
        100,     //No
        240,    //DateTime
        1576    //Msg
    }
};
int ColumnCount[HISTORY_MAX] = {
    8, 7, 6, 7, 5, 3, 3
};

QTableWidget *HistoryTable[HISTORY_MAX];
const char *CmdTab[HISTORY_MAX] = {
    "Log_GetAlarm",
    "Log_GetControl",
    "Log_GetEvent",
    "Log_GetConfig",
    "Log_GetSystem",
    "Ext_Eps_GetLog",
    "Log_GetSystem",
};


CHistory::CHistory(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CHistory)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    ColumnInit();
 /*   if(gSoftwareModel == "EPS_HANRIVER")      // 한강수계 표준RTU
    {
        ui->btnAlarm->setVisible(false);
        ui->btnControl->setVisible(false);
        ui->btnEvent->setVisible(false);
        ui->btnConfig->setVisible(false);
        ui->btnSystem->setVisible(false);
        mIndex = 5;
    }
    else
    if(gSoftwareModel == "ORION_TMS")      // 수질 TMS
  */  {
        ui->btnAlarm->setVisible(false);
        ui->btnControl->setVisible(false);
        ui->btnEvent->setVisible(false);
        ui->btnConfig->setVisible(false);
        ui->btnSystem->setVisible(false);
        mIndex = 6;
    }
//    else
//        mIndex = 0;

//    if(gSoftwareModel != "ORION_TMS")      // 수질 TMS
 //       ui->title->setVisible(false);
    qDebug() << "history index:" << mIndex;
    ui->stackedWidget->setCurrentIndex(mIndex);
    mFromDate = QDate::currentDate();
    mToDate = mFromDate.addDays(1);
    ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));
    ui->btnToDate->setText(mToDate.toString(tr("yyyy-MM-dd")));

    for(int i = 0; i < HISTORY_MAX; i++)
    {
        mPage[i] = -1;
        mTotalPage[i] = 0;
    }
}

CHistory::~CHistory()
{
    delete ui;
    for(int i = 0; i < HISTORY_MAX; i++)
        foreach (const HISTORY_TAB * history, mHistoryList[i])
            delete history;
}

void CHistory::paintEvent(QPaintEvent *)
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


void CHistory::ColumnInit()
{
    HistoryTable[ALARM] = ui->tableAlarm;
    HistoryTable[CONTROL] = ui->tableControl;
    HistoryTable[EVENT] = ui->tableEvent;
    HistoryTable[CONFIG] = ui->tableConfig;
    HistoryTable[SYSTEM] = ui->tableSystem;
    HistoryTable[EPS] = ui->tableEps;
    HistoryTable[SYSTEM_TMS] = ui->tableTms;
    for(int i = 0; i < HISTORY_MAX; i++)
    {
        QTableWidget *table = HistoryTable[i];
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
        for(int c = 0; c < ColumnCount[i]; c++)
            table->horizontalHeader()->resizeSection(c, ColumnWidth[i][c]);
    }
}

void CHistory::onRead(QString& cmd, QJsonObject& jobject)
{
    if(g_DebugDisplay)
        qDebug() << "CHistory::onRead : " << cmd;

    QJsonArray array = jobject["Result"].toArray();
    int index;
    if(array.size() == 0)
    {
        CInfomationDialog dlg("자료가 없습니다.");
        dlg.exec();
        return;
    }
    if(cmd == "Log_GetAlarm")
    {
        index = ALARM;
        QString str;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
            str = str.mid(5, 14);
            item->Item[0] = str;
            item->Item[1] = itemObject["Tag"].toString();
            item->Item[2] = itemObject["Desc"].toString();
            item->Item[3] = itemObject["Status"].toString();
            item->Item[4] = itemObject["OldStatus"].toString();
            item->Item[5] = itemObject["Value"].toString();
            item->Item[6] = itemObject["AckUser"].toString();
            mHistoryList[ALARM].append(item);
        }
    }
    else
    if(cmd == "Log_GetControl")
    {
        index = CONTROL;
        QString str;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
            str = str.mid(5, 14);
            item->Item[0] = str;
            item->Item[1] = itemObject["User"].toString();
            item->Item[2] = itemObject["Tag"].toString();
            item->Item[3] = itemObject["Desc"].toString();
            item->Item[4] = itemObject["Value"].toString();
            item->Item[5] = itemObject["Result"].toString();
            mHistoryList[CONTROL].append(item);
        }
    }
    else
    if(cmd == "Log_GetEvent")
    {
        index = EVENT;
        QString str;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
            str = str.mid(5, 14);
            item->Item[0] = str;
            item->Item[1] = itemObject["Tag"].toString();
            item->Item[2] = itemObject["Desc"].toString();
            item->Item[3] = itemObject["Value"].toString();
            mHistoryList[EVENT].append(item);
        }
    }
    else
    if(cmd == "Log_GetConfig")
    {
        index = CONFIG;
        QString str;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
            str = str.mid(5, 14);
            item->Item[0] = str;
            item->Item[1] = itemObject["User"].toString();
            item->Item[2] = itemObject["Kind"].toString();
            item->Item[3] = itemObject["Item"].toString();
            item->Item[4] = itemObject["Pre"].toString();
            item->Item[5] = itemObject["Post"].toString();
            mHistoryList[CONFIG].append(item);
        }
    }
    else
    if(cmd == "Log_GetSystem")
    {
        if(mIndex == 6)
            index = SYSTEM_TMS;
        else
            index = SYSTEM;
        QString str;
//        qDebug() << "cmd == Log_GetSystem:" << array.size();
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
            str = str.mid(5, 14);
            item->Item[0] = str;
            if(mIndex == 6)
            {
                item->Item[1] = itemObject["Msg"].toString();
                mHistoryList[SYSTEM_TMS].append(item);
            }
            else
            {
                item->Item[1] = itemObject["User"].toString();
                item->Item[2] = itemObject["Kind"].toString();
                item->Item[3] = itemObject["Msg"].toString();
                mHistoryList[SYSTEM].append(item);
            }
        }
    }
    else
    if(cmd == "Ext_Eps_GetLog")
    {
        index = EPS;
        QString str;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            HISTORY_TAB *item = new HISTORY_TAB;
            QString str = itemObject["Date"].toString();
//            str = str.mid(5, 14);
            item->Item[0] = str;
            item->Item[1] = itemObject["Msg"].toString();
            mHistoryList[EPS].append(item);
        }
    }
    else
        return;
    PageInit(index, array.size());
    DataDisplay(index);
}

void CHistory::PageInit(int index, int total)
{
    mTotalPoint[index] = total;
    mStartPoint[index] = mPage[index] = 0;
    mTotalPage[index] = mTotalPoint[index] / MAX_ROW;
    if(mTotalPoint[index] % MAX_ROW)
        mTotalPage[index]++;
}

void CHistory::DataDisplay(int index)
{
    QTableWidget *table;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    table = HistoryTable[index];
    table->clearContents();
    for(int row = mStartPoint[index]; row < mStartPoint[index] + MAX_ROW; row++, cnt++)
    {
        if(row >= mHistoryList[index].size())
            break;
        str = QString("%1").arg(row+1);
        item = new QTableWidgetItem(str);
        table->setItem(cnt, 0, item);
        for(int col = 1; col < ColumnCount[index]; col++)
        {
            item = new QTableWidgetItem(mHistoryList[index][row]->Item[col-1]);
            table->setItem(cnt, col, item);
        }
    }
    PageDisplay(index);
}

void CHistory::PageDisplay(int index)
{
    QString str = QString("Page(%1/%2)").arg(mPage[index]+1).arg(mTotalPage[index]);
    ui->labelPage->setText(str);
}

void CHistory::on_btnFirst_clicked()
{
    if(mPage[mIndex] > 0)
    {
        mPage[mIndex] = 0;
        mStartPoint[mIndex] = 0;
        DataDisplay(mIndex);
    }
}

void CHistory::on_btnPre_clicked()
{
    if(mPage[mIndex] > 0)
    {
        mPage[mIndex]--;
        mStartPoint[mIndex] = mPage[mIndex] * MAX_ROW;
        DataDisplay(mIndex);
    }
}

void CHistory::on_btnNext_clicked()
{
    if(mPage[mIndex] != -1 && mPage[mIndex] != mTotalPage[mIndex]-1)
    {
        mPage[mIndex]++;
        mStartPoint[mIndex] = mPage[mIndex] * MAX_ROW;
        DataDisplay(mIndex);
    }
}

void CHistory::on_btnLast_clicked()
{
    if(mPage[mIndex] != -1 && mPage[mIndex] != mTotalPage[mIndex]-1)
    {
        mPage[mIndex] = mTotalPage[mIndex]-1;
        mStartPoint[mIndex] = mPage[mIndex] * MAX_ROW;
        DataDisplay(mIndex);
    }
}

void CHistory::on_btnAlarm_clicked()
{
    mIndex = 0;
    ui->stackedWidget->setCurrentIndex(mIndex);
    PageDisplay(mIndex);
}

void CHistory::on_btnControl_clicked()
{
    mIndex = 1;
    ui->stackedWidget->setCurrentIndex(mIndex);
    PageDisplay(mIndex);
}

void CHistory::on_btnEvent_clicked()
{
    mIndex = 2;
    ui->stackedWidget->setCurrentIndex(mIndex);
    PageDisplay(mIndex);
}

void CHistory::on_btnConfig_clicked()
{
    mIndex = 3;
    ui->stackedWidget->setCurrentIndex(mIndex);
    PageDisplay(mIndex);
}

void CHistory::on_btnSystem_clicked()
{
    mIndex = 4;
    ui->stackedWidget->setCurrentIndex(mIndex);
    PageDisplay(mIndex);
}

void CHistory::on_btnInq_clicked()
{
    HistoryTable[mIndex]->clearContents();
    foreach (const HISTORY_TAB * history, mHistoryList[mIndex])
        delete history;
    mHistoryList[mIndex].clear();

    mStartPoint[mIndex] = mPage[mIndex] = -1;
    mTotalPage[mIndex] = mTotalPoint[mIndex] = 0;
    PageDisplay(mIndex);

    QString cmd = CmdTab[mIndex];
    SendCommand(cmd);
}

void CHistory::on_btnFromDate_clicked()
{
    CalendarDlg dlg( mFromDate, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mFromDate = dlg.getDate();
        ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));
    }
}

void CHistory::on_btnToDate_clicked()
{
    CalendarDlg dlg( mToDate, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mToDate = dlg.getDate();
        ui->btnToDate->setText(mToDate.toString(tr("yyyy-MM-dd")));
    }
}

void CHistory::SendCommand(QString cmd)
{
    if(mFromDate == mToDate)
    {
        CInfomationDialog dlg("시작날짜와 종료날짜가 동일입니다.");
        dlg.exec();
        return;
    }
    int days = mFromDate.daysTo(mToDate);
    if(days < 0) // || days > 31)
    {
        CInfomationDialog dlg("종료날짜가 시작날짜보다 과거입니다.");
        dlg.exec();
        return;
    }
    QString from = mFromDate.toString("yyyy-MM-dd ");
    from += QString("%1:00:00").arg(ui->fromHour->text().toInt(),2,10,QChar('0'));
    QJsonObject argObject;
    argObject.insert(QString("StartTime"),QJsonValue(from));
    argObject.insert(QString("EndTime"),QJsonValue(mToDate.toString("yyyy-MM-dd 00:00:00")));
    argObject.insert(QString("Offset"),QJsonValue(0));
    argObject.insert(QString("Limit"),QJsonValue(1000));
    QJsonValue argValue(argObject);

    g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
}
