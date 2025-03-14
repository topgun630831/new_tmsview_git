#ifndef NUMEDIT_H
#define NUMEDIT_H

#include <QLineEdit>

class Calculator;

class NumEdit : public QLineEdit
{
    Q_OBJECT

public:
    NumEdit( QWidget *parent=0);
    ~NumEdit();

// event
	void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    QPoint	getPtKeyboard(Calculator* calc);
};

class NumEdit2 : public QLineEdit
{
    Q_OBJECT

public:
    NumEdit2( QWidget *parent=0);
    ~NumEdit2();

// event
    void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    QPoint	getPtKeyboard(Calculator* calc);
};

#endif // NUMEDIT_H
