#ifndef ALARMSUMMARY_H
#define ALARMSUMMARY_H

#include <QDialog>
#include <QJsonValue>
#include "mondialog.h"

namespace Ui {
class CAlarmSummary;
}

struct ALARM_SUM_TAB {
    QString Date;
    QString Name;
    QString Desc;
    QString Type;
    QString Status;
    QString PreStatus;
    QString Value;
    QString AckUser;
};

class CAlarmSummary : public CMonDialog
{
    Q_OBJECT

public:
    explicit CAlarmSummary(QWidget *parent = 0);
    ~CAlarmSummary();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_btnAck_clicked();
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();

    void on_btnAckAll_clicked();

private:
    Ui::CAlarmSummary *ui;
    int                 mTotalPoint;
    int                 mPage;
    int                 mStartPoint;
    int                 mTotalPage;
    QList<ALARM_SUM_TAB*>   mAlarmSummaryList;
    QJsonValue          mArgValue;
    void timerEvent(QTimerEvent *);
    void SumDisp();
    void PageDisp();
    void	paintEvent(QPaintEvent*);
};

#endif // ALARMSUMMARY_H
