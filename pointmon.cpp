#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "pointmon.h"
#include "ui_pointmon.h"
#include "groupselect.h"
#include "srsocket.h"
#include <QDebug>
#include "tag.h"
#include "digitalset.h"
#include "analogset.h"
#include "stringset.h"
#include "infomationdialog.h"
#include "property.h"
#include "propertyrange.h"
extern QString gSoftwareModel;

extern CSrSocket *g_pSrSocket;
extern const char* ALM_STATUS_MSG[6];
CTag* FindTagObject(QString& TagName);
const QString PointID = "Point";

CPointMon::CPointMon(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CPointMon)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    QTableWidget *table = ui->tablePoint;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    table->horizontalHeader()->resizeSection(0, 120);    // No
    table->horizontalHeader()->resizeSection(1, 320);   // Name
    table->horizontalHeader()->resizeSection(2, 700);   // Desc
    table->horizontalHeader()->resizeSection(3, 200);   // Status
    table->horizontalHeader()->resizeSection(4, 200);   // Status
    table->horizontalHeader()->resizeSection(5, 260);   // Value

    mbStarted = false;
    mPage = -1;
    mTotalPage = 0;
    mbDevice = false;
    mSelectedIndex = -1;
    if(g_pSrSocket->mGroupList.length() != 0)
    {
        mGroup = g_pSrSocket->mGroupList[0];
        ui->btnGroup->setText(mGroup);
        mArgValue = QJsonValue(mGroup);
        QJsonObject tmpObj;
        tmpObj.insert(QString("Id"), PointID);
        tmpObj.insert(QString("Name"), mGroup);
        g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_GroupSummaryWithRaw"), PointID, QJsonValue(tmpObj));
    }
    installEventFilter(this);
    mSelectedIndex = -1;
}

CPointMon::~CPointMon()
{
    killTimer(mTimerId);
    delete ui;
    foreach (const POINT_TAB * point, mPointList)
        delete point;
    mPointList.clear();
}

void CPointMon::paintEvent(QPaintEvent *)
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

int CPointMon::getTagPos(QString tag)
{
    for(int i = 0; i < mTotalPoint; i++)
        if(mPointList[i]->Name == tag)
            return i;
   return 0;
}

void CPointMon::onRead(QString& cmd, QJsonObject& jobject)
{
    if(cmd == "Tag_GroupSummaryWithRaw" || cmd == "Tag_DeviceSummaryWithRaw")
    {
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray array = ob["Summaries"].toArray();

        QString str;
        mTotalPoint = array.size();
        mStartPoint = mPage = 0;
        mTotalPage = mTotalPoint / 15;
        if(mTotalPoint % 15)
            mTotalPage++;
        PageDisp();

        foreach (const POINT_TAB * point, mPointList)
            delete point;
        mPointList.clear();

        foreach (const QJsonValue& value, array)
        {
            QJsonObject pointObject = value.toObject();
            POINT_TAB *point = new POINT_TAB;
            point->Name = pointObject["Name"].toString();
            point->Desc = pointObject["Desc"].toString();
            point->Rw = pointObject["Rw"].toString();
            point->TagType = pointObject["TagType"].toString();
            point->RawValue = pointObject["RawValue"].toString();
            quint16 st = (quint16)pointObject["AlarmSt"].toDouble();
            if(st < 6)
                point->AlarmSt = ALM_STATUS_MSG[st];
            st = (quint16)pointObject["TagSt"].toDouble();
            if(st == 0)
                point->TagSt = "Normal";
            else
            if(st & 0x01)
                point->TagSt = "OffLine";
            else
            if(st & 0x02)
                point->TagSt = "Under";
            else
            if(st & 0x04)
                point->TagSt = "Over";
            QString val = pointObject["Value"].toString();
            if(point->TagType == "Digital")
            {
                QString unit = pointObject["Unit"].toString();
                int pos = unit.indexOf(QChar('/'));
                point->OnMsg = unit.left(pos);
                point->OffMsg = unit.mid(pos+1);
                if(val == "1")
                    point->Value = point->OnMsg;
                else
                    point->Value = point->OffMsg;
            }
            else
            if(point->TagType == "Analog" || point->TagType == "Pulse")
            {
                point->Unit = pointObject["Unit"].toString();
                point->Value = val + " " + point->Unit;
            }
            else
            if(point->TagType == "String")
                point->Value = val;
            mPointList.append(point);
        }
        g_pSrSocket->mPointInitOk = true;
        InitDisp();
        mbStarted = true;
        mTimerId = startTimer(1000);
    }
/*    else
    if(cmd == "Tag_UnitGroupValues" || cmd == "Tag_UnitDeviceValues")
    {
        QJsonArray array = jobject["Result"].toArray();
        for(int j = 0; j < array.size(); j += 4)
        {
            int i = getTagPos(array[j].toString());
            QString val = array[j+1].toString();
            if(mPointList[i]->TagType == "String") // String Tag
               mPointList[i]->Value = val;
            else
            if(mPointList[i]->TagType == "Digital") // String Tag
            {
                if(val == "1")
                    mPointList[i]->Value = mPointList[i]->OnMsg;
                else
                    mPointList[i]->Value = mPointList[i]->OffMsg;
            }
            else
            if(mPointList[i]->TagType == "Analog" || mPointList[i]->TagType == "Pulse")
                mPointList[i]->Value = val + " " +
                                    mPointList[i]->Unit;
            quint16 stat = (quint16)array[j+2].toString().toDouble();
            mPointList[i]->AlarmSt;
            stat = (quint16)array[j+3].toString().toDouble();
            mPointList[i]->TagSt = stat;
            i++;
        }
        ValueDisp();
    }
*/    else
    if(cmd == "Tag_ChangedWithRaw")
    {
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray array = ob["ChangedValues"].toArray();
        for(int j = 0; j < array.size(); j += 5)
        {
            int i = getTagPos(array[j].toString());
            QString val = array[j+1].toString();
            if(mPointList[i]->TagType == "String") // String Tag
               mPointList[i]->Value = val;
            else
            if(mPointList[i]->TagType == "Digital") // String Tag
            {
                if(val == "1")
                    mPointList[i]->Value = mPointList[i]->OnMsg;
                else
                    mPointList[i]->Value = mPointList[i]->OffMsg;
            }
            else
            if(mPointList[i]->TagType == "Analog" || mPointList[i]->TagType == "Pulse")
                mPointList[i]->Value = val + " " +
                                    mPointList[i]->Unit;
            mPointList[i]->RawValue = array[j+2].toString();
            quint16 stat = (quint16)array[j+3].toDouble();
            if(stat < 6)
               mPointList[i]->AlarmSt = ALM_STATUS_MSG[stat];
            stat = (quint16)array[j+4].toDouble();
            if(stat == 0)
                mPointList[i]->TagSt = "Normal";
            else
            if(stat & 0x01)
                mPointList[i]->TagSt = "OffLine";
            else
            if(stat & 0x02)
                mPointList[i]->TagSt = "Under";
            else
            if(stat & 0x04)
                mPointList[i]->TagSt = "Over";
            i++;
        }
        ValueDisp();
    }
 }

