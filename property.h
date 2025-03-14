#ifndef PROPERTY_H
#define PROPERTY_H

#include <QDialog>
#include <QJsonValue>
#include "pointmon.h"

namespace Ui {
class CProperty;
}

class CProperty : public QDialog
{
    Q_OBJECT

public:
    explicit CProperty(PROPERTY_TAB *p, QWidget *parent = 0);
    ~CProperty();

private slots:
    void on_save_clicked();
    void on_ok_clicked();

private:
    Ui::CProperty *ui;
    bool changeCompare(QJsonArray& jArray);
    void changeproperty(QJsonArray jArray);
    void	paintEvent(QPaintEvent*);
    PROPERTY_TAB *mProperty;
    bool    UseLowLowAlarm;
    double  LowLowAlarmValue;
    bool    UseLowAlarm;
    double  LowAlarmValue;
    bool    UseHighAlarm;
    double  HighAlarmValue;
    bool    UseHighHighAlarm;
    double  HighHighAlarmValue;
    int    Reversed;
    QString    DigitalEvent;
    QString    DigitalAlarm;
};

#endif // PROPERTY_H
