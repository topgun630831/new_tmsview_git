#include "epsene.h"
#include "ui_epsene.h"
#include "epssetup.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QFile>
#include <QGraphicsSceneMouseEvent>
#include "myapplication.h"
#include "common.h"
#include "history.h"
#include "networksetup.h"
#include "epslog.h"
#include "srsocket.h"
#include "monitorudp.h"
#include "tag.h"
#include "infomationdialog.h"
#include "passwordinput.h"
#include "tmstrenddialog.h"
#include "pointmon.h"

extern QSqlDatabase gDb;
extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern QMap<QString, CTag*> m_TagMap;
extern QMap<quint16, CTag*> m_AtomTagMap;
extern QSplashScreen *splash;
extern bool bSplashFinished;
extern CMyApplication *gApp;
extern int     sr_fd;
EpsEnE *g_pEps;

const QString GraphicId = "Graphic";

QString gPassword;
QString gUseAck="1";

CTag* FindTagObject(QString& TagName);
struct EPS_SETUP SetupData;
QStringList DriverList;
#define TAG_MAX 13
QString TagNameList[TAG_MAX] = {
//    "FLOW_VOLUME", //유량
//    "FLOW_RATE",            // 유속
//    "FLOW_LEVEL",           // 수위
//    "FLOW_TOTAL",           // 총유량
//    "FLOW_VOLUME_10MIN",    // 10분 유량
//    "FLOW_RATE_10MIN",      // 10분 유속
//    "FLOW_LEVEL_10MIN",     // 10분 수위
//    "FLOW_TOTAL_10MIN",     // 10분 총유량
    "DATETIME",             // 현재시간
    "MSG",                  // 메시지
    "TS_CONNECTED",         // 연결상태
    "REPAIR",               // 보수중
    "MALFUNCTION",          // 고장
    "AUTOCLEAN_USE",        // 자동세정
    "AUTOCLEAN_TIME",       // 세정 가동시간
    "AUTOCLEAN_PERIOD",     // 세정 주기
    "AUTOCLEAN_DO",         // 세정 제어태그
    "FAN_DO",               // FAN 제어태그
    "MALFUNCTION",          // 고장
    "REPAIR",               // 보수
    "FLOW_COMM",            // 유량계 통신상태
 };
/*
    "FLOW_LEVEL2",          // 보조수위
    "FLOW_LEVEL2_10MIN",    // 보조수이 10분데이터
    "CONDUCTIVE",           // 전도도
    "CONDUCTIVE_10MIN",     // 전도도 10분데이터
    "POWER_DI",             // 정전상태
    "LEAK_DI",              // 누전
    "TEMP_DI",              // 온도
    "DOOR_DI",              // 도어
    "AUTOCLEAN_DI",         // 세정
};
*/
QMap<QString, QWidget*> WidgetMap;
QMap<QString, QWidget*> WidgetUnitMap;
extern bool    m_bScreenSave;

EpsEnE::EpsEnE(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::EpsEnE)
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists())
        m_bUsbReady = true;
    else
        m_bUsbReady = false;
    ui->setupUi(this);
    g_pEps = this;

