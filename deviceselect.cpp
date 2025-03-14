#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QScrollBar>
#include <QDebug>
#include "deviceselect.h"
#include "ui_deviceselect.h"

extern QSqlDatabase gDb;

#define DEVICE_MAX 12
QString EpsDriverList[DEVICE_MAX] = {
    "SmartFlo",
    "Nivus",
    "MACE",
    "RenaissanceFlow",
    "PenToughFlow",
    "WoojinFlow",
    "XonicFlow",
    "SmartFlo",
    "Oft2000Flow",
    "Modbus",
    "Modbus",
    "System",
};

QString DescList[DEVICE_MAX] = {
    "그린텍",
    "Nivus",
    "MaceFlow",
    "르네상스",
    "펜타포우",
    "우진",
    "Xonic",
    "수인테크",
    "OFT-2000",
    "CK 테크피아",
    "Raven-Eye",
    "4-20mA",
};

DeviceSelect::DeviceSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceSelect)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    ui->tblDevice->setAlternatingRowColors(true);
    ui->tblDevice->setSortingEnabled(true);
    ui->tblDevice->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tblDevice->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tblDevice->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QScrollBar *scroll  = ui->tblDevice->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 20px; height: 30px}");

    for(int i = 0; i < DEVICE_MAX; i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(DescList[i]);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tblDevice->insertRow(i);
        ui->tblDevice->setItem(i, 0, item);
    }

/*
    QString dbname =  "/data/project2.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        qDebug() << "QSqlError : " << err.databaseText();
    }
    QSqlQuery query("SELECT Desc FROM DriverList");
    int i = 0;
    while(query.next())
    {
        QString driver = query.value(0).toString();
        QTableWidgetItem *item = new QTableWidgetItem(driver);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tblDevice->insertRow(i);
        ui->tblDevice->setItem(i++, 0, item);
    }
    gDb.close();
*/
}

DeviceSelect::~DeviceSelect()
{
    delete ui;
}

void DeviceSelect::on_ok_clicked()
{
    QList<QTableWidgetItem *> list = ui->tblDevice->selectedItems();
    if(list.size() >= 1)
    {
        QString desc = list[0]->text();

        for(int i = 0; i < DEVICE_MAX;i++)
        {
            if(DescList[i] == desc)
            {
                sDriver = EpsDriverList[i];
                sDesc = DescList[i];
                accept();
            }
        }
/*
        QString dbname =  "/data/project2.db";
        gDb.setDatabaseName(dbname);
        if(gDb.open() != true)
        {
            QSqlError err = gDb.lastError();
            qDebug() << "QSqlError : " << err.databaseText();
        }
        QString str("SELECT Driver FROM DriverList WHERE Desc='");
        str += desc;
        str += "'";
        QSqlQuery query(str);
        if(query.next())
        {
            sDriver = query.value(0).toString();
            sDesc = desc;
            accept();
        }
        gDb.close();
*/
    }
}

void DeviceSelect::paintEvent(QPaintEvent *)
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
