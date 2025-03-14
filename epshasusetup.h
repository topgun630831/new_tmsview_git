#ifndef EPSHASUSETUP_H
#define EPSHASUSETUP_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class EpsHasuSetup;
}

class EpsHasuSetup : public QDialog
{
    Q_OBJECT

public:
    explicit EpsHasuSetup(QWidget *parent = 0);
    ~EpsHasuSetup();

private slots:
    void on_close_clicked();
    void on_serverSave_clicked();

    void on_btnAck_clicked();

private:
    Ui::EpsHasuSetup *ui;
    void RunStart(bool start);
    void timerEvent(QTimerEvent *);
};

#endif // EPSHASUSETUP_H
