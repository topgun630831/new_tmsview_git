#ifndef DISELECT_H
#define DISELECT_H

#include <QDialog>

namespace Ui {
class DiSelect;
}

class DiSelect : public QDialog
{
    Q_OBJECT

public:
    int nDi;
    explicit DiSelect(int di, int total = 16, QWidget *parent = 0);
    ~DiSelect();

private slots:
    void on_ok_clicked();

private:
    Ui::DiSelect *ui;
    void	paintEvent(QPaintEvent*);
};

#endif // DISELECT_H
