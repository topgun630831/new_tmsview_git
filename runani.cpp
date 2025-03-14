#include <QDialog>
#include "math.h"
#include "runani.h"
#include "tag.h"
#include "digitalset.h"
#include "analogset.h"
#include "stringset.h"
#include "questiondialog.h"
#include "infomationdialog.h"

CTag* FindTagObject(QString& TagName);

CRunObject::CRunObject()
{
	m_pObj = NULL;
	m_nButton = false;
	m_bWhileOn = false;
	m_bNonAni = false;
	m_pButton = NULL;
}

CRunObject::CRunObject(QWidget *parent, CDrawObject* pObj)
{
	m_pParent = parent;
	m_bWhileOn = false;
	m_pObj = pObj; 
	m_nButton = NONE;
	bLock = false;
	m_bNonAni = false;
	lpoint = QPoint(0,0);
	connect(this, SIGNAL(sigDataChanged()), parent, SLOT(slotDataChanged()));
}

CRunObject::~CRunObject() {

}

void CRunObject::Exec(CTag* pTag)
{
	pTag = pTag;
}

void CRunObject::sendSignal()
{
    CSrView* pView = (CSrView*)m_pParent;
	pView->m_bChanged = true;
//	emit sigDataChanged();
}

void CRunObject::Exec(QPoint& point)
{
	point = point;
}

void CRunObject::Exec(double m_Value)
{
	m_Value = m_Value;
}

///////////////////////////////////////
////-- CRunColor    ///////////////////////////
///////////////////////////////////////

CRunFColorD::CRunFColorD(){}
CRunFColorD::CRunFColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunObject(parent,pObj)
{
	m_pCfgColor = pCfgColor;
}
CRunFColorD::~CRunFColorD() {}

void CRunFColorD::Exec(double m_Value)
{
	QColor color;
	if(m_Value == 0)
		color = m_pCfgColor->cOff;
	else
		color = m_pCfgColor->cOn;
	m_pObj->m_logbrush.setColor(color);

}

void CRunFColorD::Exec(CTag* pTag)
{
	QColor color;

	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value == 0)
			color = m_pCfgColor->cOff;
		else
			color = m_pCfgColor->cOn;
	}
	else // alarm
	{
		if(pTag->m_nAlmStatus == ALM_NORMAL)
			color = m_pCfgColor->cOff;
		else
			color = m_pCfgColor->cOn;

	}
	m_pObj->m_logbrush.setColor(color);
}
///////////////////////////////////////
////-- CRunFColor    //////////////////
///////////////////////////////////////
CRunFColorA::CRunFColorA() {}
CRunFColorA::CRunFColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunFColorD(parent, pObj, pCfgColor) 
{
	m_pCfgColor = pCfgColor;
	m_V1 = m_pCfgColor->vhihi_hi;
	m_V2 = m_pCfgColor->vhi_normal;
	m_V3 = m_pCfgColor->vnormal_lo;	
	m_V4 = m_pCfgColor->vlo_lolo;

}
CRunFColorA::~CRunFColorA() {}

void CRunFColorA::Exec(double m_Value)
{
	QColor color;
	if(m_Value >= m_V1)
		color = m_pCfgColor->cHiHi;
	else
	if(m_Value >= m_V2)
		color = m_pCfgColor->cHi;
	else
	if(m_Value <= m_V4)
		color = m_pCfgColor->cLoLo;
	else
	if(m_Value <= m_V3)
		color = m_pCfgColor->cLo;
	else
		color = m_pCfgColor->cNormal;
	m_pObj->m_logbrush.setColor(color);
}

void CRunFColorA::Exec(CTag* pTag)
{
	QColor color;
	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value >= m_V1)
			color = m_pCfgColor->cHiHi;
		else
		if(pTag->m_Value >= m_V2)
			color = m_pCfgColor->cHi;
		else
		if(pTag->m_Value <= m_V4)
			color = m_pCfgColor->cLoLo;
		else
		if(pTag->m_Value <= m_V3)
			color = m_pCfgColor->cLo;
		else
			color = m_pCfgColor->cNormal;
	}
	else // alarm  3 type
	{
		if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 0)  // HIHILoLo
		{
			switch(pTag->isHLAlarm())
			{	
				case HL_HIHI_ALARM:
					color = m_pCfgColor->cHiHi;
					break;
				case HL_HI_ALARM:
					color = m_pCfgColor->cHi;
					break;
				case HL_LO_ALARM:
					color = m_pCfgColor->cLo;
					break;
				case HL_LOLO_ALARM:
					color = m_pCfgColor->cLoLo;
					break;
				case ALM_NORMAL:
					color = m_pCfgColor->cNormal;
					break;
				default:
					color = m_pCfgColor->cNormal;
					break;
			}
		}
		else
		{
			if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 1)  // Major Minor
			{
				if(pTag->m_bMajorDev || pTag->m_bMinorDev)	
				{
					if(pTag->isDevAlarm() == DEV_MAJOR_ALARM)
						color = m_pCfgColor->cNormal;
					else
					if(pTag->isDevAlarm() == DEV_MINOR_ALARM)
						color = m_pCfgColor->cHi;
					else
						color = m_pCfgColor->cHiHi;
				}
			}
			else
			{
				if(pTag->m_bROC) // Rate Of Change
				{
					if(pTag->isRocAlarm() == ROC_ALARM)
						color = m_pCfgColor->cHi;
					else
						color = m_pCfgColor->cHiHi;
				}
			}
		}
	}
	m_pObj->m_logbrush.setColor(color);
}

///////////////////////////////////////
////-- CRunColor    ///////////////////////////
///////////////////////////////////////

CRunLColorD::CRunLColorD(){}
CRunLColorD::CRunLColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunObject(parent,pObj)
{
	m_pCfgColor = pCfgColor;
}

CRunLColorD::~CRunLColorD() {}

void CRunLColorD::Exec(double m_Value)
{
	QColor color;
	if(m_Value == 0)
		color = m_pCfgColor->cOff;
	else
		color = m_pCfgColor->cOn;
	m_pObj->m_logpen.setColor(color);
}

void CRunLColorD::Exec(CTag* pTag)
{
	QColor color;
	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value == 0)
			color = m_pCfgColor->cOff;
		else
			color = m_pCfgColor->cOn;
	}
	else // alarm
	{
		if(pTag->m_nAlmStatus == ALM_NORMAL)
			color = m_pCfgColor->cOff;
		else
			color = m_pCfgColor->cOn;
	}
	m_pObj->m_logpen.setColor(color);
}
///////////////////////////////////////
////-- CRunLColor    ///////////////////////////
///////////////////////////////////////

CRunLColorA::CRunLColorA(){}
CRunLColorA::CRunLColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunLColorD(parent, pObj, pCfgColor) 
{
	m_pCfgColor = pCfgColor;

	m_V1 = m_pCfgColor->vhihi_hi;
	m_V2 = m_pCfgColor->vhi_normal;
	m_V3 = m_pCfgColor->vnormal_lo;	
	m_V4 = m_pCfgColor->vlo_lolo;
}
CRunLColorA::~CRunLColorA() {}

void CRunLColorA::Exec(double m_Value)
{
	QColor color;
	if(m_Value >= m_V1)
		color = m_pCfgColor->cHiHi;
	else
	if(m_Value >= m_V2)
		color = m_pCfgColor->cHi;
	else
	if(m_Value <= m_V4)
		color = m_pCfgColor->cLoLo;
	else
	if(m_Value <= m_V3)
		color = m_pCfgColor->cLo;
	else
		color = m_pCfgColor->cNormal;
	m_pObj->m_logpen.setColor(color);
}


