#include "runani.h"
#include "button.h"
#include "QJsonObject"

void appendRunList(QString& TagName, CRunObject *pRun);
void removeRunList(QString& TagName, CSrView* pView);

CfgFill::CfgFill()
{
	sName		  = "";
	nPosition	  = 1;		// bottom
	nDisplayValue = 0;		// None
	nVMax		  =	100;
	nVMin		  =	0;
	nPMax		  =	100;
	nPMin         = 0;
}

void CfgFill::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    nPosition		= (int)jObj["Position"].toDouble();
    nDisplayValue	= (int)jObj["DisplayValue"].toDouble();
    nVMax			= (int)jObj["VMax"].toDouble();
    nVMin			= (int)jObj["VMin"].toDouble();
    nPMax			= (int)jObj["PMax"].toDouble();
    nPMin			= (int)jObj["PMin"].toDouble();
	int c, r, g, b;
    c = (int)jObj["cBack"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cBack.setRgb(r, g, b); 

    c = (int)jObj["cFill"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cFill.setRgb(r, g, b); 

    c = (int)jObj["cText"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cText.setRgb(r, g, b); 
}

/*-----------------------------------------------------------------*/

CfgMove::CfgMove()
{
	sName		= "";				
	nTouch		= 0;
	nLocate     = 0;
	nVMax		= 100;				// LEFT, TOP
	nVMin		= 0;
	nPMin		= 0;				// RIGHT, BOTTOM
	nPMax		= 100;
	nCodition	= -1;

	m_bIndirect	  = false;
}

CfgMove::~CfgMove()
{
}

void CfgMove::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nType			= (int)jObj["MoveType"].toDouble();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    nTouch			= (int)jObj["Touch"].toDouble();
    nLocate			= (int)jObj["Locate"].toDouble();
    nVMax			= (int)jObj["VMax"].toDouble();
    nVMin			= (int)jObj["VMin"].toDouble();
    nPMax			= (int)jObj["PMax"].toDouble();
    nPMin			= (int)jObj["PMin"].toDouble();
    nCodition		= (int)jObj["Codition"].toDouble();
}

CfgSize::CfgSize()
{
	sName	 = "";
	nVMax	 = 100;					// 값 
	nVMin	 =   0;
	nPMin	 =   0;					// 퍼센트 
	nPMax	 = 100;
	nDefault =   0;               // 0,1,2 (Size Default) 
	m_bIndirect	  = false;
}

CfgSize::~CfgSize()
{
}

void CfgSize::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nType			= (int)jObj["SizeType"].toDouble();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    nVMax			= (int)jObj["VMax"].toDouble();
    nVMin			= (int)jObj["VMin"].toDouble();
    nPMax			= (int)jObj["PMax"].toDouble();
    nPMin			= (int)jObj["PMin"].toDouble();
    nDefault		= (int)jObj["Default"].toDouble();
}
/*-----------------------------------------------------------------*/

CfgRotate::CfgRotate()
{
	sName		= "";
	nVCCW		= 100;
	nVCW		= 0;
	nRCCW		= 360;
	nRCW		= 0;
}

CfgRotate::~CfgRotate()
{
}

void CfgRotate::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    nVCCW			= (int)jObj["VCCW"].toDouble();
    nVCW			= (int)jObj["VCW"].toDouble();
    nRCCW			= (int)jObj["RCCW"].toDouble();
    nRCW			= (int)jObj["RCW"].toDouble();
    pt.setX((int)jObj["X"].toDouble());
    pt.setY((int)jObj["Y"].toDouble());
}
/*-----------------------------------------------------------------*/

CfgColor::CfgColor()
{
	sName		= "";
	btype		= 0;
	bcheck		= 0;
	nAlarm		= 0;
	
	vhihi_hi	= 0;
	vhi_normal  = 0;
	vnormal_lo  = 0;
	vlo_lolo    = 0;
}

CfgColor::~CfgColor()
{
}

