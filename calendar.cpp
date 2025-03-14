#include <QApplication>
#include <QtGui>
#include <QPainter>
#include "calendar.h"

Calendar::Calendar(QWidget *parent, QDate& d, const char *name)
//	: QWidget(parent, name)
	: QWidget(parent)
{
    setWindowTitle(name);
    m_dateViewMonth = m_selectDate	= d;
    //setViewMonth(d);
    m_dateToday = QDate::currentDate();
    m_cyWeek		= 36;

    QFont font;
    font.setPointSize(14);
    setFont(font);

    connect(this, SIGNAL(sigSelected(QDate)),
		parentWidget(), SLOT(slotSelected(QDate)));
}

void Calendar::resizeEvent(QResizeEvent *)
{
//    setViewMonth(m_dateToday);
    setViewMonth(m_dateViewMonth);
}

void Calendar::setViewMonth(QDate dtViewMonth)
{
	QDate		dt;
	QDate		dtBasic;
	int			nMaxRow = -1;
	int			nDayOfWeek;
	bool		bEnd = false;
	QString		strDate;
    int         selectRow = -1;
    int         selectCol = -1;

	m_rcSelect = QRect(-1, -1, -1, -1);

	m_dateViewMonth	= dtViewMonth;

    dtBasic.setDate(dtViewMonth.year(), dtViewMonth.month(), 1);
	nDayOfWeek = dtBasic.dayOfWeek();
	// 7이면 sunday인데, 0으로 바꾼다
	nDayOfWeek = (nDayOfWeek == 7) ? 0: nDayOfWeek;

	// 달력에서의 각 날짜를 얻는다
	for (int row = 0; row < 6; row++)
	{
		for (int col = 0; col < 7; col++)
		{
			dt = dtBasic.addDays(-(nDayOfWeek - (col + 0)) + ((row + 0) * 7));
			m_cellinfo[row][col].date	= dt;
			m_cellinfo[row][col].str.sprintf("%d", dt.day());
            if(m_selectDate == dt)
            {
                selectRow = row;
                selectCol = col;
            }
            if (dtViewMonth.month() == dt.month() &&
				dt.day() == dtViewMonth.daysInMonth())
				bEnd = true;
		}
		if (bEnd)
		{
			nMaxRow = row + 1;
			break;
		}
	}

	m_nMaxRow = nMaxRow;

	resizeCellInfo();
    if(selectRow != -1 && selectCol != -1)
    {
        m_rcSelect	= decreseRect(m_cellinfo[selectRow][selectCol].rect);
        m_strSelect	= m_cellinfo[selectRow][selectCol].str;
    }
    else
        m_rcSelect.setRect(-1,-1,-1,-1);
}

void Calendar::resizeCellInfo()
{
	QSize		sizeGrid;
	int			nRestCol;
	int			nRestRow;
	float		fPlusCol;
	float		fPlusRow;
	float		fTempCol;
	float		fTempRow;
	int			nAdditionCol;
	int			nAdditionRow;
	int			nTempCol = 0;
	int			nTempRow;

	// 각각의 영역을 균등하게 배분한다(총길이를 갯수에 맞게 나누고, 나머지 pixel을 균등배분)
	// 방법 : 나머지/갯수 값을 loop에 따라 누적해서 더한 값이 정수를 넘을때, 나눠진 값에 1을 더한다
	sizeGrid.setWidth(width() / 7);
	sizeGrid.setHeight((height() - m_cyWeek) / m_nMaxRow);
	nRestCol	= width() % 7;
	nRestRow	= (height() - m_cyWeek) % m_nMaxRow;
	nTempRow	= m_cyWeek;
	fTempRow	= 0.0;
	fPlusRow	= 1.0;

	for (int row = 0; row < m_nMaxRow; row++)
	{
		fTempRow += (float)nRestRow / (float)m_nMaxRow;
		if (fTempRow >= fPlusRow)
		{
			fPlusRow += 1.0;
			nAdditionRow = 1;
		}
		else
		{
			nAdditionRow = 0;
		}

		fTempCol	= 0.0;
		fPlusCol	= 1.0;
		nTempCol	= 0;
		for (int col = 0; col < 7; col++)
		{
			fTempCol += (float)nRestCol / 7.0;
			if (fTempCol >= fPlusCol)
			{
				fPlusCol += 1.0;
				nAdditionCol = 1;
			}
			else
			{
				nAdditionCol = 0;
			}

			m_cellinfo[row][col].rect.setRect(nTempCol,
				nTempRow,
				sizeGrid.width() + nAdditionCol + 1,
				sizeGrid.height() + nAdditionRow + 1);
			nTempCol += sizeGrid.width() + nAdditionCol;
        }
		nTempRow += sizeGrid.height() + nAdditionRow;
	}

	repaint();
}

QRect Calendar::decreseRect(QRect rect)
{
	QRect	rect2;

	rect2.setCoords(rect.left() + 1, rect.top() + 1,
		rect.right() - 1, rect.bottom() - 1);

	return rect2;
}

