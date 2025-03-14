#include "epssetup.h"
#include "ui_epssetup.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "compsetup.h"
#include "diselect.h"
#include "doselect.h"
#include "aiselect.h"
#include "deviceselect.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "tag.h"
#include "epsorder.h"
#include "analogset.h"

extern QSqlDatabase gDb;
extern struct EPS_SETUP SetupData;
extern QStringList DriverList;
extern bool g_DebugDisplay;
extern QMap<QString, CTag*> m_TagMap;
extern bool    m_bScreenSave;

EpsSetup::EpsSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpsSetup)
{
    ui->setupUi(this);
    SetupCopy(&SetupData, &WorkData);
    InitDisp();
    startTimer(1000);
}

EpsSetup::~EpsSetup()
{
    delete ui;
}

void EpsSetup::InitDisp()
{
    ui->serverIp->setText(WorkData.ServerIP);
    ui->siteCode->setText(WorkData.SiteCode);
    ui->btnFlowmeter->setText(WorkData.FlowMeterDesc);
    ui->levelMax->setText(QString().number(WorkData.LevelMax));
    ui->levelMin->setText(QString().number(WorkData.LevelMin));
    ui->subMin->setText(QString().number(WorkData.SubLevelEuMin));
    ui->subMax->setText(QString().number(WorkData.SubLevelEuMax));

    bool use;
    LabelDisp(WorkData.SubLevel, ui->btnSubLevel, "AI");
    if(WorkData.SubLevel == 0)
    {
        ui->subMin->setEnabled(false);
        ui->subMax->setEnabled(false);
    }

    ui->condMin->setText(QString().number(WorkData.ConductMeterEuMin));
    ui->condMax->setText(QString().number(WorkData.ConductMeterEuMax));

    LabelDisp(WorkData.ConductMeter, ui->btnCond, "AI");
    if(WorkData.ConductMeter == 0)
    {
        ui->condMin->setEnabled(false);
        ui->condMax->setEnabled(false);
    }

    if(WorkData.AutoCompUse == 0)
        use = false;
    else
        use = true;

    LabelDisp(WorkData.CleanDo, ui->btnCompDo, "DO");

    use = (WorkData.CleanDoReverse == 0) ? false : true;
    ui->compDoRev->setChecked(use);

    LabelDisp(WorkData.FanDo, ui->btnFan, "DO");

    use = (WorkData.FanDoReverse == 0) ? false : true;
    ui->fanRev->setChecked(use);

    LabelDisp(WorkData.BlackOutDi, ui->btnBreakOut, "DI");

    use = (WorkData.BlackOutReverse == 0) ? false : true;
    ui->breakOutRev->setChecked(use);

    LabelDisp(WorkData.LeakDi, ui->btnLeak, "DI");

    use = (WorkData.LeakDireverse == 0) ? false : true;
    ui->leakRev->setChecked(use);

    LabelDisp(WorkData.DoorOpenDi, ui->btnDoorOpen, "DI");

    use = (WorkData.DoorOpenDiReverse == 0) ? false : true;
    ui->doorDiRev->setChecked(use);

    LabelDisp(WorkData.HiTempDi, ui->btnHiTemp, "DI");

    use = (WorkData.HiTempDiReverse == 0) ? false : true;
    ui->hiTempRev->setChecked(use);

    LabelDisp(WorkData.CleanDi, ui->btnCompDi, "DI");

    use = (WorkData.CleanDiReverse == 0) ? false : true;
    ui->compDiRev->setChecked(use);


//    ui->btnRainDi->setVisible(false);
//    ui->rainDiRev->setVisible(false);
//    ui->labelRain->setVisible(false);
    LabelDisp(WorkData.RainDi, ui->btnRainDi, "DI");

    use = (WorkData.RainDiReverse == 0) ? false : true;
    ui->rainDiRev->setChecked(use);

    MainLevelDisplay();

    LabelDisp(WorkData.FlowDi, ui->btnFlowTotal, "DI");
    use = (WorkData.FlowTotalReverse == 0) ? false : true;
    ui->FlowTotalRev->setChecked(use);
    ui->flowTotalRatio->setText(QString().number(WorkData.FlowTotalRatio));

    RateDisplay();
    ui->rateMin->setText(QString().number(WorkData.RateMin));
    ui->rateMax->setText(QString().number(WorkData.RateMax));

    VolumeDisplay();
    ui->volumeMin->setText(QString().number(WorkData.VolumeMin));
    ui->volumeMax->setText(QString().number(WorkData.VolumeMax));

    DriverDisplay();

    if(WorkData.FlowMeter != "System")
        ui->btnFlowTotalWrite->setVisible(false);

    ui->FlowmeterPort->setCurrentIndex(WorkData.FlowMeterPort);
    ui->volumeUse->setChecked((bool)WorkData.VolmumeUse);
    ui->levelUse->setChecked((bool)WorkData.LevelUse);
    ui->rateUse->setChecked((bool)WorkData.RateUse);
    ui->totalUse->setChecked((bool)WorkData.TotalUse);
//    if(!WorkData.RainDi)
    {
        ui->btnRainTotalWrite->setVisible(false);
            ui->btnRainDayWrite->setVisible(false);
    }
}

