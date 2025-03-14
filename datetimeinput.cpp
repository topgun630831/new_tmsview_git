#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QDateTime>
#include "datetimeinput.h"
#include "ui_datetimeinput.h"
#include "calendardlg.h"

extern bool    m_bScreenSave;

DateTimeInput::DateTimeInput(QDateTime datetime, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateTimeInput)
{
    ui->setupUi(this);
    mDate = datetime.date();
    ui->btnDate->setText(mDate.toString(tr("yyyy년 MM월 dd일")));
    ui->editHour->setText(datetime.toString("hh"));
    ui->editMin->setText(datetime.toString("mm"));

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    startTimer(1000);
}

DateTimeInput::~DateTimeInput()
{
    delete ui;
}

void DateTimeInput::on_btnDate_clicked()
{
    CalendarDlg dlg( mDate, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mDate = dlg.getDate();
        ui->btnDate->setText(mDate.toString(tr("yyyy년 MM월 dd일")));
    }
}

void DateTimeInput::on_ok_clicked()
{
    mHour = ui->editHour->text().toInt();
    mMin = ui->editMin->text().toInt();
    QDialog::accept();
}

void DateTimeInput::paintEvent(QPaintEvent *)
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

void DateTimeInput::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
