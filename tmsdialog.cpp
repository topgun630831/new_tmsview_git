#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include "tmsdialog.h"
#include "ui_tmsdialog.h"
#include "myapplication.h"
#include "networksetup.h"
#include "epslog.h"
#include "srsocket.h"
#include "monitorudp.h"
#include "tag.h"
#include "history.h"
#include "infomationdialog.h"
#include "questiondialog.h"
#include "tmstrenddialog.h"
#include "common.h"
#include "pointmon.h"
#include "tmsdata.h"
#include "msgview.h"
#include "testsetup.h"
#include "tmspassword.h"
#include "tmspoint.h"
#include "monitorudp.h"
#include "usbmenudialog.h"

extern QSqlDatabase gDb;
extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern QMap<QString, CTag*> m_TagMap;
extern QMap<quint16, CTag*> m_AtomTagMap;
extern CTag* FindTagObject(QString& TagName);
extern CMyApplication *gApp;
extern int     sr_fd;
extern int g_bTrend;
bool   g_Flag[TAG_MAX];
TmsDialog *g_pTmsDlg;
const QString GraphicId = "Graphic";
QString TmsTagNameList[TAG_MAX] = {
    "TON00",
    "TOP00",
    "TOC00",
    "PHY00",
    "SUS00",
    "COD00",
    "BOD00",
    "FLW01",
    "SAM00",
    "DORON",
    "PWRON",
    "FMLON",
    "FMRON",
    "SAM00_POS",
    "SAM00_DOOR",
    "SAM00_POS",
    "SAM00_RUN",
    "SAM00_DOOR",
    "SAM00_TIME",
    "DATETIME",
    "TS_CONNECTED",
    "FLOW_DAYTOTAL",
    "FLW01_RATE",
};
QString TmsStatusTag[9] = {
    "TON00_STS",
    "TOP00_STS",
    "TOC00_STS",
    "PHY00_STS",
    "SUS00_STS",
    "COD00_STS",
    "BOD00_STS",
    "FLW01_STS",
    "SAM00_STS",
};

QString TmsEquipStatusTag[9] = {
    "TON00_EQUIP_STS",
    "TOP00_EQUIP_STS",
    "TOC00_EQUIP_STS",
    "PHY00_EQUIP_STS",
    "SUS00_EQUIP_STS",
    "COD00_EQUIP_STS",
    "BOD00_EQUIP_STS",
    "FLW01_EQUIP_STS",
    "SAM00_EQUIP_STS",
};

QString TmsCommTag[9] = {
    "TON00_COMM",
    "TOP00_COMM",
    "TOC00_COMM",
    "PHY00_COMM",
    "SUS00_COMM",
    "COD00_COMM",
    "BOD00_COMM",
    "FLW01_COMM",
    "SAM00_COMM",
};

QString TmsCheckTag[9] = {
    "TON00_CHECK",
    "TOP00_CHECK",
    "TOC00_CHECK",
    "PHY00_CHECK",
    "SUS00_CHECK",
    "COD00_CHECK",
    "BOD00_CHECK",
    "FLW01_CHECK",
    "SAM00_CHECK",
};
QString TmsStatusText[7] = {
    "정상",
    "일시정지",
    "유량없음",
    "교정중",
    "점검중",
    "통신불량",
    "동작불량"
};
/*
const char *OFF_LABEL = "QLabel {background-color : green;color : #AEE239;}";
const char *ON_LABEL = "QLabel {background-color : red;color : #AEE239;}";
const char *DISABLE_LABEL = "QLabel {background-color : gray;color : darkGray;}";
*/
const char *OFF_LABEL = "QLabel {background-color : #38A8FF;color : white;}";
const char *ON_LABEL = "QLabel {background-color : #F04031;color : white;}";
const char *DISABLE_LABEL = "QLabel {background-color : #1D1E1F;color : #4D4D4D;}";
QString gTmsVersion;
bool    m_bTmsInitOk;
QString g_sLastValue[8];
extern QString     gPassword;
bool m_bUseToc;
//bool    g_bToc10;
bool m_bGas;    //이송가스
extern bool    m_bScreenSave;

