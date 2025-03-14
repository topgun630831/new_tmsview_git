#ifndef DATETIMEINPUT_H
#define DATETIMEINPUT_H

#include <QDialog>
#include <QDate>

namespace Ui {
class DateTimeInput;
}

class DateTimeInput : public QDialog
{
    Q_OBJECT

public:
    explicit DateTimeInput(QDateTime datetime, QWidget *parent = 0);
    ~DateTimeInput();
    QDate mDate;
    int     mHour;
    int     mMin;

private slots:
    void on_btnDate_clicked();

    void on_ok_clicked();

private:
    Ui::DateTimeInput *ui;
    void	paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *);
};

#endif // DATETIMEINPUT_H
