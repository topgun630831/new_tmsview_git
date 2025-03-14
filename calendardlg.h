#ifndef CALENDARDLG_H
#define CALENDARDLG_H

#include <QDialog>
#include <QDateTime>
#include <QPushButton>
#include <QLabel>
#include "calendar.h"

extern uint Popup2ScreenSaveCount;

class CalendarDlg : public QDialog
{
    Q_OBJECT
public:
    CalendarDlg(QDate& date, QWidget *parent=0);
	~CalendarDlg();

// Event
protected:
    void	paintEvent(QPaintEvent*);
    bool	eventFilter(QObject*, QEvent *e);

// Signal
signals:

// Slot
public slots:
	void	slotSelected(QDate date);
	void	slotPrev();
	void	slotNext();
    void	slotCurrent();

// member functions
public:
	QDate	getDate();
	void	setDate(QDate& date);

// member variables
protected:
    void timerEvent(QTimerEvent *);
    QPushButton*	m_pbtnPrev;
	QPushButton*	m_pbtnNext;
    QPushButton*	m_plbDate;
	Calendar*		m_pCalendar;
	QPushButton*	m_pbtnOK;
	QDate			m_dateSelected;
	uint	m_nIdle;
};


#endif // CALENDARDLG_H
