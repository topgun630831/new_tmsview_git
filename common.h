#ifndef COMMON_H
#define COMMON_H

#include <QDialog>
#include "mondialog.h"

namespace Ui {
class CComMon;
}

struct COMM_SELECT {
    QString Driver;
    QString Device;
    QString Port;
};

class CComMon : public CMonDialog
{
    Q_OBJECT

public:
    explicit CComMon(QWidget *parent = 0);
    ~CComMon();
    QStringList         mPortList;
    QList<COMM_SELECT*> mCommList;
    void onRead(QString &cmd, QJsonObject&);

signals:
    void sigSelect();
private slots:
    void on_btnSelect_clicked();
    void on_btnClear_clicked();
    void on_btnMonStart_clicked();
    void on_btnReset_clicked();
    void on_checkHex_clicked();
    void on_checkSpace_clicked();

    void on_btnManualSampling_clicked();

private:
    Ui::CComMon *ui;
    bool m_bHexDisp;
    QString mDriver;
    QString mDevice;
    QString mPort;
    bool mbPort;
    bool mbStarted;
    int m_nHexStart;
    void SendCommand(const char *command);
    void InsertRow(QString s1, QString s2, QString s3, QString s4);
    QString KindStr(QString kind);
    void	paintEvent(QPaintEvent*);
};

#endif // COMMON_H
