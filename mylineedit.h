#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <qlineedit.h>

class MyKeyboard;

class MyLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    MyLineEdit( QWidget *parent=0);
	~MyLineEdit();

// event
	void	mouseReleaseEvent(QMouseEvent*);

signals:

public slots:
	void	slotMyInsert(const QString&);

private:
	QPoint	getPtKeyboard(MyKeyboard* keyboard);
};

#endif // MYLINEEDIT_H
