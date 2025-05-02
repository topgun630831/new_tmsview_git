#include <QApplication>
#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QDesktopWidget>
#include "sensorselect.h"
#include "ui_sensorselect.h"
#include "diselect.h"
#include "infomationdialog.h"

extern bool    m_bScreenSave;

SensorSelect::SensorSelect( int FlowNum, bool WaterTemp, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SensorSelect)
{
    ui->setupUi(this);

    mFlowLabel[0] = ui->f_label1;
    mFlowLabel[1] = ui->f_label2;
    mFlowLabel[2] = ui->f_label3;
    mFlowLabel[3] = ui->f_label4;
    mFlowLabel[4] = ui->f_label5;
    mFlowComboBox[0] = ui->f_comPort1;
    mFlowComboBox[1] = ui->f_comPort2;
    mFlowComboBox[2] = ui->f_comPort3;
    mFlowComboBox[3] = ui->f_comPort4;
    mFlowComboBox[4] = ui->f_comPort5;
    mFlowCheckBox[0] = ui->rate1;
    mFlowCheckBox[1] = ui->rate2;
    mFlowCheckBox[2] = ui->rate3;
    mFlowCheckBox[3] = ui->rate4;
    mFlowCheckBox[4] = ui->rate5;
//    mProtocolComboBox[0] = ui->protocol_1;
//    mProtocolComboBox[1] = ui->protocol_2;
//    mProtocolComboBox[2] = ui->protocol_3;
//    mProtocolComboBox[3] = ui->protocol_4;
//    mProtocolComboBox[4] = ui->protocol_5;
    mRatio[0] = ui->ratio_1;
    mRatio[1] = ui->ratio_2;
    mRatio[2] = ui->ratio_3;
    mRatio[3] = ui->ratio_4;
    mRatio[4] = ui->ratio_5;

    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < mFlowComboBox[i]->count(); j++)
            mFlowComboBox[i]->model()->setData(mFlowComboBox[i]->model()->index(j, 0), QSize(50, 50), Qt::SizeHintRole);
//        for(int j = 0; j < mProtocolComboBox[i]->count(); j++)
//            mProtocolComboBox[i]->model()->setData(mProtocolComboBox[i]->model()->index(j, 0), QSize(50, 50), Qt::SizeHintRole);
    }
    nFlowNum = FlowNum;
    NumChangeCheck();

    sTnComPort = "Com1";
    sTpComPort = "Com2";
    sTocComPort = "Com3";
    sPhComPort = "Com4";
    sSsComPort = "Com5";
    sCodComPort = "Com6";
    sFlowComPort = "Com7";
    sSamplerComPort = "Com8";

    ui->num->setText(QString("%1").arg(FlowNum));
//    ui->ratio->setText(QString("%1").arg(FlowRatio));
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
    //ui->Analog_toc->setVisible(false);

//    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    m_timerId = startTimer(1000*60*5-10);

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
//     InitDisp(sFlow, false, "Flow", ui->flowOld, ui->flowNew, 0);

    if(bAnalog_flow)
        ui->Analog_flow->setChecked(true);
//    if(bAnalog_tn)
//        ui->Analog_tn->setChecked(true);
//    if(bAnalog_tp)
//        ui->Analog_tp->setChecked(true);
//    if(bAnalog_cod)
//        ui->Analog_cod->setChecked(true);
//    if(bAnalog_ph)
//        ui->Analog_ph->setChecked(true);
//    if(bAnalog_ss)
//        ui->Analog_ss->setChecked(true);
//    if(bAnalog_toc)
//        ui->Analog_toc->setChecked(true);
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

    if(sSamModel == "Io")
    {
        chkExternalIO(true);
    }
    else
        chkExternalIO(false);

    for(int i = 0; i < 5; i++)
    {
        mFlowComboBox[i]->setCurrentText(m_FlowPort[i]);
//        mProtocolComboBox[i]->setCurrentText(m_FlowProtocol[i]);
        mFlowCheckBox[i]->setChecked(m_bFlowRate[i]);
        mRatio[i]->setText(QString("%1").arg(m_Ratio[i]));
    }
    ui->sampDoorReverse->setChecked(m_bDoorReversed);
    if(m_nSampDoor)
    {
        QString address = QString("DI %1").arg(m_nSampDoor);
        ui->sampDoor->setText(address);
//        ui->chkExternalIO->setChecked(m_bSamplerDoorExternal);
    }


//    ui->rate->setVisible(false);
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
    //sFlow = ui->FlowGroup->checkedButton()->text();
    //if(sFlow == "NewFlow")
    //    bStx_flow = false;
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

//    sSamModel = ui->SamModelGroup->checkedButton()->text();
    bTn = ui->TN->isChecked();
    bTp = ui->TP->isChecked();
    bCod = ui->COD->isChecked();
    bPh = ui->PH->isChecked();
    bSs = ui->SS->isChecked();
    bFlow = true; //ui->FLOW->isChecked();
    bSampler = true; //ui->SAMPLER->isChecked();
    bToc = ui->TOC->isChecked();
    nFlowNum = ui->num->text().toInt();
