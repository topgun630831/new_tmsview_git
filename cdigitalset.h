#ifndef CDIGITALSET_H
#define CDIGITALSET_H

#include <QDialog>

namespace Ui {
class CDigitalSet;
}

class CDigitalSet : public QDialog
{
    Q_OBJECT

public:
    explicit CDigitalSet(QWidget *parent = 0);
    ~CDigitalSet();

private:
    Ui::CDigitalSet *ui;
};

#endif // CDIGITALSET_H