TmsDialog::TmsDialog(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TmsDialog)
{
    m_bInitOk = 0;
    ui->setupUi(this);

    QList<QScreen*> screens = qApp->screens();
        for(int ii = 0; ii < screens.length(); ++ii) {
            QSize pixelSize = screens[ii]->size();
            QSizeF physicalSize = screens[ii]->physicalSize();
            double devicePixelRatio = screens[ii]->devicePixelRatio();
            double logicalDPIX = screens[ii]->logicalDotsPerInchX();
            double logicalDPIY = screens[ii]->logicalDotsPerInchY();
            double logicalDPI = screens[ii]->logicalDotsPerInch();
            double physicalDPIX = screens[ii]->physicalDotsPerInchX();
            double physicalDPIY = screens[ii]->physicalDotsPerInchY();
            double physicalDPI = screens[ii]->physicalDotsPerInch();

            double pixelValX = pixelSize.width();
            double pixelValY = pixelSize.height();
            double physicalSizeX_cm = physicalSize.width() / 10.0;
            double physicalSizeY_cm = physicalSize.height() / 10.0;
            double calcPixelPerCMX = pixelValX / physicalSizeX_cm;
            double calcPixelPerCMY = pixelValY / physicalSizeY_cm;

            double givenLogicalDotsPerCMX = logicalDPIX * 2.54;
            double givenLogicalDotsPerCMY = logicalDPIY * 2.54;
            double givenLogicalDotsPerCM = logicalDPI * 2.54;

            double givenPhysicalDotsPerCMX = physicalDPIX * 2.54;
            double givenPhysicalDotsPerCMY = physicalDPIY * 2.54;
            double givenPhysicalDotsPerCM = physicalDPI * 2.54;

            double ratioLogicalDPCMvsPPCMX = givenLogicalDotsPerCMX / calcPixelPerCMX;
            double ratioLogicalDPCMvsPPCMY = givenLogicalDotsPerCMY / calcPixelPerCMY;
            double ratioPhysicalDPCMvsPPCMX = givenPhysicalDotsPerCMX / calcPixelPerCMX;
            double ratioPhysicalDPCMvsPPCMY = givenPhysicalDotsPerCMY / calcPixelPerCMY;

            qDebug() << "\n\nScreen: " << ii;
            qDebug() << "Device Pixel Ratio: " << devicePixelRatio;
            qDebug() << "Pixel in X-Direction: " << pixelValX;
            qDebug() << "Pixel in Y-Direction: " << pixelValY;
            qDebug() << "Physical Size X-Direction in CM: " << physicalSizeX_cm;
            qDebug() << "Physical Size Y-Direction in CM: " << physicalSizeY_cm;
            qDebug() << "Calculated Pixel Per CM in X-Direction: " << calcPixelPerCMX;
            qDebug() << "Calculated Pixel Per CM in Y-Direction: " << calcPixelPerCMY;
            qDebug() << "Qt Logical Dots Per CM in X-Direction: " << givenLogicalDotsPerCMX;
            qDebug() << "Qt Logical Dots Per CM in Y-Direction: " << givenLogicalDotsPerCMY;
            qDebug() << "Qt Logical Dots Per CM Average: " << givenLogicalDotsPerCM;
            qDebug() << "Qt Physical Dots Per CM in X-Direction: " << givenPhysicalDotsPerCMX;
            qDebug() << "Qt Physical Dots Per CM in Y-Direction: " << givenPhysicalDotsPerCMY;
            qDebug() << "Qt Physical Dots Per CM Average: " << givenPhysicalDotsPerCM;
            qDebug() << "Ratio of Logical Dots Per CM vs Pixel Per CM in X-Direction: " << ratioLogicalDPCMvsPPCMX;
            qDebug() << "Ratio of Logical Dots Per CM vs Pixel Per CM in Y-Direction: " << ratioLogicalDPCMvsPPCMY;
            qDebug() << "Ratio of Physical Dots Per CM vs Pixel Per CM in X-Direction: " << ratioPhysicalDPCMvsPPCMX;
            qDebug() << "Ratio of Physical Dots Per CM vs Pixel Per CM in Y-Direction: " << ratioPhysicalDPCMvsPPCMY;
        }

    DbLoad();

    m_imgBackGround.load(":/images/background.png");
//    m_Box.load(":/images/box.png");
    m_SamplerDoorOn.load(":/images/sampler_open.png");
    m_SamplerDoorOff.load(":/images/sampler_close.png");
    m_SamplerRunOn.load(":/images/sampling.png");
    m_SamplerRunOff.load(":/images/sampler_wait.png");
    m_SamplerRunFault.load(":/images/sampler_fault.png");
    m_DOROn.load(":/images/door_on.png");
    m_DOROff.load(":/images/door_off.png");
    m_PWROn.load(":/images/power_on.png");
    m_PWROff.load(":/images/power_off.png");
    m_FMLOn.load(":/images/sampl_off.png");
    m_FMLOff.load(":/images/sampl_on.png");
    m_FMROn.load(":/images/sampr_off.png");
    m_FMROff.load(":/images/sampr_on.png");
    m_Check.load(":/images/점검중.png");
    m_CommOn.load(":/images/comm_error.png");
    m_CommOff.load(":/images/comm_normal.png");
    g_pTmsDlg = this;

    CTag* pTag;
    for(int i = 0; i < TAG_MAX; i++)
    {
        if(i < 9 && g_Flag[i] == false)
                continue;
        pTag = new CTag(TmsTagNameList[i]);
        m_TagMap[TmsTagNameList[i]] = pTag;
    }
    for(int i = 0; i < 9; i++)
    {
        //if(i < 6 && g_Flag[i] == false)
        if(g_Flag[i] == false)
            continue;
        {
            pTag = new CTag(TmsStatusTag[i]);
            m_TagMap[TmsStatusTag[i]] = pTag;
//            if(i < 6)
//                StsTagIndexMap[TmsStatusTag[i]] = i;
        }
    }
    for(int i = 0; i < 8; i++)
    {
        if(g_Flag[i] == false)
            continue;
        {
            pTag = new CTag(TmsEquipStatusTag[i]);
            m_TagMap[TmsEquipStatusTag[i]] = pTag;
        }
    }
//    if(g_bToc10)
//        TmsCheckTag[2] = "TOC10_CHECK";
    for(int i = 0; i < 9; i++)
    {
//        if(i < 6 && g_Flag[i] == false)
        if(g_Flag[i] == false)
            continue;
        pTag = new CTag(TmsCheckTag[i]);
        m_TagMap[TmsCheckTag[i]] = pTag;
        mpCheckTag[i] = pTag;
   //     if(i < 6)
            CheckTagIndexMap[TmsCheckTag[i]] = i;
    }
//    if(g_bToc10)
//        TmsCommTag[2] = "TOC10_COMM";
    for(int i = 0; i < 9; i++)
    {
//        if(i < 6 && g_Flag[i] == false)
        if(g_Flag[i] == false)
            continue;
        {
            pTag = new CTag(TmsCommTag[i]);
            m_TagMap[TmsCommTag[i]] = pTag;
            mpCommTag[i] = pTag;
          //  if(i < 6)
                CommTagIndexMap[TmsCommTag[i]] = i;
        }
    }
    for(int i = 0; i < 9; i++)
    {
        mCheckFlag[i] = false;
    }

    RoundupLoad();

    QString name = "TMS_MSG";
    pTag = new CTag(name);
    m_TagMap[name] = pTag;

    name = "TMS_FATAL";
    pTag = new CTag(name);
    m_TagMap[name] = pTag;

    mValueReadCmd = "Tag_Changed";
    mInfoCmd = "Tag_Summary";
    mPasswordCmd = "ShareData_Get";
    makeCommand();

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(&m_TimerClose, SIGNAL(timeout()), this, SLOT(onTimerClose()));

    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);

