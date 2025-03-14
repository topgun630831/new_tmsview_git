#ifndef DIGITALSET_H
#define DIGITALSET_H

#include <QDialog>

namespace Ui {
class CDigitalSet;
}
class CTag;
class CDigitalSet : public QDialog
{
    Q_OBJECT

public:
    explicit CDigitalSet(QString msg, CTag *pTag, CTag *pTag2, bool bTwo, QWidget *parent = 0);
    ~CDigitalSet();
    bool    m_bOn;

protected:
    void	paintEvent(QPaintEvent*);

private slots:
    void on_btnOk_clicked();

private:
    Ui::CDigitalSet *ui;
};

#endif // DIGITALSET_H
