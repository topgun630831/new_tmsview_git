#ifndef COMMONSELECT_H
#define COMMONSELECT_H

#include <QDialog>
#include "common.h"

namespace Ui {
class CComMonSelect;
}

class CComMonSelect : public QDialog
{
    Q_OBJECT

public:
    explicit CComMonSelect(QStringList portList, QList<COMM_SELECT*> commList, QWidget *parent = 0);
    ~CComMonSelect();
    bool    mbPort;
    QString mDriver;
    QString mDevice;
    QString mPort;

private slots:
    void on_btnPortSelect_clicked();

    void on_ok_clicked();

    void on_tableWidgetComm_doubleClicked(const QModelIndex &index);

    void on_tableWidgetPort_doubleClicked(const QModelIndex &index);

    void on_tableWidgetComm_clicked(const QModelIndex &index);

    void on_tableWidgetPort_clicked(const QModelIndex &index);

private:
    Ui::CComMonSelect *ui;
    bool    bSelected;
    int     mSelectedIndex;
    void	paintEvent(QPaintEvent*);
};

#endif // COMMONSELECT_H
