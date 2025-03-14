#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QDesktopWidget>
#include "sensorselect.h"
#include "ui_sensorselect.h"

extern bool    m_bScreenSave;

SensorSelect::SensorSelect( int FlowNum, float FlowRatio, bool WaterTemp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensorSelect)
{
    ui->setupUi(this);

    sTnComPort = "Com1";
    sTpComPort = "Com2";
    sTocComPort = "Com3";
    sPhComPort = "Com4";
    sSsComPort = "Com5";
    sCodComPort = "Com6";
    sFlowComPort = "Com7";
    sSamplerComPort = "Com8";

    ui->num->setText(QString("%1").arg(FlowNum));
    ui->ratio->setText(QString("%1").arg(FlowRatio));
    ui->WaterTemp_ss->setChecked(WaterTemp);
    installEventFilter(this);
    bAnalog_tn = false;
    bAnalog_tp = false;
    bAnalog_cod = false;
    bAnalog_ph = false;
    bAnalog_ss = false;
    bAnalog_toc = false;
    bAnalog_flow = false;
/*    bTnCchk =
    bTpCchk =
    bCodCchk =
    bPhCchk =
    bSsCchk =
    bTocCchk =
    bStx_flow =
    bStx_tn =
    bStx_tp =
    bStx_cod =
    bStx_ph =
    bStx_ss =
    bStx_toc =
    bStx_sampler = false;
*/
    bTn =
    bTp =
    bCod =
    bPh =
    bSs =
    bToc = false;
    bFlow =
    bSampler = true;

//    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    m_timerId = startTimer(1000*60*5);

}

SensorSelect::~SensorSelect()
{
    delete ui;
}

void SensorSelect::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
        QDialog::reject();
}

void SensorSelect::disp()
{
    if(bTn)
        ui->TN->setChecked(true);
    if(bTp)
        ui->TP->setChecked(true);
    if(bCod)
        ui->COD->setChecked(true);
    if(bPh)
        ui->PH->setChecked(true);
    if(bSs)
        ui->SS->setChecked(true);
    if(bToc)
        ui->TOC->setChecked(true);
 //   if(bSampler)
  //      ui->SAMPLER->setChecked(true);
    //if(bFlow)
  //      ui->FLOW->setChecked(true);
/*
    if(ui->TN->isChecked())
        InitDisp(sTn, bTnCchk, "Analyzer", ui->tnOld, ui->tnNew, ui->TnCchk);
    if(ui->TP->isChecked())
        InitDisp(sTp, bTpCchk, "Analyzer", ui->tpOld, ui->tpNew, ui->TpCchk);
    if(ui->COD->isChecked())
        InitDisp(sCod, bCodCchk, "Analyzer", ui->codOld, ui->codNew, ui->CodCchk);
    if(ui->PH->isChecked())
        InitDisp(sPh, bPhCchk, "Analyzer", ui->phOld, ui->phNew, ui->PhCchk);
    if(ui->SS->isChecked())
        InitDisp(sSs, bSsCchk, "Analyzer", ui->ssOld, ui->ssNew, ui->SsCchk);
    if(ui->TOC->isChecked())
        InitDisp(sToc, bTocCchk, "Analyzer", ui->tocOld, ui->tocNew, ui->TocCchk);
    if(ui->FLOW->isChecked())
*/
     InitDisp(sFlow, false, "Flow", ui->flowOld, ui->flowNew, 0);

    if(bAnalog_flow)
        ui->Analog_flow->setChecked(true);
    if(bAnalog_tn)
        ui->Analog_tn->setChecked(true);
    if(bAnalog_tp)
        ui->Analog_tp->setChecked(true);
    if(bAnalog_cod)
        ui->Analog_cod->setChecked(true);
    if(bAnalog_ph)
        ui->Analog_ph->setChecked(true);
    if(bAnalog_ss)
        ui->Analog_ss->setChecked(true);
    if(bAnalog_toc)
        ui->Analog_toc->setChecked(true);
    if(bGas_toc)
        ui->Gas_toc->setChecked(true);

    if(sSamModel == "Year100")
        ui->Year100->setChecked(true);
    else
        if(sSamModel == "Hanchang")
        ui->Hanchang->setChecked(true);
    else
    if(sSamModel == "Watersam")
        ui->Watersam->setChecked(true);
    else
    if(sSamModel == "Io")
        ui->Io->setChecked(true);
    else
    if(sSamModel == "Korbi")
        ui->Korbi->setChecked(true);
    else
    if(sSamModel == "Ilho")
        ui->Ilho->setChecked(true);
    else
    if(sSamModel == "Dongil")
        ui->Dongil->setChecked(true);

    ui->rate->setVisible(false);
}

