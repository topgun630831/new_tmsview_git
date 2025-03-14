#include "tmspoint.h"
#include "ui_tmspoint.h"
#include "srsocket.h"
#include <QDebug>
#include <QListView>
#include "tag.h"
#include "infomationdialog.h"

extern CSrSocket *g_pSrSocket;
extern const char* ALM_STATUS_MSG[6];
CTag* FindTagObject(QString& TagName);
//extern QString GraphicId;
const QString PointID = "Point";

TmsPoint::TmsPoint(QString& group, QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TmsPoint)
{
    ui->setupUi(this);

    ui->tablePoint->horizontalHeader()->resizeSection(0, 450);    // 항목
    ui->tablePoint->horizontalHeader()->resizeSection(1, 350);   // 현재값

    ui->tablePoint_2->horizontalHeader()->resizeSection(0, 450);    // 항목
    ui->tablePoint_2->horizontalHeader()->resizeSection(1, 350);   // 현재값

    for(int i = 0; i < g_pSrSocket->mGroupList.size(); i++)
    {
        ui->comboBox->addItem(g_pSrSocket->mGroupList[i]);
        ui->comboBox->model()->setData(ui->comboBox->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);
    }
    installEventFilter(this);
    mGroup = group;
    mbStarted = false;
    mPointInitOk = false;
    GroupChanged();
    ui->comboBox->setCurrentText(mGroup);
    ui->comboBox->setView(new QListView);
}

TmsPoint::~TmsPoint()
{
    killTimer(mTimerId);
    delete ui;
}

int TmsPoint::getTagPos(QString tag)
{
    for(int i = 0; i < mPointList.size(); i++)
        if(mPointList[i]->Name == tag)
            return i;
   return 0;
}

void TmsPoint::onRead(QString& cmd, QJsonObject& jobject)
{
    static QString gas;
    static QString msam;
    static int msam_pos=0;
    static bool bGas = false;
    if(cmd == "Tag_GroupSummary")
    {
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray array = ob["Summaries"].toArray();

        QString str;

        foreach (const POINT_TAB * point, mPointList)
            delete point;
        mPointList.clear();

        int pos = 0;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject pointObject = value.toObject();
            POINT_TAB *point = new POINT_TAB;
            point->Name = pointObject["Name"].toString();
            if(point->Name == "TOC00_MTM1" || point->Name == "TOC10_MTM1")
            {
                gas = pointObject["Value"].toString();
                bGas = true;
                continue;
            }
            point->Desc = pointObject["Desc"].toString();
            point->TagType = pointObject["TagType"].toString();
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
                if(bGas == true && (point->Name == "TOC00_MSAM" || point->Name == "TOC10_MSAM"))
                {
                   point->Value = val + "," + gas + " "+ point->Unit;
                   msam = val;
                   msam_pos = pos;
                }
                else
                    point->Value = val + " " + point->Unit;
            }
            else
            if(point->TagType == "String")
                point->Value = val;
            mPointList.append(point);
            pos++;
        }
        mPointInitOk = true;
        InitDisp();
        mbStarted = true;
        mTimerId = startTimer(1000);
    }
    else
    if(cmd == "Tag_Changed")
    {
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray array = ob["ChangedValues"].toArray();
        for(int j = 0; j < array.size(); j += 4)
        {
            QString name = array[j].toString();
            int i = getTagPos(name);
            QString val = array[j+1].toString();
//            if(mPointList[i]->Name == "TOC00_MTM1" || mPointList[i]->Name == "TOC10_MTM1")
            if(name == "TOC00_MTM1" || name == "TOC10_MTM1")
            {
                gas = val;
                bGas = true;
                mPointList[msam_pos]->Value = msam + "," + gas + " "+  mPointList[msam_pos]->Unit;
            }
            else
            {
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
                {
                    if(bGas == true && (mPointList[i]->Name == "TOC00_MSAM" || mPointList[i]->Name == "TOC10_MSAM"))
                    {
                        mPointList[i]->Value = val + "," + gas + " "+  mPointList[i]->Unit;
                        msam = val;
                    }
                    else
                        mPointList[i]->Value = val + " " + mPointList[i]->Unit;
                }
                quint16 stat = (quint16)array[j+2].toDouble();
                if(stat < 6)
                   mPointList[i]->AlarmSt = ALM_STATUS_MSG[stat];
                stat = (quint16)array[j+3].toDouble();
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
            }
            i++;
        }
        ValueDisp();
    }
}

void TmsPoint::InitDisp()
{
    QTableWidget *table = ui->tablePoint;
    QTableWidgetItem *item;
    QString str;
    int i, row;
    for(i = 0; i < mPointList.size(); i++)
    {
        row = i;
        if(i >= 15)
        {
            table = ui->tablePoint_2;
            row = i - 15;
        }
        if(row >= 15)
            break;
        item = new QTableWidgetItem(mPointList[i]->Desc);
        table->setItem(row, 0, item);
        item = new QTableWidgetItem(mPointList[i]->Value);
        table->setItem(row, 1, item);
    }
}

void TmsPoint::ValueDisp()
{
    QTableWidget *table = ui->tablePoint;
    QTableWidgetItem *item;
    QString str;
    int row;
    for(int i = 0; i < mPointList.size(); i++)
    {
        row = i;
        if(i >= 15)
        {
            table = ui->tablePoint_2;
            row = i - 15;
        }
        item = new QTableWidgetItem(mPointList[i]->Value);
        table->setItem(row, 1, item);
    }
}

void TmsPoint::timerEvent(QTimerEvent *event)
{
    if(mbStarted == false)
        return;
    if (event->timerId() == mTimerId)
    {
        if(mPointInitOk == true)
            g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_Changed"), PointID, QJsonValue(PointID));
        else
        {
            QJsonObject tmpObj;
            tmpObj.insert(QString("Id"), PointID);
            tmpObj.insert(QString("Name"), mGroup);
            g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_GroupSummary"), PointID, QJsonValue(tmpObj));
        }
    }
}

void TmsPoint::GroupChanged()
{
    QJsonObject tmpObj;
    tmpObj.insert(QString("Id"), PointID);
    tmpObj.insert(QString("Name"), mGroup);
    g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_GroupSummary"), PointID, QJsonValue(tmpObj));
}

void TmsPoint::on_change_clicked()
{
    mGroup = ui->comboBox->currentText();
    ui->tablePoint->clearContents();
    ui->tablePoint_2->clearContents();
    GroupChanged();
}
