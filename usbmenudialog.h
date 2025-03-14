#ifndef USBMENUDIALOG_H
#define USBMENUDIALOG_H

#include <QDialog>

namespace Ui {
class UsbMenuDialog;
}

class UsbMenuDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UsbMenuDialog(QWidget *parent = 0);
    ~UsbMenuDialog();
    bool bUnstoring;
    bool bSetup;

private slots:
    void on_Unstoring_clicked();

    void on_Setup_clicked();

private:
    Ui::UsbMenuDialog *ui;
    void	paintEvent(QPaintEvent*);
};

#endif // USBMENUDIALOG_H