//    m_Timer.start(100);

    installEventFilter(this);

    m_Button[0] = ui->btnTn;
    m_Button[1] = ui->btnTp;
    m_Button[2] = ui->btnToc;
    m_Button[3] = ui->btnPh;
    m_Button[4] = ui->btnSs;
    m_Button[5] = ui->btnCod;
    m_Button[6] = ui->btn7;

    Value[0] = ui->val1;
    Value[1] = ui->val2;
    Value[2] = ui->val3;
    Value[3] = ui->val4;
    Value[4] = ui->val5;
    Value[5] = ui->val6;
    Value[6] = ui->val7;

    Unit[0] = ui->unit1;
    Unit[1] = ui->unit2;
    Unit[2] = ui->unit3;
    Unit[3] = ui->unit4;
    Unit[4] = ui->unit5;
    Unit[5] = ui->unit6;
    Unit[6] = ui->unit7;

    Status[0][0] = ui->status1_1;
    Status[0][1] = ui->status1_2;
    Status[1][0] = ui->status2_1;
    Status[1][1] = ui->status2_2;
    Status[2][0] = ui->status3_1;
    Status[2][1] = ui->status3_2;
    Status[3][0] = ui->status4_1;
    Status[3][1] = ui->status4_2;
    Status[4][0] = ui->status5_1;
    Status[4][1] = ui->status5_2;
    Status[5][0] = ui->status6_1;
    Status[5][1] = ui->status6_2;
    Status[6][0] = ui->status7_1;
    Status[6][1] = ui->status7_2;
    Status[7][0] = ui->status8_1;
    Status[7][1] = ui->status8_2;
    Status[8][0] = ui->status8_1;
    Status[8][1] = ui->status9_2;

    StatusLabel[0][0] = ui->sensor_status_1;
    StatusLabel[0][1] = ui->server_status_1;
    StatusLabel[1][0] = ui->sensor_status_2;
    StatusLabel[1][1] = ui->server_status_2;
    StatusLabel[2][0] = ui->sensor_status_3;
    StatusLabel[2][1] = ui->server_status_3;
    StatusLabel[3][0] = ui->sensor_status_4;
    StatusLabel[3][1] = ui->server_status_4;
    StatusLabel[4][0] = ui->sensor_status_5;
    StatusLabel[4][1] = ui->server_status_5;
    StatusLabel[5][0] = ui->sensor_status_6;
    StatusLabel[5][1] = ui->server_status_6;
    StatusLabel[6][0] = ui->sensor_status_7;
    StatusLabel[6][1] = ui->server_status_7;
    StatusLabel[7][0] = ui->sensor_status_8;
    StatusLabel[7][1] = ui->server_status_8;
    StatusLabel[8][0] = ui->sensor_status_8;
    StatusLabel[8][1] = ui->server_status_9;

    TagIndexMap["TON00"] = 0;
    TagIndexMap["TOP00"] = 1;
    TagIndexMap["TOC00"] = 2;
    TagIndexMap["PHY00"] = 3;
    TagIndexMap["SUS00"] = 4;
    TagIndexMap["COD00"] = 5;
//    TagIndexMap["SAM00"] = 5;
//    TagIndexMap["FLW01"] = 6;
    StsTagIndexMap["TON00_STS"] = 0;
    StsTagIndexMap["TOP00_STS"] = 1;
    StsTagIndexMap["TOC00_STS"] = 2;
    StsTagIndexMap["PHY00_STS"] = 3;
    StsTagIndexMap["SUS00_STS"] = 4;
    StsTagIndexMap["COD00_STS"] = 5;
    StsTagIndexMap["FLW01_STS"] = 7;
    StsTagIndexMap["SAM00_STS"] = 8;

    EquipStsTagIndexMap["TON00_EQUIP_STS"] = 0;
    EquipStsTagIndexMap["TOP00_EQUIP_STS"] = 1;
    EquipStsTagIndexMap["TOC00_EQUIP_STS"] = 2;
    EquipStsTagIndexMap["PHY00_EQUIP_STS"] = 3;
    EquipStsTagIndexMap["SUS00_EQUIP_STS"] = 4;
    EquipStsTagIndexMap["COD00_EQUIP_STS"] = 5;
//    EquipStsTagIndexMap["SAM00_EQUIP_STS"] = 6;
    EquipStsTagIndexMap["FLW01_EQUIP_STS"] = 7;

    if(QDir("/app/wwwroot/tmp/usbdisk").exists())
        m_bUsbReady = true;
    else
        m_bUsbReady = false;
    m_watcher.addPath("/app/wwwroot/tmp/");
    connect(&m_watcher, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged(const QString&)));
    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
    {
        gApp->setOverrideCursor(Qt::PointingHandCursor);
        qDebug() << "Mouse ON";
    }
    else
    {
        gApp->setOverrideCursor(Qt::BlankCursor);
        qDebug() << "Mouse OFF";
    }
    m_watcher2.addPath("/tmp/");
    connect(&m_watcher2, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged2(const QString&)));
    m_bMousePress = false;
    connect(this, SIGNAL(sigSysMenu()), this, SLOT(slotSysMenu()));
    for(int i = 0; i < 7; i++)
    {
        if(g_Flag[i] == false || i == 6)
        {
            m_Button[i]->setVisible(false);
            Value[i]->setVisible(false);
            Unit[i]->setVisible(false);
            for(int j = 0 ; j < 2; j++)
            {
                Status[i][j]->setVisible(false);
                StatusLabel[i][j]->setVisible(false);
            }
        }
    }
    m_bInitOk = true;
    ui->tms_fatal->setVisible(false);

    ui->MsgList->setStyleSheet("QListWidget {background-color: rgb(31, 41, 55);"
                               "margin:5px;}");
 //                             //"padding:20px;}");
    ui->MsgList->setSpacing(3);
    m_Timer.start(100);
}

