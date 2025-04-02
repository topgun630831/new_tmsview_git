#ifndef TMSSETUP_H
#define TMSSETUP_H

#include <QDialog>
#include <QJsonArray>
#include <QList>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QTimer>
#include "mondialog.h"
#define TAG_MAX 23

namespace Ui {
class TmsSetup;
}

struct TMS_ITEM_TAB {
    QString Id;
    QString Code;
    QString Desc;
    QString Protocol;
    int     RoundUp;
    QString Port;
    int     ScanTime;
    int     Timeout;
    QString ConstList;
    int     Upload;
    int     UseStxChecksum;
    int     UseCchk;
    int     Analog;
};

struct TMS_TAG_TAB {
    const char *suffix;
    const char *TagType;
    const char *Desc;
    const char *Driver;
    const char *Address;
    const char *Rw;
    const char *DataType;
    const char *On;
    const char *Off;
    const char *Unit;
    int  Save;
    const char *InitValue;
};

class TmsSetup : public CMonDialog
{
    Q_OBJECT

public:
    explicit TmsSetup(bool flag[9], bool checkFlag[9], QWidget *parent = 0);
    ~TmsSetup();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_save_clicked();
    void on_sensorSelect_clicked();
    void on_door_clicked();
    void on_power_clicked();
    void on_sampL_clicked();
    void on_sampR_clicked();
    void on_tableSensor_itemClicked(QTableWidgetItem *item);
    void on_program2_clicked();
    void on_general2_clicked();
    void on_sensor2_clicked();
    void on_checkSet_clicked();
    void on_checkReset_clicked();
    void on_digitChange_clicked();
    void on_general2_2_clicked();
    void on_chkAll_clicked();
    void on_DataDelete_clicked();
    void on_protocol_currentIndexChanged(const QString &arg1);
    void on_comPort_currentIndexChanged(const QString &arg1);
    void on_round_textChanged(const QString &arg1);
    void on_scanTime_textChanged(const QString &arg1);
    void on_setup_clicked();
    void on_timeout_textChanged(const QString &arg1);
    void on_UseStxChecksum_stateChanged(int);
//    void on_chkExternalIO_clicked(bool checked);
    void on_f_comPort1_currentIndexChanged(const QString &arg1);
    void on_usecchk_stateChanged(int);
//    void on_const_2_clicked();
//    void on_changeConsts_clicked();

//    void on_sampDoor_clicked();

    void on_MonitorPort_currentTextChanged(const QString &monitor);
//    void onTimer();
    void on_TimeSyncSet_clicked();

    void on_timeSync_clicked();

    void on_upload_clicked();

    void on_check_clicked();

    void on_networkSet_clicked();

    void on_flow_setup_clicked();

private:
    Ui::TmsSetup *ui;
    QList<TMS_ITEM_TAB *> TmsItemList;
    int                 m_timerId;
    int                 SelectedRow;
    int                 FlowRow;
    int m_nDORON;
    int m_nFMLON;
    int m_nFMRON;
    int m_nPWRON;
    int m_nSampDoor;
    bool m_bUseToc;
    QTimer              m_Timer;
    QCheckBox *mCheckBox[9];
    QCheckBox *mCheckBox2[9];
    QLineEdit *mLineEdit[9];
    bool mbProtocolEdit;
    bool mbFlag[9];
    QLabel *checkLabel[9];
    bool mbCheckFlag[9];
    int         m_nFlowNum;
    float       m_nFlowRatio;
    bool        m_bWaterTempSs;
    QGroupBox   *m_GroupBox[5];
    bool        m_bFlowRate[5];
    float   m_Ratio[5];
    QString m_FlowPort[5];
    QString m_FlowProtocol[5];
    bool m_bSamplerDoorExternal;
    bool m_bDoorReversed;
//    QLineEdit   *m_ConstEdit[5][7];
//    QLabel      *m_ConstDate[5];
    QString     m_sSamplerModel;
    int        m_nDiCount;
    bool        m_bIniting;
    bool tntp;
    bool phss;
    bool toccod;
    QString mPassword;
    QString mServerIp;
    QString mServerPort;
    void DispGeneral();
    int SetDi(const char* name, QPushButton *btn, QCheckBox *chk);
    int SetDi2(const char* name);
    void DiDisp(int pos, QPushButton *btn);
    void SetItem(int row, const TMS_ITEM_TAB *tmsitem, bool bAdd=true);
    void SetItem(QString str, int row, int col);
    void SqlExec(QString sql);
    void SensorAdd(QString Desc, QString Id, QString Code, QString Protocol, int Round,
                             QString Port, int ScanTime, int Timeout, QString ConstList, int Upload, int UseCchk, int Analog, int Stx);
    void RunStart(bool start);
    void CheckMultiSensor(bool &tntp, bool &phss, bool &toccod);
    void DbSave();
    void IoSave(const char *Id, QString Desc);
    void SensorSave(const TMS_ITEM_TAB *item, int max, int sensor, int *select, bool bOld=false);
    void TagAdd(QString name, QString group, const char *tagtype, QString desc, QString driver,
                QString device, QString addr, const char * rw, const char *data, const char *on, const char *off,
                int rev, QString unit, int round, int save, const char * InitValue, float ratio);
    void DigitChange(QString name, int pos, bool bExtTms=true);
    int DigitGet(QString name);
    void CheckControl(int value);
    void UseStxSet(QString Name, int use);
    void CheckDisp();
    void DispSamplerDoor();
    void SaveSamplerDoor();
    void TableChange(QString name, int pos);
    void CheckFlow();
    int SensorIndex(QString desc);
    int CodePos(QString code);
    void timerEvent(QTimerEvent *);
};

#endif // TMSSETUP_H
