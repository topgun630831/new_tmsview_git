#ifndef GROUPSELECT_H
#define GROUPSELECT_H

#include <QDialog>

namespace Ui {
class CGroupSelect;
}

class CGroupSelect : public QDialog
{
    Q_OBJECT

public:
    explicit CGroupSelect(QStringList list, QStringList listDevice, QWidget *parent = 0);
    ~CGroupSelect();
    QString mGroup;
    bool mbDevice;

private slots:
    void on_ok_clicked();

    void on_tableGroup_doubleClicked(const QModelIndex &index);

    void on_btnDeviceSelect_clicked();

    void on_tableDevice_doubleClicked(const QModelIndex &index);

    void on_tableDevice_clicked(const QModelIndex &index);

    void on_tableGroup_clicked(const QModelIndex &index);

private:
    Ui::CGroupSelect *ui;
    int mSelectedIndex;
    bool m_btnDeviceSelect;
};

#endif // GROUPSELECT_H