TmsDialog::~TmsDialog()
{
    m_Timer.stop();

    QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
    while (i != m_TagMap.constEnd()) {
        delete i.value();
        i++;
    }
    m_TagMap.clear();
    delete ui;
}

void TmsDialog::SetFlag(QString name)
{
   for(int i = 0; i < TAG_MAX; i++)
    {
        if(TmsTagNameList[i] == name)
        {
            g_Flag[i] = true;
            break;
        }
    }
}

void TmsDialog::DbLoad()
{
    for(int i = 0; i < 11; i++)
        g_Flag[i] = false;
//    g_bToc10 = false;
    QString dbname =  "/data/project2.db";
    gDb.setDatabaseName(dbname);
    gDb.open();
    QSqlQuery query("SELECT Code, Desc, Enabled, Upload, Analog, UseCchk FROM Ext_Tms_Item");
    m_bGas = false;
    while(query.next())
    {
        QString code = query.value(0).toString();
        QString desc = query.value(1).toString();
        int enabled = query.value(2).toInt();
        int upload = query.value(3).toInt();
        int analog = query.value(4).toInt();
        if(desc == "TOC")
        {
            if(upload == 1)
                m_bUseToc = true;
            if(analog == 1)
            {
//                g_bToc10 = true;
//                TmsTagNameList[2] = "TOC10";
            }
            int cchk = query.value(5).toInt();
            if(cchk == 1)
            {
                m_bGas = true;
            }
        }
        if(enabled)
            SetFlag(code);
    }

    gDb.close();
    for(int i = 12; i < 19; i++)
        g_Flag[i] = g_Flag[7];          // 채수기
    for(int i = 19; i < TAG_MAX; i++)
        g_Flag[i] = true;
}

void TmsDialog::RoundupLoad()
{
    QString dbname =  "/data/project2.db";
    gDb.setDatabaseName(dbname);
    gDb.open();
    for(int i = 0; i < TAG_MAX; i++)
    {
        CTag* pTag;
        if(m_TagMap.contains(TmsTagNameList[i]))
        {
            if((pTag = m_TagMap[TmsTagNameList[i]]) != NULL)
            {
                QString str = QString("SELECT RoundUp FROM Tag Where Name='%1'").arg(TmsTagNameList[i]);
                QSqlQuery query(str);
                if(query.next())
                    pTag->setRoundUp(query.value(0).toInt());
            }
        }
    }

    gDb.close();
}

void TmsDialog::addTag(const char* n)
{
    QString name = n;
    CTag *pTag = new CTag(name);
    m_TagMap[name] = pTag;
}

extern bool g_bExit;

void TmsDialog::on_sysinfoBtn_clicked()
{
    if(PasswordCheck())
    {
        CNetworkSetup dlg(false);
        dlg.exec();
        if(g_bExit)
            accept();
    }
}

void TmsDialog::on_historyBtn_clicked()
{
    CHistory dlg;
    dlg.exec();
}

void TmsDialog::on_commBtn_clicked()
{
    CComMon dlg;
    dlg.exec();
}

void TmsDialog::on_setupBtn_clicked()
{
    if(PasswordCheck())
    {
        TmsSetup dlg(g_Flag, mCheckFlag);
        if(dlg.exec() == QDialog::Accepted)
        {
            QJsonObject argObject;
            CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Restart"), argObject, QJsonValue(QString("all")));
        }
    }
}

void TmsDialog::on_dataBtn_clicked()
{
    TmsData dlg;
    dlg.exec();
}

void TmsDialog::makeCommand()
{
//    qDebug() << "TmsDialog::makeCommand()";
    m_bTmsInitOk = false;
    mCommCount = 100;
    mPassCount = 100;

    QJsonArray argArray;
    QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
    while (i != m_TagMap.constEnd())
    {
        argArray.append(QJsonValue(i.key()));
        i++;
    }
    QJsonObject tmpObj;
    mValueRead = makeCommand(mValueReadCmd,QJsonValue(GraphicId));

    tmpObj.insert(QString("Id"), GraphicId);
    tmpObj.insert(QString("TagList"), argArray);
    mInfoRead = makeCommand(mInfoCmd, QJsonValue(tmpObj));

    QJsonArray passArray;
    passArray.append(QJsonValue(QString("TMS_Password")));
    passArray.append(QJsonValue(QString("TMS_Version")));
    mPasswordRead = makeCommand(mPasswordCmd, QJsonValue(passArray));
}

QByteArray TmsDialog::makeCommand(QString cmd, QJsonValue jValue)
{
    QString nid = "0";
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;

    tmpObject.insert(QString("Command"),QJsonValue(cmd));
    tmpObject.insert(QString("Id"),QJsonValue(nid));
    tmpObject.insert(QString("Args"),jValue);
    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);
    data.insert(0, QString("BEST%1").arg(data.size(),6,10,QChar('0')));
    return data;
}

