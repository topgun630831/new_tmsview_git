#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QDebug>
#include "passwordinput.h"
#include "ui_passwordinput.h"

extern bool    m_bScreenSave;

PasswordInput::PasswordInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PasswordInput)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);
    startTimer(1000);
}

PasswordInput::~PasswordInput()
{
    delete ui;
}

void PasswordInput::paintEvent(QPaintEvent *)
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

void PasswordInput::on_ok_clicked()
{
    m_sPassword = ui->PasswordEdit->text();
    QDialog::accept();
}

void PasswordInput::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
