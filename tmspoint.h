#ifndef TMSPOINT_H
#define TMSPOINT_H

#include <QDialog>
#include <QJsonValue>
#include "mondialog.h"
#include "pointmon.h"

namespace Ui {
class TmsPoint;
}

class TmsPoint : public CMonDialog
{
    Q_OBJECT

public:
    explicit TmsPoint(QString& group, QWidget *parent = 0);
    ~TmsPoint();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:
    void on_change_clicked();

    void on_comboBox_currentIndexChanged(const QString &group);

private:
    Ui::TmsPoint *ui;
    bool                mbStarted;
    int                 mTimerId;
    QList<POINT_TAB*>   mPointList;
    QString             mGroup;
    bool                mPointInitOk;
    void timerEvent(QTimerEvent *);
    void InitDisp();
    void ValueDisp();
    int getTagPos(QString tag);
    void GroupChanged();
};

#endif // TMSPOINT_H