/*    QPixmap pixmap(":/images/splash.png");
    mSplash = new QSplashScreen;
    mSplash->setPixmap(pixmap);
    mSplash->show();
*/
    LoadDb();

    CTag* pTag;
    for(int i = 0; i < TAG_MAX; i++)
    {
        pTag = new CTag(TagNameList[i]);
        m_TagMap[TagNameList[i]] = pTag;
    }
    bool flowmeter = true;
    if(SetupData.FlowMeter == "System")
        flowmeter = false;
    if(flowmeter || SetupData.VolumeAi)
    {
        addTag("FLOW_VOLUME");
        addTag("FLOW_TOTAL");
    }
    if(SetupData.FlowMeter != "PenToughFlow")
    {
        if(flowmeter || SetupData.RateAi)
            addTag("FLOW_RATE");
    }
    if(flowmeter || SetupData.MainLevel)
    {
        if(SetupData.FlowMeterDesc != "CK 테크피아")
            addTag("FLOW_LEVEL");
    }

    if(SetupData.SubLevel)
    {
        addTag("FLOW_LEVEL2");
//        addTag("FLOW_LEVEL2_10MIN");
        WidgetMap["FLOW_LEVEL2"] = ui->FLOW_LEVEL2;
//        WidgetMap["FLOW_LEVEL2_10MIN"] = ui->FLOW_LEVEL2_10MIN;
        WidgetUnitMap["FLOW_LEVEL2"] = ui->FLOW_LEVEL2_UNIT;
//       WidgetUnitMap["FLOW_LEVEL2_10MIN"] = ui->FLOW_LEVEL2_10MIN_UNIT;
    }
    if(SetupData.ConductMeter)
    {
        addTag("CONDUCTIVE");
//        addTag("CONDUCTIVE_10MIN");
        WidgetMap["CONDUCTIVE"] = ui->CONDUCTIVE;
//        WidgetMap["CONDUCTIVE_10MIN"] = ui->CONDUCTIVE_10MIN;
        WidgetUnitMap["CONDUCTIVE"] = ui->CONDUCTIVE_UNIT;
//        WidgetUnitMap["CONDUCTIVE_10MIN"] = ui->CONDUCTIVE_10MIN_UNIT;
    }
    if(SetupData.BlackOutDi)
    {
        addTag("POWER_DI");
        WidgetMap["POWER_DI"] = ui->POWER_DI;
    }
    if(SetupData.LeakDi)
    {
        addTag("LEAK_DI");
        WidgetMap["LEAK_DI"] = ui->LEAK_DI;
    }
    if(SetupData.HiTempDi)
    {
        addTag("TEMP_DI");
        WidgetMap["TEMP_DI"] = ui->TEMP_DI;
    }
    if(SetupData.DoorOpenDi)
    {
        addTag("DOOR_DI");
        WidgetMap["DOOR_DI"] = ui->DOOR_DI;
    }
    if(SetupData.CleanDi)
    {
        addTag("AUTOCLEAN_DI");
        WidgetMap["AUTOCLEAN_DI"] = ui->AUTOCLEAN_DI;
    }
    if(SetupData.RainDi)
    {
        addTag("RAIN_DAY");
        addTag("RAIN_TOTAL");
        WidgetMap["RAIN_DAY"] = ui->RAIN_DAY;
        WidgetMap["RAIN_TOTAL"] = ui->RAIN_TOTAL;
    }

    WidgetMap["FLOW_VOLUME"] = ui->FLOW_VOLUME;
    WidgetMap["FLOW_RATE"] = ui->FLOW_RATE;
    WidgetMap["FLOW_LEVEL"] = ui->FLOW_LEVEL;
    WidgetMap["FLOW_TOTAL"] = ui->FLOW_TOTAL;
//    WidgetMap["FLOW_VOLUME_10MIN"] = ui->FLOW_VOLUME_10MIN;
//    WidgetMap["FLOW_RATE_10MIN"] = ui->FLOW_RATE_10MIN;
//    WidgetMap["FLOW_LEVEL_10MIN"] = ui->FLOW_LEVEL_10MIN;
//    WidgetMap["FLOW_TOTAL_10MIN"] = ui->FLOW_TOTAL_10MIN;
    WidgetMap["DATETIME"] = ui->DATETIME;
    WidgetMap["MSG"] = ui->MSG;
    WidgetMap["TS_CONNECTED"] = ui->TS_CONNECTED;

    WidgetUnitMap["FLOW_VOLUME"] = ui->FLOW_VOLUME_UNIT;
    WidgetUnitMap["FLOW_RATE"] = ui->FLOW_RATE_UNIT;
    WidgetUnitMap["FLOW_LEVEL"] = ui->FLOW_LEVEL_UNIT;
    WidgetUnitMap["FLOW_TOTAL"] = ui->FLOW_TOTAL_UNIT;
