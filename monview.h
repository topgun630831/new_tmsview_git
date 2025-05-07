#ifndef MONVIEW_H
#define MONVIEW_H

#include <QDialog>
#include "mondialog.h"

namespace Ui {
class MonView;
}

class MonView : public CMonDialog
{
    Q_OBJECT

public:
    explicit MonView(QWidget *parent = 0);
    ~MonView();
    void onRead(QString& cmd, QJsonObject& jobject);

private slots:

    void on_update_clicked();

    void on_pause_clicked(bool checked);

private:
    Ui::MonView *ui;
    int         StartMsgIndex;
    bool        m_Pause;
    void timerEvent(QTimerEvent *);
};

#endif // MONVIEW_H
