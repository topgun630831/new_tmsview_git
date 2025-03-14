#ifndef DEVICESELECT_H
#define DEVICESELECT_H

#include <QDialog>

namespace Ui {
class DeviceSelect;
}

class DeviceSelect : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceSelect(QWidget *parent = 0);
    ~DeviceSelect();
    QString sDriver;
    QString sDesc;

private slots:
    void on_ok_clicked();

private:
    Ui::DeviceSelect *ui;
    void	paintEvent(QPaintEvent*);
};

#endif // DEVICESELECT_H
