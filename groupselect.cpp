#include <QScrollBar>
#include "groupselect.h"
#include "ui_groupselect.h"
extern QString gSoftwareModel;

CGroupSelect::CGroupSelect(QStringList list, QStringList listdevice, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CGroupSelect)
{
    ui->setupUi(this);

    int i = 0;
    QTableWidget *table = ui->tableGroup;
    QScrollBar *scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;}");
    table->setRowCount(list.size());
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    mbDevice = false;
    foreach(QString group, list)
    {
        table->verticalHeader()->resizeSection(i,   50);
        QTableWidgetItem *item = new QTableWidgetItem(group);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 0, item);
        i++;
    }
    table = ui->tableDevice;
    scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;}");
    table->setRowCount(listdevice.size());
    table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    i = 0;
    foreach(QString group, listdevice)
    {
        table->verticalHeader()->resizeSection(i,   50);
        QTableWidgetItem *item = new QTableWidgetItem(group);
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        table->setItem(i, 0, item);
        i++;
    }
    ui->stackedWidget->setCurrentIndex(0);
    m_btnDeviceSelect = false;
    mSelectedIndex = -1;
//    if(gSoftwareModel == "ORION_TMS")
    {
        ui->btnDeviceSelect->setVisible(false);
        ui->ok->setVisible(false);
    }
}

CGroupSelect::~CGroupSelect()
{
    delete ui;
}

void CGroupSelect::on_ok_clicked()
{
    if(mbDevice)
        on_tableDevice_doubleClicked(QModelIndex());
    else
        on_tableGroup_doubleClicked(QModelIndex());
    mSelectedIndex = -1;
}

void CGroupSelect::on_tableGroup_doubleClicked(const QModelIndex &)
{
    QList<QTableWidgetItem *> list = ui->tableGroup->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        QTableWidgetItem *item = ui->tableGroup->item(row,0);
        mGroup = item->text();
        mbDevice = false;
        accept();
    }
}

void CGroupSelect::on_btnDeviceSelect_clicked()
{
    if(m_btnDeviceSelect == false)
    {
        ui->btnDeviceSelect->setText(tr("Group"));
        ui->stackedWidget->setCurrentIndex(1);
        m_btnDeviceSelect = true;
    }
    else
    {
        ui->btnDeviceSelect->setText(tr("Device"));
        ui->stackedWidget->setCurrentIndex(0);
        m_btnDeviceSelect = false;
    }
}

void CGroupSelect::on_tableDevice_doubleClicked(const QModelIndex &)
{
    QList<QTableWidgetItem *> list = ui->tableDevice->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        QTableWidgetItem *item = ui->tableDevice->item(row,0);
        mGroup = item->text();
        mbDevice = true;
        accept();
    }
}

void CGroupSelect::on_tableDevice_clicked(const QModelIndex &index)
{
    QList<QTableWidgetItem *> list = ui->tableDevice->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if(mSelectedIndex == row)
            on_tableDevice_doubleClicked(index);
        else
            mSelectedIndex = row;
    }
}

void CGroupSelect::on_tableGroup_clicked(const QModelIndex &index)
{
    QList<QTableWidgetItem *> list = ui->tableGroup->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if(mSelectedIndex == row)
            on_tableGroup_doubleClicked(index);
        else
            mSelectedIndex = row;
    }
}
