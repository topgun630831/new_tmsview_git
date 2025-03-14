#include <QPainter>
#include "analogset.h"
#include "ui_analogset.h"
#include <QDesktopWidget>
#include "tag.h"

CAnalogSet::CAnalogSet(QString msg, CTag *pTag, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CAnalogSet)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    if(msg != "")
        ui->label->setText(msg);
    ui->editTagName->setText(pTag->GetTagName());
    ui->editDesc->setText(pTag->getDesc());
    ui->editValue->setText(QString().arg(pTag->getValue()));
    ui->labelUnit->setText(pTag->getUnit());
    installEventFilter(this);
}

CAnalogSet::~CAnalogSet()
{
    delete ui;
}

void CAnalogSet::on_btnOk_clicked()
{
    m_sValue = ui->editValue->text();
    m_Value = m_sValue.toDouble();
    accept();
}

void CAnalogSet::paintEvent(QPaintEvent *)
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
