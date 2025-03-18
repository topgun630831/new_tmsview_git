#include "lcdtestdialog.h"
#include "ui_lcdtestdialog.h"
#include <QPainter>

extern bool    m_bScreenSave;

Qt::GlobalColor testColor[] = {
    Qt::white,
    Qt::black,
    Qt::red,
    Qt::darkRed,
    Qt::green,
    Qt::darkGreen,
    Qt::blue,
    Qt::darkBlue,
    Qt::cyan,
    Qt::darkCyan,
    Qt::magenta,
    Qt::darkMagenta,
    Qt::yellow,
    Qt::darkYellow,
    Qt::gray,
    Qt::darkGray,
    Qt::lightGray
};
CLcdTestDialog::CLcdTestDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLcdTestDialog)
{
    ui->setupUi(this);
    startTimer(1000);
}

CLcdTestDialog::~CLcdTestDialog()
{
    delete ui;
}

void CLcdTestDialog::mousePressEvent(QMouseEvent* )
{
}

void CLcdTestDialog::paintEvent(QPaintEvent *)
{
    QPainter *painter = new QPainter(this);
    int y = 0;
    for(int i =0; i < 17; i++)
    {
        painter->fillRect(0, y, 1920, y+70,    testColor[i]);
        y += 70;
    }
    painter->end();
}

void CLcdTestDialog::timerEvent(QTimerEvent *)
{
//    if (m_bScreenSave == true)
//        QDialog::reject();
}