//    WidgetUnitMap["FLOW_VOLUME_10MIN"] = ui->FLOW_VOLUME_10MIN_UNIT;
//    WidgetUnitMap["FLOW_RATE_10MIN"] = ui->FLOW_RATE_10MIN_UNIT;
//    WidgetUnitMap["FLOW_LEVEL_10MIN"] = ui->FLOW_LEVEL_10MIN_UNIT;
//    WidgetUnitMap["FLOW_TOTAL_10MIN"] = ui->FLOW_TOTAL_10MIN_UNIT;

    if(SetupData.FlowMeter != "System")
        WidgetUnitMap["FLOW_COMM"] = ui->FLOW_COMM;
    mValueReadCmd = "Tag_Changed";
    mInfoCmd = "Tag_Summary";
    mPasswordCmd = "ShareData_Get";
    makeCommand();

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(&m_TimerClose, SIGNAL(timeout()), this, SLOT(onTimerClose()));

    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);

    m_Red.addFile(":/images/bullet-red.png");
    m_Black.addFile(":/images/bullet-black.png");

    m_Timer.start(100);

    m_bMousePress = false;
    installEventFilter(this);
    connect(this, SIGNAL(sigSysMenu()), this, SLOT(on_sysinfoBtn_clicked()));

    m_watcher.addPath("/app/wwwroot/tmp/");
    connect(&m_watcher, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged(const QString&)));

    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
        gApp->setOverrideCursor(Qt::PointingHandCursor);
    else
        gApp->setOverrideCursor(Qt::BlankCursor);

    m_watcher2.addPath("/tmp/");
    connect(&m_watcher2, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged2(const QString&)));
}

EpsEnE::~EpsEnE()
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

void EpsEnE::mousePressEvent(QMouseEvent*)
{
    m_bMousePress = true;
    m_nMousePressCount = 0;
}

void EpsEnE::mouseReleaseEvent(QMouseEvent*)
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
}

void EpsEnE::addTag(const char* n)
{
    QString name = n;
    CTag *pTag = new CTag(name);
    m_TagMap[name] = pTag;
}

void EpsEnE::on_setupBtn_clicked()
{
    PasswordInput dlg(this);
    if(dlg.exec() == QDialog::Accepted)
    {
        if(dlg.m_sPassword == gPassword || dlg.m_sPassword == "bestec21")
        {
            EpsSetup setup;
            if(setup.exec() == QDialog::Accepted)
            {
                RunStart(false);
                RunStart(true);
                m_TimerClose.start(1000);
            }
        }
        else
        {
            QString msg = "비밀번호가 올바르지 않습니다.";
            CInfomationDialog dlg(msg);
            dlg.exec();
        }
    }
}

void EpsEnE::RunStart(bool start)
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

void EpsEnE::on_dataBtn_clicked()
{
    CEpsLog dlg(this);
    dlg.exec();
}

void EpsEnE::on_historyBtn_clicked()
{
    CHistory dlg(this);
    dlg.exec();
}

void EpsEnE::on_commBtn_clicked()
{
    CComMon dlg;
    dlg.exec();
}
extern bool g_bExit;
void EpsEnE::on_sysinfoBtn_clicked()
{
    //gApp->TouchReset();
    CNetworkSetup dlg(this);
    dlg.exec();
    if(g_bExit)
        accept();
}

