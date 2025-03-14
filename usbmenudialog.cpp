#include "usbmenudialog.h"
#include "ui_usbmenudialog.h"

#include <QPainter>
#include <QDebug>
#include <QPixmap>
#include <QDesktopWidget>

UsbMenuDialog::UsbMenuDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UsbMenuDialog)
{
    ui->setupUi(this);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
}

UsbMenuDialog::~UsbMenuDialog()
{
    delete ui;
}

void UsbMenuDialog::paintEvent(QPaintEvent *)
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


void UsbMenuDialog::on_Unstoring_clicked()
{
    bUnstoring = true;
    bSetup = false;
    QDialog::accept();
}

void UsbMenuDialog::on_Setup_clicked()
{
    bUnstoring = false;
    bSetup = true;
    QDialog::accept();
}