void EpsSetup::UpdateDb()
{
    QString dbname =  "/data/project.db";
    QString str = QString("UPDATE EpsSetup SET LevelMax=%1,SubLevel=%2,"
      "SubLevelEuMin=%3,SubLevelEuMax=%4,"
      "ConductMeter=%5,ConductMeterEuMin=%6,ConductMeterEuMax=%7,"
      "AutoCompUse=%8,AutoCompTime=%9,AutoCompInterval=%10,"
      "ServerIP='%11',CleanDo=%12,CleanDoReverse=%13,"
      "FanDo=%14,FanDoReverse=%15,BlackOutDi=%16,BlackOutReverse=%17,"
      "LeakDi=%18,LeakDireverse=%19,DoorOpenDi=%20,DoorOpenDiReverse=%21,"
      "HiTempDi=%22,HiTempDiReverse=%23,CleanDi=%24,CleanDiReverse=%25,"
      "SiteCode='%26', FlowMeter='%27', FlowMeterDesc='%28',"
      "MainLevel=%29,LevelMin=%30,FlowDi=%31,"
      "RateAi=%32,RateMin=%33,RateMax=%34,"
      "VolumeAi=%35,VolumeMin=%36,VolumeMax=%37,FlowTotalReverse=%38,FlowTotalRatio=%39,"
      "RainDi=%40,RainDiReverse=%41")
      .arg(WorkData.LevelMax).arg(WorkData.SubLevel)
      .arg(WorkData.SubLevelEuMin).arg(WorkData.SubLevelEuMax)
      .arg(WorkData.ConductMeter).arg(WorkData.ConductMeterEuMin).arg(WorkData.ConductMeterEuMax)
      .arg(WorkData.AutoCompUse).arg(WorkData.AutoCompTime).arg(WorkData.AutoCompInterval)
      .arg(WorkData.ServerIP).arg(WorkData.CleanDo).arg(WorkData.CleanDoReverse)
      .arg(WorkData.FanDo).arg(WorkData.FanDoReverse).arg(WorkData.BlackOutDi).arg(WorkData.BlackOutReverse)
      .arg(WorkData.LeakDi).arg(WorkData.LeakDireverse).arg(WorkData.DoorOpenDi).arg(WorkData.DoorOpenDiReverse)
      .arg(WorkData.HiTempDi).arg(WorkData.HiTempDiReverse).arg(WorkData.CleanDi).arg(WorkData.CleanDiReverse)
      .arg(WorkData.SiteCode).arg(WorkData.FlowMeter).arg(WorkData.FlowMeterDesc)
      .arg(WorkData.MainLevel).arg(WorkData.LevelMin).arg(WorkData.FlowDi)
      .arg(WorkData.RateAi).arg(WorkData.RateMin).arg(WorkData.RateMax)
      .arg(WorkData.VolumeAi).arg(WorkData.VolumeMin).arg(WorkData.VolumeMax)
      .arg(WorkData.FlowTotalReverse).arg(WorkData.FlowTotalRatio).arg(WorkData.RainDi).arg(WorkData.RainDiReverse);
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
        ErrDisp();
    RainTagCheck();

    QSqlQuery query(str);
    query.exec();
    str = QString("UPDATE Ext_Eps SET SiteCode='%1', UsingAutoClean=%2, AutoCleanPeriod=%3, AutoCleanTime=%4, FlowLevelLimit=%5")
            .arg(WorkData.SiteCode).arg(WorkData.AutoCompUse).
            arg(WorkData.AutoCompInterval).arg(WorkData.AutoCompTime).arg(WorkData.LevelMax);
    QSqlQuery query2(str);
    query2.exec();

    MappingUpdate(WorkData.SubLevel, "FLOW_LEVEL2");
    MappingUpdate(WorkData.ConductMeter, "CONDUCTIVE");
    AddressUpdate(WorkData.SubLevel, "FLOW_LEVEL2", "ai");
    AddressUpdate(WorkData.ConductMeter, "CONDUCTIVE", "ai");
    AiEuUpdate("FLOW_LEVEL2", WorkData.SubLevelEuMin, WorkData.SubLevelEuMax);
    AiEuUpdate("CONDUCTIVE", WorkData.ConductMeterEuMin, WorkData.ConductMeterEuMax);

    MappingUpdate(WorkData.CleanDo, "AUTOCLEAN_DO");
    MappingUpdate(WorkData.FanDo, "FAN_DO");
    AddressUpdate(WorkData.CleanDo, "AUTOCLEAN_DO", "do");
    AddressUpdate(WorkData.FanDo, "FAN_DO", "do");
    DigitalRevUpdate("AUTOCLEAN_DO", WorkData.CleanDoReverse);
    DigitalRevUpdate("FAN_DO", WorkData.FanDoReverse);

    MappingUpdate(WorkData.BlackOutDi, "POWER_DI");
    MappingUpdate(WorkData.LeakDi, "LEAK_DI");
    MappingUpdate(WorkData.DoorOpenDi, "DOOR_DI");
    MappingUpdate(WorkData.HiTempDi, "TEMP_DI");
    MappingUpdate(WorkData.CleanDi, "AUTOCLEAN_DI");
    MappingUpdate(WorkData.RainDi, "RAIN_DAY");

    AddressUpdate(WorkData.BlackOutDi, "POWER_DI","di");
    AddressUpdate(WorkData.LeakDi, "LEAK_DI", "di");
    AddressUpdate(WorkData.DoorOpenDi, "DOOR_DI", "di");
    AddressUpdate(WorkData.HiTempDi, "TEMP_DI", "di");
    AddressUpdate(WorkData.CleanDi, "AUTOCLEAN_DI", "di");
    QString address;
    QString datatype="R32";
    if(WorkData.RainDi != 0)
    {
        if(WorkData.RainDiReverse)
            address = QString("rcounter%1").arg(WorkData.RainDi);
        else
            address = QString("counter%1").arg(WorkData.RainDi);
        AiDriverUpdate("RAIN_TOTAL", "io", address, 1, datatype, "RW");
    }

    DigitalRevUpdate("POWER_DI", WorkData.BlackOutReverse);
    DigitalRevUpdate("LEAK_DI", WorkData.LeakDireverse);
    DigitalRevUpdate("DOOR_DI", WorkData.DoorOpenDiReverse);
    DigitalRevUpdate("TEMP_DI", WorkData.HiTempDiReverse);
    DigitalRevUpdate("AUTOCLEAN_DI", WorkData.CleanDiReverse);
    //if(WorkData.RainDi != 0)
    //    DigitalRevUpdate("RAIN_DI", WorkData.RainDiReverse);

    WorkData.VolmumeUse = (int)ui->volumeUse->isChecked();
    WorkData.LevelUse = (int)ui->levelUse->isChecked();
    WorkData.RateUse = (int)ui->rateUse->isChecked();
    WorkData.TotalUse = (int)ui->totalUse->isChecked();

    for (int i = 0; i < DriverList.size(); ++i) {
        int enabled = 0;
        QString driver = DriverList.at(i);
        if (driver == WorkData.FlowMeter || driver == "System")
            enabled = 1;
        DriverUpdate(driver, enabled);
    }
    ModbusDriverUpdate();

    if(WorkData.FlowMeter == "System")
    {
        int enabled = 1;
        if(WorkData.FlowTotalReverse)
            address = QString("rcounter%1").arg(WorkData.FlowDi);
        else
            address = QString("counter%1").arg(WorkData.FlowDi);
        AiDriverUpdate("FLOW_TOTAL", "io", address, WorkData.FlowTotalRatio, datatype, "RW");
        if(WorkData.FlowDi == 0)
            enabled = 0;
        EnableUpdate("FLOW_TOTAL", enabled);
        enabled = 1;
        if(WorkData.VolumeAi == 0)
            enabled = 0;
        EnableUpdate("FLOW_VOLUME", enabled);
//        MappingUpdate(enabled, "FLOW_VOLUME");
//        MappingUpdate(enabled, "FLOW_TOTAL");
        enabled = 1;
        if(WorkData.RateAi == 0)
            enabled = 0;
        EnableUpdate("FLOW_RATE", enabled);
//        MappingUpdate(enabled, "FLOW_RATE");
        enabled = 1;
        if(WorkData.MainLevel == 0)
            enabled = 0;
        EnableUpdate("FLOW_LEVEL", enabled);
//        MappingUpdate(enabled, "FLOW_LEVEL");

        address = QString("ai%1").arg(WorkData.VolumeAi);
        AiDriverUpdate("FLOW_VOLUME", "io", address, 0, datatype);
        address = QString("ai%1").arg(WorkData.RateAi);
        AiDriverUpdate("FLOW_RATE", "io", address, 0, datatype);
        address = QString("ai%1").arg(WorkData.MainLevel);
        AiDriverUpdate("FLOW_LEVEL", "io", address, 0, datatype);

        AiEuUpdate("FLOW_LEVEL", WorkData.LevelMin, WorkData.LevelMax);
        AiEuUpdate("FLOW_RATE", WorkData.RateMin, WorkData.RateMax);
        AiEuUpdate("FLOW_VOLUME", WorkData.VolumeMin, WorkData.VolumeMax);
    }
    else
    {
        float ratio = 1;
        address = "유량";
        if(WorkData.FlowMeterDesc == "Nivus Lan")
        {
            datatype = "S16";
            address = "30001";
        }
        else
        if(WorkData.FlowMeterDesc == "Raven-Eye")
        {
            datatype = "R32";
            address = "40003";
        }
        else
        if(WorkData.FlowMeterDesc == "CK 테크피아")
        {
            datatype = "R32";
            address = "34113";
        }
        if(WorkData.FlowMeterDesc == "Nivus")
            ratio = 3600;
        else
        if(WorkData.FlowMeterDesc == "Nivus Lan" || WorkData.FlowMeterDesc == "수인테크")
            ratio = 3.6;
        else
            ratio = 1;
        AiDriverUpdate("FLOW_VOLUME", "Flow", address, ratio, datatype);
        if(WorkData.FlowMeterDesc == "펜타포우")
        {
            WorkData.RateUse = 0;
        }
        else
        {
            address = "유속";
            if(WorkData.FlowMeterDesc == "Nivus Lan")
                address = "30003";
            else
            if(WorkData.FlowMeterDesc == "Raven-Eye")
                address = "40005";
            else
            if(WorkData.FlowMeterDesc == "CK 테크피아")
                address = "34115";

            if(WorkData.FlowMeterDesc == "Nivus" || WorkData.FlowMeterDesc == "Nivus Lan")
                ratio = 0.001;
            else
                ratio = 1;
            AiDriverUpdate("FLOW_RATE", "Flow", address, ratio, datatype);
        }
        address = "수위";
        if(WorkData.FlowMeterDesc == "Nivus Lan")
            address = "30002";
        else
        if(WorkData.FlowMeterDesc == "Raven-Eye")
            address = "40001";

        if(WorkData.FlowMeterDesc == "Raven-Eye")
            ratio = 10;
        else
        if(WorkData.FlowMeterDesc == "그린텍" || WorkData.FlowMeterDesc == "수인테크")
            ratio = 1000;
        else
            ratio = 1;
        AiDriverUpdate("FLOW_LEVEL", "Flow", address, ratio, datatype);
        address = "총유량";
        if(WorkData.FlowMeterDesc == "Nivus Lan")
        {
            datatype = "R32";
            address = "30104";
        }
        else
        if(WorkData.FlowMeterDesc == "Raven-Eye")
        {
            datatype = "U32";
            address = "40007";
        }
        else
        if(WorkData.FlowMeterDesc == "CK 테크피아")
        {
            datatype = "U32";
            address = "34121";
        }
        if(WorkData.FlowMeterDesc == "Nivus")
            ratio = 0.001;
        else
            ratio = 1;
        AiDriverUpdate("FLOW_TOTAL", "Flow", address, ratio, datatype);

        EnableUpdate("FLOW_VOLUME", 1);
        EnableUpdate("FLOW_TOTAL", 1);
        if(WorkData.FlowMeterDesc == "펜타포우")
            EnableUpdate("FLOW_RATE", 0);
        else
            EnableUpdate("FLOW_RATE", 1);
        if(WorkData.FlowMeterDesc == "CK 테크피아")
            EnableUpdate("FLOW_LEVEL", 0);
        else
            EnableUpdate("FLOW_LEVEL", 1);
//        EnableUpdate("FLOW_LEVEL", 1);
    }

    CommStatusDriverUpdate();   // FLOW_COMM

    MappingUpdate(WorkData.VolmumeUse, "FLOW_VOLUME");
    MappingUpdate(WorkData.LevelUse, "FLOW_LEVEL");
    MappingUpdate(WorkData.RateUse, "FLOW_RATE");
    MappingUpdate(WorkData.TotalUse, "FLOW_TOTAL");

    ServerIpUpdate();
    PortUpdate();

    gDb.close();
}

