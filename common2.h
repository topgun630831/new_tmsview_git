#ifndef COMMON2_H
#define COMMON2_H

#include <QDialog>
#include "common.h"
#include "mondialog.h"

namespace Ui {
class CComMon2;
}

class CComMon2 : public CMonDialog
{
    Q_OBJECT

public:
    explicit CComMon2(QWidget *parent = 0);
    ~CComMon2();
    QStringList         mPortList;
    QList<COMM_SELECT*> mCommList;
    void onRead(QString &cmd, QJsonObject&);

private slots:
    void on_btnSelect_clicked();
    void on_btnHexDisp_clicked();
    void on_btnClear_clicked();
    void on_btnMonStart_clicked();
    void on_btnReset_clicked();

private:
    Ui::CComMon2 *ui;
    bool m_bHexDisp;
    QString mDriver;
    QString mDevice;
    QString mPort;
    bool mbPort;
    void SendCommand(const char *command);
    void InsertRow(QString s1, QString s2, QString s3, QString s4);
    QString KindStr(QString kind);
};

#endif // COMMON_H
