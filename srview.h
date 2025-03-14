/****************************************************************************
**
** Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef SRVIEW_H
#define SRVIEW_H

#include <QRadioButton>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QDialog>
#include <QMainWindow>
#include <qpainter.h>
#include <QtGui>
#include <QFile>
#include <QTcpSocket>
#include <QtSql>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTableWidget>
#include <QFileSystemWatcher>

#include "mylineedit.h"
#include "cpparser.h"
#include "mondialog.h"

#ifdef WIN32
	#define FONTSIZE(n)	setPointSize(n)
#else
	#define FONTSIZE(n)	setPixelSize(n)
#endif

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QLCDNumber;
class QPainter;
class CDrawObject;
class QTcpSocket;
class CWMINet;
class CRunObject;
class CTag;

enum DrawShape
{
	rectangle       = 0,
	roundrectangle  = 1,
	polyline        = 2,
	arc             = 3,
	trend           = 4,
	slider          = 5,
	monitor         = 6,
	text            = 7,
	ellipse         = 8,
	line            = 9,
	polygon         = 10,
	button          = 11,
	SELECT          = 12,
	bezier          = 13,
	chord           = 14,
	pie             = 15,
	bitmap          = 16,
	shapelist       = 17,
    arrow           = 18,
    triangle        = 19,
    symbol          = 20,
    
	modeText        = 900,
    modeConfig      = 901,
    modeRun         = 902,
};

/*static char*    DrawShapeName[] = 
{
	"Rectangle",
	"RoundRectangle",
	"Polyline",
	"Arc",
	"Trend",
	"Slider",
	"Monitor",
	"Text",
	"Ellipse",
	"Line",
	"Polygon",
	"Button",
	"Select",
	"Bezier",
	"Chord",
	"Pie",
	"Bitmap",
	"Group",
	"Arrow",
	"Triangle",
};
*/
typedef QList<CDrawObject *> DrawList;
typedef QList<QObject *> LINKLIST;
typedef QList<CRunObject *> RUNLIST;
typedef QList<CParser *> PARSERLIST;

#define	WMI_CMD_CONNECT			0x00
#define	WMI_CMD_RTDATA			0x01
#define	WMI_CMD_RTREND			0x02
#define	WMI_CMD_HTREND			0x03
#define	WMI_CMD_COMMMON			0x06

#define	WMI_SCMD_INIT			0x01

#define	WMI_SCMD_OPENPAGE		0x01
#define	WMI_SCMD_ADDTAG			0x02
#define	WMI_SCMD_VALREQ			0x03
#define	WMI_SCMD_CONTROLTAG		0x04
#define	WMI_SCMD_PARAMETER		0x05

#define	WMI_SCMD_TOPIC_READ		0x01
#define	WMI_SCMD_TOPIC_SELECT	0x02
#define	WMI_SCMD_FRAME_REQ		0x03
#define	WMI_SCMD_IDLE			0x04
#define	WMI_SCMD_COUNTER_RESET	0x05


typedef struct {
	char	ID[4];
	quint32	Seq;
	quint32	Len;
	quint16	SubCmd;
	quint16	CmdGroup;
} 
#ifndef WIN32
__attribute__((__packed__))
#endif
WMINET_HEADER;

//class CSrView : public QMainWindow
class CSrView : public CMonDialog
{
    Q_OBJECT

public:
    CSrView(const QString& folder, const QString& fileName, CSrView* pMain);
    CSrView(const QString& folder, const QString& fileName, const QString ipAddr, const int port);
    ~CSrView();
    void onRead(QString& cmd, QJsonObject& jobject);
    void addTag(QString& TagName);
	bool m_bChanged;
	QString		m_folder;
	QString		m_WinName;
	QString		m_OpenWinName;
	bool m_bOpening;
    CSrView*	m_pMain;
    QByteArray  mValueRead;
    QByteArray  mInfoRead;
    QString     mChangedCmd;
    QString     mInfoCmd;
    int         mCommCount;
// event
	bool	eventFilter(QObject*, QEvent *e);
	void	mouseReleaseEvent(QMouseEvent*);
	void	mousePressEvent(QMouseEvent*);
	bool	InputPassword();
	void	ClearAll();
    void    makeCommand();
    bool    writeTag(QString tagname, QString value, int delay);
    void    WinLoad();
    QByteArray makeCommand(QString cmd, QJsonValue argValue);

signals:
	void	sigSysMenu();
//    void newMessage(const QString &from, const QString &message);

private slots:
    void onTimer();
    void onSysMenuTimer();
    void onTimerPopup();
	void slotDataChanged();
    void handleFileChanged(const QString&);
    void handleFileChanged2(const QString&);
public slots:
	void	slotOpenWindow(const QString& name, int nOpen);
	void	slotCloseWindow();
	void	slotTMSData();
	void	slotTMSInfo();
	void	slotSewer();
	void	slotU_City();
	void	slotRainSensor();
	void	slotCommMon();
    void	slotTrend();
    void	slotSysMenu();
    void	slotPointMon();
    void    slotAlarmSummary();
    void	slotLog();
    void	slotEpsTestSetup();

private:
    void fileLoad(const QString& fileName, bool bLoad);
	void processLine(QString& line);
    void pageLoad(QJsonObject& jPage);
    void listLoad(QJsonArray& array, DrawList& oblist);
	void paintEvent(QPaintEvent*);
	CRunObject* HitTestNext(QPoint& point);
	CRunObject* FindObject(QObject* pObj);
	void OnBlink(RUNLIST& List, bool& blink);

    bool        mbError;
    QString     msError;
    QLabel		*imageLabel;
	DrawList	layer1List;
	DrawList	layer2List;
	QString 	m_PageName;			// 페이지 이름 
    QString     m_PageDescription;	// 페이지 설명 
	int		   	m_VerNo;			// Version
	QColor		m_PageColor;		// 페이지 칼라 
	QColor		m_BColor;			// 비트맵 투명 이미지 
	QRect 		m_PageRect;			// 페이지 윤곽 (QRect) 상하좌우 
	QRect 		m_FillRect;			// 페이지 윤곽 (QRect) 상하좌우 
	QSize 		m_PageSize;			// 페이지 사이즈 
	int			m_Option;			// 페이지 옵션 
	int			m_BPosition;		// 0,1,2 (바둑판,가운데,윈도우 크기대로);
	int			m_Security;			// 현재 페이지 사용가능자 등급 1-8
	bool		m_bBackImg;
	QString		m_BackImgName;		// 배경그림 파일명
	QBrush	    m_BackBrush;		// Brush Attrib 
	QImage		m_BackImage;
	QTimer		m_Timer;
	QTimer		m_SysMenuTimer;
	QTimer		m_TimerPopup;
	RUNLIST		m_user;
	bool		m_bMain;
	uint		m_nIdle;
	RUNLIST		m_slowBlink;
	RUNLIST		m_normalBlink;
	RUNLIST		m_fastBlink;
	bool		m_bSlow;
	bool		m_bNormal;
	bool		m_bFast;
	int			m_nSlow;
	int			m_nNormal;
	QList<QString> m_TagList;
	int			m_nTimerCnt;
	bool		m_bMousePress;
	int			m_nMousePressCount;
	bool		m_bDoEnding;
	int			m_nPort;
	QString		m_ipAddr;
	bool		m_bFileLoadOk;
    QList<TAG_WRITE_DELAY*>   m_listDelay;
    void loadStyleSheet(const QString &sheetName);
    QFileSystemWatcher  m_watcher;
    QFileSystemWatcher  m_watcher2;
    bool                m_bUsbReady;
};

#endif