void CfgColor::linkLoad(QJsonObject& jObj)
{
    m_nType         = (int)jObj["ColorType"].toDouble();
    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    btype			= (int)jObj["bType"].toDouble();
    bcheck			= (int)jObj["Condition"].toDouble();
    nAlarm			= (int)jObj["Alarm"].toDouble();
    bValue			= (int)jObj["Value"].toDouble();
    bDeviation		= (int)jObj["Deviation"].toDouble();
    bROC			= (int)jObj["ROC"].toDouble();
    vhihi_hi		= jObj["HIHI_HI"].toDouble();
    vhi_normal		= jObj["HI_NOR"].toDouble();
    vnormal_lo		= jObj["Normal_Lo"].toDouble();
    vlo_lolo		= jObj["Lo_Lo"].toDouble();

	int c, r, g, b;
    c = (int)jObj["cHiHi"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cHiHi.setRgb(r, g, b); 

    c = (int)jObj["cHi"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cHi.setRgb(r, g, b); 

    c = (int)jObj["cNormal"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cNormal.setRgb(r, g, b);

    c = (int)jObj["cLo"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cLo.setRgb(r, g, b);

    c = (int)jObj["cLoLo"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cLoLo.setRgb(r, g, b);

    c = (int)jObj["cOn"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cOn.setRgb(r, g, b);

    c = (int)jObj["cOff"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cOff.setRgb(r, g, b);
}
/*-----------------------------------------------------------------*/

CfgBlink::CfgBlink()
{
	sName		= "";
	btype		= 0;
	bcheck		= 0;
	nspeed		= 0;

	vhihi_hi	= 0;
	vhi_normal  = 0;
	vnormal_lo  = 0;
	vlo_lolo    = 0;

	cHiHi		= false;
	cHi			= false;	
	cNormal		= false;
	cLo			= false;
	cLoLo		= false;

	cOn			= false;
	cOff		= false;
	cAlarm		= false;
	m_bIndirect	  = false;
}

void CfgBlink::linkLoad(QJsonObject& jObj)
{
	int c, r, g, b;

    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    btype			= (int)jObj["bType"].toDouble();
    bcheck			= (int)jObj["Check"].toDouble();
    nspeed			= (int)jObj["Speed"].toDouble();

    c = (int)jObj["cFill"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cFill.setRgb(r, g, b); 

    c = (int)jObj["cLine"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cLine.setRgb(r, g, b); 

    c = (int)jObj["cText"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	cText.setRgb(r, g, b); 

    vhihi_hi		= jObj["HIHI_HI"].toDouble();
    vhi_normal		= jObj["HI_NOR"].toDouble();
    vnormal_lo		= jObj["Normal_Lo"].toDouble();
    vlo_lolo		= jObj["Lo_Lo"].toDouble();
    cHiHi			= (int)jObj["cHiHi"].toDouble();
    cHi				= (int)jObj["cHi"].toDouble();
    cNormal			= (int)jObj["cNormal"].toDouble();
    cLo				= (int)jObj["cLo"].toDouble();
    cLoLo			= (int)jObj["cLoLo"].toDouble();
    cOn				= (int)jObj["cOn"].toDouble();
    cOff			= (int)jObj["cOff"].toDouble();
    cAlarm			= (int)jObj["cAlarm"].toDouble();

}

/*-----------------------------------------------------------------*/

CfgHide::CfgHide()
{
	sName		= "";
	
	vhihi_hi	= 0;
	vhi_normal  = 0;
	vnormal_lo  = 0;
	vlo_lolo    = 0;

	btype		= false;  //digital,analog

	bHiHi       = false;
	bHi         = false;
	bNormal     = false;
	bLo         = false;
	bLoLo       = false;
	
	nOn         = 0;
	m_bIndirect	  = false;
}

void CfgHide::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    vhihi_hi		= jObj["HIHI_HI"].toDouble();
    vhi_normal		= jObj["HI_NOR"].toDouble();
    vnormal_lo		= jObj["Normal_Lo"].toDouble();
    vlo_lolo		= jObj["Lo_Lo"].toDouble();
    btype			= (int)jObj["bType"].toDouble();
    bHiHi			= (int)jObj["HiHi"].toDouble();
    bHi				= (int)jObj["Hi"].toDouble();
    bNormal			= (int)jObj["Normal"].toDouble();
    bLo				= (int)jObj["Lo"].toDouble();
    bLoLo			= (int)jObj["LoLo"].toDouble();
    nOn				= (int)jObj["On"].toDouble();
}

/*-----------------------------------------------------------------*/

CfgText::CfgText()
{
	sName		= "";
	sFormat		= "%3d";

	btype	= 0;
	bZero	= 0;
	sOnMsg	= "ON";
	sOffMsg	= "OFF";

	bUnit	= true;
	m_bIndirect	  = false;

	bComma = false;
}

void CfgText::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    sFormat			= jObj["Format"].toString();
    btype			= (int)jObj["bType"].toDouble();
    bZero			= (int)jObj["bZero"].toDouble();
    sOnMsg			= jObj["OnMsg"].toString();
    sOffMsg			= jObj["OffMsg"].toString();
    bUnit			= (int)jObj["bUnit"].toDouble();
    bComma			= (int)jObj["bComma"].toDouble();
}

/////////////////////////////////////////////////////////////

CfgData::CfgData()
{
	sName		= "";
	sName2      = "";

	HotKey		= "";
	nSecurity	= 0;
	
	ntype		= 0;     // (Digital, Analog,SCRIPT)
	nDigital	= 2;     // 0 -6 (7 type)
	nAnalog		= 0;     // 0 -4 (5 type)
	
	nValue		= 0.0f;
	nTime		= 0;

	sMessage    = "";
	sScript		= "";

	bConfrm     = false;
	m_bIndirect	  = false;
	m_bIndirect2	  = false;
}

void CfgData::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
    sName2			= jObj["Name2"].toString();
    m_nTagType		= (int)jObj["TagType"].toDouble();
    ntype			= (int)jObj["DataType"].toDouble();
    nDigital		= (int)jObj["Digital"].toDouble();
    nAnalog			= (int)jObj["Analog"].toDouble();
    sValue			= jObj["Value"].toString();
    nValue          = sValue.toDouble();
    nTime			= (int)jObj["nTime"].toDouble();
    sMessage		= jObj["Message"].toString();
    sScript			= jObj["Script"].toString();
    bConfrm			= (int)jObj["bConfrm"].toDouble();
}

Cfgfile::Cfgfile()
{
	nOpen		= 0;
	bClose		= false;
	tOpen		= 0;
	sOpen		= "";
	tClose		= 1;				// 0: Function Call 기능이 없는 버전, 1: Function Call 기능
									// (2003.07.23)부터	0 -> 1변경
	sClose		= "";
	bCurrent_Close = false;
	m_bIndirect = false;

	// Function Call 기능을 위한 변수 선언 초기화(2003.07.23)
	nOption		= 0;

	// 윈도우 이름 재정의를 위한 변수 선언(2005. 02. 18)
	bUseTitle			= false;
}

Cfgfile::~Cfgfile()
{
}

void Cfgfile::linkLoad(QJsonObject& jObj)
{
//[LINK],FILE,1,0,popup,0,1,0,0,Moudule.Func(Topic, ...),0,,

    nOpen			= (int)jObj["bOpen"].toDouble();
    bClose			= (int)jObj["bClose"].toDouble();
    sOpen			= jObj["sOpen"].toString();
    tOpen			= (int)jObj["tOpen"].toDouble();
    tClose			= (int)jObj["tClose"].toDouble();
    bCurrent_Close	= (int)jObj["CurrentClose"].toDouble();
    nOption			= (int)jObj["nOption"].toDouble();
    strFuncName		= jObj["FuncName"].toString();
    bUseTitle		= (int)jObj["TagType"].toDouble();
    strTagName		= jObj["TagName"].toString();
    strStringValue	= jObj["StringValue"].toString();
}

CfgFrame::CfgFrame()
{
	sName		= "";
	sFrameName  = "";
	btype       = false;
	nOn         = false;
	nTime		= 500;
	m_bIndirect	= false;
}

CfgFrame::~CfgFrame()
{
}

void CfgFrame::linkLoad(QJsonObject& )
{
//	int cnt = list.size();
//	cnt = 2;
}

CfgPipeFlow::CfgPipeFlow()
{
	sName		= "";
	m_nDirection  = 0;
	m_nSpeed      = 0;
	m_bSwap		  = false;
	m_nArray	  = 0;
	m_nThick	  = 4;
	m_bBackground = false;
}

CfgPipeFlow::~CfgPipeFlow()
{
}

void CfgPipeFlow::linkLoad(QJsonObject& )
{
//	int cnt = list.size();
//	cnt = 2;
}

CfgToolTip::CfgToolTip()
{
	sToolTip = "";
}

void CfgToolTip::linkLoad(QJsonObject& )
{
//	int cnt = list.size();
//	cnt = 2;
}

CfgGauge::CfgGauge()
{
    sName		= "";
    sFormat		= "%3d";

    btype	= 0;
    bZero	= 0;
    sOnMsg	= "ON";
    sOffMsg	= "OFF";

    bUnit	= true;
    m_bIndirect	  = false;

    bComma = false;
}

void CfgGauge::linkLoad(QJsonObject& jObj)
{
    sName			= jObj["Name"].toString();
/*    m_nTagType		= (int)jObj["TagType"].toDouble();
    sFormat			= jObj["Format"].toString();
    btype			= (int)jObj["bType"].toDouble();
    bZero			= (int)jObj["bZero"].toDouble();
    sOnMsg			= jObj["OnMsg"].toString();
    sOffMsg			= jObj["OffMsg"].toString();
    bUnit			= (int)jObj["bUnit"].toDouble();
    bComma			= (int)jObj["bComma"].toDouble();
*/
}

CDrawObject::CDrawObject(CSrView* pSrView) //: QObject(pSrView)
{
    m_pSrView = pSrView;
	m_bDelete = false;
	m_bText = false;
}

CDrawObject::~CDrawObject() 
{
	while (!m_LinkList.isEmpty())
	{
		CfgObject* pObj =  (CfgObject*)m_LinkList.takeFirst();
        removeRunList(pObj->sName, m_pSrView);
		delete pObj;
	}
}

bool CDrawObject::objectLoad(QJsonObject jObj)
{
    m_drawshape				= jObj["Shape"].toString();
    m_sName 				= jObj["Name"].toString();
    m_3dtype1				= (int)jObj["3dType1"].toDouble();
    m_3dtype2				= (int)jObj["3dType2"].toDouble();
    m_position.setLeft((int)jObj["Left"].toDouble());
    m_position.setTop((int)jObj["Top"].toDouble());
    m_position.setRight((int)jObj["Right"].toDouble());
    m_position.setBottom((int)jObj["Bottom"].toDouble());
	m_position = m_position.normalized();
    m_nBkMode				= (int)jObj["BkMode"].toDouble();

    m_logpen.setStyle((Qt::PenStyle)((int)jObj["PenStyle"].toDouble()));
    m_logpen.setWidth((int)jObj["PenWidthX"].toDouble()); //    m_logpen.lopnWidth.x

	int c, r, g, b;
	QColor color;
    c = (int)jObj["PenColor"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;

	color.setRgb(r, g, b); 
	m_logpen.setColor(color);
	m_logpen.setJoinStyle(Qt::RoundJoin);
	m_logpen.setCapStyle(Qt::RoundCap);

    m_logbrush.setStyle((Qt::BrushStyle)((int)jObj["BStyle"].toDouble()));

    c = (int)jObj["BColor"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	color.setRgb(r, g, b); 
	m_logbrush.setColor(color);
	return true;
}

void CDrawObject::addRunList(QString& Name, CRunObject* pRun, int nTagType)
{
	if(nTagType < EXPRESSION)
	{
		appendRunList(Name, pRun);
	}
	else
	{
		CParser* pParser = new CParser(Name);
        pParser->m_pView = m_pSrView;
		if(pParser->Validate(pRun))
		{
			pParser->AddTagChain(pRun);
			pParser->Exec();
		}
		else
		{	delete pRun;
			delete pParser;
		}
	}
}

bool CDrawObject::linkLoad(QWidget *parent, QJsonObject& jObj, RUNLIST& runList, RUNLIST& slowList, RUNLIST& normalList, RUNLIST& fastList, bool isButton)
{
    QString sType = jObj["Type"].toString();
    if(sType == "COLOR")
	{
		CfgColor* pObj = new CfgColor;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		if(pObj->m_nType == 0)
		{
			if(pObj->btype)
			{
				CRunFColorA*	pRun = new CRunFColorA(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
			else
			{
				CRunFColorD*	pRun = new CRunFColorD(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
		}
		else
		if(pObj->m_nType == 1)
		{
			if(pObj->btype)
			{
				CRunLColorA*	pRun = new CRunLColorA(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
			else
			{
				CRunLColorD*	pRun = new CRunLColorD(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
		}
		else
		if(pObj->m_nType == 2)
		{
			if(pObj->btype)
			{
				CRunTColorA*	pRun = new CRunTColorA(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
			else
			{
				CRunTColorD*	pRun = new CRunTColorD(parent, this, pObj);
				addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			}
		}
	}
	else
    if(sType == "MOVE")
	{
		CfgMove* pObj = new CfgMove;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		if(pObj->m_nType == 0)
		{
			CRunMoveAH*	pRun = new CRunMoveAH(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
		else
		{
			CRunMoveAV*	pRun = new CRunMoveAV(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
	}
	else
    if(sType == "SIZE")
	{
		CfgSize* pObj = new CfgSize;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);

		if(pObj->m_nType == 0)
		{
			CRunSizeAH*	pRun = new CRunSizeAH(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
		else
		{
			CRunSizeAV*	pRun = new CRunSizeAV(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
	}
	else
    if(sType == "ROTATE")
	{
		CfgRotate* pObj = new CfgRotate;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		CRunRotate*	pRun = new CRunRotate(parent, this, pObj);
		addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
	}
	else
    if(sType == "FILL")
	{
	}
	else
    if(sType == "TEXT")
	{
		CfgText* pObj = new CfgText;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		CRunText*	pRun = new CRunText(parent, this, pObj);
		addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
	}
	else
    if(sType == "HIDE")
	{
		CfgHide* pObj = new CfgHide;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);

		if(pObj->btype)
		{
			CRunHideA*	pRun = new CRunHideA(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
		else
		{
			CRunHideD*	pRun = new CRunHideD(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		}
	}
	else
    if(sType == "BLINK")
	{
		CfgBlink* pObj = new CfgBlink;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		CRunBlink*	pRun = new CRunBlink(parent, this, pObj);
		addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
		if(pObj->nspeed == 0)
			slowList.append(pRun);
		else
		if(pObj->nspeed == 1)
			normalList.append(pRun);
		else
		if(pObj->nspeed == 2)
			fastList.append(pRun);
	}
	else
    if(sType == "FILE")
	{
		Cfgfile* pObj = new Cfgfile;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);
		CRunOpen*	pRun = new CRunOpen(parent, this, pObj);
		runList.append(pRun);

		if(isButton)
		{
			CMyButtonObject* pButton = static_cast<CMyButtonObject *>(this);
			pRun->m_pButton = pButton->button;
		}
    }
	else
    if(sType == "DATA")
	{
		CfgData* pObj = new CfgData;
        pObj->linkLoad(jObj);
		m_LinkList.append(pObj);

		QString name = objectName();
		if(pObj->ntype == 0)	// Digital
		{
			CRunUIDataD*	pRun = new CRunUIDataD(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
            if(pObj->sName2 != "")
                addRunList(pObj->sName2, (CRunObject*)pRun, pObj->m_nTagType);
            runList.append(pRun);
			if(isButton)
			{
				CMyButtonObject* pButton = static_cast<CMyButtonObject *>(this);
				pRun->m_pButton = pButton->button;
			}
		}
		else
        if(pObj->ntype == 1)	// Analog
        {
			CRunUIDataA*	pRun = new CRunUIDataA(parent, this, pObj);
			addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
			runList.append(pRun);
			if(isButton)
			{
				CMyButtonObject* pButton = static_cast<CMyButtonObject *>(this);
				pRun->m_pButton = pButton->button;
			}
		}
        else
        if(pObj->ntype == 2)	// String
        {
            CRunUIDataS*	pRun = new CRunUIDataS(parent, this, pObj);
            addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
            runList.append(pRun);
            if(isButton)
            {
                CMyButtonObject* pButton = static_cast<CMyButtonObject *>(this);
                pRun->m_pButton = pButton->button;
            }
        }
    }
	else
    if(sType == "FRAME")
	{
	}
	else
    if(sType == "PIPEFLOW")
	{
	}
    else
    if(sType == "GAUGE")
    {
        CfgGauge* pObj = new CfgGauge;
        pObj->linkLoad(jObj);
        m_LinkList.append(pObj);
        CRunGauge*	pRun = new CRunGauge(parent, this, pObj);
        addRunList(pObj->sName, (CRunObject*)pRun, pObj->m_nTagType);
    }
    return true;
}

void CDrawObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawRect(m_position);
}

bool CDrawObject::HitTest(QPoint& point)
{
	if(m_position.contains(point))
		return true;
	return false;
}

CMyRectObject::CMyRectObject(CSrView* pSrView): CDrawObject(pSrView)
{
}

bool CMyRectObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);

	m_position.adjust(0, 0, -1, -1);
	m_position.adjust(0, 0, -1, -1);

	return true;
}

void CMyRectObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawRect(m_position);

}

CMyRoundRectObject::CMyRoundRectObject(CSrView* pSrView):CDrawObject(pSrView)
{
    m_pSrView = pSrView;
}

bool CMyRoundRectObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);
    m_corner.setX((int)jObj["CornerX"].toDouble());
    m_corner.setY((int)jObj["CornerY"].toDouble());

	m_position.adjust(0, 0, -1, -1);
	m_position.adjust(0, 0, -1, -1);

	return true;
}

void CMyRoundRectObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawRoundRect(m_position, m_corner.x(), m_corner.y());

}

CMyEllipseObject::CMyEllipseObject(CSrView* pSrView):CDrawObject(pSrView)
{
	
}

bool CMyEllipseObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);
	return true;
}

void CMyEllipseObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawEllipse(m_position);
}

CMyTextObject::CMyTextObject(CSrView* pSrView):CDrawObject(pSrView)
{
	m_bText = true;
}

bool CMyTextObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);
    m_Text			= jObj["Text"].toString();

    int c, r, g, b;
    QColor color;
    c = (int)jObj["TextColor"].toDouble();
    r = c & 0xff;
    g = (c & 0xff00) >> 8;
    b = (c & 0xff0000) >> 16;

    color.setRgb(r, g, b);
    m_logpen.setColor(color);

    m_dwEditStyle = (int)jObj["Style"].toDouble();
    wFormat =  (int)jObj["Format"].toDouble();
    int PointSize = (int)jObj["PointSize"].toDouble();
    int Weight = (int)jObj["Weight"].toDouble();
    int Italic = (int)jObj["Italic"].toDouble();
    int Underline = (int)jObj["Underline"].toDouble();
    int StrikeOut = (int)jObj["StrikeOut"].toDouble();

    m_nPointSize = PointSize;

    QString Family = jObj["FaceName"].toString();

//	m_TextFont.setFamily(Family);

    m_TextFont.setWeight(QFont::Normal);
    if(Weight == 25)
        m_TextFont.setWeight(QFont::Light);
    else
    if(Weight == 50)
        m_TextFont.setWeight(QFont::Normal);
    else
    if(Weight == 63)
        m_TextFont.setWeight(QFont::DemiBold);
    else
    if(Weight == 75)
        m_TextFont.setBold(true);
    else
    if(Weight == 87)
        m_TextFont.setWeight(QFont::Black);
    if(Italic)
        m_TextFont.setItalic(true);
    if(Underline)
        m_TextFont.setUnderline(true);
    if(StrikeOut)
        m_TextFont.setStrikeOut(true);

    m_TextFont.setPointSize(m_nPointSize);

    int flag = wFormat & 0xf;

    if(flag == 0)
        m_Align = Qt::AlignLeft;
    else
    if(flag == 1)
        m_Align = Qt::AlignCenter;
    else
        m_Align = Qt::AlignRight;
    m_Align |= Qt::AlignTop | Qt::TextWordWrap; //| Qt::Text  ::TextSingleLine;

    return true;
}

void CMyTextObject::objectLoadCommon(QStringList& , int )
{
}

void CMyTextObject::objectLoadNewLine(QStringList& list)
{
	m_Text			+= "\n";
	m_Text			+= list[1];
	if(list.size() > 2)
	{
		objectLoadCommon(list, 2);
	}

}

void CMyTextObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);

	p.setFont(m_TextFont);
//	int y = m_position.top()+m_nPointSize+1;
//	p.drawText(m_position.left(),y, m_Text);

    QTextCodec *codec = QTextCodec::codecForName("eucKR");
    QString str = codec->toUnicode(m_Text.toLocal8Bit());

//    QString str(encodedString);
//    QByteArray encodedString = codec->toUnicode(m_Text.toLocal8Bit());
//    QString str(encodedString);
    p.drawText(m_position,m_Align, m_Text);
}

CMyButtonObject::CMyButtonObject(CSrView* pSrView):CDrawObject(pSrView)
{
}

CMyButtonObject::~CMyButtonObject()
{
    button->removeEventFilter(m_pSrView);
	delete button;
}

bool CMyButtonObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);

    m_Text = jObj["Text"].toString();
    m_wType = (int)jObj["Type"].toDouble();
    m_sActOn = jObj["ActOn"].toString();
    m_sActOff = jObj["ActOff"].toString();
    m_bOnOff = (int)jObj["OnOff"].toDouble();
    m_bDown = (int)jObj["Down"].toDouble();
    m_dwEditStyle = (int)jObj["EditStyle"].toDouble();

	int c, r, g, b;
    c = (int)jObj["TextColor"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;

	m_TextColor.setRgb(r, g, b); 
	m_logpen.setColor(m_TextColor);

    m_nPointSize = (int)jObj["PointSize"].toDouble();
    int Weight = (int)jObj["Weight"].toDouble();
    int Italic = (int)jObj["Italic"].toDouble();
    int Underline = (int)jObj["Underline"].toDouble();
    int StrikeOut = (int)jObj["StrikeOut"].toDouble();

//	QString Family = (int)jObj["TextColor"].toDouble();

//	m_TextFont.setFamily(Family);

	m_TextFont.setWeight(Weight);
	if(Italic)
		m_TextFont.setItalic(true);
	if(Underline)
		m_TextFont.setUnderline(true);
	if(StrikeOut)
		m_TextFont.setStrikeOut(true);

    m_TextFont.setPointSize(m_nPointSize);
	
    button = new Button2(m_Text, m_TextColor, m_pSrView);
	button->setGeometry(m_position);
    button->setVisible(true);
	return true;
}

CMyBitmapObject::CMyBitmapObject(CSrView* pSrView):CDrawObject(pSrView)
{
	
}

bool CMyBitmapObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);

    m_bSrc = jObj["Src"].toDouble();

	int c, r, g, b;
	QColor color;
    c = jObj["TrabsColor"].toDouble();
	r = c & 0xff;
	g = (c & 0xff00) >> 8;
	b = (c & 0xff0000) >> 16;
	m_trans_color.setRgb(r, g, b); 

	QString name;
    name += m_pSrView->m_folder;
	name += "/windows/images/";
    name += m_pSrView->m_WinName;
	name += "_";
	name += m_sName;
	name += ".bmp";


	if(m_Image.load(name) == false)
        qDebug("Bitmap Load Fail Name(%s)", name.toLocal8Bit().data());
	return true;
}

void CMyBitmapObject::Draw(QPainter& p)
{
//	QBrush brush(m_trans_color);
//	p.setBackground(brush);
//	p.setBackgroundMode (Qt::TransparentMode);
//	p.drawImage(m_position,m_Image);

	if(m_bSrc == false)
	{
		QBitmap theMask = m_Image.createMaskFromColor(m_trans_color);
		m_Image.setMask(theMask);
	}
	p.drawPixmap(m_position,m_Image);
}

CMyLineObject::CMyLineObject(CSrView* pSrView):CDrawObject(pSrView)
{
	
}

bool CMyLineObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);
	return true;
}

void CMyLineObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawLine(m_position.left(), m_position.top(), m_position.right(), m_position.bottom());
}

//---------------------------------------------------------------------------//
// CMyPolyObject
//---------------------------------------------------------------------------//

CMyPolyObject::CMyPolyObject(CSrView* pSrView) : CDrawObject(pSrView)
{
        m_points        = NULL;
        m_nCount        = 0;
        m_nAllocPoints = 0;
        m_symbreak      = false;
        m_size          = m_position.size();
		m_rectmode      = false;
}

CMyPolyObject::~CMyPolyObject()
{
    if (m_points != NULL)
                delete m_points;
}

bool CMyPolyObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);
    m_nCount = (int)jObj["Count"].toDouble();
    if(m_nCount <= 0)
        return false;
	m_points = new QPoint[m_nCount];
	QPolygon polygon(m_nCount);
	m_Polygon = polygon;
    QJsonArray ar = jObj["Points"].toArray();
    for(int i = 0; i < m_nCount; i++)
	{
        m_points[i].setX((int)ar[i*2].toDouble());
        m_points[i].setY((int)ar[i*2+1].toDouble());
		m_Polygon.setPoint(i, m_points[i]);
	}
	return true;
}

void CMyPolyObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawPolyline(m_points, m_nCount);
}


CMyPolygonObject::CMyPolygonObject(CSrView* pSrView) : CMyPolyObject(pSrView)
{
}

CMyPolygonObject::~CMyPolygonObject()
{
}

void CMyPolygonObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawPolygon(m_points, m_nCount);
}

CMyPieObject::CMyPieObject(CSrView* pSrView) : CMyPolyObject(pSrView)
{
}

CMyPieObject::~CMyPieObject()
{
}

void CMyPieObject::Draw(QPainter& p)
{
//        QRect           r = m_position;
		if (m_nCount > 2)
		{
			p.setPen(m_logpen);
			p.setBrush(m_logbrush);
//			p.drawPie(m_position, );
//	        p.drawPie(r,n_points[0],n_points[2]);
		}
}

CMyArcObject::CMyArcObject(CSrView* pSrView):CMyPieObject(pSrView)
{
}

CMyArcObject::~CMyArcObject()
{
}

void CMyArcObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawPolygon(m_points, m_nCount);
}

CMyChordObject::CMyChordObject(CSrView* pSrView):CMyPieObject(pSrView)
{
}

CMyChordObject::~CMyChordObject()
{
}

void CMyChordObject::Draw(QPainter& p)
{
	p.setPen(m_logpen);
	p.setBrush(m_logbrush);
	p.drawPolygon(m_points, m_nCount);
}

#define	Y_Ge_Fx			0
#define	Y_Le_Fx			1
#define X_Ge_PointX		2
#define X_Le_PointX		3

CMyBezierObject::CMyBezierObject(CSrView* pSrView):CMyPolyObject(pSrView)
{
	m_count = 0;
}

CMyBezierObject::~CMyBezierObject()
{
}

void CMyBezierObject::Draw(QPainter& p)
{
	QPainterPath path;
	path.moveTo(m_Polygon.at(0));
	path.cubicTo(m_Polygon.at(1),
					 m_Polygon.at(2),
					 m_Polygon.at(3));	
	p.strokePath(path, m_logpen);
}

CMyGaugeObject::CMyGaugeObject(CSrView* pSrView):CDrawObject(pSrView)
{
    mSpeedGauge = new QcGaugeWidget(pSrView);
}

CMyGaugeObject::~CMyGaugeObject()
{
}

bool CMyGaugeObject::objectLoad(QJsonObject jObj)
{
    CDrawObject::objectLoad(jObj);

    m_nType = jObj["Type"].toDouble();
    mSpeedGauge->addArc(55);
    if(m_nType == 1)
    {
        mSpeedGauge->addDegrees(65)->setValueRange(0,100);
        QcColorBand *clrBand = mSpeedGauge->addColorBand(50);
        clrBand->setValueRange(0,100);
        mSpeedGauge->addValues(80)->setValueRange(0,100);
        mSpeedGauge->addLabel(70)->setText("Km/h");
        QcLabelItem *lab = mSpeedGauge->addLabel(40);
        lab->setText("0");
        mSpeedNeedle = mSpeedGauge->addNeedle(60);
        mSpeedNeedle->setLabel(lab);
        mSpeedNeedle->setColor(Qt::blue);
        mSpeedNeedle->setValueRange(0,100);
        mSpeedGauge->addBackground(7);
    }
    else
    {
        mSpeedGauge->addBackground(99);
        QcBackgroundItem *bkg1 = mSpeedGauge->addBackground(92);
        bkg1->clearrColors();
        bkg1->addColor(0.1,Qt::black);
        bkg1->addColor(1.0,Qt::white);

        QcBackgroundItem *bkg2 = mSpeedGauge->addBackground(88);
        bkg2->clearrColors();
        bkg2->addColor(0.1,Qt::gray);
        bkg2->addColor(1.0,Qt::darkGray);

        mSpeedGauge->addDegrees(65)->setValueRange(0,80);
        mSpeedGauge->addColorBand(50);

        mSpeedGauge->addValues(80)->setValueRange(0,80);

        mSpeedGauge->addLabel(70)->setText("Km/h");
        QcLabelItem *lab = mSpeedGauge->addLabel(40);
        lab->setText("0");
        mSpeedNeedle = mSpeedGauge->addNeedle(60);
        mSpeedNeedle->setLabel(lab);
        mSpeedNeedle->setColor(Qt::white);
        mSpeedNeedle->setValueRange(0,80);
        mSpeedGauge->addBackground(7);
        mSpeedGauge->addGlass(88);
    }

    mSpeedGauge->setGeometry(m_position);
    mSpeedGauge->setVisible(true);
    mSpeedNeedle->setCurrentValue(75);
    return true;
}

void CMyGaugeObject::Draw(QPainter& )
{
    /*
    QPainterPath path;
    path.moveTo(m_Polygon.at(0));
    path.cubicTo(m_Polygon.at(1),
                     m_Polygon.at(2),
                     m_Polygon.at(3));
    p.strokePath(path, m_logpen);*/
}

void CMyGaugeObject::setCurrentValue(double value)
{
    mSpeedNeedle->setCurrentValue(value);
}
