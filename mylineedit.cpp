#include <QtGui>
#include <QApplication>
#include "mykeyboard.h"
#include "mylineedit.h"
#include "QDesktopWidget"

extern QWidget *psetup;
extern QWidget *loadcontrol;
MyLineEdit::MyLineEdit( QWidget *parent )
 //   : QLineEdit( parent, name )
    : QLineEdit( parent)
{
}

MyLineEdit::~MyLineEdit()
{
}

void MyLineEdit::mouseReleaseEvent(QMouseEvent*)
{
    if(isReadOnly())
        return;
	QPoint	point;

    qDebug() << "echomode:" << echoMode();
    MyKeyboard* keyboard = new MyKeyboard(this, "keyboard", text(), echoMode(), maxLength());

//    keyboard->setFixedSize(1024, 600);
//    keyboard->setGeometry(0,0,1024,600);
    // 적당한 키보드의 위치를 얻어낸다
//	point = getPtKeyboard(keyboard);
//	keyboard->move(point);

	keyboard->exec();
    setText(keyboard->m_Text);
//    selectAll();
//    deselect();
/*    if(psetup)
        psetup->update();
    if(loadcontrol)
        loadcontrol->update();
*/
}
//x=366, y=190
QPoint MyLineEdit::getPtKeyboard(MyKeyboard*)
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

 	// 화면영역을 넘어가면 위치를 보정한다
    qWarning() << "x, w" << point.x() << w;
    if (point.x() < 0)
	{
		point.setX(0);
	}
//	else if (point.x() + keyboard->width() > w)
      else if (point.x() + 530 > w)
    {
        point.setX(w - 530);
        if ((point.y() + 186) > h)
        {
            ptTmp = QPoint(mapToGlobal(rect().topLeft()));
            point.setY(480 - 186 - 200);
        }
    }
    else
	if (point.y() < 0)
	{
		point.setY(0);
    }
    else
    if ((point.y() + 186) > h)
	{
		ptTmp = QPoint(mapToGlobal(rect().topLeft()));
        point.setY(480 - 186 - 200);
//        ptTmp = QPoint(mapToGlobal(rect().topRight()));
//        point.setX(ptTmp.x());
    }

	return point;
}

void MyLineEdit::slotMyInsert(const QString& )
{
//	if (str == QString(QChar(0x08)))	// backspace 일 경우
//		backspace();
//	else
//		insert(str);
}
