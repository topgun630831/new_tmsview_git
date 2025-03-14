#include "digitalset.h"
#include <QPainter>
#include "ui_digitalset.h"
#include <QDesktopWidget>
#include "tag.h"
#include <QDebug>
#include "infomationdialog.h"

CDigitalSet::CDigitalSet(QString msg, CTag *pTag, CTag *pTag2, bool bTwo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDigitalSet)
{
    ui->setupUi(this);
    if(msg != "")
        ui->label->setText(msg);
    const QRect screen = QApplication::desktop()->screenGeometry();
    ui->editTagName1->setText(pTag->GetTagName());
    ui->editDesc1->setText(pTag->getDesc());
    if(bTwo)
    {
        ui->editTagName2->setText(pTag2->GetTagName());
        ui->editDesc2->setText(pTag2->getDesc());
    }
    else
    {
        ui->groupBoxTag1->setTitle("태그");
        ui->groupBoxTag2->setVisible(false);
        QRect rect = ui->groupBox->geometry();
        rect.setY(190);
        rect.setHeight(91);
        ui->groupBox->setGeometry(rect);
        rect = ui->btnOk->geometry();
        rect.setY(300);
        rect.setHeight(60);
        ui->btnOk->setGeometry(rect);
        rect = ui->btnCancel->geometry();
        rect.setY(300);
        rect.setHeight(60);
        ui->btnCancel->setGeometry(rect);
        rect = geometry();
        rect.setHeight(380);
        setGeometry(rect);
        ui->btnOn->setText(pTag->m_OnMsg);
        ui->btnOff->setText(pTag->m_OffMsg);
    }
    if(pTag->getValue())
        ui->btnOff->setChecked(true);
    else
        ui->btnOn->setChecked(true);
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);
}

CDigitalSet::~CDigitalSet()
{
    delete ui;
}

void CDigitalSet::on_btnOk_clicked()
{
    if(ui->btnOn->isChecked() == false && ui->btnOff->isChecked() == false)
    {
        CInfomationDialog dlg("제어를 선택하지 않았습니다.");
        dlg.exec();
        return;
    }
    m_bOn = ui->btnOn->isChecked();
    accept();
}

void CDigitalSet::paintEvent(QPaintEvent *)
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
