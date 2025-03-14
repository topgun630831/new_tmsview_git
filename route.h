#ifndef ROUTE_H
#define ROUTE_H

#include <QDialog>
#include <QTableWidgetItem>
#include "networksetup.h"
#include <QTableWidgetItem>

namespace Ui {
class Route;
}

class Route : public QDialog
{
    Q_OBJECT

public:
    explicit Route(QList<Gateway_TAB *> *gateway, QWidget *parent = 0);
    ~Route();
    bool mbChanged;

private slots:
    void on_add_clicked();
    void on_remove_clicked();

    void on_tableWidget_itemClicked(QTableWidgetItem *item);

private:
    Ui::Route *ui;
    QList<Gateway_TAB *> *GatewayList;
    void SetItem(QString str, int row, int col);
    int FindGateway(Gateway_TAB * route);
    void	paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *);
};

#endif // ROUTE_H
