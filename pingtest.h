#ifndef PINGTEST_H
#define PINGTEST_H

#include <QDialog>

namespace Ui {
class PingTest;
}

class PingTest : public QDialog
{
    Q_OBJECT

public:
    explicit PingTest(QString ip, QWidget *parent = 0);
    ~PingTest();

private slots:
    void on_btnTest_clicked();

    void on_comboBox_currentTextChanged(const QString &arg1);

private:
    Ui::PingTest *ui;
    void paintEvent(QPaintEvent *);
};

#endif // PINGTEST_H
