#include <QPainter>
#include <QDesktopWidget>
#include <QPixmap>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "epsorder.h"
#include "ui_epsorder.h"
#include "questiondialog.h"
#include "infomationdialog.h"

extern QSqlDatabase gDb;
extern bool g_DebugDisplay;

EpsOrder::EpsOrder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EpsOrder)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);
    QString dbname =  "/data/project.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        qDebug() << "QSqlError : " << err.databaseText();
    }

    QSqlQuery query("SELECT Desc FROM Ext_Eps_Mapping Order By Num");
    while(query.next())
    {
        QString desc = query.value(0).toString();
        DescList.append(desc);
    }
    ui->tableWidget->setRowCount(DescList.length());
    display();
    gDb.close();
}

EpsOrder::~EpsOrder()
{
    delete ui;
}

void EpsOrder::paintEvent(QPaintEvent *)
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

void EpsOrder::on_top_clicked()
{
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if(row != 0)
        {
            QString curr = DescList[row];
            for(int i = row; i > 0; i--)
                DescList[i] = DescList[i-1];
            DescList[0] = curr;
        }
        display();
        ui->tableWidget->setCurrentCell(0,0);
    }
}

void EpsOrder::on_up_clicked()
{
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    int pos = -1;
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        pos = row;
        if(row != 0)
        {
            QString up = DescList[row-1];
            QString curr = DescList[row];
            DescList[row] = up;
            DescList[row-1] = curr;
            pos = row-1;
        }
        display();
    }
    if(pos >= 0)
        ui->tableWidget->setCurrentCell(pos,0);
}

void EpsOrder::on_down_clicked()
{
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    int pos = -1;
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if((row+1) < DescList.length())
        {
            QString down = DescList[row+1];
            QString curr = DescList[row];
            DescList[row] = down;
            DescList[row+1] = curr;
            pos = row+1;
        }
        display();
    }
    if(pos >= 0)
        ui->tableWidget->setCurrentCell(pos,0);
}

void EpsOrder::on_bottom_clicked()
{
    QList<QTableWidgetItem *> list = ui->tableWidget->selectedItems();
    int len = DescList.length();
    if(list.size() >= 1)
    {
        int row = list[0]->row();
        if((row+1) < len)
        {
            QString curr = DescList[row];
            for(int i = row; i < len-1; i++)
                DescList[i] = DescList[i+1];
            DescList[len-1] = curr;
        }
        display();
    }
    ui->tableWidget->setCurrentCell(len,0);
}

void EpsOrder::on_ok_clicked()
{
    CQuestionDialog dlg("변경된 내용을 적용하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        QString dbname =  "/data/project.db";
        gDb.setDatabaseName(dbname);
        if(gDb.open() == true)
        {
            for (int i = 0; i < DescList.length(); i++)
            {
                QString str = QString("UPDATE Ext_Eps_Mapping SET Num=%1 WHERE Desc='%2'").arg(i).arg(DescList[i]);
                QSqlQuery query(str);
                query.exec();
                if(g_DebugDisplay)
                    qDebug() << "sql:" << str;
            }
            gDb.close();
        }
    }
}

void EpsOrder::display()
{
    for (int i = 0; i < DescList.length(); i++)
    {
        QTableWidgetItem *item = new QTableWidgetItem(DescList.at(i));
        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget->setItem(i, 0, item);
    }
}
