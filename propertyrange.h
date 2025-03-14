#ifndef PROPERTYRANGE_H
#define PROPERTYRANGE_H

#include <QDialog>
#include <QJsonValue>
#include "mondialog.h"
#include "pointmon.h"

namespace Ui {
class PropertyRange;
}

class PropertyRange : public CMonDialog
{
    Q_OBJECT

public:
    explicit PropertyRange(POINT_TAB *pointTab, QWidget *parent = 0);
    ~PropertyRange();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_save_clicked();

    void on_Ratio_textChanged(const QString &arg1);

    void on_UseRatio_clicked();

private:
    Ui::PropertyRange *ui;
    bool changeCompare(QJsonObject& jObject);
    void changeproperty(QJsonObject jObject);
    void paintEvent(QPaintEvent*);
    void RatioEnableDisbale();
    void timerEvent(QTimerEvent *);
    PROPERTY_TAB mProperty;
    double RawMin;
    double RawMax;
    double EuMin;
    double EuMax;
    double Ratio;
    int    RoundUp;
    double Deadband;
    bool   Reversed;
    int    mTimerId;
    POINT_TAB *mPointTab;
};

#endif // PROPERTYRANGE_H
