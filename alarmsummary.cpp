#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "alarmsummary.h"
#include "ui_alarmsummary.h"
#include "srsocket.h"
#include <QDebug>

extern CSrSocket *g_pSrSocket;
CAlarmSummary *g_AlarmSum;
CAlarmSummary::CAlarmSummary(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CAlarmSummary)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    g_AlarmSum = this;
    QTableWidget *table = ui->tableAlarmSummary;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->horizontalHeader()->resizeSection(0, 100);    // Date
    table->horizontalHeader()->resizeSection(1, 120);   // Name
    table->horizontalHeader()->resizeSection(2, 240);   // Desc
    table->horizontalHeader()->resizeSection(3, 120);   // Type
    table->horizontalHeader()->resizeSection(4, 100);   // Status
    table->horizontalHeader()->resizeSection(5, 100);   // PreStatus
    table->horizontalHeader()->resizeSection(6, 120);   // Value
    table->horizontalHeader()->resizeSection(7, 100);   // AckUser

    g_pSrSocket->sendCommand((QWidget*)this, QString("Alarm_Get"), QString(""));
    mPage = -1;
    mTotalPage = 0;
}

CAlarmSummary::~CAlarmSummary()
{
    g_AlarmSum = 0;
    delete ui;
    foreach (const ALARM_SUM_TAB * point, mAlarmSummaryList)
        delete point;
    mAlarmSummaryList.clear();
}

void CAlarmSummary::paintEvent(QPaintEvent *)
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


void CAlarmSummary::onRead(QString& cmd, QJsonObject& jobject)
{
//    qDebug() << "CAlarmSummary::onRead : " << cmd;
    QJsonArray array = jobject["Result"].toArray();
//    qDebug() << "size : " << array.size();
    if(cmd == "Alarm_Get")
    {
        QString str;
        mTotalPoint = array.size();
        mStartPoint = mPage = 0;
        mTotalPage = mTotalPoint / 15;
        if(mTotalPoint % 15)
            mTotalPage++;
        PageDisp();

        foreach (const ALARM_SUM_TAB * point, mAlarmSummaryList)
            delete point;
        mAlarmSummaryList.clear();

        foreach (const QJsonValue& value, array)
        {
            QJsonObject pointObject = value.toObject();
            ALARM_SUM_TAB *point = new ALARM_SUM_TAB;
            QString str = pointObject["Date"].toString();
            str = str.mid(11, 8);
            point->Date = str;
            point->Name = pointObject["Tag"].toString();
            point->Desc = pointObject["Desc"].toString();
            point->Type = pointObject["Type"].toString();
            point->Status = pointObject["Status"].toString();
            point->PreStatus = pointObject["OldStatus"].toString();
            point->Value = pointObject["Value"].toString();
            point->AckUser = pointObject["AckUser"].toString();
            mAlarmSummaryList.append(point);
        }
        SumDisp();
    }
    else
    if(cmd == "@AlarmEvent")
    {
        QJsonObject value = jobject["Result"].toObject();
        ALARM_SUM_TAB *point = new ALARM_SUM_TAB;
        QString str = value["Date"].toString();
        str = str.mid(11, 8);
        point->Date = str;
        point->Name = value["Tag"].toString();
        point->Desc = value["Desc"].toString();
        point->Type = value["Type"].toString();
        point->Status = value["Status"].toString();
        point->PreStatus = value["OldStatus"].toString();
        point->Value = value["Value"].toString();
        point->AckUser = value["AckUser"].toString();
        bool bExist = false;
        foreach (ALARM_SUM_TAB * sum, mAlarmSummaryList)
        {
            if(sum->Name == point->Name)
            {
               bExist = true;
               sum->Date = point->Date;
               sum->Name = point->Name;
               sum->Desc = point->Desc;
               sum->Type = point->Type;
               sum->Status = point->Status;
               sum->PreStatus = point->PreStatus;
               sum->AckUser = point->AckUser;
               delete point;
               break;
            }
        }
        if(bExist == false)
        {
            mAlarmSummaryList.append(point);
            mTotalPoint = mAlarmSummaryList.size();
            mTotalPage = mTotalPoint / 15;
            if(mTotalPoint % 15)
                mTotalPage++;
            PageDisp();
        }
        SumDisp();
    }
}

void CAlarmSummary::SumDisp()
{
    QTableWidget *table = ui->tableAlarmSummary;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    for(int row = mStartPoint; row < mStartPoint + 15; row++, cnt++)
    {
        if(row >= mAlarmSummaryList.size())
            break;
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Date);
        table->setItem(cnt, 0, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Name);
        table->setItem(cnt, 1, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Desc);
        table->setItem(cnt, 2, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Type);
        table->setItem(cnt, 3, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Status);
        table->setItem(cnt, 4, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->PreStatus);
        table->setItem(cnt, 5, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->Value);
        item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        table->setItem(cnt, 6, item);
        item = new QTableWidgetItem(mAlarmSummaryList[row]->AckUser);
        table->setItem(cnt, 7, item);
    }
    str = "";
    if(cnt < 15)
    {
        for(int i = cnt; i < 15; i++)
            for(int j = 0; j < 5; j++)
            {
                item = new QTableWidgetItem(str);
                table->setItem(i, j, item);
            }
    }
}

void CAlarmSummary::timerEvent(QTimerEvent *)
{
}

void CAlarmSummary::on_btnFirst_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    if(mPage != 0)
    {
        mPage = 0;
        mStartPoint = 0;
        SumDisp();
        PageDisp();
    }
}

void CAlarmSummary::on_btnPre_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    if(mPage != 0)
    {
        mPage--;
        mStartPoint = mPage * 15;
        SumDisp();
        PageDisp();
    }
}

void CAlarmSummary::on_btnNext_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    if(mTotalPage > 1 && mPage != mTotalPage-1)
    {
        mPage++;
        mStartPoint = mPage * 15;
        SumDisp();
        PageDisp();
    }
}

void CAlarmSummary::on_btnLast_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    if(mTotalPage > 1 && mPage != mTotalPage-1)
    {
        mPage = mTotalPage-1;
        mStartPoint = mPage * 15;
        SumDisp();
        PageDisp();
    }
}

void CAlarmSummary::PageDisp()
{
    QString str = QString("Page(%1/%2)").arg(mPage+1).arg(mTotalPage);
    ui->labelPage->setText(str);
}

void CAlarmSummary::on_btnAck_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    QJsonArray array;
    QList<QTableWidgetItem *> list = ui->tableAlarmSummary->selectedItems();
    if(list.size() < 1)
        return;
    foreach (const QTableWidgetItem *item, list)
    {
        if(item->column() == 1 && item->text() != "")     // Tag Name
            array.append(QJsonValue(QString(item->text())));
    }
    QJsonValue argValue = QJsonValue(array);
    g_pSrSocket->sendCommand(0, QString("Alarm_Ack"), QString(""), argValue);
}

void CAlarmSummary::on_btnAckAll_clicked()
{
    ui->tableAlarmSummary->clearSelection();
    QJsonArray array;
    foreach (const ALARM_SUM_TAB * sum, mAlarmSummaryList)
        array.append(QJsonValue(sum->Name));
    QJsonValue argValue = QJsonValue(array);
    g_pSrSocket->sendCommand(0, QString("Alarm_Ack"), QString(""), argValue);
}