void TmsDialog::onRead(QString& cmd, QJsonObject& jobject)
{
//    if(m_bScreenSave)
//        return;
    if(g_DebugDisplay)
    {
        qDebug() << "TmsDialog::onRead : " << cmd;
        qDebug() << jobject;
    }
    QString error = jobject["Error"].toString();
    if(!error.isEmpty())
    {
       qDebug() << " Error!!!" + error + ", " + jobject["Result"].toString();
    }
    QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
    int len = m_TagMap.count();
    int cnt = 0;
    if(cmd == mInfoCmd)
    {
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray ar = ob["Summaries"].toArray();
        if(g_DebugDisplay)
            qDebug() << "Count:" << ar.count();
        foreach (const QJsonValue& val, ar)
        {
            if(cnt >= len)
                break;
            QJsonObject jObj = val.toObject();
            QString name = jObj["Name"].toString();
            QString str = jObj["TagType"].toString();
            CTag *pTag;
            if((pTag = FindTagObject(name)) != NULL)
            {
                QString unit = jObj["Unit"].toString();
                QString Rw = jObj["Rw"].toString();
                pTag->setRw(Rw);
                if(str == "Digital")
                {
                    pTag->setTagType(0);
                    int pos = unit.indexOf(QChar('/'));
                    pTag->m_OnMsg = unit.left(pos);
                    pTag->m_OffMsg = unit.mid(pos+1);
                }
                else
                if(str == "Analog")
                {
                    pTag->setTagType(1);
                    pTag->setUnit(unit);
                }
                else
                if(str == "Pulse")
                {
                    pTag->setTagType(2);
                    pTag->setUnit(unit);
                }
                else
                if(str == "String")
                {
                    pTag->setTagType(3);
                }
                pTag->setDesc(jObj["Desc"].toString());
                str = jObj["Value"].toString();
                if(pTag->getTagType() == 3)  // String Tag
                    pTag->setValue(str);
                else
                    pTag->setValue(str.toDouble());
                pTag->m_Addr = jObj["Address"].toString();
                pTag->setAlarmStatus((quint16)jObj["AlarmSt"].toDouble());
                pTag->setTagStatus((quint16)jObj["TagSt"].toDouble());
                cnt++;
             //   i++;
                if(pTag->getTagType() == 0) // Digital
                {
                    if(pTag->getValue() == 1)
                        str = pTag->m_OnMsg;
                    else
                        str = pTag->m_OffMsg;
                }
                valueDisplay(name, pTag, str);
                if(name == "TMS_MSG" && str != OldMessage)
                    MsgAdd(str);
                if(name == "TMS_FATAL")
                {
                    ui->tms_fatal->setText(str);
                    if(str != "")
                    {
                        ui->tms_fatal->setVisible(true);
                    }
                    else
                    {
                        ui->tms_fatal->setVisible(false);
                    }
                }
            }
        }
        m_bTmsInitOk = true;
        mCommCount = 100;
        update();
    }
    else
    if(cmd == mValueReadCmd)
    {
        cnt = 0;
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray ar = ob["ChangedValues"].toArray();
        for(int j = 0; j < ar.size(); j += 4)
        {
            QString name = ar[j].toString();
            CTag *pTag = m_TagMap[name];
            if(pTag)
            {
                QString val = ar[j+1].toString();
                if(pTag->getTagType() == 3) // String Tag
                   pTag->setValue(val);
                else
                   pTag->setValue(val.toDouble());
                quint16 stat = (quint16)ar[j+2].toDouble();
                pTag->setAlarmStatus(stat);
                stat = (quint16)ar[j+3].toDouble();
                pTag->setTagStatus(stat);
                if(pTag->getTagType() == 0) // Digital
                {
                    if(pTag->getValue() == 1)
                        val = pTag->m_OnMsg;
                    else
                        val = pTag->m_OffMsg;
                }
                valueDisplay(name, pTag, val);
                if(name == "TMS_MSG" && val != OldMessage)
                    MsgAdd(val);
            }
            i++;
        }
        update();
    }
    else
    if(cmd == "@WriteEvent")
    {
        QJsonObject jObj = jobject["Result"].toObject();
        QString name = jObj["Tag"].toString();
        QString str = jObj["Value"].toString();
        CTag* pTag = FindTagObject(name);
        if(pTag != NULL)
        {
            if(pTag->getTagType() == 3)    //String Tag
                pTag->setValue(str);
            else
                pTag->setValue(str.toDouble());
            if(pTag->getTagType() == 0) // Digital
            {
                if(pTag->getValue() == 1)
                    str = pTag->m_OnMsg;
                else
                    str = pTag->m_OffMsg;
            }
            valueDisplay(name, pTag, str);
        }
    }
    else
    if(cmd == "ShareData_Get")
    {
        QJsonArray ar = jobject["Result"].toArray();
        foreach (const QJsonValue& val, ar)
        {
            QJsonObject jObj = val.toObject();
            QString item = jObj["Item"].toString();
            if(item == "TMS_Password")
            {
                mPassword = jObj["Value"].toString();
                if(mPassword[0] == '@')
                {
                    QString password = mPassword.mid(1);
                    QByteArray array;
                    array.append(password);
                    array = array.fromBase64( array);
                    mPassword = QString::fromUtf8(array.data());
                }
                gPassword = mPassword;
            }
            else
            if(item == "TMS_Version")
            {
                gTmsVersion = jObj["Value"].toString();
                gTmsVersion.insert(4, " ");
                gTmsVersion.insert(9, " ");
                ui->Version->setText(gTmsVersion);
            }
        }
    }
}

void TmsDialog::MsgAdd(QString msg)
{
    OldMessage = msg;
/*    QDateTime date = QDateTime::currentDateTime();
    QString str = date.toString("yyyy-MM-dd hh:mm:ss");
    str += " ";
    str += msg;
*/    MsgList.push_front(msg);
    if(MsgList.size() > 30)
        MsgList.pop_back();
    if(g_DebugDisplay)
        qDebug() << "MsgList:" << MsgList.size();
    ui->MsgList->clear();
    for(int i = 0; i < MsgList.size(); i++)
    {
        if(i >= 3)
            break;
        ui->MsgList->insertItem(i, MsgList[i]);
//        if(ui->MsgList->count() == 3)
//            ui->MsgList->removeItemWidget(ui->MsgList->item(ui->MsgList->count()-1));
    }
}