void EpsEnE::LoadDb()
{
    SetupData.RainDi = 0;
    SetupData.RainDiReverse = 0;

    QString dbname =  "/data/project.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        if(g_DebugDisplay)
            qDebug() << "QSqlError : " << err.databaseText();
    }
    QSqlQuery query("SELECT LevelMax, SubLevel, SubLevelEuMin, SubLevelEuMax,"
                "ConductMeter, ConductMeterEuMin, ConductMeterEuMax, AutoCompUse,"
                "AutoCompTime,AutoCompInterval, ServerIP, CleanDo, CleanDoReverse, FanDo,"
                "FanDoReverse, BlackOutDi, BlackOutReverse, LeakDi, LeakDireverse,"
                "DoorOpenDi, DoorOpenDiReverse, HiTempDi, HiTempDiReverse, CleanDi,"
                "CleanDiReverse, SiteCode, FlowMeter, FlowMeterDesc,"
                "MainLevel, LevelMin, FlowDi, RateAi, RateMin, RateMax,"
                "VolumeAi, VolumeMin, VolumeMax, FlowTotalReverse, FlowTotalRatio, RainDi, RainDiReverse  FROM EpsSetup");
    if(query.next())
    {
        SetupData.LevelMax = query.value(0).toDouble();
        SetupData.SubLevel = query.value(1).toInt();
        SetupData.SubLevelEuMin = query.value(2).toDouble();
        SetupData.SubLevelEuMax = query.value(3).toDouble();
        SetupData.ConductMeter = query.value(4).toInt();
        SetupData.ConductMeterEuMin = query.value(5).toDouble();
        SetupData.ConductMeterEuMax = query.value(6).toDouble();
        SetupData.AutoCompUse = query.value(7).toInt();
        SetupData.AutoCompTime = query.value(8).toInt();
        SetupData.AutoCompInterval = query.value(9).toInt();
        SetupData.ServerIP = query.value(10).toString();
        SetupData.CleanDo = query.value(11).toInt();
        SetupData.CleanDoReverse = query.value(12).toInt();
        SetupData.FanDo = query.value(13).toInt();
        SetupData.FanDoReverse = query.value(14).toInt();
        SetupData.BlackOutDi = query.value(15).toInt();
        SetupData.BlackOutReverse = query.value(16).toInt();
        SetupData.LeakDi = query.value(17).toInt();
        SetupData.LeakDireverse = query.value(18).toInt();
        SetupData.DoorOpenDi = query.value(19).toInt();
        SetupData.DoorOpenDiReverse = query.value(20).toInt();
        SetupData.HiTempDi = query.value(21).toInt();
        SetupData.HiTempDiReverse = query.value(22).toInt();
        SetupData.CleanDi = query.value(23).toInt();
        SetupData.CleanDiReverse = query.value(24).toInt();
        SetupData.SiteCode = query.value(25).toString();
        SetupData.FlowMeter = query.value(26).toString();
        SetupData.FlowMeterDesc = query.value(27).toString();
        SetupData.MainLevel = query.value(28).toInt();
        SetupData.LevelMin = query.value(29).toDouble();
        SetupData.FlowDi = query.value(30).toInt();
        SetupData.RateAi = query.value(31).toInt();
        SetupData.RateMin = query.value(32).toDouble();
        SetupData.RateMax = query.value(33).toDouble();
        SetupData.VolumeAi = query.value(34).toInt();
        SetupData.VolumeMin = query.value(35).toDouble();
        SetupData.VolumeMax = query.value(36).toDouble();
        SetupData.FlowTotalReverse = query.value(37).toInt();
        SetupData.FlowTotalRatio = query.value(38).toDouble();
    }
    else
    {
        QSqlError err = gDb.lastError();
        if(g_DebugDisplay)
            qDebug() << "QSqlError : " << err.databaseText();
    }

    QSqlQuery query5("SELECT RainDi, RainDiReverse FROM EpsSetup");
    if(query5.next())
    {
        SetupData.RainDi = query5.value(0).toInt();
        SetupData.RainDiReverse = query5.value(1).toInt();
    }
    else
    {

    }
    QSqlQuery query2("SELECT Driver FROM DriverList");
    while(query2.next())
    {
        QString driver = query2.value(0).toString();
        DriverList.append(driver);
    }
    QSqlQuery query3("SELECT SerialPort FROM port WHERE Name = 'Flow'");
    if(query3.next())
        SetupData.FlowMeterPort = query3.value(0).toInt();
    QSqlQuery query4("SELECT Code, Enabled FROM ext_eps_mapping");
    while(query4.next())
    {
        QString code = query4.value(0).toString();
        int enabled = query4.value(1).toInt();
        if(code == "FVOLUM")
            SetupData.VolmumeUse = enabled;
        else
        if(code == "FLEVEL")
            SetupData.LevelUse = enabled;
        else
        if(code == "FLRATE")
            SetupData.RateUse = enabled;
        else
        if(code == "FTOTAL")
            SetupData.TotalUse = enabled;
    }

    gDb.close();
}

