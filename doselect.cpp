#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "doselect.h"
#include "ui_doselect.h"

DoSelect::DoSelect(int ndo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoSelect)
{
    nDo = ndo;
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    if(nDo == 0) ui->none->setChecked(true);
    else if(nDo == 1) ui->do01->setChecked(true);
    else if(nDo == 2) ui->do02->setChecked(true);
    else if(nDo == 3) ui->do03->setChecked(true);
    else if(nDo == 4) ui->do04->setChecked(true);
    else if(nDo == 5) ui->do05->setChecked(true);
    else if(nDo == 6) ui->do06->setChecked(true);
    else if(nDo == 7) ui->do07->setChecked(true);
    else if(nDo == 8) ui->do08->setChecked(true);
}

DoSelect::~DoSelect()
{
    delete ui;
}

void DoSelect::paintEvent(QPaintEvent *)
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

void DoSelect::on_ok_clicked()
{
    if(ui->none->isChecked())
        nDo = 0;
    else if(ui->do01->isChecked()) nDo = 1;
    else if(ui->do02->isChecked()) nDo = 2;
    else if(ui->do03->isChecked()) nDo = 3;
    else if(ui->do04->isChecked()) nDo = 4;
    else if(ui->do05->isChecked()) nDo = 5;
    else if(ui->do06->isChecked()) nDo = 6;
    else if(ui->do07->isChecked()) nDo = 7;
    else if(ui->do08->isChecked()) nDo = 8;
    accept();
}
