#ifndef SEWERDIALOG2_H
#define SEWERDIALOG2_H

#include <QDialog>
#include <QJsonValue>
#include <QDate>
#include <QTableWidget>
#include "mondialog.h"

namespace Ui {
class CSewerDialog2;
}
struct DUMP_TAB {
    QString Item[8];
};
#define SEWER_MAX_ROW     14

class CSewerDialog2 : public CMonDialog
{
    Q_OBJECT

public:
    explicit CSewerDialog2(QWidget *parent = 0);
    ~CSewerDialog2();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_btnFromDate_clicked();
    void on_btnInq_clicked();
    void on_btnFirst_clicked();
    void on_btnPre_clicked();
    void on_btnNext_clicked();
    void on_btnLast_clicked();

private:
    Ui::CSewerDialog2   *ui;
    int                 mTotalPoint;
    int                 mPage;
    int                 mStartPoint;
    int                 mTotalPage;
    QDate               mFromDate;
    QList<DUMP_TAB*> mDumpList;
    void PageInit(int total);
    void DataDisplay();
    void PageDisplay();
    void SendCommand(QString cmd);
    int CodeIndex(QString code);
    int EpsCodeIndex(QString code);
};

#endif // SEWERDIALOG2_H