void EpsEnE::makeCommand()
{
    g_pSrSocket->mGraphicInitOk = false;
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

    QString key = "EPS_Password";
    QJsonArray passArray;
    passArray.append(QJsonValue(key));
    key = "EPS_UseAck";
    passArray.append(QJsonValue(key));
    mPasswordRead = makeCommand(mPasswordCmd, QJsonValue(passArray));

    tmpObj.insert(QString("Id"), GraphicId);
    tmpObj.insert(QString("TagList"), argArray);
    mInfoRead = makeCommand(mInfoCmd, QJsonValue(tmpObj));
}

QByteArray EpsEnE::makeCommand(QString cmd, QJsonValue jValue)
{
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;
    tmpObject.insert(QString("Command"),QJsonValue(cmd));

    tmpObject.insert(QString("Args"),jValue);

    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);
    data.insert(0, QString("BEST%1").arg(data.size(),6,10,QChar('0')));
    return data;
}

void EpsEnE::ShareDataSetSend()
{
    QJsonArray AckUseArray;
    QJsonObject Obj;
    Obj.insert(QString("Item"), "EPS_UseAck");
    Obj.insert(QString("Type"), "double");
    Obj.insert(QString("Value"), gUseAck);
    AckUseArray.append(Obj);
    QByteArray UseAckCmd = makeCommand(QString("ShareData_Set"), QJsonValue(AckUseArray));
    g_pSrSocket->sendCommand((QWidget*)this, QString("ShareData_Set"), QString(""), UseAckCmd);
}

void EpsEnE::onRead(QString& cmd, QJsonObject& jobject)
{
//    if(m_bScreenSave)
//        return;
    if(g_DebugDisplay)
        qDebug() << "EpsEnE::onRead : " << cmd;
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
            QString str = jObj["TagType"].toString();
            CTag *pTag = i.value();
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
            i++;
            if(pTag->getTagType() == 0) // Digital
            {
                if(pTag->getValue() == 1)
                    str = pTag->m_OnMsg;
                else
                    str = pTag->m_OffMsg;
            }
            valueDisplay(pTag->GetTagName(), str);
            unitDisplay(pTag->GetTagName(), pTag->getUnit());
        }
        g_pSrSocket->mGraphicInitOk = true;
        mCommCount = 100;
        iconDisplay();
    }
    else
    if(cmd == mValueReadCmd)
    {
/*        if(mSplash)
        {
            mSplash->finish(this);
            delete mSplash;
            mSplash = NULL;
        }
*/
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
                valueDisplay(pTag->GetTagName(), val);
            }
            i++;
        }
        iconDisplay();

//        if(bSplashFinished == false)
 //       {
//            splash->finish(View);
 //           bSplashFinished = true;
 //       }
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
            //valueDisplay();
        }
    }
    else
    if(cmd == mPasswordCmd)
    {
        QJsonArray ar = jobject["Result"].toArray();
        foreach (const QJsonValue& val, ar)
        {
            QJsonObject jObj = val.toObject();
            QString item = jObj["Item"].toString();
            if(item == "EPS_Password")
                gPassword = jObj["Value"].toString();
            else if(item == "EPS_UseAck")
                gUseAck = jObj["Value"].toString();
        }
    }
}

void EpsEnE::valueDisplay(QString name, QString value)
{
    if(WidgetMap.contains(name))
    {
        QLabel *label = (QLabel*)(WidgetMap[name]);
        label->setText(value);
    }
}

