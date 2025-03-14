#include <QApplication>
#include <QDesktopWidget>
#include <QString>
#include <QPainter>
#include <QEvent>
#include <QPixmap>
#include <QDebug>
#include "calendardlg.h"
extern bool    m_bScreenSave;

CalendarDlg::CalendarDlg(QDate& d, QWidget *parent)
        :QDialog(parent)
{
    m_dateSelected = d;

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );

    QFont font;
    font.setPointSize(14);
    setFont(font);

	QString str;

    setWindowTitle(tr("Calendar"));

	setFocusPolicy(Qt::ClickFocus);

	// prev 버튼 생성
	m_pbtnPrev = new QPushButton(this);
    m_pbtnPrev->setGeometry(0, 0, 100, 60);
	m_pbtnPrev->setText("<<");

	// 날짜 라벨 생성
    m_plbDate = new QPushButton(this);
//	m_plbDate->setAlignment(Qt::AlignCenter);
    m_plbDate->setGeometry(200, 0, 180, 60);

	// next 버튼 생성
	m_pbtnNext = new QPushButton(this);
    m_pbtnNext->setGeometry(500, 0, 100, 60);
	m_pbtnNext->setText(">>");

	// 달력 컴포넌트 생성
    m_pCalendar = new Calendar(this, m_dateSelected, "m_pCalendar");
    m_pCalendar->setGeometry(0, 60, 600, 440);

	// OK 버튼 생성
	m_pbtnOK = new QPushButton(this);
    m_pbtnOK->setGeometry(200, 500, 200, 60);
	m_pbtnOK->setText("OK");

	str.sprintf("%d.%d", m_dateSelected.year(), m_dateSelected.month());
	m_plbDate->setText(str);

    connect(m_pbtnPrev, SIGNAL(clicked()), this, SLOT(slotPrev()));
    connect(m_pbtnNext, SIGNAL(clicked()), this, SLOT(slotNext()));
    connect(m_plbDate, SIGNAL(clicked()), this, SLOT(slotCurrent()));
    connect(m_pbtnOK, SIGNAL(clicked()), this, SLOT(accept()));

    startTimer(1000);
    m_nIdle = 0;

    this->move(212,20);
    installEventFilter(this);
}

void CalendarDlg::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}

CalendarDlg::~CalendarDlg()
{
	delete m_pbtnPrev;
	delete m_plbDate;
	delete m_pbtnNext;
	delete m_pCalendar;
	delete m_pbtnOK;
}

void CalendarDlg::slotSelected(QDate date)
{
    if(m_dateSelected == date)
        QDialog::accept();
	m_dateSelected = date;
}

QDate CalendarDlg::getDate()
{
	return m_dateSelected;
}

void CalendarDlg::setDate(QDate& date)
{
	m_dateSelected = date;
}

void CalendarDlg::slotPrev()
{
	QString	str;

	m_dateSelected = m_dateSelected.addMonths(-1);

	m_pCalendar->setViewMonth(m_dateSelected);
	str.sprintf("%d.%d", m_dateSelected.year(), m_dateSelected.month());
	m_plbDate->setText(str);
    update();
}


void CalendarDlg::slotNext()
{
	QString	str;

	m_dateSelected = m_dateSelected.addMonths(1);

	m_pCalendar->setViewMonth(m_dateSelected);

	str.sprintf("%d.%d", m_dateSelected.year(), m_dateSelected.month());
	m_plbDate->setText(str);
    update();
}

void CalendarDlg::slotCurrent()
{
    QString	str;

    m_dateSelected = QDate::currentDate();

    m_pCalendar->setViewMonth(m_dateSelected);

    str.sprintf("%d.%d", m_dateSelected.year(), m_dateSelected.month());
    m_plbDate->setText(str);
    update();
}

void CalendarDlg::paintEvent(QPaintEvent *)
{
    QPainter	p(this);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);

    QSize size = frameSize();
    p.setPen(pen);
    p.drawLine(0, 0, size.rwidth(), 0);
    p.drawLine(0, 0, 0, size.rheight());
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawLine(size.rwidth()+1, 0, size.rwidth()+1, size.rheight());
    p.drawLine(0, size.rheight()+1, size.rwidth()+1, size.rheight());
}

bool CalendarDlg::eventFilter(QObject* , QEvent *e)
{
    return false;
    if(e->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "CalendarDlg.png";
        pixmap.save(name);
        qWarning() << "Captured : " + name;
        return false;
    }
    return false;
}


