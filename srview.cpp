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
#include "sewerdialog2.h"
#include "epslog.h"
#include <unistd.h>
#include <QtNetwork>
#include <QTime>
#include <QGraphicsSceneMouseEvent>
#include "myapplication.h"
#include "srview.h"
#include "calculator.h"
#include "calendardlg.h"
#include "mykeyboard.h"
#include "runani.h"
#include "button.h"
#include "tmsinfo.h"
#include "wminet.h"
#include "tag.h"
//#include "ui_sewerdialog2.h"
#include "networksetup.h"
#include "common.h"
#include "common2.h"
#include "pointmon.h"
#include "alarmsummary.h"
#include "history.h"
#include "infomationdialog.h"
#include "srsocket.h"
#include <QDebug>
#include <QDesktopWidget>
#include "myapplication.h"
#include "monitorudp.h"
#include "tmstrenddialog.h"
#include "testsetup.h"

extern CSrSocket *g_pSrSocket;
//QSqlDatabase db;

extern bool    m_bScreenSave;
extern CMyApplication *gApp;
extern int     sr_fd;

bool g_bReboot;
bool g_bPowerOff;
CSrView	*g_pMainView=NULL;
QMap<QString, CTag*> m_TagMap;
QMap<quint16, CTag*> m_AtomTagMap;

extern bool g_DebugDisplay;
extern bool g_WindowsRun;

extern int		g_HasuPort;
extern QString gSoftwareModel;

extern int XMaxRes;
extern int YMaxRes;

int			m_wdtSockFd;
int			m_wdtID;
QString GraphicId = "Graphic";

void appendRunList(QString& TagName, CRunObject *pRun)
{
    if(g_DebugDisplay)
        qDebug() << "appendRunList = " << TagName;
	CTag* pTag;
	if(!m_TagMap.contains(TagName))
	{
        if(g_DebugDisplay)
        qDebug() << "append = " << TagName;
        pTag = new CTag(TagName);
		m_TagMap[TagName] = pTag;
	}
	else
		pTag = m_TagMap[TagName];
	pTag->setRunList(pRun);
}

bool appendParserList(QString& TagName, CParser *pParser)
{
    if(g_DebugDisplay)
           qDebug() << "appendParserList = " << TagName;
    CTag* pTag = NULL;
	if(m_TagMap.contains(TagName))
	{
//        qDebug() << "append = " << TagName;
        pTag = m_TagMap[TagName];
		pTag->setParserList(pParser);
		return true;
	}
	return false;
}

void removeRunList(QString& TagName, CSrView* pView)
{
    if(g_DebugDisplay)
        qDebug() << "removeRunList = " << TagName;
	CTag* pTag;
	if(m_TagMap.contains(TagName))
	{
//        qDebug() << "remove = " << TagName;
        pTag = m_TagMap[TagName];
		pTag->removeRunList(pView);
	}
}

CTag* FindTagObject(QString& TagName)
{
	CTag* pTag;
	if(m_TagMap.contains(TagName))
		pTag = m_TagMap[TagName];
	else
    {
		pTag = NULL;
        qDebug() << "Not found:" << TagName;
    }
	return pTag;
}

