#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "route.h"
#include "ui_route.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include <QDebug>

extern bool    m_bScreenSave;

Route::Route(QList<Gateway_TAB *> *gatewaylist, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Route)
{
    GatewayList = gatewaylist;
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    QTableWidget *table = ui->tableWidget;
    table->horizontalHeader()->resizeSection(0, 200);    // Interface
    table->horizontalHeader()->resizeSection(1, 300);   // Dest
    table->horizontalHeader()->resizeSection(2, 300);   // Netmask
    table->horizontalHeader()->resizeSection(3, 300);   // GatewayIp

    table->setRowCount(GatewayList->length());
    int row = 0;
    foreach (const Gateway_TAB * gateway, *GatewayList) {
        SetItem(gateway->Interface, row, 0);
        SetItem(gateway->Dest, row, 1);
        SetItem(gateway->Netmask, row, 2);
        SetItem(gateway->GatewayIp, row, 3);
        row++;
    }
    mbChanged = false;
    startTimer(1000);
}

Route::~Route()
{
    delete ui;
}

void Route::paintEvent(QPaintEvent *)
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

void Route::SetItem(QString str, int row, int col)
{
    QTableWidgetItem *item = new QTableWidgetItem(str);
    ui->tableWidget->setItem(row, col, item);
}

void Route::on_add_clicked()
{
    QString Interface = ui->Interface->currentText();
    QString Dest = ui->Dest->text();
    QString Netmask = ui->Netmask->text();
    QString GatewayIp = ui->GatewayIp->text();

    if(CNetworkSetup::IpValidate(Dest, QString(tr("IP 주소가 올바르지 않습니다"))) == false)
        return;
    if(CNetworkSetup::IpValidate(Netmask, QString(tr("넷마스크 주소가 올바르지 않습니다"))) == false)
        return;
    if(CNetworkSetup::IpValidate(GatewayIp, QString(tr("게이트웨이 주소가 올바르지 않습니다"))) == false)
        return;

    QStringList DestList = Dest.split(QChar('.'));
    QStringList NetmaskList = Netmask.split(QChar('.'));
    if(DestList.length() != 4 || NetmaskList.length() != 4)
    {
        CInfomationDialog dlg(tr("IP나 넷마스크가 올바르지 않습니다."));
        dlg.exec();
        return;
    }
    int DestVal[4];
    int NetmaskVal[4];
    for(int i = 0; i < 4; i++)
    {
        DestVal[i] = DestList[i].toInt();
        NetmaskVal[i] = NetmaskList[i].toInt();
        if((DestVal[i] & NetmaskVal[i]) != DestVal[i])
        {
            QString str = QString("%1, %2, >> %3").arg(DestVal[i]).arg(NetmaskVal[i]).arg(DestVal[i] & NetmaskVal[i]);
            qDebug() <<  str;
            CInfomationDialog dlg(tr("IP주소와 넷마스크 조합이 올바르지 않습니다."));
            dlg.exec();
            return;
        }
    }

    Gateway_TAB * gateway = new Gateway_TAB;
    gateway->Interface = Interface;
    gateway->Dest = Dest;
    gateway->Netmask = Netmask;
    gateway->GatewayIp = GatewayIp;

    if((FindGateway(gateway)) != -1)
    {
        CInfomationDialog dlg(tr("중복된 자료입니다."));
        dlg.exec();
        return;
        delete gateway;
    }

    GatewayList->append(gateway);

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(row+1);
    SetItem(gateway->Interface, row, 0);
    SetItem(gateway->Dest, row, 1);
    SetItem(gateway->Netmask, row, 2);
    SetItem(gateway->GatewayIp, row, 3);
    mbChanged = true;
}

void Route::on_remove_clicked()
{
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        ui->tableWidget->removeRow(row);
        Gateway_TAB * gateway = GatewayList->at(row);        delete gateway;
        GatewayList->removeAt(row);
        mbChanged = true;
    }
}
void Route::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    int row = item->row();
    QTableWidgetItem *item2 = ui->tableWidget->item(row,0);
    int index = ui->Interface->findText(item2->text());
    ui->Interface->setCurrentIndex(index);
    item2 = ui->tableWidget->item(row,1);
    ui->Dest->setText(item2->text());
    item2 = ui->tableWidget->item(row,2);
    ui->Netmask->setText(item2->text());
    item2 = ui->tableWidget->item(row,3);
    ui->GatewayIp->setText(item2->text());
}

int Route::FindGateway(Gateway_TAB * route)
{
    int index = 0;
    foreach (const Gateway_TAB * gateway, *GatewayList) {\
       if(gateway->Interface == route->Interface && gateway->Dest == route->Dest &&
                gateway->Netmask == route->Netmask && gateway->GatewayIp == route->GatewayIp)
            return index;
        index++;
    }
    return -1;
}

void Route::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
