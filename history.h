#ifndef HISTORY_H
#define HISTORY_H

#include <QDialog>
#include <QJsonValue>
#include <QDate>
#include <QTableWidget>
#include "mondialog.h"

namespace Ui {
class CHistory;
}
#define MAX_ROW     18
#define HISTORY_MAX 7

enum PAGE_DEF
{
    ALARM,
    CONTROL,
    EVENT,
    CONFIG,
    SYSTEM,
    EPS,
    SYSTEM_TMS
};

struct HISTORY_TAB {
    QString Item[8];
};

class CHistory : public CMonDialog
{
    Q_OBJECT

public:
    explicit CHistory(QWidget *parent = 0);
    ~CHistory();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();
    void on_btnAlarm_clicked();
    void on_btnControl_clicked();
    void on_btnEvent_clicked();
    void on_btnConfig_clicked();
    void on_btnSystem_clicked();
    void on_btnInq_clicked();
    void on_btnFromDate_clicked();
    void on_btnToDate_clicked();

private:
    Ui::CHistory *ui;
    int                 mIndex;
    int                 mTotalPoint[HISTORY_MAX];
    int                 mPage[HISTORY_MAX];
    int                 mStartPoint[HISTORY_MAX];
    int                 mTotalPage[HISTORY_MAX];
    QDate               mFromDate;
    QDate               mToDate;
    QList<HISTORY_TAB*> mHistoryList[HISTORY_MAX];
    void PageInit(int index, int total);
    void DataDisplay(int index);
    void PageDisplay(int index);
    void ColumnInit();
    void SendCommand(QString cmd);
    void	paintEvent(QPaintEvent*);
};

#endif // HISTORY_H
