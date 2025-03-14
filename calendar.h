#ifndef CALENDAR_H
#define CALENDAR_H

#include <qwidget.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qstring.h>
#include <qpoint.h>

typedef struct
{
	QRect		rect;
	QDate		date;
	QString		str;
} DAY_INFO;

class Calendar : public QWidget
{
    Q_OBJECT
public:
    Calendar( QWidget *parent, QDate& d, const char *name=0);

// event
protected:
	void	resizeEvent(QResizeEvent *);
    void	paintEvent(QPaintEvent *);
    void	mousePressEvent(QMouseEvent* pEvent);
    bool	eventFilter(QObject*, QEvent *e);

signals:
	void	sigSelected(QDate);

// member function
public:
	void	setViewMonth(QDate dt);

protected:
	QRect	decreseRect(QRect rect);
	void	resizeCellInfo();;

// member variables
protected:
	int			m_cyWeek;
	QDate		m_dateToday;
	QDate		m_dateViewMonth;
    QDate		m_selectDate;
    DAY_INFO	m_cellinfo[6][7];
	int			m_nMaxRow;
	QRect		m_rcSelect;
	QString		m_strSelect;
};


#endif // CALENDAR_H
