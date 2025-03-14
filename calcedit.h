#ifndef CALCEDIT_H
#define CALCEDIT_H

#include <QLineEdit>

class Calculator;

class CalcEdit : public QLineEdit
{
    Q_OBJECT

public:
    CalcEdit( QWidget *parent=0);
    ~CalcEdit();

// event
	void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    QPoint	getPtKeyboard(Calculator* calc);
};

#endif // CALCEDIT_H