void CPointMon::InitDisp()
{
    QTableWidget *table = ui->tablePoint;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    for(int row = mStartPoint; row < mStartPoint + 15; row++, cnt++)
    {
        if(row >= mPointList.size())
            break;
        str = QString("%1").arg(row+1);
        item = new QTableWidgetItem(str);
        table->setItem(cnt, 0, item);
        item = new QTableWidgetItem(mPointList[row]->Name);
        table->setItem(cnt, 1, item);
        item = new QTableWidgetItem(mPointList[row]->Desc);
        table->setItem(cnt, 2, item);
        int column = 3;
//        if(gSoftwareModel != "ORION_TMS")
        {
            item = new QTableWidgetItem(mPointList[row]->AlarmSt);
            table->setItem(cnt, column++, item);
            item = new QTableWidgetItem(mPointList[row]->TagSt);
            table->setItem(cnt, column++, item);
        }
        item = new QTableWidgetItem(mPointList[row]->Value);
        item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        table->setItem(cnt, column, item);
    }
    str = "";
    if(cnt < 15)
    {
        for(int i = cnt; i < 15; i++)
            for(int j = 0; j < 6; j++)
            {
                item = new QTableWidgetItem(str);
                table->setItem(i, j, item);
            }
    }
}

void CPointMon::ValueDisp()
{
    QTableWidget *table = ui->tablePoint;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    for(int row = mStartPoint; row < mStartPoint + 15;row++)
    {
        if(row >= mPointList.size())
            break;
        int column = 3;
        item = new QTableWidgetItem(mPointList[row]->AlarmSt);
        table->setItem(cnt, column++, item);
        item = new QTableWidgetItem(mPointList[row]->TagSt);
        table->setItem(cnt, column++, item);
        item = new QTableWidgetItem(mPointList[row]->Value);
        item->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
        table->setItem(cnt, column, item);
        cnt++;
    }
}

void CPointMon::on_btnGroup_clicked()
{
    CGroupSelect dlg(g_pSrSocket->mGroupList, g_pSrSocket->mDeviceList, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        killTimer(mTimerId);
        mbDevice = dlg.mbDevice;
        mGroup = dlg.mGroup;
        ui->btnGroup->setText(mGroup);
        mArgValue = QJsonValue(mGroup);
        QJsonObject tmpObj;
        tmpObj.insert(QString("Id"), PointID);
        tmpObj.insert(QString("Name"), mGroup);
        if(mbDevice)
            g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_DeviceSummaryWithRaw"), PointID, QJsonValue(tmpObj));
        else
            g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_GroupSummaryWithRaw"), PointID, QJsonValue(tmpObj));
        ui->control->setEnabled(false);
        ui->range->setEnabled(false);
//        ui->alarm->setEnabled(false);
        mSelectedIndex = -1;
    }
}

