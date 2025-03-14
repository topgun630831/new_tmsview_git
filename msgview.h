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

private:
    Ui::MsgView *ui;
};

#endif // MSGVIEW_H
