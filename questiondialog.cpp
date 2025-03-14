#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include <QDebug>
#include "questiondialog.h"
#include "ui_questiondialog.h"

CQuestionDialog::CQuestionDialog(QString msg, bool bCheckBox, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CQuestionDialog)
{
    ui->setupUi(this);
    ui->labelMsg->setText(msg);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    ui->checkBox->setVisible(bCheckBox);
    installEventFilter(this);
}

CQuestionDialog::~CQuestionDialog()
{
    delete ui;
}

void CQuestionDialog::changeEvent(QEvent *e)
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

void CQuestionDialog::paintEvent(QPaintEvent *)
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
int rNameAdd5;
bool CQuestionDialog::eventFilter(QObject* , QEvent *e)
{
     return false;
    if(e->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "question" + QString("%1").arg(rNameAdd5++) + ".png";
        pixmap.save(name, "png"/* .toAscii()*/);
        qWarning() << "Captured : " + name;
        return false;
    }
    return false;
}

void CQuestionDialog::on_btnOk_clicked()
{
    m_bCheck = ui->checkBox->isChecked();
    QDialog::accept();
}