void CPointMon::timerEvent(QTimerEvent *event)
{
    if(mbStarted == false)
        return;
    if (event->timerId() == mTimerId)
    {
        if(g_pSrSocket->mPointInitOk == true)
            g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_ChangedWithRaw"), PointID, QJsonValue(PointID));
        else
        {
            QJsonObject tmpObj;
            tmpObj.insert(QString("Id"), PointID);
            tmpObj.insert(QString("Name"), mGroup);
            if(mbDevice)
                g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_DeviceSummaryWithRaw"), PointID, QJsonValue(tmpObj));
            else
                g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_GroupSummaryWithRaw"), PointID, QJsonValue(tmpObj));
        }
    }
}

void CPointMon::on_btnFirst_clicked()
{
    if(mPage > 0)
    {
        mPage = 0;
        mStartPoint = 0;
        InitDisp();
        ValueDisp();
        PageDisp();
    }
}

void CPointMon::on_btnPre_clicked()
{
    if(mPage > 0)
    {
        mPage--;
        mStartPoint = mPage * 15;
        InitDisp();
        ValueDisp();
        PageDisp();
    }
}

void CPointMon::on_btnNext_clicked()
{
    if(mTotalPage > 0 &&mPage != mTotalPage-1)
    {
        mPage++;
        mStartPoint = mPage * 15;
        InitDisp();
        ValueDisp();
        PageDisp();
    }
}

void CPointMon::on_btnLast_clicked()
{
    if(mTotalPage > 0 && mPage != mTotalPage-1)
    {
        mPage = mTotalPage-1;
        mStartPoint = mPage * 15;
        InitDisp();
        ValueDisp();
        PageDisp();
    }
}

void CPointMon::PageDisp()
{
    QString str = QString("Page(%1/%2)").arg(mPage+1).arg(mTotalPage);
    ui->labelPage->setText(str);
}

void CPointMon::on_tablePoint_control(int row)
{
    int index = mStartPoint+row;
    QTableWidgetItem *item = ui->tablePoint->item(row,1);
    QString str = item->text();
    CTag Tag(str);
    Tag.setDesc(mPointList[index]->Desc);
    Tag.setUnit(mPointList[index]->Unit);
    Tag.m_OnMsg = mPointList[index]->OnMsg;
    Tag.m_OffMsg = mPointList[index]->OffMsg;
    Tag.setValue(mPointList[index]->Value);
    Tag.setValue(mPointList[index]->Value.toDouble());
    if(mPointList[index]->Rw != "W" &&
        mPointList[index]->Rw != "RW")
    {
        str = "제어할 수 없는 태그입니다.";
        CInfomationDialog dlg(str);
        dlg.exec();
        return;
    }
    QString msg;
    if(mPointList[index]->TagType == "Analog")
    {
        CAnalogSet Dlg(msg, &Tag, this);
        if(Dlg.exec() == QDialog::Accepted)
            Tag.writeValue(Dlg.m_sValue);
    }
    else    // Digital
    if(mPointList[index]->TagType == "Digital")
    {
        CDigitalSet Set(msg, &Tag, &Tag, false, this);
        if(Set.exec() == QDialog::Accepted)
        {
            double value;
            if(Set.m_bOn)
                value = 1;
            else
                value = 0;
            Tag.writeValue(value);
        }
    }
    else    // String
    if(mPointList[index]->TagType == "String")
    {
        CStringSet Dlg(msg, &Tag, this);
        if(Dlg.exec() == QDialog::Accepted)
            Tag.writeValue(Dlg.m_sValue);
    }
}
void CPointMon::on_control_clicked()
{
    if(mSelectedIndex > 0)
        on_tablePoint_control(mSelectedIndex);
}
void CPointMon::on_range_clicked()
{
    if(mSelectedIndex >= 0)
    {
        mbAlarm = false;
        QJsonArray array;
        QTableWidgetItem *item = ui->tablePoint->item(mSelectedIndex,1);
        QString name = item->text();
        PropertyRange dlg(mPointList[mSelectedIndex]);
        dlg.exec();
    }
}

void CPointMon::on_tablePoint_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    int index = mStartPoint+row;
    if(index >= mTotalPoint)
    {
        ui->control->setEnabled(false);
        ui->range->setEnabled(false);
        mSelectedIndex = -1;
        return;
    }
    mSelectedIndex = row;
    if(mPointList[index]->Rw == "W" || mPointList[index]->Rw == "RW")
        ui->control->setEnabled(true);
    else
        ui->control->setEnabled(false);
    if(mPointList[index]->TagType == "String")
        ui->range->setEnabled(false);
    else
        ui->range->setEnabled(true);
}
