#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "aiselect.h"
#include "ui_aiselect.h"

AiSelect::AiSelect(int ai, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AiSelect)
{
    nAi = ai;
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    if(nAi == 0) ui->none->setChecked(true);
    else if(nAi == 1) ui->ai01->setChecked(true);
    else if(nAi == 2) ui->ai02->setChecked(true);
    else if(nAi == 3) ui->ai03->setChecked(true);
    else if(nAi == 4) ui->ai04->setChecked(true);
    else if(nAi == 5) ui->ai05->setChecked(true);
    else if(nAi == 6) ui->ai06->setChecked(true);
}

AiSelect::~AiSelect()
{
    delete ui;
}

void AiSelect::paintEvent(QPaintEvent *)
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

void AiSelect::on_ok_clicked()
{
    if(ui->none->isChecked())
        nAi = 0;
    else if(ui->ai01->isChecked()) nAi = 1;
    else if(ui->ai02->isChecked()) nAi = 2;
    else if(ui->ai03->isChecked()) nAi = 3;
    else if(ui->ai04->isChecked()) nAi = 4;
    else if(ui->ai05->isChecked()) nAi = 5;
    else if(ui->ai06->isChecked()) nAi = 6;
    accept();
}
