#include "touchtest.h"
#include "ui_touchtest.h"
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QDateTime>
#include "mondialog.h"
#include <QJsonObject>
#include <QJsonArray>
#include "monitorudp.h"
extern int XMaxRes;
extern int YMaxRes;

CTouchTest::CTouchTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTouchTest)
{
    ui->setupUi(this);
    mImage = new QImage(QSize(XMaxRes,YMaxRes),
                        QImage::Format_ARGB32_Premultiplied);

    QPainter painter(mImage);
    painter.fillRect(0, 0, XMaxRes, YMaxRes, Qt::black);
    m_bLog = false;
    ui->listWidget->setVisible(false);
}

CTouchTest::~CTouchTest()
{
    system("killall touchuart");
    delete ui;
    delete mImage;
}

void CTouchTest::mousePressEvent(QMouseEvent* event)
{
    mStartPoint = mEndPoint = event->pos();
    QDateTime time = QDateTime::currentDateTime();
    if(m_bLog)
    {
        QString str = QString("%1 Press(%2,%3)").arg(time.toString("yyyy-MM-dd hh:mm:ss")).arg(mStartPoint.x()).arg(mStartPoint.y());
        ui->listWidget->addItem(str);
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
}

void CTouchTest::mouseMoveEvent(QMouseEvent *event)
{
    mEndPoint = event->pos();
    QDateTime time = QDateTime::currentDateTime();
    if(m_bLog)
    {
        QString str = QString("%1 Move(%2,%3)").arg(time.toString("yyyy-MM-dd hh:mm:ss")).arg(mEndPoint.x()).arg(mEndPoint.y());
        ui->listWidget->addItem(str);
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
    paint();
    mStartPoint = event->pos();
}

void CTouchTest::mouseReleaseEvent(QMouseEvent *event)
{
    mEndPoint = event->pos();
    QDateTime time = QDateTime::currentDateTime();
    if(m_bLog)
    {
        QString str = QString("%1 Release(%2,%3)").arg(time.toString("yyyy-MM-dd hh:mm:ss")).arg(mEndPoint.x()).arg(mEndPoint.y());
        ui->listWidget->addItem(str);
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
    paint();
}

void CTouchTest::paint()
{
    QPainter painter(mImage);
    painter.setPen(QPen(Qt::white, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    if(mStartPoint != mEndPoint)
        painter.drawLine(mStartPoint, mEndPoint);
    else
        painter.drawPoint(mStartPoint);
    update(QRect(mStartPoint, mEndPoint).normalized()
                                     .adjusted(-2, -2, +2, +2));
}

void CTouchTest::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, *mImage, dirtyRect);
}

void CTouchTest::on_clear_clicked()
{
    QPainter painter(mImage);
    painter.fillRect(0, 0, XMaxRes, YMaxRes, Qt::black);
    ui->listWidget->clear();
    update();
}

void CTouchTest::on_run_clicked()
{
    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);

    QString proc = "Sun";
    bool SaveStatus=false;
    argObject.insert(QString("Proc"),QJsonValue(proc));
    argObject.insert(QString("Enabled"),QJsonValue(true));
    argObject.insert(QString("SaveStatus"),QJsonValue(SaveStatus));

    CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Set"), argObject, vObject);
}

void CTouchTest::on_checkBox_clicked()
{
    if(ui->checkBox->isChecked())
    {
        m_bLog = true;
        ui->listWidget->setVisible(true);
    }
    else
    {
        m_bLog = false;
        ui->listWidget->setVisible(false);
    }
}

void CTouchTest::on_run_2_clicked()
{
    system("/usr/sbin/touchuart &");
}

void CTouchTest::on_listWidget_clicked(const QModelIndex &index)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = QString("%1 History Click").arg(time.toString("yyyy-MM-dd hh:mm:ss"));
    ui->listWidget->addItem(str);
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}