CSrView::CSrView(const QString& folder, const QString& fileName, const QString	ipAddr, const int port) : CMonDialog(0)
{
    mCommCount = 100;
    m_nMousePressCount = 0;
	m_bMousePress = false;
	m_bDoEnding = false;
	m_folder = folder;
	g_pMainView = this;
    m_bFileLoadOk = false;
	setBackgroundRole(QPalette::Background);

//    db = QSqlDatabase(db1);

	m_nPort = port;
	m_ipAddr = ipAddr;


    int ipos = m_OpenWinName.indexOf(QChar('.'));
    m_WinName = m_OpenWinName.left(ipos);

	int pos = fileName.indexOf(QChar('.'));
	m_WinName = fileName.left(pos);

	QString name = folder;
	name += "/windows/";
	name += fileName;
    name += ".json";

	m_bOpening = false;
	
    m_PageSize.setWidth(XMaxRes);
    m_PageSize.setHeight(YMaxRes);
    fileLoad(name, true);

	m_BackBrush.setColor(m_PageColor);

	move(0, 0);

    mChangedCmd = "Tag_Changed";
    mInfoCmd = "Tag_Summary";
    makeCommand();
    QString str;
	if(m_PageDescription == "")
		str = m_PageName;
	else
		str = m_PageDescription;
    setWindowTitle(str);
    resize(m_PageSize);
    
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
    connect(&m_SysMenuTimer, SIGNAL(timeout()), this, SLOT(onSysMenuTimer()));
	connect(this, SIGNAL(sigSysMenu()), this, SLOT(slotSysMenu()));
	m_Timer.start(100);
	m_SysMenuTimer.start(1000);
	m_bMain = true;
	m_nIdle = 0;
	m_bSlow = m_bNormal = m_bFast = false;
	m_nSlow = m_nNormal = 0;
	m_nTimerCnt = 0;
	installEventFilter(this);
    g_pSrSocket->mGraphicInitOk = false;

//    loadStyleSheet(QString("QTDark.stylesheet"));
//    loadStyleSheet(QString("test"));
//    repaint();

    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("Network_Get"), argObject, vValue);

    m_watcher.addPath("/sun/web/tmp/");
    connect(&m_watcher, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged(const QString&)));

    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
        gApp->setOverrideCursor(Qt::PointingHandCursor);
    else
        gApp->setOverrideCursor(Qt::BlankCursor);

    m_watcher2.addPath("/tmp/");
    connect(&m_watcher2, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged2(const QString&)));
}
CSrView::CSrView(const QString& folder, const QString& fileName, CSrView* pMain) : CMonDialog(pMain)
{
    QPalette newPalette = palette();
    newPalette.setColor(QPalette::Window, Qt::black);
    setPalette(newPalette);

    g_pSrSocket->mGraphicInitOk = false;
    m_pMain = pMain;
	setBackgroundRole(QPalette::Background);

	int ipos = fileName.indexOf(QChar('.'));
	m_WinName = fileName.left(ipos);

	m_bDoEnding = false;
	m_folder = folder;
	QString name = m_folder;
	name += "/windows/";
	name += fileName;
    name += ".json";
//	fileLoad(name, true);

	m_BackBrush.setColor(m_PageColor);

    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - m_PageRect.center() );
//	QPoint pos(m_PageRect.left(), m_PageRect.top());
//	move(pos);

    pMain->makeCommand();
    QString str;
	if(m_PageDescription == "")
		str = m_PageName;
	else
		str = m_PageDescription;
    setWindowTitle(str);
    resize(m_PageSize);
	connect(&m_TimerPopup, SIGNAL(timeout()), this, SLOT(onTimerPopup()));
	m_TimerPopup.start(500);
	m_bMain = false;
	m_nIdle = 0;
	m_bSlow = m_bNormal = m_bFast = false;
	m_nSlow = m_nNormal = 0;
	installEventFilter(this);

	repaint();
}

void CSrView::ClearAll()
{
    CDrawObject *pObject;
	
    int i = 0;
    for (i = 0; i < layer1List.size(); ++i) {
		pObject = layer1List.at(i);
		delete pObject;
	}
    layer1List.clear();

    for (i = 0; i < layer2List.size(); ++i) {
	while (!layer2List.isEmpty())
        delete (CDrawObject*)layer2List.at(i);
	}

	layer2List.clear();
//    CRunObject*	pRun;
	for (i = 0; i < m_user.size(); ++i) {
//		pRun = m_user.at(i);
//		delete pRun;
	}
    m_user.clear();
//	while (!m_user.isEmpty())
//		m_user.removeFirst();
    if(m_bMain)
	{
		QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
		while (i != m_TagMap.constEnd()) {
			delete i.value();
			i++;
		}
        m_TagMap.clear();
	}

	m_slowBlink.clear();
	m_normalBlink.clear();
	m_fastBlink.clear();
}

CSrView::~CSrView()
{
	ClearAll();
    while(!m_listDelay.isEmpty())
    {
        TAG_WRITE_DELAY *cmd;
        cmd = m_listDelay.takeFirst();
        delete cmd;
    }
}

CRunObject* CSrView::HitTestNext(QPoint& point)
{
	CRunObject* pObj;
	for (int i = 0; i < m_user.size(); ++i) {
		pObj = m_user.at(i);
		if(!pObj->m_pObj->m_bDelete)				// single Object
			if(pObj->m_pObj->HitTest(point))
				return pObj;
	}
	return NULL;
}

CRunObject* CSrView::FindObject(QObject* pObj)
{
	CRunObject* pRunObj;
	for (int i = 0; i < m_user.size(); ++i) {
		pRunObj = m_user.at(i);
        if(pRunObj->m_pButton == (Button2*)pObj)
		{
			return pRunObj;
		}
	}
	return NULL;
}

