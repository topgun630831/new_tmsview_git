#ifndef DOSELECT_H
#define DOSELECT_H

#include <QDialog>

namespace Ui {
class DoSelect;
}

class DoSelect : public QDialog
{
    Q_OBJECT

public:
    explicit DoSelect(int ndo, QWidget *parent = 0);
    ~DoSelect();
    int nDo;

private slots:
    void on_ok_clicked();

private:
    Ui::DoSelect *ui;
    void	paintEvent(QPaintEvent*);
};

#endif // DOSELECT_H