void SensorSelect::InitDisp(QString sensor, bool check, QString compare, QRadioButton *old, QRadioButton *newB, QCheckBox *checkbox)
{
    if(sensor == compare)
    {
        old->setChecked(true);
        if(checkbox != 0)
            checkbox->setChecked(check);
    }
    else
    {
        newB->setChecked(true);
        if(checkbox != 0)
            checkbox->setVisible(false);
    }
}

void SensorSelect::paintEvent(QPaintEvent *)
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

void SensorSelect::on_ok_clicked()
{
    sFlow = ui->FlowGroup->checkedButton()->text();
    if(sFlow == "NewFlow")
        bStx_flow = false;
/*    sTn = ui->TnGroup->checkedButton()->text();
    sTp = ui->TpGroup->checkedButton()->text();
    sCod = ui->CodGroup->checkedButton()->text();
    sPh = ui->PhGroup->checkedButton()->text();
    sSs = ui->SsGroup->checkedButton()->text();
    sSamModel = ui->SamModelGroup->checkedButton()->text();
    sToc = ui->TocGroup->checkedButton()->text();
    if(sTn == "NewAnalyzer")
        bStx_tn = false;
    if(sTp == "NewAnalyzer")
        bStx_tp = false;
    if(sCod == "NewAnalyzer")
        bStx_cod = false;
    if(sPh == "NewAnalyzer")
        bStx_ph = false;
    if(sSs == "NewAnalyzer")
        bStx_ss = false;
    if(sToc == "NewAnalyzer")
        bStx_toc = false;
*/
    sSamModel = ui->SamModelGroup->checkedButton()->text();
    bTn = ui->TN->isChecked();
    bTp = ui->TP->isChecked();
    bCod = ui->COD->isChecked();
    bPh = ui->PH->isChecked();
    bSs = ui->SS->isChecked();
    bFlow = true; //ui->FLOW->isChecked();
    bSampler = true; //ui->SAMPLER->isChecked();
    bToc = ui->TOC->isChecked();
    nFlowNum = ui->num->text().toInt();
    nFlowRatio = ui->ratio->text().toFloat();
    bRate = ui->rate->isChecked();
    sTn = sTp = sCod = sPh = sSs = sToc = "NewAnalyzer";

/*
    bTnCchk = ui->TnCchk->isChecked();
    bTpCchk = ui->TpCchk->isChecked();
    bCodCchk = ui->CodCchk->isChecked();
    bPhCchk = ui->PhCchk->isChecked();
    bSsCchk = ui->SsCchk->isChecked();
    bTocCchk = ui->TocCchk->isChecked();
*/
    bAnalog_flow = ui->Analog_flow->isChecked();
    bAnalog_tn = ui->Analog_tn->isChecked();
    bAnalog_tp = ui->Analog_tp->isChecked();
    bAnalog_cod = ui->Analog_cod->isChecked();
    bAnalog_ph = ui->Analog_ph->isChecked();
    bAnalog_ss = ui->Analog_ss->isChecked();
    bAnalog_toc = ui->Analog_toc->isChecked();
    bGas_toc = ui->Gas_toc->isChecked();
    bWaterTemp_ss = ui->WaterTemp_ss->isChecked();

    QDialog::accept();
}
/*
void SensorSelect::onTimer()
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
*/
/*
void SensorSelect::on_tnOld_clicked()
{
    ui->TnCchk->setVisible(true);
}

void SensorSelect::on_tnNew_clicked()
{
    ui->TnCchk->setVisible(false);
}

void SensorSelect::on_tpOld_clicked()
{
    ui->TpCchk->setVisible(true);
}

void SensorSelect::on_tpNew_clicked()
{
    ui->TpCchk->setVisible(false);
    bStx_tp = false;
}

void SensorSelect::on_codOld_clicked()
{
    ui->CodCchk->setVisible(true);
}

void SensorSelect::on_codNew_clicked()
{
    ui->CodCchk->setVisible(false);
    bStx_cod = false;
}

void SensorSelect::on_phOld_clicked()
{
    ui->PhCchk->setVisible(true);
}

void SensorSelect::on_phNew_clicked()
{
    ui->PhCchk->setVisible(false);
    bStx_ph = false;
}

void SensorSelect::on_ssOld_clicked()
{
    ui->SsCchk->setVisible(true);
}

void SensorSelect::on_ssNew_clicked()
{
    ui->SsCchk->setVisible(false);
    bStx_ss = false;
}

void SensorSelect::on_tocOld_clicked()
{
    ui->TocCchk->setVisible(true);
}

void SensorSelect::on_tocNew_clicked()
{
    ui->TocCchk->setVisible(false);
    bStx_toc = false;
}
*/
