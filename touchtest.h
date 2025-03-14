#ifndef TOUCHTEST_H
#define TOUCHTEST_H

#include <QDialog>
#include <QImage>

namespace Ui {
class CTouchTest;
}

class CTouchTest : public QDialog
{
    Q_OBJECT

public:
    explicit CTouchTest(QWidget *parent = 0);
    ~CTouchTest();

private:
    Ui::CTouchTest *ui;
    QPoint mStartPoint;
    QPoint mEndPoint;
    QImage *mImage;
    bool    m_bLog;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void paint();
private slots:
    void on_clear_clicked();
    void on_run_clicked();
    void on_checkBox_clicked();
    void on_run_2_clicked();
    void on_listWidget_clicked(const QModelIndex &index);
};

#endif // TOUCHTEST_H
