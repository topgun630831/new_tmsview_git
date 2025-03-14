#ifndef STRINGSET_H
#define STRINGSET_H

#include <QDialog>

namespace Ui {
class CStringSet;
}

class CTag;
class CStringSet : public QDialog
{
    Q_OBJECT

public:
    explicit CStringSet(QString msg, CTag *pTag, QWidget *parent = 0);
    ~CStringSet();
    QString m_sValue;

protected:
    void	paintEvent(QPaintEvent*);

private slots:
    void on_btnOk_clicked();

private:
    Ui::CStringSet *ui;
};

#endif // STRINGSET_H
