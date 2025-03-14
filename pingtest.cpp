#include <QDesktopWidget>
#include <QPainter>
#include <QFile>
#include "pingtest.h"
#include "ui_pingtest.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

extern QSqlDatabase gDb;

PingTest::PingTest(QString ip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PingTest)
{
    ui->setupUi(this);

    ui->comboBox->addItem(ip);
    gDb.setDatabaseName(tr("/data/project2.db"));
    if(gDb.open() == true)
    {
        QSqlQuery query("SELECT Ip from Port Where Kind = 'TcpServer' OR Kind = 'TcpClient'");
        while (query.next())
        {
            QString ip = query.value(0).toString();
            if(ui->comboBox->findText(ip) == -1)
                ui->comboBox->addItem(ip);
        }
        gDb.close();
    }
    ui->Ip->setText(ip);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
}

PingTest::~PingTest()
{
    delete ui;
}

void PingTest::paintEvent(QPaintEvent *)
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

void PingTest::on_btnTest_clicked()
{
    ui->list->clear();
    ui->list->addItem(QString("Ping Testing....."));
    QString ping = QString("ping -c 3 -W 1 %1 > /tmp/ping.txt").arg(ui->Ip->text());
    system(ping.toLocal8Bit().data());
    QFile file("/tmp/ping.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray ar;
    for(int i = 0; i < 2; i++)
        ar = file.readLine();
    ar = file.readAll();
    ui->list->addItem(QString(ar.data()));
    file.close();
}

void PingTest::on_comboBox_currentTextChanged(const QString &arg1)
{
    ui->Ip->setText(arg1);
}
