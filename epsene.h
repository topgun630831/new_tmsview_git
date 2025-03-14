#ifndef EPSENE_H
#define EPSENE_H

#include <QDialog>
#include <QMainWindow>
#include <QTimer>
#include <QIcon>
#include <QSplashScreen>
#include <QFileSystemWatcher>
#include <QLabel>
#include "mondialog.h"

namespace Ui {
class EpsEnE;
}

class EpsEnE : public CMonDialog
{
    Q_OBJECT

public:
    explicit EpsEnE(QWidget *parent = 0);
    ~EpsEnE();
    void onRead(QString& cmd, QJsonObject& jobject);
    bool    writeTag(QString tagname, QString value, int delay);
    QByteArray makeCommand(QString cmd, QJsonValue jValue);
    void ShareDataSetSend();
    QByteArray  mValueRead;
    QByteArray  mInfoRead;
    QByteArray  mPasswordRead;
    QString     mValueReadCmd;
    QString     mInfoCmd;
    QString     mPasswordCmd;
    int         mCommCount;
    int         mPassCount;

signals:
    void	sigSysMenu();

private slots:
    void on_setupBtn_clicked();
    void on_dataBtn_clicked();
    void on_historyBtn_clicked();
    void on_commBtn_clicked();
    void on_sysinfoBtn_clicked();
    void onTimer();
    void onTimerClose();
    void handleFileChanged(const QString&);
    void handleFileChanged2(const QString&);

    void on_pointBtn_clicked();

private:
    QList<TAG_WRITE_DELAY*>   m_listDelay;
    QTimer              m_Timer;
    QTimer              m_TimerClose;
    QIcon               m_Red;
    QIcon               m_Black;
    Ui::EpsEnE          *ui;
    QSplashScreen       *mSplash;
    QFileSystemWatcher  m_watcher;
    QFileSystemWatcher  m_watcher2;
    bool                m_bUsbReady;
    bool                m_bMousePress;
    int                 m_nMousePressCount;
    void LoadDb();
    void    makeCommand();
    void valueDisplay(QString name, QString value);
    void unitDisplay(QString name, QString value);
    void iconDisplay();
    void TextColorChange(const char* name, QLabel* label);
    void addTag(const char* n);
    void RunStart(bool start);
    void	mouseReleaseEvent(QMouseEvent*);
    void	mousePressEvent(QMouseEvent*);
};

#endif // EPSENE_H
