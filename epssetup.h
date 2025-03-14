#ifndef EPSSETUP_H
#define EPSSETUP_H

#include <QDialog>

namespace Ui {
class EpsSetup;
}

struct EPS_SETUP {
    double LevelMax;
    int SubLevel;
    double SubLevelEuMin;
    double SubLevelEuMax;
    int ConductMeter;
    double ConductMeterEuMin;
    double ConductMeterEuMax;
    int AutoCompUse;
    int AutoCompTime;
    int AutoCompInterval;
    QString ServerIP;
    int CleanDo;
    int CleanDoReverse;
    int FanDo;
    int FanDoReverse;
    int BlackOutDi;
    int BlackOutReverse;
    int LeakDi;
    int LeakDireverse;
    int DoorOpenDi;
    int DoorOpenDiReverse;
    int HiTempDi;
    int HiTempDiReverse;
    int CleanDi;
    int CleanDiReverse;
    int RainDi;
    int RainDiReverse;
    QString SiteCode;
    QString FlowMeter;
    QString FlowMeterDesc;
    int MainLevel;
    double LevelMin;
    int FlowDi;
    int RateAi;
    double RateMin;
    double RateMax;
    int VolumeAi;
    double VolumeMin;
    double VolumeMax;
    int     FlowTotalReverse;
    double  FlowTotalRatio;
    int     FlowMeterPort;
    int     VolmumeUse;
    int     LevelUse;
    int     RateUse;
    int     TotalUse;
};

class EpsSetup : public QDialog
{
    Q_OBJECT

public:
    explicit EpsSetup(QWidget *parent = 0);
    ~EpsSetup();

private slots:
    void on_ok_clicked();
    void on_btnFlowmeter_clicked();
    void on_btnCompDo_clicked();
    void on_btnFan_clicked();
    void on_btnBreakOut_clicked();
    void on_btnLeak_clicked();
    void on_btnDoorOpen_clicked();
    void on_btnHiTemp_clicked();
    void on_btnCompDi_clicked();
    void on_btnDelete_clicked();
    void on_btnSubLevel_clicked();
    void on_btnCond_clicked();
    void on_btnMainLevel_clicked();
    void on_btnFlowTotal_clicked();
    void on_btnRate_clicked();
    void on_btnOrder_clicked();
    void on_btnComp_clicked();
    void on_btnVolume_clicked();
    void on_btnFlowTotalWrite_clicked();
    void on_FlowmeterPort_currentIndexChanged(int index);
    void on_btnRainDi_clicked();
    void on_btnRainTotalWrite_clicked();

    void on_btnRainDayWrite_clicked();

private:
    Ui::EpsSetup *ui;
    void InitDisp();
    void UpdateDb();
    void SetupCopy(struct EPS_SETUP *s, struct EPS_SETUP *d);
    bool ChangedCheck();
    void ErrDisp();
    struct EPS_SETUP WorkData;
    void MappingUpdate(int use, const char* tag);
    void AddressUpdate(int address, const char* tag, const char* pre);
    void AiEuUpdate(const char* tag, double min, double max);
    void DigitalRevUpdate(const char *tag, int rev);
    void DriverUpdate(QString driver, int enabled);
    void ModbusDriverUpdate();
    void AiDriverUpdate(const char *Tag, const char *Device, QString Address,float Ratio, QString DataType , const char* = "R");
    void MainLevelDisplay();
    void RateDisplay();
    void VolumeDisplay();
    void DriverDisplay();
    void LabelDisp(int pos, QPushButton *label, const char* prefix);
    void DbDelete(const char* table);
    void ServerIpUpdate();
    void EnableUpdate(const char *Tag, int enabled);
    void PortUpdate();
    void CommStatusDriverUpdate();
    void timerEvent(QTimerEvent *);
    void RainTagCheck(void);
};

#endif // EPSSETUP_H
