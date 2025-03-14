#ifndef EPSORDER_H
#define EPSORDER_H

#include <QDialog>

namespace Ui {
class EpsOrder;
}

class EpsOrder : public QDialog
{
    Q_OBJECT

public:
    explicit EpsOrder(QWidget *parent = 0);
    ~EpsOrder();

private slots:
    void on_top_clicked();

    void on_up_clicked();

    void on_down_clicked();

    void on_bottom_clicked();

    void on_ok_clicked();

private:
    void	paintEvent(QPaintEvent*);
    void display();
    Ui::EpsOrder *ui;
    QStringList DescList;
};

#endif // EPSORDER_H
