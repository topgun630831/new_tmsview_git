#ifndef SENSORSELECT_H
#define SENSORSELECT_H

#include <QDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QTimer>

namespace Ui {
class SensorSelect;
}

class SensorSelect : public QDialog
{
    Q_OBJECT

public:
    explicit SensorSelect(int nFlowNum, float FlowRatio, bool WaterTemp, QWidget *parent = 0);
    ~SensorSelect();
    void disp();

    bool bCod;
    bool bTn;
    bool bTp;
    bool bPh;
    bool bSs;
    bool bFlow;
    bool bSampler;
    bool bToc;
    int  nFlowNum;
    float nFlowRatio;
    bool bRate;
    QString sTn;
    QString sTp;
    QString sPh;
    QString sSs;
    QString sCod;
    QString sFlow;
    QString sSamModel;
    QString sToc;
    QString sTnComPort;
    QString sTpComPort;
    QString sPhComPort;
    QString sSsComPort;
    QString sCodComPort;
    QString sFlowComPort;
    QString sSamplerComPort;
    QString sTocComPort;
/*    bool    bTnCchk;
    bool    bTpCchk;
    bool    bCodCchk;
    bool    bPhCchk;
    bool    bSsCchk;
    bool    bTocCchk;
    bool    bSamplerCchk;
*/    bool    bAnalog_flow;
    bool    bAnalog_tn;
    bool    bAnalog_tp;
    bool    bAnalog_cod;
    bool    bAnalog_ph;
    bool    bAnalog_ss;
    bool    bAnalog_toc;
    bool    bStx_flow;
/*    bool    bStx_tn;
    bool    bStx_tp;
    bool    bStx_cod;
    bool    bStx_ph;
    bool    bStx_ss;
    bool    bStx_toc;
*/    bool    bStx_sampler;
    bool    bWaterTemp_ss;
    bool    bGas_toc;

private slots:
    void on_ok_clicked();
/*
    void onTimer();
    void on_tnOld_clicked();
    void on_tnNew_clicked();
    void on_tpOld_clicked();
    void on_tpNew_clicked();
    void on_codOld_clicked();
    void on_codNew_clicked();
    void on_phOld_clicked();
    void on_phNew_clicked();
    void on_ssOld_clicked();
    void on_ssNew_clicked();
    void on_tocOld_clicked();
    void on_tocNew_clicked();
*/

private:
//    QTimer              m_Timer;
    int                 m_timerId;
    Ui::SensorSelect *ui;
    void	paintEvent(QPaintEvent*);
    void InitDisp(QString sensor, bool check, QString compare, QRadioButton *old, QRadioButton *newB, QCheckBox *checkbox);
    void timerEvent(QTimerEvent *);
};

#endif // SENSORSELECT_H
