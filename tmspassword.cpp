#include "tmspassword.h"
#include "ui_tmspassword.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QPainter>
#include <QDebug>

extern bool    m_bScreenSave;

TmsPassword::TmsPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TmsPassword)
{
    ui->setupUi(this);
    mButton[0] = ui->pushButton_0;
    mButton[1] = ui->pushButton_1;
    mButton[2] = ui->pushButton_2;
    mButton[3] = ui->pushButton_3;
    mButton[4] = ui->pushButton_4;
    mButton[5] = ui->pushButton_5;
    mButton[6] = ui->pushButton_6;
    mButton[7] = ui->pushButton_7;
    mButton[8] = ui->pushButton_8;
    mButton[9] = ui->pushButton_9;
    mLabel[0] = ui->label_1;
    mLabel[1] = ui->label_2;
    mLabel[2] = ui->label_3;
    mLabel[3] = ui->label_4;
    mLabel[4] = ui->label_5;
    mLabel[5] = ui->label_6;
    mLabel[6] = ui->label_7;
    mLabel[7] = ui->label_8;
    mLabel[8] = ui->label_9;
    mLabel[9] = ui->label_10;
    for(int i = 0; i < 10; i++)
        connect(mButton[i], SIGNAL(clicked()), this, SLOT(digitClicked()));
    mInputPos = 0;

    const QRect screen = QApplication::desktop()->screenGeometry();
//    QRect rec(0,0,491,437);
    this->move( screen.center() - this->rect().center() );
//    this->move( screen.center() - rec.center());
    startTimer(1000);
}

TmsPassword::~TmsPassword()
{
    delete ui;
}

void TmsPassword::digitClicked()
{
    if(mInputPos >= 10)
        return;
    QPushButton *clickedButton = qobject_cast<QPushButton *>(sender());
    mInputText += clickedButton->text();
    for(int i = 0; i < mInputPos; i++)
        mLabel[i]->setText("●");
    mLabel[mInputPos]->setText(mInputText.at(mInputPos));
    mInputPos++;
//    mLabel[mInputPos++]->setText(tr("●"));
}

void TmsPassword::on_pushButton_back_clicked()
{
    if(mInputPos > 0)
    {
        mLabel[--mInputPos]->setText(tr(" "));
        mInputText.chop(1);
    }
}

void TmsPassword::paintEvent(QPaintEvent *)
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

void TmsPassword::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
