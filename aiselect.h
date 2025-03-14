#ifndef AISELECT_H
#define AISELECT_H

#include <QDialog>

namespace Ui {
class AiSelect;
}

class AiSelect : public QDialog
{
    Q_OBJECT

public:
    int nAi;
    explicit AiSelect(int ai, QWidget *parent = 0);
    ~AiSelect();

private slots:
    void on_ok_clicked();

private:
    Ui::AiSelect *ui;
    void	paintEvent(QPaintEvent*);
};

#endif // AISELECT_H
