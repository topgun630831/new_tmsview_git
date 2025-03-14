#ifndef IPEDIT_H
#define IPEDIT_H

#include <QLineEdit>

class Calculator;

class IpEdit : public QLineEdit
{
    Q_OBJECT

public:
    IpEdit( QWidget *parent=0);
    ~IpEdit();

// event
	void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    QPoint	getPtKeyboard(Calculator* calc);
};

class IpEdit2 : public QLineEdit
{
    Q_OBJECT

public:
    IpEdit2( QWidget *parent=0);
    ~IpEdit2();

// event
    void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:

private:
    QPoint	getPtKeyboard(Calculator* calc);
};

#endif // IPEDIT_H
