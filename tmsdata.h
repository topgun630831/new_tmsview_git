#ifndef TMSDATA_H
#define TMSDATA_H

#include <QDialog>
#include <QFileSystemWatcher>
#include <QTableWidget>
#include <QJsonValue>
#include <QDate>
#include <QLabel>
#include "mondialog.h"

namespace Ui {
class TmsData;
}

struct TMS_DUMP_TAB {
    QString Item[17];
};

#define TMS_MAX_ROW     18

class TmsData : public CMonDialog
{
    Q_OBJECT

public:
    explicit TmsData(QWidget *parent = 0);
    ~TmsData();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void handleFileChanged(const QString&);
    void on_fivemin_clicked();
    void on_btnUsb_clicked();
    void on_btnDate_allHour_clicked();
    void on_btnFirst_allHour_clicked();
    void on_btnPre_allHour_clicked();
    void on_btnNext_allHour_clicked();
    void on_btnLast_allHour_clicked();
    void on_btnDate_all5Min_clicked();
    void on_btnFirst_all5Min_clicked();
    void on_btnPre_all5Min_clicked();
    void on_btnNext_all5Min_clicked();
    void on_btnLast_all5Min_clicked();
    void on_allData_clicked();
    void on_selectedData_clicked();
    void on_sampler_clicked();
    void on_btnInq10Sec_clicked();
    void on_btnDate_indi5Min_clicked();
    void on_btnFirst_indi5Min_clicked();
    void on_btnPre_indi5Min_clicked();
    void on_btnNext_indi5Min_clicked();
    void on_btnLast_indi5Min_clicked();
    void on_btnInqSampler_clicked();
    void on_btnDate_indi10Sec_clicked();
    void on_btnFirst_indi10Sec_clicked();
    void on_btnPre_indi10Sec_clicked();
    void on_btnNext_indi10Sec_clicked();
    void on_btnLast_indi10Sec_clicked();
    void on_selectedData10Sec_clicked();
    void on_allDataHour_clicked();
    void on_editHour_all5Min_textChanged(const QString &arg1);
    void on_editHour_indi5Min_textChanged(const QString &arg1);
    void on_editHour_indi10Sec_textChanged(const QString &arg1);
    void on_comboBox_indi5Min_currentIndexChanged(const QString &arg1);
    void on_comboBox_indi10Sec_currentIndexChanged(const QString &arg1);
    void on_manage_clicked();
    void on_table5Min_clicked(const QModelIndex &);
    void on_table10Sec_clicked(const QModelIndex &);
    void on_btnResend_clicked();
    void on_btnDaySet_clicked();
    void on_btnResendFromDate_clicked();
    void on_btnResendToDate_clicked();

    void on_table5Min_pressed(const QModelIndex &index);

    void on_table10Sec_pressed(const QModelIndex &index);


private:
    void Inq10Sec();
    Ui::TmsData *ui;
    QFileSystemWatcher m_watcher;
    int                 mPageIndex;
    int                 mTotalPoint[4];
    int                 mPage[4];
    int                 mStartPoint[4];
    int                 mTotalPage[4];
    QDate               mDate[4];
    int                 mFromHour[4];
    QList<TMS_DUMP_TAB*> mDumpList[4];
    QLabel              *mNoData[4];
    QTableWidget        *mTableWidget[5];
    bool                mbInitOk;
    bool                mbFirst[5];
    int                 nFile;
    QDate               mResendFromDate;
    QDate               mResendToDate;
    int                 mResendFromHour;
    int                 mResendToHour;
    int                 m_nIndex;
    int                 m_nFlowNum;
    void    buttonEnable();
    void PageInit(int total);
    void DataDisplay();
    void PageDisplay();
    void SendCommand(QString cmd);
    int CodeIndex(QString code);
    void DateClicked(QPushButton *btn, QTableWidget *pTable);//
    void InqClicked(QTableWidget *pTable);
    void FirstClicked(QTableWidget *pTable, QLineEdit *pEdit, QPushButton *btnDate);
    void PreClicked();
    void NextClicked();
    void LastClicked(QTableWidget *pTable, QLineEdit *pEdit, QPushButton *btnDate);
    int FindData(QString time, QString kind);
};

#endif // TMSDATA_H
