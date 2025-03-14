#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QDebug>

#include "infomationdialog.h"
#include "ui_infomationdialog.h"

CInfomationDialog::CInfomationDialog(QString msg, bool wait, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInfomationDialog)
{
    ui->setupUi(this);
    ui->labelMsg->setText(msg);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);
    if(!wait)
        ui->btnOk->setVisible(false);
}

CInfomationDialog::~CInfomationDialog()
{
    delete ui;
}

void CInfomationDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CInfomationDialog::paintEvent(QPaintEvent *)
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

int rNameAdd4;
bool CInfomationDialog::eventFilter(QObject* , QEvent *e)
{
    return false;
    if(e->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "info" + QString("%1").arg(rNameAdd4++) + ".png";
        pixmap.save(name, "png"/* .toAscii()*/);
        qWarning() << "Captured : " + name;
        return false;
    }
    return false;
}