void TmsDialog::valueDisplay(QString name, CTag *pTag, QString strVal)
{
//	if(m_bScreenSave)
//		return;
    int index;
    QString str;
    if((index = TagIndex(name)) >= 0)
    {
       Value[index]->display(strVal);
       if(index <= 6 && m_TagMap.contains(TmsCommTag[index]))
       {
           CTag* pCommTag = m_TagMap[TmsCommTag[index]];
           if(pCommTag->getValue() == 0)  //통신불량이면
           {
               int round = pTag->getRoundUp();
               if(round > 0)
                   round--;
               QString strV = QString("%1").arg(0.0, 9, 'f', round);
               if(index <= 6)
                   Value[index]->display(strV);
               else
               if(index == 8)
               {
                   str = strV;
                   str += " ";
                   str += pTag->getUnit();
                   ui->sampler_temp->setText(str);
               }
               else
               if(index == 7)
                   ui->rate->display(strV);
//                   ui->rate->setText(strV);
           }
           g_sLastValue[index] =  strVal;
       }

    }
    else
    if((index = StsTagIndex(name)) >= 0)
    {
        int val = pTag->getValue();
        if(val < 0 && val >= 7)
            val = 0;
        qDebug() << "Status:" << index << "," << TmsStatusText[val];
        Status[index][1]->setText(TmsStatusText[val]);
    }
    else
    if((index = EquipStsTagIndex(name)) >= 0)
    {
        int val = pTag->getValue();
        if(val < 0 && val >= 7)
            val = 0;
        qDebug() << "EquipStatus:" << index << "," << TmsStatusText[val];
        Status[index][0]->setText(TmsStatusText[val]);
    }
    else
    if((index = CommTagIndex(name)) >= 0)
    {
        int val = pTag->getValue();
/*        if(val)     // 0:통신이상 1:정상
            Status[index][4]->setFrameShape(QFrame::NoFrame);
        else
            Status[index][4]->setFrameShape(QFrame::WinPanel);
*/
        if(index <= 6)
        {
            CTag* pTag;
            float val;
            if(m_TagMap.contains(TmsCommTag[index]))
            {
                if((pTag = m_TagMap[TmsTagNameList[index]]) != NULL)
                {
                    QString strV;
                    val = pTag->getValue();
                    CTag* pCommTag = m_TagMap[TmsCommTag[index]];
                    if(pCommTag->getValue() == 0 && m_TagMap.contains(TmsTagNameList[index]))  //통신불량이면
                    {
                        if((pTag = m_TagMap[TmsTagNameList[index]]) != NULL)
                            val = 0;

                        int round = pTag->getRoundUp();
                        if(round > 0)
                            round--;
                        strV = QString("%1").arg(val, 9, 'f', round);
                    }
                    else
                        strV = g_sLastValue[index];
                    if(index <= 6)
                        Value[index]->display(strV);
                    else
                    if(index == 8)
                    {
                        str = strV;
                        str += " ";
                        str += pTag->getUnit();
                        ui->sampler_temp->setText(str);
                    }
                    else
                    if(index == 7)
                        ui->rate->display(strV);
//                        ui->rate->setText(strV);
                }
            }
        }
    }
    else
    if(name == "FLW01")
    {
        ui->total->display(strVal);
    }
 //       ui->total->setText(strVal);
    else
 /*   if(name == "FLOW_DAYTOTAL")
        ui->daytotal->setText(strVal);
    else
*/    if(name == "FLW01_RATE")
    {
        if(mpCommTag[8]->getValue() == 0)  //통신불량이면
        {
            int round = pTag->getRoundUp();
            if(round > 0)
                round--;
            strVal = QString("%1").arg(0.0, 9, 'f', round);
        }

        ui->rate->display(strVal);
//        ui->rate->setText(strVal);
    }
    else
    if(name == "FLW01_COMM")
    {
        pTag = m_TagMap[TmsTagNameList[7]];
        float val = pTag->getValue();
        if(mpCommTag[7]->getValue() == 0)  //통신불량이면
            val = 0.0;

        int round = pTag->getRoundUp();
        if(round > 0)
            round--;
        strVal = QString("%1").arg(val, 9, 'f', round);
        ui->rate->display(strVal);
//        ui->rate->setText(strVal);
    }
    else
    if(name == "SAM00")
    {
        if(mpCommTag[8]->getValue() == 0)  //통신불량이면
        {
            int round = pTag->getRoundUp();
            if(round > 0)
                round--;
            strVal = QString("%1").arg(0.0, 9, 'f', round);
        }
        str = strVal;
        str += " ";
        str += pTag->getUnit();
        ui->sampler_temp->setText(str);
    }
    else
    if(name == "SAM00_COMM")
    {
        pTag = m_TagMap[TmsTagNameList[8]];
        float val = pTag->getValue();
        if(mpCommTag[8]->getValue() == 0)  //통신불량이면
            val = 0.0;

        int round = pTag->getRoundUp();
        if(round > 0)
            round--;
        strVal = QString("%1").arg(val, 9, 'f', round);
        ui->sampler_temp->setText(strVal);
    }
    else
    if(name == "SAM00_POS")
        ui->sampler_pos->setText(strVal);
    else
    if(name == "DATETIME")
        ui->datetime->setText(strVal);
    /*
    else
    if(name == "SAM00_RUN")
    {
        double val = pTag->getValue();
        str = "대기중";
        if( val == 1)
            str = "작동중";
        if( val == 2)
            str = "실패";
//        ui->sampler_run->setText(str);
    }
//    else
//    if(name == "SAM00_DOOR")
//        ui->sampler_door->setText(strVal);
//    else
//    if(name == "SAM00_TIME")
//        ui->sampler_time->setText(strVal);
    else
    if(name == "DORON")
    {
        if(pTag->getValue() == 1)
            ui->DORON->setStyleSheet(ON_LABEL);
        else
            ui->DORON->setStyleSheet(OFF_LABEL);
    }
    else
    if(name == "PWRON")
    {
        if(pTag->getValue() == 1)
            ui->PWRON->setStyleSheet(ON_LABEL);
        else
            ui->PWRON->setStyleSheet(OFF_LABEL);
    }
    else
    if(name == "FMRON")
    {
        if(pTag->getValue() == 1)
            ui->FMRON->setStyleSheet(ON_LABEL);
        else
            ui->FMRON->setStyleSheet(OFF_LABEL);
    }
    else
    if(name == "FMLON")
    {
        if(pTag->getValue() == 1)
            ui->FMLON->setStyleSheet(ON_LABEL);
        else
            ui->FMLON->setStyleSheet(OFF_LABEL);
    }
    else
    if(name == "TS_CONNECTED")
    {
        ui->connected->setText(strVal);
    }*/
    else
    if((index = CheckTagIndex(name)) >= 0)
    {
        int val = pTag->getValue();
        if(index < 9)
        {
            mCheckFlag[index] = val;
/*
            if(index < 6)
            {
                if(val)
                    Status[index][3]->setFrameShape(QFrame::WinPanel);
                else
                    Status[index][3]->setFrameShape(QFrame::NoFrame);
            }*/
        }
    }
}