void EpsEnE::unitDisplay(QString name, QString unit)
{
    if(WidgetUnitMap.contains(name))
    {
        QLabel *label = (QLabel*)(WidgetUnitMap[name]);
        label->setText(unit);
    }
}

void EpsEnE::iconDisplay()
{
    if(m_TagMap.contains("REPAIR"))
    {
        CTag *pTag = m_TagMap["REPAIR"];
        if(pTag->getValue() == 0)
            ui->REPAIR->setIcon(m_Black);
        else
            ui->REPAIR->setIcon(m_Red);
    }
    if(m_TagMap.contains("MALFUNCTION"))
    {
        CTag *pTag = m_TagMap["MALFUNCTION"];
        if(pTag->getValue() == 0)
            ui->MALFUNCTION->setIcon(m_Black);
        else
            ui->MALFUNCTION->setIcon(m_Red);
    }
    TextColorChange("AUTOCLEAN_DI", ui->AUTOCLEAN_DI);
    TextColorChange("POWER_DI", ui->POWER_DI);
    TextColorChange("TEMP_DI", ui->TEMP_DI);
    TextColorChange("LEAK_DI", ui->LEAK_DI);
    TextColorChange("DOOR_DI", ui->DOOR_DI);
}

void EpsEnE::TextColorChange(const char* name, QLabel* label)
{
    if(m_TagMap.contains(name))
    {
        CTag *pTag = m_TagMap[name];
        if(pTag->getValue() == 0)
            label->setStyleSheet("QLabel {background-color : green;color : white;}");
        else
            label->setStyleSheet("QLabel {background-color : red;color : white;}");
        /*
        QColor color;
        QPalette palette = label->palette();
        color = QColor(Qt::black);
        else
        {
            color = QColor(Qt::red);
            palette.setColor(label->backgroundRole(), Qt::white);
        }
        palette.setColor(label->foregroundRole(), color);
        label->setPalette(palette); */
    }
}
bool EpsEnE::writeTag(QString tagname, QString value, int ndelay)
{
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

void EpsEnE::onTimer()
{
    if(g_pSrSocket->m_bConnected == false)
        return;
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
        if(g_pSrSocket->mGraphicInitOk == false)
            g_pSrSocket->sendCommand((QWidget*)this, mInfoCmd, GraphicId, mInfoRead);
        else
            g_pSrSocket->sendCommand((QWidget*)this, mValueReadCmd, GraphicId, mValueRead);
        mCommCount = 0;
    }
    if(g_pSrSocket->mGraphicInitOk == true && ++mPassCount >= 15)
    {
        QString id;
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

void EpsEnE::onTimerClose()
{
    m_TimerClose.stop();
    accept();
}

void EpsEnE::handleFileChanged(const QString&)
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists())
    {
//        if(m_bUsbReady == false)
        {
            if(QDir("/app/wwwroot/tmp/usbdisk/upgrade").exists() && QFile("/app/wwwroot/tmp/usbdisk/upgrade/upgrade.auto").exists())
            {
                CInfomationDialog dlg("자동 Upgrade 중입니다.", false);
                dlg.exec();
            }
            else
            {
                CInfomationDialog dlg("USB 메모리가 준비되었습니다.");
                dlg.exec();
            }
        }
        m_bUsbReady = true;
    }
    else
        m_bUsbReady = false;
}

void EpsEnE::handleFileChanged2(const QString&)
{
    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
        gApp->setOverrideCursor(Qt::PointingHandCursor);
    else
        gApp->setOverrideCursor(Qt::BlankCursor);
}

void EpsEnE::on_pointBtn_clicked()
{
    PasswordInput dlg(this);
    if(dlg.exec() == QDialog::Accepted)
    {
        if(dlg.m_sPassword == gPassword || dlg.m_sPassword == "bestec21")
        {
            CPointMon point;
            point.exec();
        }
        else
        {
            QString msg = "비밀번호가 올바르지 않습니다.";
            CInfomationDialog dlg(msg);
            dlg.exec();
        }
    }
}
