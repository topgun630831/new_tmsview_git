#include <QtGui>
#include <QApplication>
#include <QString>
#include <QDebug>
#include "calculator.h"
#include "numedit.h"
#include <QDesktopWidget>

NumEdit::NumEdit( QWidget *parent )
 //   : QLineEdit( parent, name )
    : QLineEdit( parent)
{
}

NumEdit::~NumEdit()
{
}

void NumEdit::mouseReleaseEvent(QMouseEvent*)
{
    if(isReadOnly())
        return;
    QPoint	point;

    selectAll();

    QString str = text();
    Calculator calc(str, NUMKEY, this);
    // 적당한 키보드의 위치를 얻어낸다
 //   point = getPtKeyboard(&calc);
//    calc.move(point);
    if(calc.exec() == QDialog::Accepted)
    {
        setText(calc.m_Value);
    }

    deselect();
//    setup->update();
}
// x=250,y=260
QPoint NumEdit::getPtKeyboard(Calculator* calc)
{
//	QWidget	*d = QApplication::desktop();
    QDesktopWidget	*d = QApplication::desktop();
    int		w = d->width();		// 화면 전체크기 width
    int		h = d->height();	// 화면 전체크기 height
	QPoint	point;
	QPoint	ptTmp;

	// keyboard가 edit 위치 밑에 뜨도록 한다
	point = QPoint(mapToGlobal(rect().bottomLeft()));
    point.setY(point.y());

    int width = calc->width();
    int height = calc->height();

    width = 491;
    height = 437;

    qDebug() << "w=" << w << " h=" << h << " width=" << width << " height=" << height;
 	// 화면영역을 넘어가면 위치를 보정한다
	if (point.x() < 0)
	{
		point.setX(0);
	}
    else if ((point.x() + width) > w)
	{
        point.setX(w - width);
    }
    if (point.y() < 0)
	{
		point.setY(0);
	}
    else if ((point.y() + height) > h)
	{
        ptTmp = QPoint(mapToGlobal(rect().topLeft()));
        point.setY(w - height);
//        ptTmp = QPoint(mapToGlobal(rect().topRight()));
//        point.setX(ptTmp.x());
    }
	return point;
}

NumEdit2::NumEdit2( QWidget *parent )
 //   : QLineEdit( parent, name )
    : QLineEdit( parent)
{
}

NumEdit2::~NumEdit2()
{
}

void NumEdit2::mouseReleaseEvent(QMouseEvent*)
{
    if(isReadOnly())
        return;
    QPoint	point;

    selectAll();

    QString str;// = text();
    Calculator calc(str, NUMKEY, this);
    // 적당한 키보드의 위치를 얻어낸다
 //   point = getPtKeyboard(&calc);
//    calc.move(point);
    if(calc.exec() == QDialog::Accepted)
    {
        setText(calc.m_Value);
    }

    deselect();
//    setup->update();
}
// x=250,y=260
QPoint NumEdit2::getPtKeyboard(Calculator* calc)
{
//	QWidget	*d = QApplication::desktop();
    QDesktopWidget	*d = QApplication::desktop();
    int		w = d->width();		// 화면 전체크기 width
    int		h = d->height();	// 화면 전체크기 height
    QPoint	point;
    QPoint	ptTmp;

    // keyboard가 edit 위치 밑에 뜨도록 한다
    point = QPoint(mapToGlobal(rect().bottomLeft()));
    point.setY(point.y());

    int width = calc->width();
    int height = calc->height();

    width = 491;
    height = 437;

    qDebug() << "w=" << w << " h=" << h << " width=" << width << " height=" << height;
    // 화면영역을 넘어가면 위치를 보정한다
    if (point.x() < 0)
    {
        point.setX(0);
    }
    else if ((point.x() + width) > w)
    {
        point.setX(w - width);
    }
    if (point.y() < 0)
    {
        point.setY(0);
    }
    else if ((point.y() + height) > h)
    {
        ptTmp = QPoint(mapToGlobal(rect().topLeft()));
        point.setY(w - height);
//        ptTmp = QPoint(mapToGlobal(rect().topRight()));
//        point.setX(ptTmp.x());
    }
    return point;
}
