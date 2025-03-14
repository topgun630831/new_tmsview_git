#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "diselect.h"
#include "ui_diselect.h"

DiSelect::DiSelect(int di, int total, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiSelect)
{
    nDi = di;
    ui->setupUi(this);
/*
    if(total == 4)
    {
        setGeometry(0,0,467, 294);
        ui->none->setGeometry(20,100, 91,40);
        ui->ok->setGeometry(103,210,121,40);
        ui->cancel->setGeometry(233,210,121,40);
        ui->groupBox->setGeometry(20,10,411,171);
        ui->di05->setVisible(false);
        ui->di06->setVisible(false);
        ui->di07->setVisible(false);
        ui->di08->setVisible(false);
        ui->di09->setVisible(false);
        ui->di10->setVisible(false);
        ui->di11->setVisible(false);
        ui->di12->setVisible(false);
        ui->di13->setVisible(false);
        ui->di14->setVisible(false);
        ui->di15->setVisible(false);
        ui->di16->setVisible(false);
    }
    else
    if(total == 6)
    {
        setGeometry(0,0,631, 294);
        ui->none->setGeometry(20,100, 91,40);
        ui->ok->setGeometry(186,210,121,40);
        ui->cancel->setGeometry(316,210,121,40);
        ui->groupBox->setGeometry(20,10,581,171);
        ui->di07->setVisible(false);
        ui->di08->setVisible(false);
        ui->di09->setVisible(false);
        ui->di10->setVisible(false);
        ui->di11->setVisible(false);
        ui->di12->setVisible(false);
        ui->di13->setVisible(false);
        ui->di14->setVisible(false);
        ui->di15->setVisible(false);
        ui->di16->setVisible(false);
    }
    */
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    if(nDi == 0) ui->none->setChecked(true);
    else if(nDi == 1) ui->di01->setChecked(true);
    else if(nDi == 2) ui->di02->setChecked(true);
    else if(nDi == 3) ui->di03->setChecked(true);
    else if(nDi == 4) ui->di04->setChecked(true);
    else if(nDi == 5) ui->di05->setChecked(true);
    else if(nDi == 6) ui->di06->setChecked(true);
 /*   else if(nDi == 7) ui->di07->setChecked(true);
    else if(nDi == 8) ui->di08->setChecked(true);
    else if(nDi == 9) ui->di09->setChecked(true);
    else if(nDi == 10) ui->di10->setChecked(true);
    else if(nDi == 11) ui->di11->setChecked(true);
    else if(nDi == 12) ui->di12->setChecked(true);
    else if(nDi == 13) ui->di13->setChecked(true);
    else if(nDi == 14) ui->di14->setChecked(true);
    else if(nDi == 15) ui->di15->setChecked(true);
    else if(nDi == 16) ui->di16->setChecked(true);
    */
}

DiSelect::~DiSelect()
{
    delete ui;
}

void DiSelect::paintEvent(QPaintEvent *)
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

void DiSelect::on_ok_clicked()
{
    if(ui->none->isChecked())
        nDi = 0;
    else if(ui->di01->isChecked()) nDi = 1;
    else if(ui->di02->isChecked()) nDi = 2;
    else if(ui->di03->isChecked()) nDi = 3;
    else if(ui->di04->isChecked()) nDi = 4;
    else if(ui->di05->isChecked()) nDi = 5;
    else if(ui->di06->isChecked()) nDi = 6;
/*    else if(ui->di07->isChecked()) nDi = 7;
    else if(ui->di08->isChecked()) nDi = 8;
    else if(ui->di09->isChecked()) nDi = 9;
    else if(ui->di10->isChecked()) nDi = 10;
    else if(ui->di11->isChecked()) nDi = 11;
    else if(ui->di12->isChecked()) nDi = 12;
    else if(ui->di13->isChecked()) nDi = 13;
    else if(ui->di14->isChecked()) nDi = 14;
    else if(ui->di15->isChecked()) nDi = 15;
    else if(ui->di16->isChecked()) nDi = 16;
    */
    accept();
}
