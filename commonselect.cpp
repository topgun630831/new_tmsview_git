#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "commonselect.h"
#include "ui_commonselect.h"
#include <QScrollBar>
#include <QDebug>
#define SENSOR_ROW_HIGHT   80

CComMonSelect::CComMonSelect(QStringList portList, QList<COMM_SELECT*> commList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CComMonSelect)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    QScrollBar *scroll  = ui->tableWidgetComm->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;}");
    QTableWidget *table = ui->tableWidgetComm;
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    int i = 0;
    table->setRowCount(commList.size());
    foreach(struct COMM_SELECT *comm, commList)
    {
        table->verticalHeader()->resizeSection(i, 50);
        QTableWidgetItem *item = new QTableWidgetItem(comm->Driver);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 0, item);
        item = new QTableWidgetItem(comm->Device);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 1, item);
        item = new QTableWidgetItem(comm->Port);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 2, item);
        i++;
    }

    scroll  = ui->tableWidgetPort->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;}");
    table = ui->tableWidgetPort;
    i = 0;
    table->setRowCount(portList.size());
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    foreach(QString port, portList)
    {
        table->verticalHeader()->resizeSection(i,   50);
        QTableWidgetItem *item = new QTableWidgetItem(port);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 0, item);
        i++;
    }
    mSelectedIndex = -1;
    mbPort = false;
    ui->tableWidgetPort->verticalHeader()->setDefaultSectionSize( SENSOR_ROW_HIGHT );
    ui->tableWidgetComm->verticalHeader()->setDefaultSectionSize( SENSOR_ROW_HIGHT );
}

CComMonSelect::~CComMonSelect()
{
    delete ui;
}

void CComMonSelect::on_btnPortSelect_clicked()
{
    mbPort = ui->btnPortSelect->isChecked();
    if(mbPort)
        ui->stackedWidget->setCurrentIndex(1);
    else
        ui->stackedWidget->setCurrentIndex(0);
    mSelectedIndex = -1;
}

void CComMonSelect::on_ok_clicked()
{
    bSelected = false;
    if(mbPort == false)
        on_tableWidgetComm_doubleClicked(QModelIndex());
    else
        on_tableWidgetPort_doubleClicked(QModelIndex());
}

void CComMonSelect::on_tableWidgetComm_doubleClicked(const QModelIndex &)
{
    QList<QTableWidgetItem *> list = ui->tableWidgetComm->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        QTableWidgetItem *item = ui->tableWidgetComm->item(row,0);
        mDriver = item->text();
        item = ui->tableWidgetComm->item(row,1);
        mDevice = item->text();
        item = ui->tableWidgetComm->item(row,2);
        mPort = item->text();
        bSelected = true;
        accept();
    }
}

void CComMonSelect::on_tableWidgetPort_doubleClicked(const QModelIndex &)
{
    QList<QTableWidgetItem *> list = ui->tableWidgetPort->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        QTableWidgetItem *item = ui->tableWidgetPort->item(row,0);
        mPort = item->text();
        bSelected = true;
        accept();
    }
}

void CComMonSelect::on_tableWidgetComm_clicked(const QModelIndex &index)
{
    QList<QTableWidgetItem *> list = ui->tableWidgetComm->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if(mSelectedIndex == row)
            on_tableWidgetComm_doubleClicked(index);
        else
            mSelectedIndex = row;
    }
}

void CComMonSelect::on_tableWidgetPort_clicked(const QModelIndex &index)
{
    QList<QTableWidgetItem *> list = ui->tableWidgetPort->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if(mSelectedIndex == row)
            on_tableWidgetPort_doubleClicked(index);
        else
            mSelectedIndex = row;
    }
}

void CComMonSelect::paintEvent(QPaintEvent *)
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