//    nFlowRatio = ui->ratio->text().toFloat();
    for(int i = 0; i < 5; i++)
    {
        m_FlowPort[i] = mFlowComboBox[i]->currentText();
//        m_FlowProtocol[i] = mProtocolComboBox[i]->currentText();
        m_bFlowRate[i] = mFlowCheckBox[i]->isChecked();
        m_Ratio[i] = mRatio[i]->text().toFloat();
    }
    for(int i = 0; i < nFlowNum; i++)
    {
        for(int j = 0; j < nFlowNum; j++)
        {
            if(i == j)
                continue;
            if(m_FlowPort[i] == m_FlowPort[j])
            {
                CInfomationDialog dlg("중복된 포트가 있습니다");
                dlg.exec();
                return;
            }
        }
    }
//    m_bSamplerDoorExternal = ui->chkExternalIO->isChecked();
    m_bDoorReversed = false;
    if(ui->sampDoorReverse->isChecked())
        m_bDoorReversed = true;

//    bRate = ui->rate->isChecked();
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
    bAnalog_tn = false;
    bAnalog_tp = false;
    bAnalog_cod = false;
    bAnalog_ph = false;
    bAnalog_ss = false;
//    bAnalog_toc = ui->Analog_toc->isChecked();
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

void SensorSelect::NumChangeCheck(void)
{
    bool visible;
    for(int i = 0; i < 5; i++)
    {
        if(i < nFlowNum)
           visible = true;
        else
            visible = false;
        mFlowLabel[i]->setVisible(visible);
        mFlowComboBox[i]->setVisible(visible);
        mFlowCheckBox[i]->setVisible(visible);
//        mProtocolComboBox[i]->setVisible(visible);
        mRatio[i]->setVisible(visible);
    }
}

void SensorSelect::on_num_textChanged(const QString &arg1)
{
    int num = arg1.toInt();
    if(num <= 0 || num > 5)
    {
        QString str = QString("%1").arg(nFlowNum);
        ui->num->setText(str);
    }
    else
    {
        nFlowNum = num;
        NumChangeCheck();
    }
}

void SensorSelect::DiDisp(int pos, QPushButton *btn)
{
    QString str = "사용안함";
    if(pos)
        str = QString("DI %1").arg(pos);
    btn->setText(str);
}

void SensorSelect::on_sampDoor_clicked()
{
    DiSelect dlg(m_nSampDoor, 6);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nSampDoor = dlg.nDi;
        if(m_nSampDoor == 0)
            m_nSampDoor = 5;
        DiDisp(m_nSampDoor, ui->sampDoor);
    }
}
/*
int SensorSelect::SetDi(const char* name, QPushButton *btn, QCheckBox *chk)
{
    int pos = 0;
    QString str = QString("select Address, Reversed from Tag where Name = '%1'").arg(name);
    QSqlQuery query(str);
    if (query.next())
    {
        QString address = query.value(0).toString();
        address = address.mid(2);
        pos = address.toInt();
        if(pos)
        {
            address = QString("DI %1").arg(pos);
            btn->setText(address);
            if(query.value(1).toInt() == 1)
                chk->setChecked(true);
        }
    }
    return pos;
}
*/
void SensorSelect::chkExternalIO(bool checked)
{
    if(checked == true)
    {
        if(m_nSampDoor == 0)
            m_nSampDoor = 5;
    }
    else
        m_nSampDoor = 0;
    DiDisp(m_nSampDoor, ui->sampDoor);
    ui->sampDoor->setEnabled(checked);
    ui->sampDoorReverse->setEnabled(checked);
}

void SensorSelect::on_Io_clicked()
{
    chkExternalIO(true);
    sSamModel = "Io";
    m_bSamplerDoorExternal = true;
}

void SensorSelect::on_Year100_clicked()
{
    chkExternalIO(false);
    sSamModel = "Year100";
    m_bSamplerDoorExternal = false;
}

void SensorSelect::on_Hanchang_clicked()
{
    chkExternalIO(false);
    sSamModel = "Hanchang";
    m_bSamplerDoorExternal = false;
}

void SensorSelect::on_Watersam_clicked()
{
    chkExternalIO(false);
    sSamModel = "Watersam";
    m_bSamplerDoorExternal = false;
}

void SensorSelect::on_Korbi_clicked()
{
    chkExternalIO(false);
    sSamModel = "Korbi";
    m_bSamplerDoorExternal = false;
}

void SensorSelect::on_Ilho_clicked()
{
    chkExternalIO(false);
    sSamModel = "Ilho";
    m_bSamplerDoorExternal = false;
}

void SensorSelect::on_Dongil_clicked()
{
    chkExternalIO(false);
    sSamModel = "Dongil";
    m_bSamplerDoorExternal = false;
}
