#ifndef FILESELECT_H
#define FILESELECT_H

#include <QDialog>
#include <QDir>

namespace Ui {
class FileSelect;
}

class FileSelect : public QDialog
{
    Q_OBJECT

public:
    explicit FileSelect(QString &dir, bool backup=true, bool bFile=true, QWidget *parent = 0);
    ~FileSelect();
    QString mSelectedFile;


private slots:
    void on_ok_clicked();
private:
    Ui::FileSelect *ui;
    QDir currentDir;
    void	paintEvent(QPaintEvent*);
    void FindFile(QString &dir);
    void FindDirectory(QString &dir, QString &filter);
};

#endif // FILESELECT_H
