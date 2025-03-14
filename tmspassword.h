#ifndef TMSPASSWORD_H
#define TMSPASSWORD_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class TmsPassword;
}

class TmsPassword : public QDialog
{
    Q_OBJECT

public:
    explicit TmsPassword(QWidget *parent = 0);
    ~TmsPassword();
    QString mInputText;

protected:
    void	paintEvent(QPaintEvent*);

private slots:
    void digitClicked();
    void on_pushButton_back_clicked();

private:
    void timerEvent(QTimerEvent *);
    Ui::TmsPassword *ui;
    QPushButton *mButton[10];
    QLabel *mLabel[10];
    int     mInputPos;
};

#endif // TMSPASSWORD_H
