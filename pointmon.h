#ifndef POINTMON_H
#define POINTMON_H

#include <QDialog>
#include <QJsonValue>
#include "mondialog.h"

namespace Ui {
class CPointMon;
}
class QTableWidgetItem;
struct POINT_TAB {
    QString Name;
    QString Desc;
    QString Status;
    QString Value;
    QString Unit;
    QString Rw;
    QString TagType;
    QString AlarmSt;
    QString TagSt;
    QString OnMsg;
    QString OffMsg;
    QString RawValue;
};

struct PROPERTY_TAB {
    QString Name;
    QString Desc;
    QString Rw;
    QString Driver;
    QString Device;
    QString Address;
    double  InitValue;
    bool    UseForcedValue;
    double  ForcedValue;
    QString LinkTags;
    QString TagType;
    QString DataType;
    double  RawMin;
    double  RawMax;
    double  EuMin;
    double  EuMax;
    double  Ratio;
    int     RoundUp;
    double  Deadband;
    double  Format;
    int    UseLowLowAlarm;
    double  LowLowAlarmValue;
    int    UseLowAlarm;
    double  LowAlarmValue;
    int    UseHighAlarm;
    double  HighAlarmValue;
    int    UseHighHighAlarm;
    double  HighHighAlarmValue;
    QString OnMsg;
    QString OffMsg;
    bool    Reversed;
    QString    DigitalEvent;
    QString    DigitalAlarm;
    QString ClearCondition;
    double  MaxValueChange;
};

class CPointMon : public CMonDialog
{
    Q_OBJECT

public:
    explicit CPointMon(QWidget *parent = 0);
    ~CPointMon();
    void onRead(QString& cmd, QJsonObject& jobject);

signals:
public slots:

private slots:
    void on_btnGroup_clicked();
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();
    void on_control_clicked();
    void on_range_clicked();
    void on_tablePoint_itemClicked(QTableWidgetItem *item);
    
private:
    Ui::CPointMon       *ui;
    QString             mGroup;
    bool                mbDevice;
    int                 mTotalPoint;
    int                 mPage;
    int                 mStartPoint;
    int                 mTotalPage;
    bool                mbStarted;
    int                 mTimerId;
    int                 mSelectedIndex;
    QList<POINT_TAB*>   mPointList;
    QJsonValue          mArgValue;
    bool                mbAlarm;
    void timerEvent(QTimerEvent *);
    void InitDisp();
    void ValueDisp();
    void PageDisp();
    void on_tablePoint_control(int row);
    int getTagPos(QString tag);
    PROPERTY_TAB mProperty;
    void	paintEvent(QPaintEvent*);
};

#endif // POINTMON_H
