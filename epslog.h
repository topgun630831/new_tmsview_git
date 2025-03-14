#ifndef EPSLOG_H
#define EPSLOG_H

#include <QDialog>
#include <QJsonValue>
#include <QDate>
#include <QTableWidget>
#include "mondialog.h"

namespace Ui {
class CEpsLog;
}
struct EPS_DUMP_TAB {
    QString Item[10];
};
#define EPS_MAX_ROW     13

class CEpsLog : public CMonDialog
{
    Q_OBJECT

public:
    explicit CEpsLog(QWidget *parent = 0);
    ~CEpsLog();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_btnFromDate_clicked();
    void on_btnInq_clicked();
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();

private:
    Ui::CEpsLog   *ui;
    int                 mTotalPoint;
    int                 mPage;
    int                 mStartPoint;
    int                 mTotalPage;
    QDate               mFromDate;
    QList<EPS_DUMP_TAB*> mDumpList;
    void PageInit(int total);
    void DataDisplay();
    void PageDisplay();
    void SendCommand(QString cmd);
    int CodeIndex(QString code);
};

#endif // SEWERDIALOG2_H
