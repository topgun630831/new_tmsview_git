#ifndef LCDTESTDIALOG_H
#define LCDTESTDIALOG_H

#include <QDialog>

namespace Ui {
class CLcdTestDialog;
}

class CLcdTestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLcdTestDialog(QWidget *parent = 0);
    ~CLcdTestDialog();

private:
    Ui::CLcdTestDialog *ui;
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *);
};

#endif // LCDTESTDIALOG_H