void EpsSetup::RainTagCheck(void)
{
    QString insertTotal = QString("INSERT INTO 'Tag' ('Name', 'GroupName', 'TagType', 'Desc', 'Enabled', 'Driver', 'Device',"
                                  "'Address', 'RwMode', 'DataType', 'InitValue', 'UseForcedValue', 'ForcedValue', 'OnMsg', "
                                  "'OffMsg', 'Reversed', 'DigitalAlarm', 'DigitalEvent', 'Deadband', 'RawMin', 'RawMax', 'EuMin',"
                                  "'EuMax', 'Unit', 'Ratio', 'RoundUp', 'UseLowLowAlarm', 'LowLowAlarmValue', 'UseLowAlarm', "
                                  "'LowAlarmValue', 'UseHighAlarm', 'HighAlarmValue', 'UseHighHighAlarm', 'HighHighAlarmValue',"
                                  "'LinkTags', 'ClearCondition', 'MaxValueChange', 'SaveValue', 'ControlLog') VALUES ('RAIN_TOTAL',"
                                  "'FLOW', 'Analog', '총 강우설량', '1', 'System', 'io', 'counter1', 'RW', 'R32', '0', '0', '0', '',"
                                  "'', '0', 'None', 'None', '0.0', '0.0', '0.0', '0.0', '0.0', 'mm', '1', '0', '0', '0.0', '0', '0.0',"
                                  "'0', '0.0', '0', '0.0', '', '', '', '0', '0')");
    QString insertDay = QString("INSERT INTO 'Tag' ('Name', 'GroupName', 'TagType', 'Desc', 'Enabled', 'Driver', 'Device', 'Address',"
                                "'RwMode', 'DataType', 'InitValue', 'UseForcedValue', 'ForcedValue', 'OnMsg', 'OffMsg', 'Reversed',"
                                "'DigitalAlarm', 'DigitalEvent', 'Deadband', 'RawMin', 'RawMax', 'EuMin', 'EuMax', 'Unit', 'Ratio',"
                                "'RoundUp', 'UseLowLowAlarm', 'LowLowAlarmValue', 'UseLowAlarm', 'LowAlarmValue', 'UseHighAlarm',"
                                "'HighAlarmValue', 'UseHighHighAlarm', 'HighHighAlarmValue', 'LinkTags', 'ClearCondition',"
                                "'MaxValueChange', 'SaveValue', 'ControlLog')"
                                " VALUES ('RAIN_DAY', 'Etc', 'Analog', '일 강우설량', '1', 'System', 'internal', '', 'RW', 'R32', '0',"
                                "'0', '0', '', '', '0', 'None', 'None', '0.0', '0.0', '0.0', '0.0', '0.0', 'mm', '1', '0', '0', '0.0',"
                                "'0', '0.0', '0', '0.0', '0', '0.0', '', '', '', '0', '0')");
    QString insertMapping = QString("INSERT INTO 'Ext_Eps_Mapping' ('Code', 'Desc', 'Tag', 'Unit', 'Enabled', 'DigitalPos',"
                                    "'Dump', 'Event', 'Num') VALUES ('RAIN1D', '강우설량', 'RAIN_DAY', 'M_M', '0', '-1', '1', '0', '13')");
    QString deleteDay = QString("delete from tag where name = 'RAIN_DAY'");
    QString deleteTotal = QString("delete from tag where name = 'RAIN_TOTAL'");
    QString addColumn1 = QString("alter table EpsSetup add column 'RainDi' TEXT");
    QString addColumn2 = QString("alter table EpsSetup add column 'RainDiReverse' TEXT");
    QString check = QString("select Code from 'Ext_Eps_Mapping' where Code = 'RAIN1D'");
    if(WorkData.RainDi == 0)
    {
        QSqlQuery query(deleteTotal);
        query.next();
        query.exec(deleteDay);
        query.next();
    }
    else
    {
        qDebug() << insertTotal;
        QSqlQuery query(insertTotal);
        query.next();
        query.exec(insertDay);
    }
    QSqlQuery query(check);
    if(!query.next())
        query.exec(insertMapping);
    query.exec(addColumn1);
    query.exec(addColumn2);
}