bool CSrView::eventFilter(QObject* pObj, QEvent *e)
{
	if(e->type() == QEvent::MouseButtonRelease)
	{
        CRunObject* pRunObj = (CRunObject*)FindObject(pObj);
		if(pRunObj)
		{
			m_bMousePress = false;
			m_nMousePressCount = 0;
			pRunObj->Exec(( CTag* )NULL );
		}
		return false;
	}
    else
    if(e->type() == QEvent::MouseButtonDblClick)
    {
        QPixmap	pixmap;
        pixmap = QPixmap::grabWidget(this);
        QString name = "/data/capture.png";
        pixmap.save(name, "png");
        qWarning() << "Captured : " + name;
    }
    return false;
}

void CSrView::mousePressEvent(QMouseEvent* event)
{
	event = event;
	if(m_bMain == false)
		m_pMain->m_nIdle = false;
	else
	{
		m_bMousePress = true;
		m_nMousePressCount = 0;
		m_nIdle = 0;		// Main일때만 시간 연장
	}
    if(event->button() == Qt::LeftButton)
	{
        QPoint pos = event->pos();
        CRunObject* pObj = (CRunObject*)HitTestNext(pos);
		if(pObj)
		{
			m_bMousePress = false;
			pObj->Exec(( CTag* )NULL );
		}
	}
}

void CSrView::mouseReleaseEvent(QMouseEvent* event)
{
	m_bMousePress = false;
	m_nMousePressCount = 0;

	if(event->button() == Qt::LeftButton)
	{
/*		QPoint pos = event->pos();
		CRunObject* pObj = (CRunObject*)HitTestNext(pos);
		if(pObj)
		{
			m_bMousePress = false;
			beeper(4);	// 200ms
			pObj->Exec(( CTag* )NULL );
		}
		else
			beeper(1);	// 50ms
*/
	}
	
}

extern bool g_bExit;

