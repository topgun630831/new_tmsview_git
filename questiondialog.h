#ifndef QUESTIONDIALOG_H
#define QUESTIONDIALOG_H

#include <QDialog>

namespace Ui {
class CQuestionDialog;
}

class CQuestionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CQuestionDialog(QString msg, bool bCheckBox = false, QWidget *parent = 0);
    ~CQuestionDialog();
    bool m_bCheck;

protected:
    void changeEvent(QEvent *e);
    void	paintEvent(QPaintEvent*);

private slots:
    void on_btnOk_clicked();

private:
    Ui::CQuestionDialog *ui;
    bool	eventFilter(QObject*, QEvent *e);
};

#endif // QUESTIONDIALOG_H