void EpsSetup::ServerIpUpdate()
{
    QString str = QString("UPDATE Port SET Ip='%1' WHERE Name='Server'").arg(WorkData.ServerIP);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::PortUpdate()
{
    int parity = 0;
    int Baudrate = 9600;
    QString str;
    if(WorkData.FlowMeterDesc == "펜타포우")
    {
        Baudrate = 38400;
//        str = QString("UPDATE Port SET SerialPort='%1', Parity=%2, Baudrate=%3 WHERE Name like 'Flow'").arg(WorkData.FlowMeterPort).arg(parity).arg(Baudrate);
//        QSqlQuery query(str);
//        query.exec();
    }
    else
    if(WorkData.FlowMeterDesc == "Nivus") // nivus 시리얼 통신시 parity를 even으로 설정
        parity = 2;
    if(WorkData.FlowMeterDesc == "OFT-2000")
        Baudrate = 115200;

    str = QString("UPDATE Port SET SerialPort='%1', Parity=%2, Baudrate=%3 WHERE Name like 'Flow%'").arg(WorkData.FlowMeterPort).arg(parity).arg(Baudrate);
//        str = QString("UPDATE port SET SerialPort='%1', Parity=%2 WHERE Name like 'Flow%'").arg(WorkData.FlowMeterPort).arg(parity);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::AiDriverUpdate(const char *Tag, const char *Device, QString Address, float Ratio, QString DataType, const char *rw)
{
    QString str = QString("UPDATE Tag SET Driver='%1', Device='%2', Address='%3', Ratio=%4, DataType='%5', RwMode='%6', Enabled='1' WHERE Name='%7'")
            .arg(WorkData.FlowMeter).arg(Device).arg(Address).arg(Ratio).arg(DataType).arg(rw).arg(Tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::CommStatusDriverUpdate()
{
    QString device;
    if(WorkData.FlowMeterDesc == "Raven-Eye" || WorkData.FlowMeterDesc == "CK 테크피아")
    {
        device = "Flow";
    }
    else
    {
        if(WorkData.FlowMeter == "System")
            device = "internal";
        else
        if(WorkData.FlowMeter == "Modbus")
            device = WorkData.FlowMeterDesc;
        else
            device = "Flow";
    }

    QString str = QString("UPDATE Tag SET Driver='%1', Device='%2', Address='%4' WHERE Name='%3'")
            .arg(WorkData.FlowMeter).arg(device).arg("FLOW_COMM").arg("Comm_Status");
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::EnableUpdate(const char *Tag, int enabled)
{
    QString str = QString("UPDATE Tag SET Enabled=%1 WHERE Name='%2'")
            .arg(enabled).arg(Tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::DriverUpdate(QString driver, int enabled)
{
    QString str = QString("UPDATE Driver SET Enabled=%1 WHERE Name='%2'").arg(enabled).arg(driver);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::ModbusDriverUpdate()
{
//    QString str1("SELECT Driver, Desc FROM DriverList");
//    QSqlQuery query1(str1);
//    while(query1.next())
 //   for (int i = 0; i < DriverList.size(); ++i)
    {
        int enabled = 0;
//        QString driver = DriverList.at(i);
//        QString sDriver = query1.value(0).toString();
//        QString sDesc = query1.value(1).toString();
        if(WorkData.FlowMeter == "Modbus")
        {
//            QString sDesc = query1.value(1).toString();
//            if(sDesc == WorkData.FlowMeterDesc)
            enabled = 1;
            QString endian = "Wordswap";
            if(WorkData.FlowMeterDesc == "CK 테크피아")
                endian = "Big";
            QString str = QString("UPDATE Driver_Modbus SET Enabled=%1,Endian='%2' WHERE Name='Flow'").arg(enabled).arg(endian);
            QSqlQuery query(str);
            query.exec();
            if(g_DebugDisplay)
                qDebug() << "sql:" << str;
        }
    }
}

void EpsSetup::MappingUpdate(int use, const char* tag)
{
    int enabled = 0;
    if(use)
        enabled = 1;
    QString str;
    str = QString("UPDATE Ext_Eps_Mapping SET Enabled=%1 WHERE Tag='%2'")
        .arg(enabled).arg(tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::AddressUpdate(int address, const char* tag, const char* pre)
{
    if(address == 0)
        return;
    QString addr = QString("%1%2").arg(pre).arg(address);
    QString str = QString("UPDATE Tag SET Address='%1' WHERE Name='%2'").arg(addr).arg(tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::AiEuUpdate(const char* tag, double min, double max)
{
    if(tag == 0)
        return;
    QString str;
    str = QString("UPDATE Tag SET RawMin=4, RawMax=20, EuMin=%1,EuMax=%2 WHERE Name='%3'")
        .arg(min).arg(max).arg(tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::DigitalRevUpdate(const char *tag, int rev)
{
    if(tag == 0)
        return;
    QString str;
        str = QString("UPDATE Tag SET Reversed=%1 WHERE Name='%2'")
        .arg(rev).arg(tag);
    QSqlQuery query(str);
    query.exec();
    if(g_DebugDisplay)
        qDebug() << "sql:" << str;
}

void EpsSetup::ErrDisp()
{
    QSqlError err = gDb.lastError();
    QString msg =  "SqlError : ";
    msg +=  err.databaseText();
    CInfomationDialog dlg(msg);
    dlg.exec();
}

void EpsSetup::on_ok_clicked()
{
    WorkData.ServerIP = ui->serverIp->text();
    WorkData.SiteCode = ui->siteCode->text();
    WorkData.LevelMax = ui->levelMax->text().toInt();
    WorkData.SubLevelEuMin = ui->subMin->text().toDouble();
    WorkData.SubLevelEuMax = ui->subMax->text().toDouble();
    WorkData.ConductMeterEuMin = ui->condMin->text().toDouble();
    WorkData.ConductMeterEuMax = ui->condMax->text().toDouble();
    WorkData.CleanDoReverse = ui->compDoRev->isChecked();
    WorkData.FanDoReverse = ui->fanRev->isChecked();
    WorkData.BlackOutReverse = ui->breakOutRev->isChecked();
    WorkData.LeakDireverse = ui->leakRev->isChecked();
    WorkData.DoorOpenDiReverse = ui->doorDiRev->isChecked();
    WorkData.HiTempDiReverse = ui->hiTempRev->isChecked();
    WorkData.CleanDiReverse = ui->compDiRev->isChecked();
    WorkData.RainDiReverse = ui->rainDiRev->isChecked();
    WorkData.RateMin = ui->rateMin->text().toDouble();
    WorkData.RateMax = ui->rateMax->text().toDouble();
    WorkData.VolumeMin = ui->volumeMin->text().toDouble();
    WorkData.VolumeMax = ui->volumeMax->text().toDouble();
    WorkData.FlowTotalReverse = ui->FlowTotalRev->isChecked();
    WorkData.FlowTotalRatio = ui->flowTotalRatio->text().toDouble();

    WorkData.VolmumeUse = ui->volumeUse->isChecked();
    WorkData.LevelUse = ui->levelUse->isChecked();
    WorkData.RateUse = ui->rateUse->isChecked();
    WorkData.TotalUse = ui->totalUse->isChecked();

    QString file = "/app/CommDrivers/" + WorkData.FlowMeter + ".dll";
    if(QFile(file).exists() == false)
    {
        QString msg = "지원하지 않는 유량계입니다.(" + file + ")";
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    if(ChangedCheck())
    {
        CQuestionDialog dlg("변경된 내용을 적용하시겠습니까?\n 적용하면 프로그램을 재기동합니다.");
        if(dlg.exec() == QDialog::Accepted)
        {
            SetupCopy(&WorkData, &SetupData);
            UpdateDb();
            accept();
        }
    }
    else
    {
        QString msg = "변경된 데이터가 없습니다.";
        CInfomationDialog dlg(msg);
        dlg.exec();
    }
}

void EpsSetup::on_btnFlowmeter_clicked()
{
    DeviceSelect dlg;
    if(dlg.exec() == QDialog::Accepted)
    {
        ui->btnFlowmeter->setText(dlg.sDesc);
        WorkData.FlowMeter = dlg.sDriver;
        WorkData.FlowMeterDesc = dlg.sDesc;
        DriverDisplay();
        if(WorkData.FlowMeter !=  "System")
        {
            ui->volumeUse->setChecked(true);
            ui->levelUse->setChecked(true);
            ui->rateUse->setChecked(true);
            ui->totalUse->setChecked(true);
        }
        if(WorkData.FlowMeter ==  "PenToughFlow")
            ui->rateUse->setChecked(false);
        if(WorkData.FlowMeterDesc ==  "CK 테크피아")
            ui->levelUse->setChecked(false);
    }
}

void EpsSetup::on_btnCompDo_clicked()
{
    DoSelect dlg(WorkData.CleanDo);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.CleanDo = dlg.nDo;
        LabelDisp(WorkData.CleanDo, ui->btnCompDo, "DO");
    }
}

void EpsSetup::on_btnFan_clicked()
{
    DoSelect dlg(WorkData.FanDo);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.FanDo = dlg.nDo;
        LabelDisp(WorkData.FanDo, ui->btnFan, "DO");
    }
}

void EpsSetup::on_btnBreakOut_clicked()
{
    DiSelect dlg(WorkData.BlackOutDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.BlackOutDi = dlg.nDi;
        LabelDisp(WorkData.BlackOutDi, ui->btnBreakOut, "DI");
    }
}

void EpsSetup::on_btnLeak_clicked()
{
    DiSelect dlg(WorkData.LeakDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.LeakDi = dlg.nDi;
        LabelDisp(WorkData.LeakDi, ui->btnLeak, "DI");
    }
}

void EpsSetup::on_btnDoorOpen_clicked()
{
    DiSelect dlg(WorkData.DoorOpenDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.DoorOpenDi = dlg.nDi;
        LabelDisp(WorkData.DoorOpenDi, ui->btnDoorOpen, "DI");
    }
}

void EpsSetup::on_btnHiTemp_clicked()
{
    DiSelect dlg(WorkData.HiTempDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.HiTempDi = dlg.nDi;
        LabelDisp(WorkData.HiTempDi, ui->btnHiTemp, "DI");
    }
}

void EpsSetup::on_btnCompDi_clicked()
{
    DiSelect dlg(WorkData.CleanDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.CleanDi = dlg.nDi;
        LabelDisp(WorkData.CleanDi, ui->btnCompDi, "DI");
    }
}
void EpsSetup::on_btnRainDi_clicked()
{
    DiSelect dlg(WorkData.RainDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        QString str;
        WorkData.RainDi = dlg.nDi;
        LabelDisp(WorkData.RainDi, ui->btnRainDi, "DI");
    }
}
void EpsSetup::SetupCopy(struct EPS_SETUP *s, struct EPS_SETUP *d)
{
    d->LevelMax = s->LevelMax;
    d->SubLevel = s->SubLevel;
    d->SubLevelEuMin = s->SubLevelEuMin;
    d->SubLevelEuMax = s->SubLevelEuMax;
    d->ConductMeter = s->ConductMeter;
    d->ConductMeterEuMin = s->ConductMeterEuMin;
    d->ConductMeterEuMax = s->ConductMeterEuMax;
    d->AutoCompUse = s->AutoCompUse;
    d->AutoCompTime = s->AutoCompTime;
    d->AutoCompInterval = s->AutoCompInterval;
    d->ServerIP = s->ServerIP;
    d->CleanDo = s->CleanDo;
    d->CleanDoReverse = s->CleanDoReverse;
    d->FanDo = s->FanDo;
    d->FanDoReverse = s->FanDoReverse;
    d->BlackOutDi = s->BlackOutDi;
    d->BlackOutReverse = s->BlackOutReverse;
    d->LeakDi = s->LeakDi;
    d->LeakDireverse = s->LeakDireverse;
    d->DoorOpenDi = s->DoorOpenDi;
    d->DoorOpenDiReverse = s->DoorOpenDiReverse;
    d->HiTempDi = s->HiTempDi;
    d->HiTempDiReverse = s->HiTempDiReverse;
    d->CleanDi = s->CleanDi;
    d->CleanDiReverse = s->CleanDiReverse;
    d->RainDi = s->RainDi;
    d->RainDiReverse = s->RainDiReverse;
    d->SiteCode = s->SiteCode;
    d->FlowMeter = s->FlowMeter;
    d->FlowMeterDesc = s->FlowMeterDesc;
    d->MainLevel = s->MainLevel;
    d->LevelMin = s->LevelMin;
    d->FlowDi = s->FlowDi;
    d->RateAi = s->RateAi;
    d->RateMin = s->RateMin;
    d->RateMax = s->RateMax;
    d->VolumeAi = s->VolumeAi;
    d->VolumeMin = s->VolumeMin;
    d->VolumeMax = s->VolumeMax;
    d->FlowTotalReverse = s->FlowTotalReverse;
    d->FlowTotalRatio = s->FlowTotalRatio;

    d->FlowMeterPort = s->FlowMeterPort;
    d->VolmumeUse = s->VolmumeUse;
    d->LevelUse = s->LevelUse;
    d->RateUse = s->RateUse;
    d->TotalUse = s->TotalUse;
}

bool EpsSetup::ChangedCheck()
{
    struct EPS_SETUP *s = &SetupData;
    struct EPS_SETUP *d = &WorkData;
    bool ret = false;

    if( d->LevelMax != s->LevelMax ||
        d->SubLevel != s->SubLevel ||
        d->SubLevelEuMin != s->SubLevelEuMin ||
        d->SubLevelEuMax != s->SubLevelEuMax ||
        d->ConductMeter != s->ConductMeter ||
        d->ConductMeterEuMin != s->ConductMeterEuMin ||
        d->ConductMeterEuMax != s->ConductMeterEuMax ||
        d->AutoCompUse != s->AutoCompUse ||
        d->AutoCompTime != s->AutoCompTime ||
        d->AutoCompInterval != s->AutoCompInterval ||
        d->ServerIP != s->ServerIP ||
        d->CleanDo != s->CleanDo ||
        d->CleanDoReverse != s->CleanDoReverse ||
        d->FanDo != s->FanDo ||
        d->FanDoReverse != s->FanDoReverse ||
        d->BlackOutDi != s->BlackOutDi ||
        d->BlackOutReverse != s->BlackOutReverse ||
        d->LeakDi != s->LeakDi ||
        d->LeakDireverse != s->LeakDireverse ||
        d->DoorOpenDi != s->DoorOpenDi ||
        d->DoorOpenDiReverse != s->DoorOpenDiReverse ||
        d->HiTempDi != s->HiTempDi ||
        d->HiTempDiReverse != s->HiTempDiReverse ||
        d->CleanDi != s->CleanDi ||
        d->CleanDiReverse != s->CleanDiReverse ||
        d->RainDi != s->RainDi ||
        d->RainDiReverse != s->RainDiReverse ||
        d->SiteCode != s->SiteCode ||
        d->FlowMeter != s->FlowMeter ||
        d->FlowMeterDesc != s->FlowMeterDesc ||
        d->MainLevel != s->MainLevel ||
        d->LevelMin != s->LevelMin ||
        d->FlowDi != s->FlowDi ||
        d->RateAi != s->RateAi ||
        d->RateMin != s->RateMin ||
        d->RateMax != s->RateMax ||
        d->VolumeAi != s->VolumeAi ||
        d->VolumeMin != s->VolumeMin ||
        d->VolumeMax != s->VolumeMax ||
        d->FlowTotalReverse != s->FlowTotalReverse ||
        d->FlowTotalRatio != s->FlowTotalRatio ||
        d->FlowMeterPort != s->FlowMeterPort ||
        d->VolmumeUse != s->VolmumeUse ||
        d->LevelUse != s->LevelUse ||
        d->RateUse != s->RateUse ||
        d->TotalUse != s->TotalUse)
            ret = true;
    return ret;
}

void EpsSetup::on_btnDelete_clicked()
{
    CQuestionDialog dlg("유량 데이터를 삭제하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        QString dbname =  "/data/data.db";
        gDb.setDatabaseName(dbname);
        if(gDb.open() != true)
            ErrDisp();
        DbDelete("Eps");
        DbDelete("EpsLog");
        DbDelete("EpsDump");
        DbDelete("EpsRT");
        gDb.close();
    }
}

void EpsSetup::DbDelete(const char* table)
{
    QString str = "DELETE From ";
    str += table;
    QSqlQuery query(str);
    query.exec();
}

void EpsSetup::on_btnSubLevel_clicked()
{
    AiSelect dlg(WorkData.SubLevel);
    if(dlg.exec() == QDialog::Accepted)
    {
        bool use = true;
        WorkData.SubLevel = dlg.nAi;
        LabelDisp(WorkData.SubLevel, ui->btnSubLevel, "AI");
        if(WorkData.SubLevel == 0)
            use = false;
        ui->subMin->setEnabled(use);
        ui->subMax->setEnabled(use);
    }
}

void EpsSetup::on_btnCond_clicked()
{
    AiSelect dlg(WorkData.ConductMeter);
    if(dlg.exec() == QDialog::Accepted)
    {
        bool use = true;
        WorkData.ConductMeter = dlg.nAi;
        LabelDisp(WorkData.ConductMeter, ui->btnCond, "AI");
        if(WorkData.ConductMeter == 0)
            use = false;
        ui->condMin->setEnabled(use);
        ui->condMax->setEnabled(use);
    }
}

void EpsSetup::on_btnMainLevel_clicked()
{
    AiSelect dlg(WorkData.MainLevel);
    if(dlg.exec() == QDialog::Accepted)
    {
        WorkData.MainLevel = dlg.nAi;
        MainLevelDisplay();
        ui->levelUse->setChecked((bool)dlg.nAi);
    }
}

void EpsSetup::on_btnFlowTotal_clicked()
{
    DiSelect dlg(WorkData.FlowDi);
    if(dlg.exec() == QDialog::Accepted)
    {
        WorkData.FlowDi = dlg.nDi;
        LabelDisp(WorkData.FlowDi, ui->btnFlowTotal, "DI");
        ui->totalUse->setChecked((bool)dlg.nDi);
    }
}

void EpsSetup::on_btnRate_clicked()
{
    AiSelect dlg(WorkData.RateAi);
    if(dlg.exec() == QDialog::Accepted)
    {
        WorkData.RateAi = dlg.nAi;
        RateDisplay();
        ui->rateUse->setChecked((bool)dlg.nAi);
    }
}

void EpsSetup::on_btnVolume_clicked()
{
    AiSelect dlg(WorkData.VolumeAi);
    if(dlg.exec() == QDialog::Accepted)
    {
        WorkData.VolumeAi = dlg.nAi;
        VolumeDisplay();
        ui->volumeUse->setChecked((bool)dlg.nAi);
    }
}

void EpsSetup::MainLevelDisplay()
{
    bool use = true;
    LabelDisp(WorkData.MainLevel, ui->btnMainLevel, "AI");
    if(WorkData.MainLevel == 0)
        use = false;
    ui->levelMin->setEnabled(use);
    ui->levelMax->setEnabled(use);
}

void EpsSetup::RateDisplay()
{
    bool use = true;
    LabelDisp(WorkData.RateAi, ui->btnRate, "AI");
    if(WorkData.RateAi == 0)
        use = false;
    ui->rateMin->setEnabled(use);
    ui->rateMax->setEnabled(use);
}

void EpsSetup::VolumeDisplay()
{
    bool use = true;
    LabelDisp(WorkData.VolumeAi, ui->btnVolume, "AI");
    if(WorkData.VolumeAi == 0)
        use = false;
    ui->volumeMin->setEnabled(use);
    ui->volumeMax->setEnabled(use);
}

void EpsSetup::DriverDisplay()
{
    MainLevelDisplay();
    RateDisplay();
    VolumeDisplay();
    bool bEnable = false;
    if(WorkData.FlowMeter == "System")
        bEnable = true;
    ui->btnMainLevel->setEnabled(bEnable);
    ui->btnRate->setEnabled(bEnable);
    ui->btnVolume->setEnabled(bEnable);
    ui->btnFlowTotal->setEnabled(bEnable);
    ui->FlowTotalRev->setEnabled(bEnable);
    ui->flowTotalRatio->setEnabled(bEnable);
    ui->levelMin->setEnabled(bEnable);
    ui->levelMax->setEnabled(bEnable);
    ui->rateMin->setEnabled(bEnable);
    ui->rateMax->setEnabled(bEnable);
    ui->volumeMin->setEnabled(bEnable);
    ui->volumeMax->setEnabled(bEnable);

    if(WorkData.FlowMeter ==  "System")
        ui->FlowmeterPort->setVisible(false);
    else
        ui->FlowmeterPort->setVisible(true);
}

void EpsSetup::on_btnOrder_clicked()
{
    EpsOrder dlg;
    dlg.exec();
}

void EpsSetup::on_btnComp_clicked()
{
    CompSetup dlg;
    dlg.exec();
}

void EpsSetup::LabelDisp(int pos, QPushButton *label, const char* prefix)
{
    QString str;
    if(pos == 0)
        str = "사용안함";
    else
        str = QString("%1 %2").arg(prefix).arg(pos);
    label->setText(str);
}

void EpsSetup::on_btnFlowTotalWrite_clicked()
{
    QString msg = "총 유량 변경";
    CTag *pTag = m_TagMap["FLOW_TOTAL"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_sValue);
}

void EpsSetup::on_FlowmeterPort_currentIndexChanged(int index)
{
    WorkData.FlowMeterPort = index;
}

void EpsSetup::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}

void EpsSetup::on_btnRainTotalWrite_clicked()
{
    QString msg = "총 강우설량";
    CTag *pTag = m_TagMap["RAIN_TOTAL"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_sValue);
}

void EpsSetup::on_btnRainDayWrite_clicked()
{
    QString msg = " 강우설량";
    CTag *pTag = m_TagMap["RAIN_DAY"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_sValue);
}