bool TmsDialog::writeTag(QString tagname, QString value, int ndelay)
{
//    qDebug() << "writeTag";
    QJsonObject jObj;
    jObj.insert(QString("Tag"),QJsonValue(tagname));
    jObj.insert(QString("Value"),QJsonValue(value));

    QJsonArray array;
    array.append(jObj);
    QString cmd("Tag_Write");
    QByteArray ar = makeCommand(cmd, QJsonValue(array));
    if(ndelay == 0)
        g_pSrSocket->sendCommand((QWidget*)this, cmd, QString(""), ar);
    else
    {
        TAG_WRITE_DELAY *delay = new TAG_WRITE_DELAY;
        delay->Cmd = cmd;
        delay->Data = ar;
        delay->Delay = ndelay;
        m_listDelay.append(delay);
    }
    return true;
}
void TmsDialog::onTimer()
{
    if(!m_listDelay.isEmpty())
    {
        TAG_WRITE_DELAY *delay;
        delay = m_listDelay[0];
        delay->Delay--;
        if(delay->Delay <= 0)
        {
            g_pSrSocket->sendCommand((QWidget*)this, delay->Cmd, QString(""), delay->Data);
            delete delay;
            m_listDelay.takeFirst();
        }
    }
    if(++mCommCount >= 10)
    {
        if(m_bTmsInitOk == false)
        {
//            qDebug() << "send:" << mInfoRead;
            g_pSrSocket->sendCommand((QWidget*)this, mInfoCmd, GraphicId, mInfoRead);
        }
        else
        {
//            qDebug() << "send:" << mValueRead;
            g_pSrSocket->sendCommand((QWidget*)this, mValueReadCmd, GraphicId, mValueRead);
        }
        mCommCount = 0;
    }
    if(++mPassCount >= 10)
    {
        QString id;
//        qDebug() << "send:" << mPasswordRead;
        g_pSrSocket->sendCommand((QWidget*)this, mPasswordCmd, id, mPasswordRead);
        mPassCount = 0;
    }
    if(m_bMousePress == true)
    {
        if( ++m_nMousePressCount > 50)		// 5초
        {
            m_bMousePress = false;
            m_nMousePressCount = 0;
            emit sigSysMenu();
        }
    }
}

void TmsDialog::onTimerClose()
{
    m_TimerClose.stop();
    accept();
}

void TmsDialog::paintEvent(QPaintEvent*)
{
    QPainter	p(this);
    QString str;

    p.drawImage(0,0,m_imgBackGround);

    if(m_bInitOk == false)
        return;
/* yskim
    int x = 25;
    int y = 114;
    for(int i = 0; i < 6; i++)
    {
        if(g_Flag[i] == true)
        {
            p.drawImage(x,y,m_Box);
        }
        y += 57;
    }
*/
    CTag *pTag;
    QString name;
    name = "SAM00_DOOR";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1750,835, m_SamplerDoorOn);
        else
            p.drawImage(1750,835, m_SamplerDoorOff);
    }
    else
        qDebug() << "Not Found SAM00_DOOR!!!!" << name;
    name = "SAM00_RUN";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1750,895, m_SamplerRunOn);
        else
        if(pTag->getValue() == 2)
            p.drawImage(1750,895, m_SamplerRunFault);
        else
            p.drawImage(1750,895, m_SamplerRunOff);
    }
    name = "DORON";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(20, 800, m_DOROn);
        else
            p.drawImage(20, 800, m_DOROff);
    }
    name = "PWRON";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(20, 870, m_PWROn);
        else
            p.drawImage(20, 870, m_PWROff);
    }
    name = "FMLON";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(20, 940, m_FMLOn);
        else
            p.drawImage(20, 940, m_FMLOff);
    }
    name = "FMRON";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(20, 1010, m_FMROn);
        else
            p.drawImage(20, 1010, m_FMROff);
    }

    int x = 638;
    int y = 158;
    for(int i = 0; i < 9; i++)
    {
        if(g_Flag[i] == true)
        {
            if((pTag = FindTagObject(TmsCommTag[i])) != NULL)
            {
                if(pTag->getValue() == 1)
                    p.drawImage(x, y, m_CommOff);
                else
                    p.drawImage(x, y, m_CommOn);
            }
            if((pTag = FindTagObject(TmsCheckTag[i])) != NULL)
            {
                if(pTag->getValue() == 1)
                    p.drawImage(x-120, y, m_Check);
            }
        }
        if(i == 2)
        {
            x = 638;
            y += 305;
        }
        else
        if(i == 5)
        {
            x = 638;
            y += 305;
        }
        else
        {
            x += 566;
        }
    }
