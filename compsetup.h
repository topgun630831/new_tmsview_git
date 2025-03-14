#ifndef COMPSETUP_H
#define COMPSETUP_H

#include <QDialog>
#include <QTimer>
#include <QLineEdit>
#include <QFileSystemWatcher>

namespace Ui {
class CompSetup;
}

class CompSetup : public QDialog
{
    Q_OBJECT

public:
    explicit CompSetup(QWidget *parent = 0);
    ~CompSetup();

private slots:
    void onTimer();
    void on_btnAuto_clicked();
    void on_btnRun_clicked();
    void on_btnTime_clicked();
    void on_btnIntv_clicked();
    void on_btnFan_clicked();
    void on_btnMalf_clicked();
    void on_btnRepair_clicked();
    void on_btnAck_clicked();

private:
    Ui::CompSetup *ui;
    QTimer  m_Timer;
    QFileSystemWatcher m_watcher;
    void	paintEvent(QPaintEvent*);
};

#endif // COMPSETUP_H