void CRunLColorA::Exec(CTag* pTag)
{
	QColor color;
	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value >= m_V1)
			color = m_pCfgColor->cHiHi;
		else
		if(pTag->m_Value >= m_V2)
			color = m_pCfgColor->cHi;
		else
		if(pTag->m_Value <= m_V4)
			color = m_pCfgColor->cLoLo;
		else
		if(pTag->m_Value <= m_V3)
			color = m_pCfgColor->cLo;
		else
			color = m_pCfgColor->cNormal;
	}
	else // alarm  3 type
	{
		if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 0)  // HIHILoLo
		{
			switch(pTag->isHLAlarm())
			{	
				case HL_HIHI_ALARM:
					color = m_pCfgColor->cHiHi;
					break;
				case HL_HI_ALARM:
					color = m_pCfgColor->cHi;
					break;
				case HL_LO_ALARM:
					color = m_pCfgColor->cLo;
					break;
				case HL_LOLO_ALARM:
					color = m_pCfgColor->cLoLo;
					break;
				case ALM_NORMAL:
					color = m_pCfgColor->cNormal;
					break;
				default:
					color = m_pCfgColor->cNormal;
					break;
			}

		}
		else
		{
			if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 1)  // Major Minor
			{
				if(pTag->m_bMajorDev || pTag->m_bMinorDev)	
				{
					if(pTag->isDevAlarm() == DEV_MAJOR_ALARM)
						color = m_pCfgColor->cNormal;
					else
					if(pTag->isDevAlarm() == DEV_MINOR_ALARM)
						color = m_pCfgColor->cHi;
					else
						color = m_pCfgColor->cHiHi;
				}
			}
			else
			{
				if(pTag->m_bROC) // Rate Of Change
				{
					if(pTag->isRocAlarm() == ROC_ALARM)
						color = m_pCfgColor->cHi;
					else
						color = m_pCfgColor->cHiHi;
				}
			}
		}
	}
	m_pObj->m_logpen.setColor(color);
}

///////////////////////////////////////
////-- CRunColor    ///////////////////////////
///////////////////////////////////////

CRunTColorD::CRunTColorD(){}
CRunTColorD::CRunTColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunObject(parent,pObj) 
{
	m_pCfgColor = pCfgColor;
}
CRunTColorD::~CRunTColorD() {}

void CRunTColorD::Exec(double m_Value)
{
	if(m_Value == 0)
		((CMyTextObject*)m_pObj)->m_logpen = m_pCfgColor->cOff;
	else
		((CMyTextObject*)m_pObj)->m_logpen = m_pCfgColor->cOn;
}

void CRunTColorD::Exec(CTag* pTag)
{
	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value == 0)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cOff;
		else
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cOn;
	}
	else // alarm
	{
		if(pTag->m_nAlmStatus == ALM_NORMAL)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cOff;
		else
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cOn;
	}
}
///////////////////////////////////////
////-- CRunTColor    ///////////////////////////
///////////////////////////////////////

CRunTColorA::CRunTColorA(){}
CRunTColorA::CRunTColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor):CRunTColorD(parent, pObj, pCfgColor) 
{
	m_pCfgColor = pCfgColor;

	m_V1 = m_pCfgColor->vhihi_hi;
	m_V2 = m_pCfgColor->vhi_normal;
	m_V3 = m_pCfgColor->vnormal_lo;	
	m_V4 = m_pCfgColor->vlo_lolo;
}
CRunTColorA::~CRunTColorA() {}

