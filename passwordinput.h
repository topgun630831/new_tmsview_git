#ifndef PASSWORDINPUT_H
#define PASSWORDINPUT_H

#include <QDialog>

namespace Ui {
class PasswordInput;
}

class PasswordInput : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordInput(QWidget *parent = 0);
    ~PasswordInput();
    QString m_sPassword;

private slots:
    void on_ok_clicked();

private:
    Ui::PasswordInput *ui;
    void	paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *);
};

#endif // PASSWORDINPUT_H
