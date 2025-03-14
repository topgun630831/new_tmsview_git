#ifndef TESTSETUP_H
#define TESTSETUP_H

#include "mondialog.h"
#include <QJsonArray>
#include <QDialog>

namespace Ui {
class TestSetup;
}

class TestSetup : public CMonDialog
{
    Q_OBJECT

public:
    explicit TestSetup(QWidget *parent = 0);
    ~TestSetup();
    void onRead(QString &cmd, QJsonObject&);
    static bool IpValidate(QString &ip, QString msg);

private slots:
    void on_save_2_clicked();
    void on_lan1DefaultGateway_clicked();
    void on_lan2DefaultGateway_clicked();

    void on_server_clicked();

    void on_network_clicked();

    void on_serverSave_clicked();

private:
    Ui::TestSetup *ui;
    QJsonArray mEthernets;
    bool        lan2Used;
    void RunStart(bool start);
};

#endif // TESTSETUP_H