/*
    name = "FLW01_CHECK";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1569,234, m_CheckOn);
        else
            p.drawImage(1569,234, m_CheckOff);
    }
    name = "FLW01_COMM";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1685,234, m_CommOff);
        else
            p.drawImage(1685,234, m_CommOn);
    }
    name = "SAM00_CHECK";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1569,478, m_CheckOn);
        else
            p.drawImage(1569,478, m_CheckOff);
    }
    name = "SAM00_COMM";
    if((pTag = FindTagObject(name)) != NULL)
    {
        if(pTag->getValue() == 1)
            p.drawImage(1685,478, m_CommOff);
        else
            p.drawImage(1685,478, m_CommOn);
    }
    */
}

void TmsDialog::TextColorChange(const char* style, QLabel* label)
{
    label->setStyleSheet(style);
}

int TmsDialog::TagIndex(QString name)
{
    int index = -1;
    if(TagIndexMap.contains(name))
        index = TagIndexMap[name];
    return index;
}

int TmsDialog::StsTagIndex(QString name)
{
    int index = -1;
    if(StsTagIndexMap.contains(name))
        index = StsTagIndexMap[name];
    return index;
}

int TmsDialog::EquipStsTagIndex(QString name)
{
    int index = -1;
    if(EquipStsTagIndexMap.contains(name))
        index = EquipStsTagIndexMap[name];
    return index;
}

int TmsDialog::CheckTagIndex(QString name)
{
    int index = -1;
    if(CheckTagIndexMap.contains(name))
        index = CheckTagIndexMap[name];
    return index;
}
int TmsDialog::CommTagIndex(QString name)
{
    int index = -1;
    if(CommTagIndexMap.contains(name))
        index = CommTagIndexMap[name];
    return index;
}

void TmsDialog::handleFileChanged(const QString&)
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists())
    {
//        if(m_bUsbReady == false)
        {
            CInfomationDialog dlg("USB 메모리가 준비되었습니다");
            dlg.exec();
        }
        m_bUsbReady = true;
        if(QFile("/app/wwwroot/tmp/usbdisk/__bestec_menu__").exists())
        {
            UsbMenuDialog dlg;
            if(dlg.exec() == QDialog::Accepted)
            {
                if(dlg.bUnstoring == true)
                {
                    system("/etc/scripts/stop");
                    system("rm -r /data/*");
                    system("rm /tmp/share.db");
                    system("/dot/dotnet /app/monitor2.dll clearinstalldate");
                    system("sync");
                    CInfomationDialog mDlg("출고상태 만들기를 완료하였습니다");
                    mDlg.exec();
                }
                else
                if(dlg.bSetup == true)
                {
                    CNetworkSetup dlg(false);
                    dlg.exec();
                }
            }
        }
    }
    else
        m_bUsbReady = false;
}

void TmsDialog::on_MsgList_clicked(const QModelIndex &)
{
    if(MsgList.size())
    {
        MsgView dlg(MsgList);
        dlg.exec();
    }
}

void TmsDialog::CheckControl(CTag *pTag)
{
    QString msg;
    if(pTag->getValue())
        msg = "점검중을 해제하시겠습니까?";
    else
        msg = "점검중으로 변경하시겠습니까?";
    CQuestionDialog dlg(msg);
    if(dlg.exec() == QDialog::Accepted)
    {
        double value;
        if(pTag->getValue())
            value = 0;
        else
            value = 1;
        pTag->writeValue(value);
    }
}

void TmsDialog::on_btnTn_clicked()
{
    TmsPointDlg(tr("TN"));
}

void TmsDialog::on_btnTp_clicked()
{
    TmsPointDlg(tr("TP"));
}

void TmsDialog::on_btnCod_clicked()
{
    TmsPointDlg(tr("COD"));
}

void TmsDialog::on_btnPh_clicked()
{
    TmsPointDlg(tr("PH"));
}

void TmsDialog::on_btnSs_clicked()
{
    TmsPointDlg(tr("SS"));
}

void TmsDialog::on_btnSampler_clicked()
{
    TmsPointDlg(tr("Sampler"));
}

void TmsDialog::on_btnFlow_clicked()
{
    TmsPointDlg(tr("Flow"));
}

void TmsDialog::on_btnToc_clicked()
{
    TmsPointDlg(tr("TOC"));
}

void TmsDialog::TmsPointDlg(QString group)
{
    TmsPoint dlg(group);
    dlg.exec();
}

void TmsDialog::mousePressEvent(QMouseEvent*)
{
    m_bMousePress = true;
    m_nMousePressCount = 0;
}

void TmsDialog::mouseReleaseEvent(QMouseEvent*)
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
}

void TmsDialog::slotSysMenu()
{
    //gApp->TouchReset();
    if(PasswordCheck())
    {
        CNetworkSetup dlg(false);
        dlg.exec();
    }
}

bool TmsDialog::PasswordCheck()
{
    bool ret = false;
    TmsPassword dlg;
    if(dlg.exec() == QDialog::Accepted)
    {
        if(dlg.mInputText == mPassword)
            ret = true;
        else
        {
            CInfomationDialog dlg("비밀번호가 올바르지 않습니다");
            dlg.exec();
        }
    }
    return ret;
}

void TmsDialog::on_btnManualSampling_clicked()
{
    ManualSampling();
}

void TmsDialog::ManualSampling()
{
    if(PasswordCheck())
    {
        CQuestionDialog dlg("수동으로 채수하시겠습니까?");
        if(dlg.exec() == QDialog::Accepted)
        {
            QString cmd = QString("SAMP00[%1]").arg(ui->datetime->text());
            writeTag(QString("SAM00_CMD"), cmd, 0);
        }
    }
}

void TmsDialog::handleFileChanged2(const QString&)
{
    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
    {
        gApp->setOverrideCursor(Qt::PointingHandCursor);
    }
    else
    {
        gApp->setOverrideCursor(Qt::BlankCursor);
    }
}
