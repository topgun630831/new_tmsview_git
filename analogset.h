#ifndef ANALOGSET_H
#define ANALOGSET_H

#include <QDialog>

namespace Ui {
class CAnalogSet;
}
class CTag;
class CAnalogSet : public QDialog
{
    Q_OBJECT

public:
    explicit CAnalogSet(QString msg, CTag *pTag, QWidget *parent = 0);
    ~CAnalogSet();
    double m_Value;
    QString m_sValue;

protected:
    void	paintEvent(QPaintEvent*);

private slots:
    void on_btnOk_clicked();

private:
    Ui::CAnalogSet *ui;
};

#endif // ANALOGSET2_H
