#ifndef INFOMATIONDIALOG_H
#define INFOMATIONDIALOG_H

#include <QDialog>

namespace Ui {
class CInfomationDialog;
}

class CInfomationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CInfomationDialog(QString msg, bool wait=true, QWidget *parent = 0);
    ~CInfomationDialog();

protected:
    void changeEvent(QEvent *e);
    void	paintEvent(QPaintEvent*);

private slots:

private:
    Ui::CInfomationDialog *ui;
    bool	eventFilter(QObject*, QEvent *e);
};

#endif // INFOMATIONDIALOG_H