void Calendar::paintEvent( QPaintEvent *)
{
	QPainter	painter(this);
	QRect		rcTemp;
	QString		strLDay;
	QRect		rect;
	int			row, col;
	QString		strWeek[7];


    strWeek[0] = "일";
    strWeek[1] = "월";
    strWeek[2] = "화";
    strWeek[3] = "수";
    strWeek[4] = "목";
    strWeek[5] = "금";
    strWeek[6] = "토";
    painter.fillRect(QWidget::rect(), QBrush(Qt::white));

	// 영역을 채운다
	for (row = 0; row < m_nMaxRow; row++)
	{
		for (col = 0; col < 7; col++)
		{
			if (m_cellinfo[row][col].date == m_dateToday)
			{
				rect = decreseRect(m_cellinfo[row][col].rect);
                painter.setPen(Qt::white);
                painter.fillRect(rect, QBrush(QColor(250, 105, 0)));
			}
			else if (m_cellinfo[row][col].date.year() != m_dateViewMonth.year() ||
				m_cellinfo[row][col].date.month() != m_dateViewMonth.month())
			{
				rect = decreseRect(m_cellinfo[row][col].rect);
				painter.fillRect(m_cellinfo[row][col].rect,
					QBrush(Qt::lightGray));
			}
		}
	}

	// 테두리를 그린다
	painter.setPen(QPen(Qt::black, 1, Qt::DotLine));
	for (int row = 0; row < m_nMaxRow; row++)
	{
		painter.drawLine(0, m_cellinfo[row][0].rect.top(),
			width() - 1, m_cellinfo[row][0].rect.top());
	}
	for (int col = 1; col < 7; col++)
	{
		painter.drawLine(m_cellinfo[0][col].rect.left(), m_cyWeek,
			m_cellinfo[0][col].rect.left(), height() - 1);
	}
	painter.setPen(Qt::black);
	painter.drawLine(0, m_cellinfo[0][0].rect.top(),
		width() - 1, m_cellinfo[0][0].rect.top());
	int y = m_cellinfo[m_nMaxRow-1][0].rect.bottom() - 1;
	painter.drawLine(0, y, width() - 1, y);

	// 요일 표시
	painter.setPen(Qt::black);
	for (int i = 0; i < 7; i++)
	{
		if (i == 0)			// sunday
			painter.setPen(Qt::red);
		else if (i == 6)	// saturday
			painter.setPen(Qt::blue);
		else
			painter.setPen(Qt::black);
		rcTemp.setRect(m_cellinfo[0][i].rect.left(), 0,
			m_cellinfo[0][i].rect.width(), m_cyWeek);
		painter.drawText(rcTemp, Qt::AlignCenter, strWeek[i]);
	}

	// 날짜 표시
	painter.setPen(Qt::black);
	for (row = 0; row < m_nMaxRow; row++)
	{
		for (col = 0; col < 7; col++)
		{
            if (m_cellinfo[row][col].date.month() == m_dateViewMonth.month())
			{
                if (m_cellinfo[row][col].date == m_dateToday)
                    painter.setPen(Qt::white);
                else
                if (m_cellinfo[row][col].date.dayOfWeek() == 7)
					painter.setPen(Qt::red);
				else
				if (m_cellinfo[row][col].date.dayOfWeek() == 6)
					painter.setPen(Qt::blue);
				else
					painter.setPen(Qt::black);
			}
			else
			{
				painter.setPen(Qt::darkGray);
			}
			rect = m_cellinfo[row][col].rect;
			painter.drawText(rect, Qt::AlignCenter, m_cellinfo[row][col].str);
        }
	}

	// 선택된 날짜가 있을 경우
    if (!m_rcSelect.isNull() && m_rcSelect.x() != -1)
	{
        painter.fillRect(m_rcSelect, QBrush(QColor(105, 210, 231)));
		painter.drawText(m_rcSelect, Qt::AlignCenter, m_strSelect);
	}
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);

    QSize size = frameSize();
    painter.setPen(pen);
    painter.drawLine(0, 0, size.rwidth(), 0);
    painter.drawLine(0, 0, 0, size.rheight());
    pen.setColor(Qt::darkGray);
    painter.setPen(pen);
    painter.drawLine(size.rwidth()+1, 0, size.rwidth()+1, size.rheight());
    painter.drawLine(0, size.rheight()+1, size.rwidth()+1, size.rheight());
}

void Calendar::mousePressEvent(QMouseEvent* pEvent)
{
    QPoint		point = pEvent->pos();

	// 날짜가 선택되었을때 처리
	for (int row = 0; row < m_nMaxRow; row++)
	{
		for (int col = 0; col < 7; col++)
		{
			if (m_cellinfo[row][col].rect.contains(point))
			{
				m_rcSelect	= decreseRect(m_cellinfo[row][col].rect);
				m_strSelect	= m_cellinfo[row][col].str;
                m_selectDate = m_cellinfo[row][col].date;
				emit sigSelected(m_cellinfo[row][col].date);
				repaint();
				break;
			}
		}
	}
}

bool Calendar::eventFilter(QObject* , QEvent *e)
{
    return false;
    if(e->type() == QEvent::MouseButtonPress)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "Calendar.png";
        pixmap.save(name);
        qWarning() << "Captured : " + name;
        return false;
    }
    return false;
}


