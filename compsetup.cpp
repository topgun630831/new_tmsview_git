#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QDebug>
#include "compsetup.h"
#include "ui_compsetup.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "tag.h"
#include "digitalset.h"
#include "analogset.h"
#include "epssetup.h"
#include "monitorudp.h"
#include "fileselect.h"
#include "epsene.h"
#include "srsocket.h"
#include "analogset.h"
extern QString gUseAck;

extern bool g_DebugDisplay;
extern QMap<QString, CTag*> m_TagMap;
extern struct EPS_SETUP SetupData;
extern EpsEnE *g_pEps;
extern CSrSocket *g_pSrSocket;

CompSetup::CompSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompSetup)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_Timer.start(1000);

    if(SetupData.CleanDo == 0)
        ui->groupBox->setEnabled(false);

    if(SetupData.FanDo == 0)
    {
        ui->labelFan->setEnabled(false);
        ui->btnFan->setEnabled(false);
        ui->runFan->setEnabled(false);
    }

    QString text = "Ack 사용안함";
    if(gUseAck == "1")
        text = "Ack 사용함";
    ui->useAck->setText(text);
}

CompSetup::~CompSetup()
{
    delete ui;
}

void CompSetup::paintEvent(QPaintEvent *)
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

void CompSetup::on_btnAuto_clicked()
{
    QString msg = "세정 자동제어";
    CTag *pTag = m_TagMap["AUTOCLEAN_USE"];
    CDigitalSet Set(msg, pTag, pTag, false, this);
    if(Set.exec() == QDialog::Accepted)
    {
        double value;
        if(Set.m_bOn)
            value = 1;
        else
            value = 0;
        pTag->writeValue(value);
    }
}

void CompSetup::on_btnRun_clicked()
{
    QString msg = "세정 가동";
    CTag *pTag = m_TagMap["AUTOCLEAN_DO"];
    CDigitalSet Set(msg, pTag, pTag, false, this);
    if(Set.exec() == QDialog::Accepted)
    {
        double value;
        if(Set.m_bOn)
            value = 1;
        else
            value = 0;
        pTag->writeValue(value);
    }
}

void CompSetup::on_btnTime_clicked()
{
    QString msg = "세정 가동시간";
    CTag *pTag = m_TagMap["AUTOCLEAN_TIME"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_Value);
}

void CompSetup::on_btnIntv_clicked()
{
    QString msg = "세정 가동주기";
    CTag *pTag = m_TagMap["AUTOCLEAN_PERIOD"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_Value);
}

void CompSetup::onTimer()
{
    QString val;
    CTag *pTag = m_TagMap["AUTOCLEAN_USE"];
    if(pTag->getValue() == 1)
        val = pTag->m_OnMsg;
    else
        val = pTag->m_OffMsg;
    ui->autoComp->setText(val);

    if(pTag->getValue() == 1)
        ui->btnRun->setEnabled(false);
    else
        ui->btnRun->setEnabled(true);
    pTag = m_TagMap["AUTOCLEAN_DO"];
    if(pTag->getValue() == 1)
        val = pTag->m_OnMsg;
    else
        val = pTag->m_OffMsg;
    ui->runComp->setText(val);

    pTag = m_TagMap["AUTOCLEAN_TIME"];
    val = QString("%1").arg(pTag->getValue());
    ui->autoCompTime->setText(val);

    pTag = m_TagMap["AUTOCLEAN_PERIOD"];
    val = QString("%1").arg(pTag->getValue());
    ui->autoCompIntv->setText(val);

    pTag = m_TagMap["FAN_DO"];
    if(pTag->getValue() == 1)
        val = pTag->m_OnMsg;
    else
        val = pTag->m_OffMsg;
    ui->runFan->setText(val);

    pTag = m_TagMap["MALFUNCTION"];
    if(pTag->getValue() == 1)
        val = pTag->m_OnMsg;
    else
        val = pTag->m_OffMsg;
    ui->malf->setText(val);

    pTag = m_TagMap["REPAIR"];
    if(pTag->getValue() == 1)
        val = pTag->m_OnMsg;
    else
        val = pTag->m_OffMsg;
    ui->repair->setText(val);

}

void CompSetup::on_btnFan_clicked()
{
    QString msg = "FAN 가동";
    CTag *pTag = m_TagMap["FAN_DO"];
    CDigitalSet Set(msg, pTag, pTag, false, this);
    if(Set.exec() == QDialog::Accepted)
    {
        double value;
        if(Set.m_bOn)
            value = 1;
        else
            value = 0;
        pTag->writeValue(value);
    }
}

void CompSetup::on_btnMalf_clicked()
{
    QString msg = "고장 설정";
    CTag *pTag = m_TagMap["MALFUNCTION"];
    CDigitalSet Set(msg, pTag, pTag, false, this);
    if(Set.exec() == QDialog::Accepted)
    {
        double value;
        if(Set.m_bOn)
            value = 1;
        else
            value = 0;
        pTag->writeValue(value);
    }
}

void CompSetup::on_btnRepair_clicked()
{
    QString msg = "보수 설정";
    CTag *pTag = m_TagMap["REPAIR"];
    CDigitalSet Set(msg, pTag, pTag, false, this);
    if(Set.exec() == QDialog::Accepted)
    {
        double value;
        if(Set.m_bOn)
            value = 1;
        else
            value = 0;
        pTag->writeValue(value);
    }
}

void CompSetup::on_btnAck_clicked()
{
    CQuestionDialog dlg("Ack 사용을 변경하시겠습니까?");
    QString text;
    if(gUseAck == "1")
        text = "Ack 사용함";
    if(dlg.exec() == QDialog::Accepted)
    {
        bool bAck;
        if(gUseAck == "0")
        {
            gUseAck = "1";
            text = "Ack 사용함";
        }
        else
        {
            gUseAck = "0";
            text = "Ack 사용안함";
        }
        ui->useAck->setText(text);
        g_pEps->ShareDataSetSend();
    }
}
