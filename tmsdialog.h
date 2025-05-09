#ifndef TMSDIALOG_H
#define TMSDIALOG_H

#include <QDialog>
#include <QMainWindow>
#include <QTimer>
#include <QLCDNumber>
#include <QLabel>
#include <QMap>
#include <QFileSystemWatcher>
#include "mondialog.h"
#include "tmssetup.h"

namespace Ui {
class TmsDialog;
}
class CTag;
class TmsDialog : public CMonDialog
{
    Q_OBJECT

public:
    explicit TmsDialog(QWidget *parent = 0);
    ~TmsDialog();
    void onRead(QString& cmd, QJsonObject& jobject);
    bool    writeTag(QString tagname, QString value, int delay);
    QByteArray makeCommand(QString cmd, QJsonValue jValue);
    QByteArray  mValueRead;
    QByteArray  mInfoRead;
    QByteArray  mPasswordRead;
    QString     mValueReadCmd;
    QString     mInfoCmd;
    QString     mPasswordCmd;
    int         mCommCount;
    int         mPassCount;
    QString     mPassword;
    void        ManualSampling();

signals:
    void	sigSysMenu();

private slots:
    void handleFileChanged(const QString&);
    void handleFileChanged2(const QString&);
//    void on_sysinfoBtn_clicked();
    void on_historyBtn_clicked();
    void on_commBtn_clicked();
    void on_setupBtn_clicked();
    void on_dataBtn_clicked();
    void onTimer();
    void onTimerClose();
    void on_MsgList_clicked(const QModelIndex &i);
    void on_btnTn_clicked();
    void on_btnTp_clicked();
    void on_btnCod_clicked();
    void on_btnPh_clicked();
    void on_btnSs_clicked();
    void on_btnSampler_clicked();
    void on_btnFlow_clicked();
    void slotSysMenu();
    bool PasswordCheck();
    void on_btnManualSampling_clicked();
    void on_btnToc_clicked();

    void on_btn7_clicked();

private:
    Ui::TmsDialog *ui;
    QList<TAG_WRITE_DELAY*>   m_listDelay;
    QTimer              m_Timer;
    QTimer              m_TimerStarter;
    QTimer              m_TimerClose;
    QImage      m_imgBackGround;
//    QImage      m_Box;
    QPushButton *m_Button[7];
    QLCDNumber  *Value[7];
    QLabel      *Unit[7];
    QLabel      *Status[9][2];
    QLabel      *StatusLabel[9][2];
    QMap<QString, int>  TagIndexMap;
    QMap<QString, int>  StsTagIndexMap;
    QMap<QString, int>  EquipStsTagIndexMap;
    QMap<QString, int>  CheckTagIndexMap;
    QMap<QString, int>  CommTagIndexMap;
    QFileSystemWatcher  m_watcher;
    QFileSystemWatcher  m_watcher2;
    bool                m_bUsbReady;
    QStringList         MsgList;
    QString             OldMessage;
    CTag                *mpCheckTag[9];
    CTag                *mpCommTag[9];
    bool                mCheckFlag[9];
    bool                m_bMousePress;
    int                 m_nMousePressCount;
    QImage		m_SamplerDoorOn;
    QImage		m_SamplerDoorOff;
    QImage		m_SamplerRunOn;
    QImage		m_SamplerRunOff;
    QImage		m_SamplerRunFault;
    QImage		m_DOROn;
    QImage		m_DOROff;
    QImage		m_PWROn;
    QImage		m_PWROff;
    QImage		m_FMLOn;
    QImage		m_FMLOff;
    QImage		m_FMROn;
    QImage		m_FMROff;
    QImage		m_Check;
    QImage		m_CommOn;
    QImage		m_CommOff;
    bool        m_bInitOk;
    void    makeCommand();
    void addTag(const char* n);
    void paintEvent(QPaintEvent*);
    void TextColorChange(const char* style, QLabel* label);
    int TagIndex(QString name);
    int StsTagIndex(QString name);
    int EquipStsTagIndex(QString name);
    int CheckTagIndex(QString name);
    int CommTagIndex(QString name);
    void valueDisplay(QString name, CTag *pTag, QString strVal);
    void MsgAdd(QString msg);
    void CheckControl(CTag *pTag);
    void DbLoad();
    void RoundupLoad();
    void SetFlag(QString name);
    void TmsPointDlg(QString group);
    void	mouseReleaseEvent(QMouseEvent*);
    void	mousePressEvent(QMouseEvent*);
};

#endif // TMSDIALOG_H
