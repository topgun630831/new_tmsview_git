#ifndef SYSTEMMANAGEMENT_H
#define SYSTEMMANAGEMENT_H

#include <QDialog>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QDir>
#include "mondialog.h"

namespace Ui {
class SystemManagement;
}

class SystemManagement : public CMonDialog
{
    Q_OBJECT

public:
    explicit SystemManagement(bool bAll=false, QWidget *parent = 0);
    ~SystemManagement();
    void     onRead(QString &cmd, QJsonObject&);
    static void    RunStart(bool start);

private slots:
    void on_btnSystemStop_clicked();
    void on_btnSystemRun_clicked();
    void on_btnReboot_clicked();
    void on_btnUpgrade_clicked();
    void on_btnSetupSave_clicked();
    void on_btnSetupRestore_clicked();
    void handleFileChanged(const QString&);
    void on_btnFactoryReset_clicked();
    void on_btnInitRootfs_clicked();
    void on_btnInitData_clicked();
    void on_btnDataSave_clicked();
    void on_btnDataRestore_clicked();
    void on_btnInitTmsData_clicked();
    void on_DeleteInterrupt_clicked();
    void on_btnRemoteRun_clicked();
    void on_btnRemoteStop_clicked();
    void loadReverseProxyConfig();
    void saveReverseProxyConfig();
    void onTimer();

private:
    Ui::SystemManagement *ui;
    void    buttonEnable();
    void    dbSync();
    QFileSystemWatcher m_watcher;
    void	paintEvent(QPaintEvent*);
    void    sendCOmmand(QString cmd);
    void scanDir(QDir dir);
    void fileCopyS(QString &src, QString &dest, QString &filename);
    QString mDest;
    QString mSrc;
    QString mSelectedFile;
    bool m_bCheck;
    int m_Timer;
    void timerEvent(QTimerEvent *);
    QString m_sReverseProxyIP;
    QString m_sReverseProxyMode;
    int m_nReverseProxyPort;
    bool m_bLoadOkProxyIP;
    QTimer  m_TimerStatus;
};

#endif // SYSTEMMANAGEMENT_H