void CSrView::slotSysMenu()
{
    //gApp->TouchReset();

    CNetworkSetup dlg(this);
	g_bReboot = false;
	g_bPowerOff = false;
	dlg.exec();
    if(g_bExit)
        accept();

	if(g_bReboot == true)
		reject();
	if(g_bPowerOff == true)
	{
		QPainter	p(this);
		QColor color(Qt::black);
		QBrush brush(color);
//		p.setBackground(brush);
//		p.eraseRect(m_FillRect);
//		p.drawText(100,100, "이젠 전원을 끄셔도 안전합니다.");
        reject();
	}
#if 0
	if(QMessageBox::question(this, tr("종료"),
					   tr(" 종료를 하면 더 이상 감시를 할 수 없습니다.\n 모든 프로그램을 종료하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
//		reject();
//		QMessageBox::information(this, tr("WMI"), tr("1분후에 전원을 끄십시요."));
	}
#endif
}

void CSrView::onTimer()
{
	if(m_bOpening)
	{
        ClearAll();
		m_bOpening = false;
		setBackgroundRole(QPalette::Background);

		int ipos = m_OpenWinName.indexOf(QChar('.'));
		m_WinName = m_OpenWinName.left(ipos);

		QString fname = m_folder;
		fname += "/windows/";
		fname += m_OpenWinName;
        fname += ".json";
        if(g_DebugDisplay)
             qDebug() << "Open filename=" << fname;
		fileLoad(fname, true);
		QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
		while (i != m_TagMap.constEnd()) {
			QString tagname = i.key();
			i++;
		}
        g_pMainView->makeCommand();

    }
    if(m_bFileLoadOk == false)
        return;
    if(m_bDoEnding == false)
    {
        if(!m_listDelay.isEmpty())
        {
            TAG_WRITE_DELAY *delay;
            delay = m_listDelay[0];
            delay->Delay--;
            if(delay->Delay <= 0)
            {
                g_pSrSocket->sendCommand((QWidget*)this, delay->Cmd, QString(""), delay->Data);
                delete delay;
                m_listDelay.takeFirst();
            }
        }
        if(++mCommCount >= 10)
        {
            if(g_pSrSocket->mGraphicInitOk == false)
                g_pSrSocket->sendCommand((QWidget*)this, mInfoCmd, GraphicId, mInfoRead);
            else
                g_pSrSocket->sendCommand((QWidget*)this, mChangedCmd, GraphicId, mValueRead);
            mCommCount = 0;
        }
    }
	if(++m_nTimerCnt < 5)
		return;


	m_nTimerCnt = 0;

	if(++m_nSlow >= 3) 
	{	OnBlink(m_slowBlink,m_bSlow);
		m_nSlow = 0;
	}
	if(++m_nNormal >= 2) 
	{	OnBlink(m_normalBlink,m_bNormal);
		m_nNormal = 0;
	}
	OnBlink(m_fastBlink,m_bFast);
	
	if(m_bChanged)
	{
//		if(m_bDoEnding == false)
//			repaint();
		m_bChanged = false;
	}
}

void CSrView::onSysMenuTimer()
{
	if(m_bMousePress == true)
	{
		if( ++m_nMousePressCount > 5)		// 10초
		{
			m_bMousePress = false;
			m_nMousePressCount = 0;
			emit sigSysMenu();
//			QMessageBox::information(this, tr("WMI"), tr("Mouse event."));
		}
	}
}

void CSrView::onTimerPopup()
{
	m_nTimerCnt = 0;

	if(++m_nSlow >= 3) 
	{	OnBlink(m_slowBlink,m_bSlow);
		m_nSlow = 0;
	}
	if(++m_nNormal >= 2) 
	{	OnBlink(m_normalBlink,m_bNormal);
		m_nNormal = 0;
	}
	OnBlink(m_fastBlink,m_bFast);

	if(m_bChanged)
	{
		repaint();
		m_bChanged = false;
	}
/*	if(++m_nsBlink_Count >= 3) 
	{	OnBlink(GetTimerS(),m_bBlinkS,rect);
		m_nsBlink_Count = 0;
	}
	if(++m_nmBlink_Count >= 2) 
	{	OnBlink(GetTimerM(),m_bBlinkM,rect);
		m_nmBlink_Count = 0;
	}
	OnBlink(GetTimerF(),m_bBlinkF,rect);
*/
 }

void CSrView::fileLoad(const QString& fileName, bool)
{
	m_bFileLoadOk  = false;
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
        QString str=QString(tr("파일을 열 수 없습니다.(%1)")).arg(m_WinName);
		m_PageSize.setHeight(200); 
        m_PageSize.setWidth(300);
        CInfomationDialog dlg(str);
        dlg.exec();
        CNetworkSetup dlg2;
        dlg2.exec();
        if(g_bExit)
            accept();
        return;
	}
	m_bFileLoadOk  = true;
    QJsonParseError err;
    QString errstr;
    QJsonDocument jDoc = QJsonDocument().fromJson(file.readAll(),&err);
    if(err.error != QJsonParseError::NoError)
    {
        errstr = err.errorString();
        qDebug() << "\n\n\nError : (" << err.offset << ") " << errstr;
        QString msg = "File Load JSon Error :(";
        msg += QString::number(err.offset);
        msg +=")";
        msg +=errstr;
        msg += " - ";
        msg += fileName;
        file.close();
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    QJsonObject jPage = jDoc.object();
    pageLoad(jPage);
    QJsonArray array = jPage["Layer1"].toArray();
    listLoad(array, layer1List);
    array = jPage["Layer2"].toArray();
    listLoad(array, layer2List);
	file.close();
}

void CSrView::pageLoad(QJsonObject& jPage)
{
    m_PageName          = jPage["Name"].toString();
    m_PageDescription   = jPage["Desc"].toString();
    m_VerNo = jPage["Ver"].toDouble();
    m_PageRect.setLeft(0);
    m_PageRect.setTop(0);
    m_PageRect.setRight((int)jPage["SizeX"].toDouble());
    m_PageRect.setBottom((int)jPage["SizeY"].toDouble());
    m_PageSize.setWidth((int)jPage["SizeX"].toDouble());
    m_PageSize.setHeight((int)jPage["SizeY"].toDouble());

    m_bBackImg = (int)jPage["UseBackImage"].toDouble();
    m_BackImgName = jPage["BackImage"].toString();

	int color, r, g, b;
    color = (int)jPage["Color"].toDouble();
	r = color & 0xff;
	g = (color & 0xff00) >> 8;
	b = (color & 0xff0000) >> 16;

	m_PageColor.setRgb(r, g, b); 
	m_BackBrush.setColor(m_PageColor);
	m_BackBrush.setStyle(Qt::SolidPattern);

	m_FillRect.setSize(m_PageSize);

	if(m_bBackImg)
	{
		QString name;
		name += m_folder;
		name += "/windows/";
		name += m_BackImgName;
		bool b = m_BackImage.load(name);
		if(b != true)
             qDebug() << "Background Image file load  failure.:" << name;
	}
}

void CSrView::listLoad(QJsonArray& array, DrawList& oblist)
{
    CDrawObject* pObject;
    bool	isButton;
    CMyTextObject* pTextObject;

    foreach(const QJsonValue jVal, array)
    {
        isButton = false;
        QJsonObject jObj = jVal.toObject();
        QString shape = jObj["Shape"].toString();
        if(shape == "Rectangle")
        {
            CMyRectObject* pRectObject = new CMyRectObject(this);
            pObject = pRectObject;
            if(pRectObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "RoundRectangle")
        {
            CMyRoundRectObject* pRectObject = new CMyRoundRectObject(this);
            pObject = pRectObject;
            if(pRectObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Polyline")
        {
            CMyPolyObject* pPolyObject = new CMyPolyObject(this);
            pObject = pPolyObject;
            if(pPolyObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
            if(shape == "Arc")
        {
            CMyArcObject* pArcObject = new CMyArcObject(this);
            pObject = (CMyArcObject*)pArcObject;
            if(pArcObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Slider")
        {
            CMyRectObject* pRectObject = new CMyRectObject(this);
            pObject = (CMyRectObject*)pRectObject;
            if(pRectObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Monitor")
        {
            CMyRectObject* pRectObject = new CMyRectObject(this);
            pObject = (CMyRectObject*)pRectObject;
            if(pRectObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Text")
        {
            pTextObject = new CMyTextObject(this);
            pObject = (CMyRectObject*)pTextObject;
            if(pTextObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
            if(shape == "Ellipse")
        {
            CMyEllipseObject* pEllipseObject = new CMyEllipseObject(this);
            pObject = pEllipseObject;
            if(pEllipseObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Line")
        {
            CMyLineObject* pLineObject = new CMyLineObject(this);
            pObject = pLineObject;
            if(pLineObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Polygon" || shape == "Arrow" || shape == "Triangle")
        {
            CMyPolygonObject* pPlygonObject = new CMyPolygonObject(this);
            pObject = pPlygonObject;
            if(pPlygonObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Button")
        {
            CMyButtonObject* pButtonObject = new CMyButtonObject(this);
            pObject = pButtonObject;
            if(pButtonObject->objectLoad(jObj))
                oblist.append(pObject);
            pButtonObject->button->installEventFilter(this);
            isButton = true;
        }
        else
        if(shape == "Chord")
        {
            CMyChordObject* pChordObject = new CMyChordObject(this);
            pObject = pChordObject;
            if(pChordObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Bitmap" || shape == "Select")
        {
            CMyBitmapObject* pBitmapObject = new CMyBitmapObject(this);
            pObject = pBitmapObject;
            if(pBitmapObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Bezier")
        {
            CMyBezierObject* pBezierObject = new CMyBezierObject(this);
            pObject = pBezierObject;
            if(pBezierObject->objectLoad(jObj))
                oblist.append(pObject);
        }
        else
        if(shape == "Gauge")
        {
            CMyGaugeObject* pGaugeObject = new CMyGaugeObject(this);
            pObject = pGaugeObject;
            if(pGaugeObject->objectLoad(jObj))
                oblist.append(pObject);
        }
#if 0
        else
        if(shape == trend)
        {
            CMyRectObject* pRectObject = new CMyRectObject(this);
            pObject = (CMyRectObject*)pRectObject;
            if(pRectObject->objectLoad(list))
                oblist.append(pObject);
        }
        else
        if(shape == shapelist)
        {
            CMyButtonObject* pButtonObject = new CMyButtonObject(this);
            pObject = pButtonObject;
            if(pButtonObject->objectLoad(list))
                oblist.append(pObject);
        }
        else
        if(shape == arrow)
        {
            CMyButtonObject* pButtonObject = new CMyButtonObject(this);
            pObject = pButtonObject;
            if(pButtonObject->objectLoad(list))
                oblist.append(pObject);
        }
        else
        if(shape == triangle)
        {
            CMyButtonObject* pButtonObject = new CMyButtonObject(this);
            pObject = pButtonObject;
            if(pButtonObject->objectLoad(list))
                oblist.append(pObject);
        }
        else
        if(shape == symbol)
        {
            CMyButtonObject* pButtonObject = new CMyButtonObject(this);
            pObject = pButtonObject;
            if(pButtonObject->objectLoad(list))
                oblist.append(pObject);
        }
        if(bText == false)
            pTextObject = false;
#endif
        if(jObj["Link"].isNull() == false)
        {
            QJsonArray ar = jObj["Link"].toArray();
            foreach(QJsonValue j, ar)
            {
                QJsonObject jObj = j.toObject();
                pObject->linkLoad(this, jObj, m_user,
                              m_slowBlink, m_normalBlink, m_fastBlink, isButton);
            }
        }
    }
/*
		else
		if(list[0] == "[LINK]")
		{
			if(pObject != NULL)
			{
				pObject->linkLoad(this, list, m_user, m_slowBlink, m_normalBlink, m_fastBlink, isButton);
			}
		}
		else
		if(list[0] == "[NEWLINE]")
		{	if(pTextObject)
				pTextObject->objectLoadNewLine(list);
		}
		else
		if(list[0] == "[END]")
			return;
        Line = stream.readLine();
     }
*/
}

void CSrView::paintEvent(QPaintEvent*)
{
//    if(m_bScreenSave)
//        return;
    QPainter	p(this);
    if(m_bBackImg)
		p.drawImage(0,0,m_BackImage);
	else
		p.fillRect(m_FillRect, m_BackBrush);
	CDrawObject* pObject;
    for (int i = 0; i < layer1List.size(); ++i) {
        pObject = (CDrawObject*)layer1List.at(i);
        if(!pObject->m_bDelete)
            pObject->Draw(p);
    }

    for (int i = 0; i < layer2List.size(); ++i) {
        pObject = (CDrawObject*)layer2List.at(i);
        if(!pObject->m_bDelete)
            pObject->Draw(p);
    }
    if(mbError)
    {
        QFont font;
        font.setPointSize(20);
        QPen		    pen;		// Line Attrib

        pen.setColor(QColor(Qt::red));
        p.setPen(pen);

        p.setFont(font);
        p.drawText(10, 300, msError);
    }
}

void CSrView::slotOpenWindow(const QString& name, int nOpen)
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
	if(nOpen != 1)	//popup이 아니면
	{
		m_bOpening = true;
		m_OpenWinName = name;
		m_nTimerCnt = 5;
		m_bChanged = true;
	}
	else
	{
//		CSrView *pSrView = new CSrView(m_folder, name, this);
        CSrView pSrView(m_folder, name, this);
//		Qt::WindowFlags flags =Qt::Dialog| Qt::FramelessWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint;
//		Qt::WindowFlags flags =Qt::Dialog| Qt::FramelessWindowHint;
        Qt::WindowFlags flags =Qt::Dialog | Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
        pSrView.setWindowFlags(flags);
        pSrView.exec();
        pSrView.m_bDoEnding = true;
        pSrView.reject();
	}
}

void CSrView::slotCloseWindow()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
	if(m_bMain == false)
		reject();
}

void CSrView::slotTMSData()
{
/*	m_bMousePress = false;
	m_nMousePressCount = 0;
	CLogInq dlg("TMS Data", m_wdtID, m_wdtSockFd, this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
	dlg.move(0, 0);
	dlg.resize(XMaxRes-6, YMaxRes-22);
	dlg.exec();
    */
}
void CSrView::slotTMSInfo()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
	CTmsInfoDialog dlg(m_ipAddr, m_nPort, this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
	dlg.move(0, 0);
	dlg.resize(XMaxRes-6, YMaxRes-22);
	dlg.exec();

	if(dlg.m_bChanged == true)
	{

//		CTag* pTag;
//		for(int i = 0; i < SensorNumber; i++)
		{
	//		if(m_TagMap.contains(SensorCode[i]))
			{
//				QString str = SensorCode[i];
		//		pTag = m_TagMap[SensorCode[i]];
//				pTag = m_TagMap[str];
//				if(pTag)
//					pTag->Update();
			}
		}	
		 m_bChanged = true;
	}


}
void CSrView::slotSewer()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
//    CSewerDialog dlg(m_ipAddr, g_HasuPort, this);
    if(gSoftwareModel == "EPS_HASU")
    {
        CEpsLog dlg(this);
        dlg.exec();
    }
    else
    {
        CSewerDialog2 dlg2(this);
        dlg2.exec();
    }
}

void CSrView::slotU_City()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
	CU_CityDialog dlg(m_ipAddr, this);
	dlg.move(0, 0);
	dlg.resize(XMaxRes-6, YMaxRes-22);
	dlg.exec();
}

void CSrView::slotRainSensor()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
	CRainSensorDlg dlg(m_ipAddr, this);
	dlg.move(0, 0);
	dlg.resize(XMaxRes-6, YMaxRes-22);
	dlg.exec();
}
void CSrView::slotCommMon()
{
	m_bMousePress = false;
	m_nMousePressCount = 0;
//    TrendDialog dlg;
//    dlg.exec();

/*    if(g_WindowsRun)
    {
        CComMon2 dlg;
        dlg.exec();
    }
    else
 */   {
        CComMon dlg;
        dlg.exec();
    }
}

void CSrView::slotTrend()
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
    TmsTrendDialog dlg;
    dlg.exec();
}

void CSrView::slotPointMon()
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
    CPointMon dlg(this);
    dlg.exec();
}

void CSrView::slotAlarmSummary()
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
    CAlarmSummary dlg(this);
    dlg.exec();
}

void CSrView::slotLog()
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
    CHistory dlg(this);
    dlg.exec();
}

void CSrView::slotEpsTestSetup()
{
    m_bMousePress = false;
    m_nMousePressCount = 0;
    TestSetup dlg;
    dlg.exec();
}

void CSrView::slotDataChanged()
{
	m_bChanged = true;
}

bool CSrView::InputPassword()
{	
	m_bMousePress = false;
	m_nMousePressCount = 0;

//	CPasswordDlg dlg(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);


//	if(dlg.exec() == QDialog::Accepted)
	{
//		QString password = dlg.GetPassword();
//        if(CheckPassword("tms", password.toLocal8Bit().data()))
//			return true;
		QMessageBox::information(this, tr("TMS"), tr("비밀번호가 올바르지 않습니다."));
	}
	return false;
}

void CSrView::OnBlink(RUNLIST& List, bool& blink)
{
	if(m_bOpening == true)
		return;
	blink = !blink;
	for (int i = 0; i < List.size(); ++i) {
		CRunBlink* pRun = (CRunBlink*)List.at(i);
		if(pRun->SetCheck())
		{
			m_bChanged = true;
			pRun->Exec(blink);
		}
	}
}

void CSrView::makeCommand()
{
    g_pSrSocket->mGraphicInitOk = false;

    QJsonArray argArray;
    QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
    while (i != m_TagMap.constEnd())
    {
        argArray.append(QJsonValue(i.key()));
        i++;
    }
    QJsonObject tmpObj;
    mValueRead = makeCommand(mChangedCmd, QJsonValue(GraphicId));
    tmpObj.insert(QString("Id"), GraphicId);
    tmpObj.insert(QString("TagList"), argArray);
    mInfoRead = makeCommand(mInfoCmd, QJsonValue(tmpObj));
}

QByteArray CSrView::makeCommand(QString cmd, QJsonValue argValue)
{
    QString nid = "0";
    QJsonDocument tmpDoc;
    QJsonObject tmpObject;
    QJsonValue vValue(QJsonValue::Null);

    tmpObject.insert(QString("Command"),QJsonValue(cmd));
    tmpObject.insert(QString("Id"),QJsonValue(nid));
    tmpObject.insert(QString("Args"),argValue);
    tmpDoc.setObject(tmpObject);

    QByteArray data = tmpDoc.toJson(QJsonDocument::Compact);
    data.insert(0, QString("BEST%1").arg(data.size(),6,10,QChar('0')));
    return data;
}

void CSrView::onRead(QString& cmd, QJsonObject& jobject)
{
//    if(m_bScreenSave)
//        return;

    if(g_DebugDisplay)
        qDebug() << "CSrView::onRead : " << cmd;
    QMap<QString, CTag*>::const_iterator i = m_TagMap.constBegin();
    int len = m_TagMap.count();
    int cnt = 0;
    if(cmd == mInfoCmd)
    {
        QString error = jobject["Error"].toString();
        if(error == "")
        {
            mbError = false;
            msError = "";
        }
        else
        {
            mbError = true;
            msError = jobject["Result"].toString();
            repaint();
            return;
        }

        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray ar = ob["Summaries"].toArray();
        if(g_DebugDisplay)
            qDebug() << "Count:" << ar.count();
        foreach (const QJsonValue& val, ar)
        {
            if(cnt >= len)
                break;
            QJsonObject jObj = val.toObject();
            QString str = jObj["TagType"].toString();
            CTag *pTag = i.value();
            QString unit = jObj["Unit"].toString();
            QString Rw = jObj["Rw"].toString();
            pTag->setRw(Rw);
            if(str == "Digital")
            {
                pTag->setTagType(0);
                int pos = unit.indexOf(QChar('/'));
                pTag->m_OnMsg = unit.left(pos);
                pTag->m_OffMsg = unit.mid(pos+1);
            }
            else
            if(str == "Analog")
            {
                pTag->setTagType(1);
                pTag->setUnit(unit);
            }
            else
            if(str == "Pulse")
            {
                pTag->setTagType(2);
                pTag->setUnit(unit);
            }
            else
            if(str == "String")
            {
                pTag->setTagType(3);
            }
            pTag->setDesc(jObj["Desc"].toString());
            str = jObj["Value"].toString();
            if(pTag->getTagType() == 3)  // String Tag
                pTag->setValue(str);
            else
                pTag->setValue(str.toDouble());
            pTag->m_Addr = jObj["Address"].toString();
            pTag->setAlarmStatus((quint16)jObj["AlarmSt"].toDouble());
            pTag->setTagStatus((quint16)jObj["TagSt"].toDouble());
            cnt++;
            i++;
        }
        repaint();
        g_pSrSocket->mGraphicInitOk = true;
        mCommCount = 100;
    }
    else
    if(cmd == mChangedCmd)
    {
        cnt = 0;
        QJsonObject ob = jobject["Result"].toObject();
        QJsonArray ar = ob["ChangedValues"].toArray();
        for(int j = 0; j < ar.size(); j += 4)
        {
            QString name = ar[j].toString();
            CTag *pTag = m_TagMap[name];
            if(pTag)
            {
                QString val = ar[j+1].toString();
                if(pTag->getTagType() == 3) // String Tag
                   pTag->setValue(val);
                else
                   pTag->setValue(val.toDouble());
                quint16 stat = (quint16)ar[j+2].toDouble();
                pTag->setAlarmStatus(stat);
                stat = (quint16)ar[j+3].toDouble();
                pTag->setTagStatus(stat);
            }
            i++;
        }
        repaint();
    }
    else
    if(cmd == "@WriteEvent")
    {
        QJsonObject jObj = jobject["Result"].toObject();
        QString name = jObj["Tag"].toString();
        QString str = jObj["Value"].toString();
        CTag* pTag = FindTagObject(name);
        if(pTag != NULL)
        {
            if(pTag->getTagType() == 3)    //String Tag
                pTag->setValue(str);
            else
                pTag->setValue(str.toDouble());
            repaint();
        }
    }
}

bool CSrView::writeTag(QString tagname, QString value, int ndelay)
{
    QJsonObject jObj;
    jObj.insert(QString("Tag"),QJsonValue(tagname));
    jObj.insert(QString("Value"),QJsonValue(value));

    QJsonArray array;
    array.append(jObj);
    QString cmd("Tag_Write");
    QByteArray ar = makeCommand(cmd, QJsonValue(array));
    if(ndelay == 0)
        g_pSrSocket->sendCommand((QWidget*)this, cmd, QString(""), ar);
    else
    {
        TAG_WRITE_DELAY *delay = new TAG_WRITE_DELAY;
        delay->Cmd = cmd;
        delay->Data = ar;
        delay->Delay = ndelay;
        m_listDelay.append(delay);
    }
    return true;
}

void CSrView::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/qss/" + sheetName/*.toLower()*/ + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());

//    ui.styleTextEdit->setPlainText(styleSheet);
    qApp->setStyleSheet(styleSheet);
//    ui.applyButton->setEnabled(false);
}

void CSrView::handleFileChanged(const QString&)
{
    if(QDir("/sun/web/tmp/usbdisk").exists())
    {
        if(m_bUsbReady == false)
        {
            CInfomationDialog dlg("USB 메모리가 준비되었습니다");
            dlg.exec();
        }
        m_bUsbReady = true;
    }
    else
        m_bUsbReady = false;
}

void CSrView::handleFileChanged2(const QString&)
{
    if(QFile("/tmp/__MOUSECURSOR_ON__").exists())
        gApp->setOverrideCursor(Qt::PointingHandCursor);
    else
        gApp->setOverrideCursor(Qt::BlankCursor);
}
