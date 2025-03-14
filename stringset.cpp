#include <QPainter>
#include "stringset.h"
#include "ui_stringset.h"
#include <QDesktopWidget>
#include "tag.h"

CStringSet::CStringSet(QString msg, CTag *pTag, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CStringSet)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    if(msg != "")
        ui->label->setText(msg);
    ui->editTagName->setText(pTag->GetTagName());
    ui->editDesc->setText(pTag->getDesc());
    ui->editValue->setText(pTag->getValueS());
    installEventFilter(this);
}

CStringSet::~CStringSet()
{
    delete ui;
}

void CStringSet::on_btnOk_clicked()
{
    m_sValue = ui->editValue->text();
    accept();
}

void CStringSet::paintEvent(QPaintEvent *)
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