void CRunTColorA::Exec(double m_Value)
{
	if(m_Value >= m_V1)
		((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHiHi;
	else
	if(m_Value >= m_V2)
		((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHi;
	else
	if(m_Value <= m_V4)
		((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLoLo;
	else
	if(m_Value <= m_V3)
		((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLo;
	else
		((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cNormal;
}

void CRunTColorA::Exec(CTag* pTag) 
{
	if(!m_pCfgColor->bcheck) // Value
	{
		if(pTag->m_Value >= m_V1)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHiHi;
		else
		if(pTag->m_Value >= m_V2)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHi;
		else
		if(pTag->m_Value <= m_V4)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLoLo;
		else
		if(pTag->m_Value <= m_V3)
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLo;
		else
			((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cNormal;
	}
	else // alarm  3 type
	{
		if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 0)  // HIHILoLo
		{
			switch(pTag->isHLAlarm())
			{	
				case HL_HIHI_ALARM:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHiHi;
					break;
				case HL_HI_ALARM:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHi;
					break;
				case HL_LO_ALARM:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLo;
					break;
				case HL_LOLO_ALARM:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cLoLo;
					break;
				case ALM_NORMAL:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cNormal;
					break;
				default:
					((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cNormal;
					break;
			}

		}
		else
		{
			if(pTag->m_bHiLo && m_pCfgColor->nAlarm == 1)  // Major Minor
			{
				if(pTag->m_bMajorDev || pTag->m_bMinorDev)	
				{
					if(pTag->isDevAlarm() == DEV_MAJOR_ALARM)
						((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cNormal;
					else
					if(pTag->isDevAlarm() == DEV_MINOR_ALARM)
						((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHi;
					else
						((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHiHi;
				}
			}
			else
			{
				if(pTag->m_bROC) // Rate Of Change
				{
					if(pTag->isRocAlarm() == ROC_ALARM)
						((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHi;
					else
						((CMyTextObject*)m_pObj)->m_TextColor = m_pCfgColor->cHiHi;
				}
			}
		}
	}
}


///////////////////////////////////////

CRunMoveAH::CRunMoveAH() {}
CRunMoveAH::CRunMoveAH(QWidget *parent, CDrawObject* pObj, CfgMove* pCfgMove):CRunObject(parent,pObj) 
{
	m_pCfgMove = pCfgMove;

	pTag = NULL;
	m_nCurrent	 = 0.0f;
	x1 = m_pCfgMove->nVMin;
	x2 = m_pCfgMove->nVMax;
	y1 = m_pCfgMove->nPMin;
	y2 = m_pCfgMove->nPMax;

	bLocate = m_pCfgMove->nLocate;

	m_bSetValue = false;
	m_dwSetValue = 0;

	vWidth = fabs( x1 - x2 );
	pWidth = fabs( y1 + y2 );
	Ratio = pWidth / vWidth;
	bLeft = false;
	OldMoveP = 0;
	if( x1 > x2 )
	{
		bLeft = true;
	//	OldMoveP = y2;
	}
	Rect = m_pObj->m_position;
	m_nButton = SLIDER;
}

CRunMoveAH::~CRunMoveAH() {}

void CRunMoveAH::Exec(QPoint& point) // Slider
{
	double MoveS, MoveP, diff;
	double p_min = fabs(y1);
	double p_max = fabs(y2);

	if(p_min > p_max)
	{
		double tmp = p_max;

		p_max = p_min;
		p_min = tmp;
	}
	if(bLocate)
		MoveP = lpoint.x() - point.x();
	else
		MoveP = point.x() - lpoint.x();

	diff = MoveP;
	MoveP += OldMoveP;

	if(MoveP < p_min || MoveP > p_max)
		return;
	else
	{
		MoveS = diff;
		OldMoveP = MoveP;
	}
	lpoint.setX(point.x());

	if(MoveS != 0)
	{
		double GapV = (bLeft) ?	x2 - x1 : x1 - x2;
		double GapP = (bLocate) ? MoveP - p_min : MoveP - p_max;
		double Value = (GapP / (p_min - p_max) * GapV) + x2;
		QRect imsi = m_pObj->m_position;
		imsi.normalized();
		if(bLocate)
			MoveS = -MoveS;
		m_pObj->m_position.moveTo(QPoint(int(MoveS), 0));
		imsi = imsi | m_pObj->m_position;
		imsi.translated(2,2);

		m_bSetValue = true;
		m_dwSetValue = Value;
	}
}

void CRunMoveAH::OutputValue()
{
//	if(!g_pCtrl->LogIn())
//		return;
//	if(pTag != NULL && m_bSetValue)
//		pTag->SetValue_His(m_dwSetValue, false);
	m_bSetValue = false;
}

void CRunMoveAH::Exec(double m_Value)
{
	if(bLock)
		return;

	double v = (bLeft) ? m_Value - x2 : m_Value - x1;

	m_nCurrent  = v * Ratio;

	QRect imsi = m_pObj->m_position;
	m_pObj->m_position.setLeft(Rect.left());
	m_pObj->m_position.setRight(Rect.right());

	double MoveP = m_nCurrent - (bLeft ? y2 : y1);
	OldMoveP = MoveP;

	if(bLeft)
		MoveP = -MoveP;
	if(MoveP != 0)
		m_pObj->m_position.moveTo(QPoint(int(MoveP), 0));
	imsi = imsi | m_pObj->m_position;
}

void CRunMoveAH::Exec(CTag* pTag)
{
	if(bLock)
		return;
	Exec(pTag->m_Value);
}
///////////////////////////////////////

CRunMoveAV::CRunMoveAV() {}
CRunMoveAV::CRunMoveAV(QWidget *parent, CDrawObject* pObj, CfgMove* pCfgMove):CRunObject(parent,pObj) 
{
	m_pCfgMove = pCfgMove;

	pTag = NULL;
	m_nCurrent	= 0.0f;	 // 현재값 
	x1 = m_pCfgMove->nVMin;
	x2 = m_pCfgMove->nVMax;
	y1 = m_pCfgMove->nPMin;
	y2 = m_pCfgMove->nPMax;

	bLocate = m_pCfgMove->nLocate;

	m_bSetValue = false;
	m_dwSetValue = 0;

	vHeight = fabs( x1 - x2 );
	pHeight = fabs( y1 + y2 );
	Ratio = pHeight / vHeight;
	bUp = false;

	OldMoveP = 0;
	if( x1 > x2 )
	{
		bUp = true;
	//	OldMoveP = y2;
	}
	Rect = m_pObj->m_position;
	m_nButton = SLIDER;
}

CRunMoveAV::~CRunMoveAV() {}

void CRunMoveAV::Exec(QPoint& point) // slider
{
	double MoveS, MoveP,diff;
	double p_min = fabs(y1), p_max = fabs(y2);
	if(p_min > p_max)
	{
		double tmp = p_max;

		p_max = p_min;
		p_min = tmp;
	}
	if(bLocate)
		MoveP = lpoint.y() - point.y(); // move - lbuttondown
	else
		MoveP = point.y() - lpoint.y(); 

	diff = MoveP;
	MoveP += OldMoveP;

	if(MoveP < p_min || MoveP > p_max)
		return;
	else
	{
		MoveS = diff;
		OldMoveP = MoveP;
	}
	lpoint.setY(point.y());

	if(MoveS != 0)
	{
		double GapV = (bUp) ?	x2 - x1 : x1 - x2;
		double GapP = (bLocate) ? MoveP - p_min : MoveP - p_max;
		double Value = (GapP / (p_min - p_max) * GapV) + x2;
		QRect imsi = m_pObj->m_position;
		imsi.normalized();
		if(bLocate)
			MoveS = -MoveS;
		m_pObj->m_position.moveTo(QPoint(0, int(MoveS)));
		imsi = imsi | m_pObj->m_position;
		imsi.translated(2, 2);
		m_bSetValue = true;
		m_dwSetValue = Value;
	}
}

void CRunMoveAV::OutputValue()
{
//	if(g_pCtrl->LogIn())
	{
//		if(pTag != NULL && m_bSetValue)
//			pTag->SetValue_His(m_dwSetValue, false);
		m_bSetValue = false;
	}
}

void CRunMoveAV::Exec(double m_Value)
{
	if(bLock)
		return;
	double v = (bUp) ? m_Value - x2 : m_Value - x1;


	m_nCurrent  = v * Ratio;

	QRect imsi = m_pObj->m_position;
	m_pObj->m_position.setTop(Rect.top());
	m_pObj->m_position.setBottom(Rect.bottom());

	double MoveP = m_nCurrent - (bUp ? y2 : y1);
	OldMoveP = MoveP;

	if(bUp)
		MoveP = -MoveP;
	if(MoveP != 0)
		m_pObj->m_position.moveTo(QPoint(0, int(MoveP)));
	imsi = imsi | m_pObj->m_position;
}

void CRunMoveAV::Exec(CTag* pTag)
{
	if(bLock)
		return;
	Exec(pTag->m_Value);
}


///////////////////////////////////////
////-- CRunSize    ///////////////////////////
///////////////////////////////////////
CRunSizeAH::CRunSizeAH() {}
CRunSizeAH::CRunSizeAH(QWidget *parent, CDrawObject* pObj, CfgSize* pCfgSize):CRunObject(parent,pObj) 
{
	m_pCfgSize = pCfgSize;

	m_nCurrent	= 0.0f;
	QRect rect = pObj->m_position;
	rect.normalized();
	m_SaveWidth = rect.width();

	x1 = m_pCfgSize->nVMax;
	x2 = m_pCfgSize->nVMin;
	y1 = m_pCfgSize->nPMax;
	y2 = m_pCfgSize->nPMin;
	Check = m_pCfgSize->nDefault;  // 기준 
}

CRunSizeAH::~CRunSizeAH() {}

void CRunSizeAH::OnSizeExpression(QRect& rect,float Per)
{
	switch(Check) 
	{
		case 0:
			rect.setRight(int(rect.left() + (m_SaveWidth*Per)));				// left
			break;
		case 1:	
			{
				int center;
				center = int(rect.right() - rect.width()/2);
				rect.setLeft(int(center - m_SaveWidth*Per / 2));
				rect.setRight(int(center + m_SaveWidth*Per / 2));
				break;
			}
		case 2:
			rect.setLeft(int(rect.right() - (m_SaveWidth*Per)));				// right
			break;
	}
}

void CRunSizeAH::Exec(double m_Value)
{
	QPoint m_delta;
	m_nCurrent  = m_Value;

	QRect imsi = m_pObj->m_position;
	imsi.normalized();

	if(m_nCurrent <= x2)
		OnSizeExpression(m_pObj->m_position,(float)y2/100);
	else
	{
		if(m_nCurrent >= x1)
			OnSizeExpression(m_pObj->m_position,(float)y1/100);
		else
		{
			float Per  = (float) ((((m_nCurrent-x2) / (x1- x2) * (y1 - y2)) + y2) / 100.);
			OnSizeExpression(m_pObj->m_position,Per);
		}
	}
	imsi = imsi | m_pObj->m_position;
}

void CRunSizeAH::Exec(CTag* pTag)
{
	Exec(pTag->m_Value);
}

///////////////////////////////////////////////////////////////////////////
CRunSizeAV::CRunSizeAV() {}

CRunSizeAV::CRunSizeAV(QWidget *parent, CDrawObject* pObj, CfgSize* pCfgSize):CRunObject(parent,pObj) 
{
	m_pCfgSize = pCfgSize;

	m_nCurrent	= 0.0f;

	QRect rect = pObj->m_position;
	rect.normalized();
	m_SaveHeight = rect.height();

	x1 = m_pCfgSize->nVMax;
	x2 = m_pCfgSize->nVMin;
	y1 = m_pCfgSize->nPMax;
	y2 = m_pCfgSize->nPMin;
	Check  =  m_pCfgSize->nDefault;  // 기준 
}

CRunSizeAV::~CRunSizeAV() {}

void CRunSizeAV::OnSizeExpression(QRect& rect,float Per)
{
	switch(Check) 
	{
		case 0:	
				rect.setBottom(int(rect.top()  +  (m_SaveHeight*Per)));				// top
				break;
		case 1:	
			{
				int center;
				center      = int(rect.bottom() - rect.height() / 2);
				rect.setTop(int(center - m_SaveHeight*Per / 2));
				rect.setBottom(int(center + m_SaveHeight*Per / 2));
				break;
			}
		case 2:
			rect.setTop(int(rect.bottom() - (m_SaveHeight*Per)));				// bottom
			break;
	}
}

void CRunSizeAV::Exec(double m_Value)
{
	QPoint m_delta;
	m_nCurrent  = m_Value;
	QRect imsi = m_pObj->m_position;
	imsi.normalized();

	if(m_nCurrent <= x2)
		OnSizeExpression(m_pObj->m_position,(float)y2/100);
	else
	{
		if(m_nCurrent >= x1) 
			OnSizeExpression(m_pObj->m_position,(float)y1/100);
		else
		{
			float Per  = (float)((((m_nCurrent-x2) / (x1- x2) * (y1 - y2)) + y2) / 100.);
			OnSizeExpression(m_pObj->m_position,Per);
		}
	}
	imsi = imsi | m_pObj->m_position;
}

void CRunSizeAV::Exec(CTag* pTag)
{
	Exec(pTag->m_Value);
}

///////////////////////////////////////
////-- CRunFill    ///////////////////////////
///////////////////////////////////////
CRunFill::CRunFill(){}
CRunFill::CRunFill(QWidget *parent, CDrawObject* pObj):CRunObject(parent,pObj)
{
/*	CTag *pTag;
	if( pTag && pTag->m_nInput == 1 ) // 역률인 경우
	{
		if( pTag->m_nRatio == (double)1.0 && pTag->m_nMin == pTag->m_nMinRaw )
		{
			x1 = 0.0;
			x2 = 1000.0;
		}
		else
		{
			x1 = pTag->m_nMinRaw;
			x2 = pTag->m_nMaxRaw;
		}
	}
	else
	{
		x1 = m_pObj->m_pCfgObj->m_CfgFill.nVMin;
		x2 = m_pObj->m_pCfgObj->m_CfgFill.nVMax;
	}
	y1 = m_pObj->m_pCfgObj->m_CfgFill.nPMin;
	y2 = m_pObj->m_pCfgObj->m_CfgFill.nPMax;
*/
 }

CRunFill::~CRunFill() {}

void CRunFill::Exec(double m_Value)
{
	m_pObj->m_nValue = m_Value;
	if(x1 > m_Value)	
		m_pObj->m_fPercent = (float) y1;
	else	
	{
		if(x2 < m_Value)	
			m_pObj->m_fPercent = (float) y2; 
		else 	
			m_pObj->m_fPercent = (float) (((m_Value-x1)/(x2-x1) * (y2-y1)) + y1);
	}
}


void CRunFill::Exec(CTag* pTag)
{
	Exec(pTag->m_Value);
}

///////////////////////////////////////
////-- CRunRotate    ////////////////////////--/
///////////////////////////////////////
CRunRotate::CRunRotate() {}

CRunRotate::CRunRotate(QWidget *parent, CDrawObject* pObj, CfgRotate* pCfgRotate):CRunObject(parent,pObj) 
{
	m_pCfgRotate = pCfgRotate;

	m_nCurrent	= 0.0f;
	min     = m_pCfgRotate->nVCW;
	max     = m_pCfgRotate->nVCCW;
	min_ang = m_pCfgRotate->nRCW;
	max_ang = m_pCfgRotate->nRCCW;
//	((CMyPolyObject*)m_pObj)->m_rectmode = false;
//	point1 = ((CMyPolyObject*)m_pObj)->GetHandlePoint(1);
//	QPoint point2 = ((CMyPolyObject*)m_pObj)->GetHandlePoint(2);
//	int xlen,ylen;
//	xlen = abs(point1.x - point2.x);
//	ylen = abs(point1.y - point2.y);
//	rad = (xlen > ylen) ? xlen : ylen;
}

CRunRotate::~CRunRotate() {}

const double Calc = ( 3.14159265358979323846264338 / 180.0 );

void CRunRotate::Exec(double m_Value)
{
	if( min == max )	
		return;
	QRect imsi = m_pObj->m_position;
	imsi.normalized();

	m_nCurrent = m_Value;

	if(max > min)
	{
		if( m_nCurrent <= min )
			m_nCurrent = 0;
		else
			m_nCurrent -= min;

		angle = (max_ang - min_ang) * m_nCurrent;
		angle /= (max - min);
		angle += min_ang;
	}
	else
	{	
		if( m_nCurrent < max )
			m_nCurrent = max;
		else
			m_nCurrent -= max;
		angle = (min_ang - max_ang) * m_nCurrent;
		angle /= (min - max);
		angle += max_ang;
	}

	QPoint spoint;
	spoint.setX((int)(point1.x() + rad * cos(angle * Calc)));
	spoint.setY((int)(point1.y() - rad * sin(angle * Calc)));

//	((CMyPolyObject*)m_pObj)->LastPoint(spoint,true);

//	imsi.setLeft(min( imsi.left(),   m_pObj->m_position.left() ));
//	imsi.setTop(min( imsi.top(),    m_pObj->m_position.top() ));
//	imsi.setRight(max( imsi.right(),  m_pObj->m_position.right() ));
//	imsi.setBottom(max( imsi.bottom(), m_pObj->m_position.bottom() ));

	if(m_pObj->m_logpen.width() > 0 && m_pObj->m_logpen.width() < 100)
	{
		int xx = m_pObj->m_logpen.width() >> 1;
		imsi.setLeft(imsi.left() - xx);
		imsi.setTop(imsi.top() - xx);
		imsi.setRight(imsi.right() + xx);
		imsi.setBottom(imsi.bottom() + xx);
	}
	imsi.normalized();
}

void CRunRotate::Exec(CTag* pTag)
{
//	Exec(popup, ((CAnalogRec*)pTag)->m_nInput ? ((CAnalogRec* )pTag)->RawValue : pTag->m_Value);
	Exec(pTag->m_Value);
}

///////////////////////////////////////
////-- CRunHide    ////////////////////////--/
///////////////////////////////////////

CRunHideD::CRunHideD() {}

CRunHideD::CRunHideD(QWidget *parent, CDrawObject* pObj, CfgHide* pCfgHide):CRunObject(parent,pObj) 
{
	m_pCfgHide = pCfgHide;
	m_nCurrent	= 0.0f;
}

CRunHideD::~CRunHideD() {}

void CRunHideD::Exec(double m_Value)
{
//	m_pObj->m_bDelete =  true;
	
/*	if(m_pCfgHide->nOn == 0)
		m_pObj->m_bDelete = (m_Value==0) ? true : false;
	else
		m_pObj->m_bDelete = (m_Value==1) ? true : false;
*/
	if(m_pCfgHide->nOn == 0)
	{
		if(m_Value)
			m_pObj->m_bDelete = false;
		else
			m_pObj->m_bDelete = true;
	}
	else
	{
		if(m_Value)
			m_pObj->m_bDelete = true;
		else
			m_pObj->m_bDelete = false;
	}
	
}


void CRunHideD::Exec(CTag* pTag)
{
	m_nCurrent = pTag->m_Value;
	if (m_pCfgHide->nOn == 0)
	{
		if(m_nCurrent)
			m_pObj->m_bDelete = false;
		else
			m_pObj->m_bDelete = true;
	}
	else
	if(m_pCfgHide->nOn == 1)
	{
		if(m_nCurrent)
			m_pObj->m_bDelete = true;
		else
			m_pObj->m_bDelete = false;
	}
	else
	if(m_pCfgHide->nOn == 2)
	{
/*		if(pTag->m_nTagType == DIGITAL)
		{
			CDigitalRec* pObj = (CDigitalRec*) pTag;
			m_pObj->m_bDelete = (pObj->m_nAlmStatus == ALM_NORMAL) ? true : false;
		}
		else
		{
			CAnalogRec* pObj = (CAnalogRec*) pTag;
			m_pObj->m_bDelete = (!pObj->isHLAlarm()) ? true : false;
		}
*/	}
}

///////////////////////////////////////
////-- CRunHide    ////////////////////////--/
///////////////////////////////////////
CRunHideA::CRunHideA() {}

CRunHideA::CRunHideA(QWidget *parent, CDrawObject* pObj, CfgHide* pCfgHide):CRunHideD(parent, pObj, pCfgHide) 
{
	m_pCfgHide = pCfgHide;

	m_nCurrent	= 0.0f;
	m_V1 = m_pCfgHide->vhihi_hi;
	m_V2 = m_pCfgHide->vhi_normal;
	m_V3 = m_pCfgHide->vnormal_lo;	
	m_V4 = m_pCfgHide->vlo_lolo;
}

CRunHideA::~CRunHideA() {}

void CRunHideA::Exec(double m_Value)
{
	m_nCurrent	= m_Value;
	if(m_nCurrent >= m_V1)
		m_pObj->m_bDelete = m_pCfgHide->bHiHi;   // HiHi
	else
	if(m_nCurrent >= m_V2)
		m_pObj->m_bDelete = m_pCfgHide->bHi;	// Hi
	else
	if(m_nCurrent <= m_V4)
		m_pObj->m_bDelete = m_pCfgHide->bLoLo; // LoLo
	else
	if(m_nCurrent <= m_V3)
		m_pObj->m_bDelete = m_pCfgHide->bLo; // Lo
	else
		m_pObj->m_bDelete = m_pCfgHide->bNormal;						// Normal
}

void CRunHideA::Exec(CTag* pTag)
{
	m_nCurrent	= pTag->m_Value;
	if(m_nCurrent >= m_V1)
		m_pObj->m_bDelete = m_pCfgHide->bHiHi;   // HiHi
	else
	if(m_nCurrent >= m_V2)
		m_pObj->m_bDelete = m_pCfgHide->bHi;	// Hi
	else
	if(m_nCurrent <= m_V4)
		m_pObj->m_bDelete = m_pCfgHide->bLoLo; // LoLo
	else
	if(m_nCurrent <= m_V3)
		m_pObj->m_bDelete = m_pCfgHide->bLo; // Lo
	else
		m_pObj->m_bDelete = m_pCfgHide->bNormal;						// Normal
}

///////////////////////////////////////
////-- CRunButton    ////////////////////////--/
///////////////////////////////////////

CRunButton::CRunButton() {}
CRunButton::CRunButton(QWidget *parent, CDrawObject* pObj):CRunObject(parent,pObj) 
{
	m_nCurrent	= 0.0f;
	m_nButton   = BUTTON;
	m_bUI		= false;
}

CRunButton::~CRunButton() {}

void CRunButton::Exec(bool check)
{
	if(!m_bUI) // 사용 정지 
	{
//		((CMyButtonObject*)m_pObj)->m_bDown = check;
		if(check) {} // 눌렀을때
			else  {} // 때었을때
//		if(g_pCtrl)
//			g_pCtrl->InvalObj(m_pObj);
	}
}

extern char SensorCode[15][10];
extern int SensorFormat[15];
extern int	SensorNumber;

///////////////////////////////////////
////-- CRunText    ///////////////////////////
///////////////////////////////////////
CRunText::CRunText() {}
CRunText::CRunText( QWidget *parent, CDrawObject* pObj, CfgText* pCfgText):CRunObject( parent, pObj ) 
{

	if(m_pObj->m_bText)
	{
		m_pCfgText = pCfgText;
//		bool bZero = m_pCfgText->bZero;
		// 2005. 02. 16. (콤마표시)
		m_bComma = m_pCfgText->bComma;
		m_nSensorIndex = -1;
//		for(int i = 0; i < SensorNumber; i++)
//		{
//			if(m_pCfgText->sName == SensorCode[i])
//			{
//				m_nSensorIndex = i;
//			}
//		}
        makeFormat();
/*		{
			int check = ((CMyTextObject*)m_pObj)->m_Text.indexOf('.');
			char	buf[20];
			if(check == -1) 
			{
				int tot = ( ( CMyTextObject* )m_pObj )->m_Text.length();
				if( bZero )
                    ::sprintf(buf, "0%d.", tot );
				else
					::sprintf(buf, "%d.", tot );
				m_Format  = "%";
				if(tot && ( ( CMyTextObject* )m_pObj )->m_Text[0] == 'e')
				{
					m_Format +=  buf;
					m_Format += "e";
				}
				else
				{
					m_Format +=  buf;
					m_Format += "f";
				}
			}
			else  
			{    
				int tot = ((CMyTextObject*)m_pObj)->m_Text.length();
				int fix = tot - ( check+1 );
				int pre = tot - ( fix + 1 );
				if(m_nSensorIndex >= 0)
					fix = SensorFormat[m_nSensorIndex];
				if(bZero)
                    ::sprintf(buf, "0%d.%d", pre+fix+1, fix );
				else
					::sprintf(buf, "%d.%d", pre+fix+1, fix );
				m_Format  = "%";
				if(( ( CMyTextObject* )m_pObj )->m_Text[0] == 'e')
				{
					m_Format +=  buf;
					m_Format += "e";
				}
				else
				{
					m_Format +=  buf;
					m_Format += "f";
				}
			}
        }*/
    }
}
CRunText::~CRunText() {}


void CRunText::makeFormat()
{
	if(m_pObj->m_bText == false)
		return;
	bool bZero = m_pCfgText->bZero;

//	if(m_nSensorIndex >= 0)
	{
		int check = ((CMyTextObject*)m_pObj)->m_Text.indexOf('.');
		char	buf[20];
        if(check == -1)
		{
			int tot = ( ( CMyTextObject* )m_pObj )->m_Text.length();
			if( bZero )
                ::sprintf(buf, "0%d.", tot );
			else
				::sprintf(buf, "%d.", tot );
			m_Format  = "%";
			if(tot && ( ( CMyTextObject* )m_pObj )->m_Text[0] == 'e')
			{
				m_Format +=  buf;
				m_Format += "e";
			}
			else
			{
				m_Format +=  buf;
				m_Format += "f";
			}
		}
		else  
		{    
			int tot = ((CMyTextObject*)m_pObj)->m_Text.length();
			int fix = tot - ( check+1 );
//			if(m_nSensorIndex >= 0)
//				fix = SensorFormat[m_nSensorIndex];
			int pre = tot - ( fix + 1 );
			if(bZero)
                ::sprintf(buf, "0%d.%d", pre+fix+1, fix );
			else
				::sprintf(buf, "%d.%d", pre+fix+1, fix );
			m_Format  = "%";
			if(( ( CMyTextObject* )m_pObj )->m_Text[0] == 'e')
			{
				m_Format +=  buf;
				m_Format += "e";
			}
			else
			{
				m_Format +=  buf;
				m_Format += "f";
			}
        }
    }
}

QString CRunText::makeComma(QString &number)
{
    int i = number.lastIndexOf('.');
    if(i == -1)
        i = number.length();
    if(i >0)
    {
       i -= 3;
       while(i > 0)
       {
         number.insert(i, ',');
         i -= 3;
       }
    }
    return number;
}

void CRunText::Exec(double m_Value)
{
	char	buf[100];

	if(m_pObj->m_bText == false)
		return;

	bool check = m_pCfgText->btype;
    if(check == 1)
	{
		if(m_Value)
			((CMyTextObject*)m_pObj)->m_Text = m_pCfgText->sOnMsg;
		else
			((CMyTextObject*)m_pObj)->m_Text = m_pCfgText->sOffMsg;
	}
	else
	{
        ::sprintf(buf, m_Format.toLocal8Bit().constData(), m_Value);
        QString str = buf;
        if(m_pCfgText->bComma)
            str = makeComma(str);
        if(m_pCfgText->bUnit)
        {
            CTag *pTag = FindTagObject(m_pCfgText->sName);
            if(pTag != NULL)
            {
                str += " ";
                str += pTag->getUnit();
            }
        }
        ((CMyTextObject*)m_pObj)->m_Text = str;
    }
}

void CRunText::Exec(QString value)
{
    ((CMyTextObject*)m_pObj)->m_Text = value;
}

void CRunText::Exec(CTag* pTag)
{
	if(m_pObj->m_bText == false)
		return;
	pTag = pTag;
	if(m_pObj->m_sName == "") return;

	QString buf;
	bool check = m_pCfgText->btype;
	if(check)
		buf.arg("%1",pTag->m_Value ? m_pCfgText->sOnMsg : m_pCfgText->sOffMsg);
	else 
	{
        QString str = buf;
        if(m_pCfgText->bComma)
            str = makeComma(str);
        ((CMyTextObject*)m_pObj)->m_Text = str;
    }
}

///////////////////////////////////////
////-- CRunUIDataD    ////////////////////////-/
///////////////////////////////////////

CRunUIDataD::CRunUIDataD() {}
CRunUIDataD::CRunUIDataD(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData):CRunObject(parent,pObj)
{
	m_pCfgData = pCfgData;

	m_bNonAni = true;
	m_bWhileOn = false;
	if(m_pCfgData->sName != "" && m_pCfgData->nDigital == 7)
		m_bWhileOn = true;
}
CRunUIDataD::~CRunUIDataD() {}

void CRunUIDataD::Exec(CTag* pTag)
{
	if(pTag == NULL)
	{
		pTag = FindTagObject(m_pCfgData->sName);
		if(pTag == NULL) return;
	}
	pTag2 = FindTagObject(m_pCfgData->sName2);
    QString msg;
	switch(m_pCfgData->nDigital)
	{
        //confirm
		case 4:
		{
            if(m_pCfgData->bConfrm)
                msg = m_pCfgData->sMessage;

            CDigitalSet Set(msg, pTag, pTag2, false, m_pParent);
			if(Set.exec() == QDialog::Accepted)
			{
                double value;
                if(Set.m_bOn)
					value = 1;
                else
					value = 0;
				pTag->writeValue(value);
			}
			break;
		}
		// Set
		case 0:
			{
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
				}
				pTag->writeValue(1);
				break;  // "1"
			}
		// reset
		case 1:	
            if(m_pCfgData->bConfrm)
            {
                CQuestionDialog dlg(m_pCfgData->sMessage);
                if(dlg.exec() != QDialog::Accepted)
                    break;
            }
            pTag->writeValue(0);
            break;
		// toggle
		case 2: 
            if(m_pCfgData->bConfrm)
            {
                CQuestionDialog dlg(m_pCfgData->sMessage);
                if(dlg.exec() != QDialog::Accepted)
                    break;
            }
            pTag->writeValue(pTag->m_Value ?  0 : 1);
				break;

		// 1 Pulse
		case 3:	
            if(m_pCfgData->bConfrm)
            {
                CQuestionDialog dlg(m_pCfgData->sMessage);
                if(dlg.exec() != QDialog::Accepted)
                    break;
            }
            pTag->writeValue(1);
            pTag->writeValue(0, 10);
            break;
		// 2 MainTain
        case 5:
			{
				if(pTag2 == NULL)
					break;
                if(m_pCfgData->bConfrm)
                    msg = m_pCfgData->sMessage;
                CDigitalSet Set(msg, pTag, pTag2, true, m_pParent);
				if(Set.exec() == QDialog::Accepted)
				{
					if(Set.m_bOn)
					{
						pTag2->writeValue(0);
						pTag->writeValue(1);
					}
					else
					{
						pTag->writeValue(0);
						pTag2->writeValue(1);
					}
				}
			}
			break;
		// 2 Pulse
        case 6:
			{
				if(pTag2 == NULL)
					break;
                if(m_pCfgData->bConfrm)
                    msg = m_pCfgData->sMessage;
                CDigitalSet Set(msg, pTag, pTag2, true, m_pParent);
				if(Set.exec() == QDialog::Accepted)
				{
					if(Set.m_bOn)
					{
						pTag->writeValue(1);
                        pTag->writeValue(0,10);
					}
					else
					{
						pTag2->writeValue(1);
                        pTag2->writeValue(0,10);
					}
				}
			}
            break;
        default:
            msg = QString("잘못된 제어 타입입니다.(%1)").arg(m_pCfgData->nDigital);
            CInfomationDialog dlg(msg);
            dlg.exec();
            break;
	}
}

void CRunUIDataD::OutputValue(bool val)
{
	val = val;
	// 로그인 여부 및 제어 권한 검사
//	if(!g_pCtrl->LogIn())
//		return;

//	CTag* pTag  = g_pCtrl->FindTagObject(m_pCfgData->sName);
//	if(!pTag)
//		return;

}

///////////////////////////////////////
////-- CRunUIDataA    ////////////////////////-/
///////////////////////////////////////

CRunUIDataA::CRunUIDataA() {}
CRunUIDataA::CRunUIDataA(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData):CRunUIDataD(parent, pObj, pCfgData)
{
}
CRunUIDataA::~CRunUIDataA() {}

void CRunUIDataA::Exec(CTag* pTag)
{
	pTag = pTag;
	m_bNonAni = true;

	if(pTag == NULL)
	{
		pTag = FindTagObject(m_pCfgData->sName);
		if(pTag == NULL) return;
	}

	double value = m_pCfgData->nValue;

	switch(m_pCfgData->nAnalog)
	{
		case 0: // (=)
			{
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
                pTag->writeValue(value);
                break;
			}
		case 1: // (+)
			{	
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
				pTag->writeValue(value+pTag->m_Value);
				break;
			}
		case 2: // (-)
			{	
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
				pTag->writeValue(pTag->m_Value-value);
				break;
			}
        case 3: // (*)
			{	
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
				pTag->writeValue(pTag->m_Value*value);
				break;
			}
        case 4:// (/)
			{
				if(m_pCfgData->bConfrm)
				{
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
				if(value != 0)
				{
					pTag->writeValue(pTag->m_Value/value);
				}
				break;
			}
        case 5:
		{
			QString msg, tag = pTag->GetTagName();
            msg = m_pCfgData->sMessage;
            CAnalogSet Dlg(msg, pTag, m_pParent);
			if(Dlg.exec() == QDialog::Accepted)
                pTag->writeValue(Dlg.m_sValue);
			break;
		}
	}
}

///////////////////////////////////////
////-- CRunUIDataS    ////////////////////////-/
///////////////////////////////////////

CRunUIDataS::CRunUIDataS() {}
CRunUIDataS::CRunUIDataS(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData):CRunUIDataD(parent, pObj, pCfgData)
{
}
CRunUIDataS::~CRunUIDataS() {}

void CRunUIDataS::Exec(CTag* pTag)
{
    pTag = pTag;
    m_bNonAni = true;

    if(pTag == NULL)
    {
        pTag = FindTagObject(m_pCfgData->sName);
        if(pTag == NULL) return;
    }

    QString value = m_pCfgData->sValue;

    switch(m_pCfgData->nAnalog)
    {

        case 0: // (=)
            {
                if(m_pCfgData->bConfrm)
                {
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
                pTag->writeValue(value);
                break;
            }
        case 1: // (+)
            {
                if(m_pCfgData->bConfrm)
                {
                    CQuestionDialog dlg(m_pCfgData->sMessage);
                    if(dlg.exec() != QDialog::Accepted)
                        break;
                }
                QString str = pTag->m_sValue+value;
                pTag->writeValue(str);
                break;
            }
        case 5:
        {
            QString msg, tag = pTag->GetTagName();
            msg = m_pCfgData->sMessage;
            CStringSet Dlg(msg, pTag, m_pParent);
            if(Dlg.exec() == QDialog::Accepted)
                pTag->writeValue(Dlg.m_sValue);
            // break;
        }
    }
}


///////////////////////////////////////
////-- CRunUIButton    /////////////////////////
///////////////////////////////////////

CRunUIButton::CRunUIButton() {}
CRunUIButton::CRunUIButton(QWidget *parent, CDrawObject* pObj):CRunObject(parent,pObj) {}
CRunUIButton::~CRunUIButton() {}

void CRunUIButton::Exec(CTag* pTag)
{
	pTag = pTag;
}

///////////////////////////////////////
////-- CRunOpen       /////////////////////////
///////////////////////////////////////

CRunOpen::CRunOpen() {}
CRunOpen::CRunOpen(QWidget *parent, CDrawObject* pObj, Cfgfile* pCfgfile):CRunObject(parent,pObj)
{
	m_pCfgfile = pCfgfile;
	connect(this, SIGNAL(sigOpenWindow(const QString&, int)), parent, SLOT(slotOpenWindow(const QString&, int)));
//	connect(this, SIGNAL(sigCloseWindow()), parent, SLOT(reject()));
	connect(this, SIGNAL(sigCloseWindow()), parent, SLOT(slotCloseWindow()));
	connect(this, SIGNAL(sigTMSData()), parent, SLOT(slotTMSData()));
	connect(this, SIGNAL(sigTMSInfo()), parent, SLOT(slotTMSInfo()));
	connect(this, SIGNAL(sigSewer()), parent, SLOT(slotSewer()));
	connect(this, SIGNAL(sigU_City()), parent, SLOT(slotU_City()));
	connect(this, SIGNAL(sigRainSensor()), parent, SLOT(slotRainSensor()));
	connect(this, SIGNAL(sigCommMon()), parent, SLOT(slotCommMon()));
    connect(this, SIGNAL(sigTrend()), parent, SLOT(slotTrend()));
    connect(this, SIGNAL(sigSysMenu()), parent, SLOT(slotSysMenu()));
    connect(this, SIGNAL(sigPointMon()), parent, SLOT(slotPointMon()));
    connect(this, SIGNAL(sigAlarmSummay()), parent, SLOT(slotAlarmSummary()));
    connect(this, SIGNAL(sigLog()), parent, SLOT(slotLog()));
    connect(this, SIGNAL(sigEpsTestSetup()), parent, SLOT(slotEpsTestSetup()));
}
CRunOpen::~CRunOpen() {}

void CRunOpen::Exec(CTag* pTag)
{
	pTag = pTag;
	// Option 0 : Open Window
	// Option 1 : Open Window
	// OPtion 2 : Function Call
	int nOption = m_pCfgfile->nOption;
	// 2005. 02. 22.
//	bool bTitle = m_pCfgfile->bUseTitle;
	QString strTagName = m_pCfgfile->strTagName;
	if(nOption == 1)
	{
		if(m_pCfgfile->strFuncName == tr("TMS"))
		{
			emit sigTMSData();
		}
		else
		if(m_pCfgfile->strFuncName == tr("TMSINFO"))
		{
			emit sigTMSInfo();
		}
		else
		if(m_pCfgfile->strFuncName == tr("HASU"))
		{
			emit sigSewer();
		}
		else
		if(m_pCfgfile->strFuncName == tr("U_CITY"))
		{
			emit sigU_City();
		}
		else
		if(m_pCfgfile->strFuncName == tr("RAIN_SENSOR"))
		{
			emit sigRainSensor();
		}
		else
		if(m_pCfgfile->strFuncName == tr("COMM_MON"))
		{
			emit sigCommMon();
		}
        else
        if(m_pCfgfile->strFuncName == tr("TREND"))
        {
            emit sigTrend();
        }
        else
		if(m_pCfgfile->strFuncName == tr("SYSINFO"))
		{
			emit sigSysMenu();
		}
        else
        if(m_pCfgfile->strFuncName == tr("POINT_MON"))
        {
            emit sigPointMon();
        }
        else
        if(m_pCfgfile->strFuncName == tr("ALARM_SUM"))
        {
            emit sigAlarmSummay();
        }
        else
        if(m_pCfgfile->strFuncName == tr("LOG"))
        {
            emit sigLog();
        }
        else
        if(m_pCfgfile->strFuncName == tr("EPS_TESTSETUP"))
        {
            emit sigEpsTestSetup();
        }
        return;
	}
	else
	if(nOption == 0)
	{
		if(m_pCfgfile->bCurrent_Close)
		{
			emit sigCloseWindow();
		}
		else
		if(m_pCfgfile->nOpen == 1 || m_pCfgfile->nOpen == 2)
		{
#ifndef WIN32
			if(m_pCfgfile->strFuncName == tr("Check"))
			{
                if(((CSrView*)m_pParent)->InputPassword() == false)
					return;
			}
#endif
			emit sigOpenWindow(m_pCfgfile->sOpen, m_pCfgfile->nOpen);
		}
	}
}


////////////////////////////////////////////////
////-- CRunBlink       /////////////////////////
////////////////////////////////////////////////
CRunBlink::CRunBlink() 
{
	m_pTag = NULL;
}
CRunBlink::CRunBlink(QWidget *parent, CDrawObject* pObj, CfgBlink* pCfgBlink):CRunObject(parent,pObj) 
{
	m_pCfgBlink = pCfgBlink;
	m_SaveColor = m_pObj->m_logbrush.color(); 
	if(m_pCfgBlink->btype)
	{
		m_V1 = m_pCfgBlink->vhihi_hi;
		m_V2 = m_pCfgBlink->vhi_normal;
		m_V3 = m_pCfgBlink->vnormal_lo;	
		m_V4 = m_pCfgBlink->vlo_lolo;
	}
	m_pTag = FindTagObject(m_pCfgBlink->sName);
}
CRunBlink::~CRunBlink() 
{
}

void CRunBlink::Exec(bool bBlink)
{
	if(m_pTag == NULL)
		return;
	if(m_pCfgBlink->bcheck == 0)
		m_pObj->m_bDelete = bBlink;
	else
	{
		QColor color;
		if(bBlink)
			color = m_pCfgBlink->cFill;
		else
			color = m_SaveColor;
		m_pObj->m_logbrush.setColor(color);
	}
}

bool CRunBlink::SetCheck()
{
	if(m_pTag == NULL)
		return false;
    if(m_pCfgBlink->btype == 0) // Digital / Analog
	{	// Digital
		if(m_pCfgBlink->cOn && m_pTag->m_Value > 0)
			return true;
        if(m_pCfgBlink->cOff && m_pTag->m_Value == 0)
			return true;
	}
	else
	{	
        bool check;
		m_nCurrent	= m_pTag->m_Value;
        if(m_nCurrent >= m_V1)
			check = m_pCfgBlink->cHiHi;	// HiHi
		else
		if(m_nCurrent >= m_V2)
			check = m_pCfgBlink->cHi;		// Hi
		else
		if(m_nCurrent <= m_V4)
			check = m_pCfgBlink->cLoLo;	// LoLo
		else
		if(m_nCurrent <= m_V3)
			check = m_pCfgBlink->cLo;		// Lo
		else
			check = m_pCfgBlink->cNormal;	// Normal
		return check;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////
// 객체 Frame처리용
///////////////////////////////////////////////////////////////////////////////////

CRunFrame::CRunFrame() 
{
	m_pTag = NULL;
	m_nFramePos = 0;
	m_nSkipCnt = 0;
}

CRunFrame::CRunFrame(QWidget *parent, CDrawObject* pObj) : CRunObject(parent,pObj) 
{
/*
	g_pCtrl->CheckAndFileDownLoad("/symbols/", pObj->m_pCfgObj->m_CfgFrame.sFrameName, ".frm", "symver.txt");
	QString str;
	str.arg("%s\\%s\\%s.Frm",WORKFOLDER, g_pCtrl->m_ProjectName, pObj->m_pCfgObj->m_CfgFrame.sFrameName);

	CFile	file;
	char	buf[1024];

	CFileException e;
	try
	{
		if(file.Open(str, CFile::modeRead))
		{
			CArchive ar(&file, CArchive::load, 1024, buf);
			m_List.Serialize(ar);
			ar.Close();
			file.Close();
		}
	}
	catch()
	{
		QString msg;
		msg.arg("서버로부터 손상된 %s.Frm 파일을 받았습니다.", pObj->m_pCfgObj->m_CfgFrame.sFrameName);
		g_pCtrl->MessageBox(msg, "WMI", MB_ICONERROR);
	}
	end_catch
*/
	m_nFramePos = 0;
	m_nSkipCnt = 0;
}

CRunFrame::~CRunFrame() 
{
//	while(m_List.GetCount())
//		delete m_List.RemoveHead();
}

void CRunFrame::Exec(double dwValue)
{

	int nVal = int(dwValue);
	if(m_nFramePos == nVal)
		return;
/*
	if(nVal >= m_List.GetCount())
		nVal = m_List.GetCount()-1;
	else
	if(nVal < 0)
		nVal = 0;
	m_nFramePos = nVal;
	int	count = 0;

	CDrawObject* pObj = NULL;
	POSITION pos = m_List.GetHeadPosition();
	while(pos)
	{
		pObj = (CDrawObject*)m_List.GetNext(pos);
		if(count++ == m_nFramePos)
			break;
		TRACE("count = %d, m_nFramePos = %d\n", count-1, m_nFramePos);
	}
	if(!pObj)
		return;

	if(m_pObj->IsKindOf(RUNTIME_CLASS(CMyBitmapObject)))
	{
		CDibRec*  pOldDIBRec = ((CMyBitmapObject*)m_pObj)->m_pDIBRec;
		CDibRec*  pNewDIBRec = ((CMyBitmapObject*)pObj)->m_pDIBRec;

		pOldDIBRec->m_pDIB->CopyImage((SECImage*)(pNewDIBRec->m_pDIB));
	}
	else
		return;
		*/
	/*
	if(g_pCtrl)
	{
		if(popup)
		{
			if(g_pCtrl->m_pCopsViewDlg)
				g_pCtrl->m_pCopsViewDlg->m_CopsView.InvalObj(m_pObj);
		}
		else
			g_pCtrl->InvalObj(m_pObj);
	} */
}

void CRunFrame::Exec()
{
//	int	count = 0;
/*
	CDrawObject* pObj = NULL;
	POSITION pos = m_List.GetHeadPosition();
	while(pos)
	{
		pObj = (CDrawObject*)m_List.GetNext(pos);
		if(count++ == m_nFramePos)
			break;
	}
	if(!pObj)
		return;

	if(m_pObj->IsKindOf(RUNTIME_CLASS(CMyBitmapObject)))
	{
		CDibRec*  pOldDIBRec = ((CMyBitmapObject*)m_pObj)->m_pDIBRec;
		CDibRec*  pNewDIBRec = ((CMyBitmapObject*)pObj)->m_pDIBRec;

		pOldDIBRec->m_pDIB->CopyImage((SECImage*)(pNewDIBRec->m_pDIB));
	}
	else
		return;
		*/
/*
	if(g_pCtrl)
	{
		if(popup)
		{
			if(g_pCtrl->m_pCopsViewDlg)
				g_pCtrl->m_pCopsViewDlg->m_CopsView.InvalObj(m_pObj);
		}
		else
			g_pCtrl->InvalObj(m_pObj);
	}*/
}

bool CRunFrame::SetCheck()
{
	bool	bFlag = false;
/*
	if(m_pObj->m_pCfgObj)
	{
		if(m_pObj->m_pCfgObj->m_CfgFrame.btype == 0)	// Discrete
		{
			if(++m_nSkipCnt < m_pObj->m_pCfgObj->m_CfgFrame.nTime / 100)
				return bFlag;

			if(m_pObj->m_pCfgObj->m_CfgFrame.nOn == 0)
				bFlag = (m_pTag->m_Value) ? true : false;
			else
			if(m_pObj->m_pCfgObj->m_CfgFrame.nOn == 1)
				bFlag = (m_pTag->m_Value) ? false : true;
			else
			if(m_pObj->m_pCfgObj->m_CfgFrame.nOn == 2)
			{
				if(m_pTag->m_nTagType == DIGITAL)
				{
					CDigitalRec* pObj = (CDigitalRec*) m_pTag;
					bFlag = (pObj->m_nAlmStatus == ALM_NORMAL) ? false : true;
				}
				else
				{
					CAnalogRec* pObj = (CAnalogRec*) m_pTag;
					bFlag = (pObj->isHLAlarm()) ? true : false;
				}
			}
			if(bFlag)
			{
				if(++m_nFramePos >= m_List.GetCount())
					m_nFramePos = 0;
			}
		}
		else	// Real
		{
			int pos = (m_pTag->m_Value) ? 1 : 0;
			if(pos != m_nFramePos)
			{	m_nFramePos = pos;
				bFlag = true;
			}
		}
	}
	*/
	m_nSkipCnt = 0;
	return bFlag;
}

///////////////////////////////////////////////////////////////////////////////////
// 객체 PipeFlow처리용
///////////////////////////////////////////////////////////////////////////////////

CRunPipeFlow::CRunPipeFlow() 
{
	m_pTag = NULL;
	m_nPipeFlowPos = 0;
	m_nSkipCnt = 0;
	m_bAnimation = false;
}

CRunPipeFlow::CRunPipeFlow(QWidget *parent, CDrawObject* pObj) : CRunObject(parent,pObj) 
{
	m_nPipeFlowPos = 0;
	m_bAnimation = false;
	m_nSkipCnt = 0;

/*	CPictureHolder TmpPicture;
	m_RenderTarget.Create32BitFromPicture(&TmpPicture, 
											m_pObj->m_position.width(),
											m_pObj->m_position.height());

	m_Flow.InitPipe(m_pObj->m_pCfgObj->m_CfgPipeFlow.cStart,
					m_pObj->m_pCfgObj->m_CfgPipeFlow.cEnd,
					m_pObj->m_pCfgObj->m_CfgPipeFlow.m_nArray,
					m_pObj->m_pCfgObj->m_CfgPipeFlow.m_bSwap);
*/
 }

CRunPipeFlow::~CRunPipeFlow() 
{
}

void CRunPipeFlow::Exec(CTag* pTag)
{
	m_bAnimation = pTag->m_Value ? true : false;

	if(m_bAnimation)
		m_pObj->m_bDelete = true;
	else
		ShowObject();
}

void CRunPipeFlow::Exec(double dwValue)
{
	m_bAnimation = dwValue ? true : false;

	if(m_bAnimation)
		m_pObj->m_bDelete = true;
	else
		ShowObject();
}

bool CRunPipeFlow::SetCheck()
{
	return m_bAnimation;
}

int CRunPipeFlow::GetSpeed()
{
//	return m_pObj->m_pCfgObj->m_CfgPipeFlow.m_nSpeed;
	return 1;
}

void CRunPipeFlow::ShowObject()
{
//	m_pObj->m_bDelete = !m_pObj->m_pCfgObj->m_CfgPipeFlow.m_bBackground;
}

void CRunPipeFlow::Exec()
{
/*	m_Flow.Render((DWORD*)m_RenderTarget.GetDIBits(), 
				  m_pObj->m_position.width(),
				  m_pObj->m_position.height(),
				  m_pObj->m_pCfgObj->m_CfgPipeFlow.m_nDirection,
				  m_pObj->m_pCfgObj->m_CfgPipeFlow.m_nThick);

	Draw(NULL);
	*/
}

void CRunPipeFlow::Draw()
{
	QRect imsi;

/*	CClientDC dc(g_pCtrl);
	imsi = m_pObj->m_position;
	imsi.normalized();
	g_pCtrl->DocToClient(imsi);
	m_RenderTarget.Draw(&dc, imsi.TopLeft());
	*/
}

///////////////////////////////////////////////////////////////////////////////////
// 객체 Gauege처리용
///////////////////////////////////////////////////////////////////////////////////

CRunGauge::CRunGauge()
{
}

CRunGauge::CRunGauge( QWidget *parent, CDrawObject* pObj, CfgGauge* ):CRunObject( parent, pObj )
{
/*	CPictureHolder TmpPicture;
    m_RenderTarget.Create32BitFromPicture(&TmpPicture,
                                            m_pObj->m_position.width(),
                                            m_pObj->m_position.height());

    m_Flow.InitPipe(m_pObj->m_pCfgObj->m_CfgPipeFlow.cStart,
                    m_pObj->m_pCfgObj->m_CfgPipeFlow.cEnd,
                    m_pObj->m_pCfgObj->m_CfgPipeFlow.m_nArray,
                    m_pObj->m_pCfgObj->m_CfgPipeFlow.m_bSwap);
*/
 }

CRunGauge::~CRunGauge()
{
}

void CRunGauge::Exec(CTag*)
{
//	m_bAnimation = pTag->m_Value ? true : false;
}

void CRunGauge::Exec(double value)
{
    ((CMyGaugeObject*)m_pObj)->setCurrentValue(value);
}
