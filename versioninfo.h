#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include "mondialog.h"
#include <QJsonValue>
#include <QJsonArray>

namespace Ui {
class CVersionInfo;
}

class CVersionInfo : public CMonDialog
{
    Q_OBJECT

public:
    explicit CVersionInfo(QWidget *parent = 0);
    ~CVersionInfo();
    void onRead(QString &cmd, QJsonObject&);

private:
    Ui::CVersionInfo *ui;
    void	paintEvent(QPaintEvent*);
    void timerEvent(QTimerEvent *);
    int m_Timer;
};

#endif // VERSIONINFO_H
