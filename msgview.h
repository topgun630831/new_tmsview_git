#ifndef MSGVIEW_H
#define MSGVIEW_H

#include <QDialog>

namespace Ui {
class MsgView;
}

class MsgView : public QDialog
{
    Q_OBJECT

public:
    explicit MsgView(QStringList list, QWidget *parent = 0);
    ~MsgView();

private slots:

private:
    Ui::MsgView *ui;
    void timerEvent(QTimerEvent *);
};

#endif // MSGVIEW_H
