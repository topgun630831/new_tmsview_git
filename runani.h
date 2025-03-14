#ifndef RUNANI_H
#define RUNANI_H

#include <QtGui>
#include "srview.h"
#include "drawobject.h"

enum TagType
{
	DIGITAL			= 0,
	ANALOG			= 1,
	PULSE			= 2,
	STRING			= 3,
	INDIRECT		= 4,
	EXPRESSION		= 5,
	TAGTYPE_ETC		= 99
};

enum ButtonType
{
	NONE			= 0,
	BUTTON			= 1,
	SLIDER			= 2,
	WHILEDOWN		= 3,
};

enum	
{
		ALM_NORMAL,
		ALM_DIGITAL,
		HL_LOLO_ALARM,
		HL_LO_ALARM,
		HL_HI_ALARM,
		HL_HIHI_ALARM,
		DEV_MAJOR_ALARM,
		DEV_MINOR_ALARM,
		ROC_ALARM,
		GENERAL_CONTROL,
		FORCE_CONTROL,
		SCHEDULE_CONTROL,
		PERI_CONTROL
 };

///////////////////////////////////////////////////////////
// RunAnimation  Base-Object
///////////////////////////////////////////////////////////
class CDrawObject;
class CTag;
class CfgText;
class Button2;
class CRunObject : public QObject
{
    Q_OBJECT
protected:
        CRunObject();					// 객체 생성자
signals:
	void	sigDataChanged();
// Constructors
public:
        CRunObject(QWidget *parent, CDrawObject* pObj);	// 객체 생성자
        virtual ~CRunObject();			// 객체 파괴자
		int 			m_nButton;		// Control Style
		bool			bLock;			// Lock을 검사
		CDrawObject*		m_pObj;			// 실행 객체
		QPoint 			lpoint;			// 위치
		bool			m_bWhileOn;
		bool			m_bNonAni;
		QWidget*		m_pParent;
//		CSrView*		m_pView;
        virtual void    Exec(CTag* pTag);	// 가상 함수
        virtual void    Exec(QString ) {};	// 가상 함수
        virtual void    Exec(double m_Value);	// 가상 함수
		virtual	void    Exec(QPoint&  point);	// 가상 함수
		void			sendSignal();
        Button2*			m_pButton;
};

//////////////////////////////////////////////////
// RunObject   Child-Object 
// Digital Animation의 색 채우기 Class
//////////////////////////////////////////////////
class CRunFColorD : public CRunObject
{
protected:
	CRunFColorD();					// 객체 생성자
	virtual ~CRunFColorD();			// 객체 파괴자
	CfgColor* m_pCfgColor;
public:
	CRunFColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성지
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Digital 객체의 색 채우기을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

//////////////////////////////////////////////////
// RunObject   Child-Object 
// Analog Animation의 색 채우기 Class
//////////////////////////////////////////////////
class CRunFColorA : public CRunFColorD
{
protected:
	CRunFColorA();					// 객체 생성자
	double m_V1,m_V2,m_V3,m_V4;		// hihi, hi, lo, lolo
	virtual ~CRunFColorA();			// 객체 파괴자
// Constructors
public:
	CRunFColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 색 채우기을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};


//////////////////////////////////////////////////
// RunObject   Child-Object
// Digital Animation의 선색 지정 Class
//////////////////////////////////////////////////
class CRunLColorD : public CRunObject
{
protected:
	CRunLColorD();					// 객체 생성자
	virtual ~CRunLColorD();			// 객체 파괴자
	CfgColor* m_pCfgColor;
public:
	CRunLColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Digital 객체의 선색 지정을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

//////////////////////////////////////////////////
// RunObject  Child-Object
// Analog Animation의 선색 지정 Class
//////////////////////////////////////////////////
class CRunLColorA : public CRunLColorD
{
protected:
	CRunLColorA();					// 객체 생성자
	double m_V1,m_V2,m_V3,m_V4;		// hihi, hi, lo, lolo
	virtual ~CRunLColorA();			// 객체 파괴자
public:
	CRunLColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 선색 지정을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};


//////////////////////////////////////////////////////////////////////-//
// RunObject Child-Object
// Digital Animation의 문자색 지정 Class
//////////////////////////////////////////////////////////////////////-//
class CRunTColorD : public CRunObject
{
protected:
	CRunTColorD();					// 객체 생성자
	virtual ~CRunTColorD();			// 객체 파괴자
	CfgColor* m_pCfgColor;
public:
	CRunTColorD(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Digital 객체의 문자색 지정을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

///////////////////////////////////////////////////////////////////////
// RunObject Child-Object
// Analog Animation의 문자색 지정 Class
//////////////////////////////////////////////////////////////////////
class CRunTColorA : public CRunTColorD
{
protected:
	CRunTColorA();					// 객체 생성자
	double m_V1,m_V2,m_V3,m_V4;		// hihi, hi, lo, lolo
	virtual ~CRunTColorA();			// 객체 파괴자
public:
	CRunTColorA(QWidget *parent, CDrawObject* pObj, CfgColor* pCfgColor);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 문자색 지정을 실행하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};


//////////////////////////////////////////////////////////////////////
// Analog Animation의 수평이동 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunMoveAH : public CRunObject
{
protected:
	CRunMoveAH();					// 객체 생성자
	virtual ~CRunMoveAH();			// 객체 파괴자
	double x1,x2,y1,y2;				// 최대값,최소값,퍼센트최대값,페센트최소값,기준값
	bool	bLocate;				// 움직이는 방향 검사
	double 	OldMoveP;				// 전의 좌표
	double	m_nCurrent;				// Tag의 값
	double	vWidth;
	double	pWidth;
	double	Ratio;
	QRect	Rect;
	bool    bLeft;

	bool	m_bSetValue;			// 제어출력여부
	double	m_dwSetValue;			// 제어출력 값
	CfgMove* m_pCfgMove;

public:
	CTag* pTag;					// Tag의 주소
	CRunMoveAH(QWidget *parent, CDrawObject* pObj, CfgMove* pCfgMove);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 수평이동을 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
	virtual	void    Exec(QPoint&  point);
	// Analog 객체의 수평이동을 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		point : 위치
	void OutputValue();
};

//////////////////////////////////////////////////////////////////////
// Analog Animation의 수직이동 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunMoveAV : public CRunObject
{
protected:
	CRunMoveAV();					// 객체 생성자
	virtual ~CRunMoveAV();			// 객체 파괴자
	double x1,x2,y1,y2;				// 최대값,최소값,퍼센트최대값,페센트최소값,기준값
	bool	bLocate;				// 움직이는 방향 검사
	double	OldMoveP;				// 전의 좌표
	double	m_nCurrent;				// Tag의 값
	double	vHeight;
	double	pHeight;
	double	Ratio;
	QRect	Rect;
	bool    bUp;

	bool	m_bSetValue;			// 제어출력여부
	double	m_dwSetValue;			// 제어출력 값
	CfgMove* m_pCfgMove;

public:
	CTag* pTag;					// Tag의 주소
	CRunMoveAV(QWidget *parent, CDrawObject* pObj, CfgMove* pCfgMove);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 수직이동을 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
	virtual	void    Exec(QPoint&  point);
	// Analog 객체의 수직이동을 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		point : 위치
	void OutputValue();
};

//////////////////////////////////////////////////////////////////////
// Analog Animation의 수평크기 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunSizeAH : public CRunObject
{
protected:
	CRunSizeAH();					// 객체 생성자
	virtual ~CRunSizeAH();			// 객체 파괴자
	double x1,x2,y1,y2;				// 최대값,최소값,퍼센트최대값,페센트최소값

	int		Check;					// 기준 : 좌(0),중간(1),우(2)
	int		m_SaveWidth;			// 폭
	double 	m_nCurrent;				// Tag의 값
	CfgSize* m_pCfgSize;

public:
	CRunSizeAH(QWidget *parent, CDrawObject* pObj, CfgSize* pCfgSize);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 수평크기 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
protected:
			void OnSizeExpression(QRect& rect,float Per);
			// 크기를 변환하는 함수
			// Parameters :
			//		rect : 사각영역
			//		Per  : 퍼센트
};

//////////////////////////////////////////////////////////////////////
// Analog Animation의 수직크기 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunSizeAV : public CRunObject
{
protected:
	CRunSizeAV();					// 객체 생성자
	virtual ~CRunSizeAV();			// 객체 파괴자
	double x1,x2,y1,y2;				// 최대값,최소값,퍼센트최대값,페센트최소값

	int		Check;					// 기준 : 위(0),중간(1),아래(2)
	int		m_SaveHeight;			// 높이
	double	m_nCurrent;				// Tag의 값
	CfgSize* m_pCfgSize;
public:
	CRunSizeAV(QWidget *parent, CDrawObject* pObj, CfgSize* pCfgSize);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체의 수직크기 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
protected:
			void OnSizeExpression(QRect& rect,float Per);
			// 크기를 변환하는 함수
			// Parameters :
			//		rect : 사각영역
			//		Per  : 퍼센트
};

//////////////////////////////////////////////////////////////////////
// Animaiton 객체 퍼센트 색 채우기 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunFill : public CRunObject
{
protected:
	CRunFill();						// 객체 생성자
	virtual ~CRunFill();			// 객체 파괴자
	double x1,x2,y1,y2;				// 최대값,최소값,퍼센트최대값,페센트최소값

public:
	CRunFill(QWidget *parent, CDrawObject* pObj);		// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체 퍼센트 색 채우기를 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

//////////////////////////////////////////////////////////////////////
// Animation 객체 돌리기 Class
//////////////////////////////////////////////////////////////////////
class CRunRotate : public CRunObject
{
protected:
	CRunRotate();					// 객체 생성자
	virtual ~CRunRotate();			// 객체 파괴자
	double 	m_nCurrent;				// Tag의 값
	int		rad;					// 반지름
	double  angle;					// 각도
	QPoint	point1,spoint;			// 좌표
	CfgRotate* m_pCfgRotate;
public:
	CRunRotate(QWidget *parent, CDrawObject* pObj, CfgRotate* pCfgRotate);	// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Analog 객체를 돌리는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
protected: 
	QPoint point;					// 좌표
	double max,min;					// 최대값,최소값
	double max_ang,min_ang;			// 최대각,최소각
};

//////////////////////////////////////////////////////////////////////
// Digital Animation 숨기기 Class
//////////////////////////////////////////////////////////////////////
class CRunHideD : public CRunObject
{
protected:
	CRunHideD();					// 객체 생성자
	virtual ~CRunHideD();			// 객체 파괴자
	double 	m_nCurrent;				// Tag 값
	CfgHide* m_pCfgHide;
public:
	CRunHideD(QWidget *parent, CDrawObject* pObj, CfgHide* pCfgHide);		// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double m_Value);
	// Digital 객체를 감추는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

//////////////////////////////////////////////////////////////////////
// Analog Animation 숨기기 Class
//////////////////////////////////////////////////////////////////////
class CRunHideA : public CRunHideD
{
protected:
	CRunHideA();					// 객체 생성자
	virtual ~CRunHideA();			// 객체 파괴자
	double 	 m_nCurrent;			// Tag 값
	double m_V1,m_V2,m_V3,m_V4;		// hihi, hi, lo, lolo
public:
	CRunHideA(QWidget *parent, CDrawObject* pObj, CfgHide* pCfgHide);		// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double   pTag);
	// Analog 객체를 감추는 함수
	// Parameters :
	//		pView : View 주소
	//		pTag  : Tag 주소
};

//////////////////////////////////////////////////////////////////////
// Button Animation 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunButton : public CRunObject
{
protected:
	CRunButton();					// 객체 생성자
	virtual ~CRunButton();			// 객체 파괴자
	double	m_nCurrent;				// Tag 값
	bool	m_bUI;					// 사용정지 검사
public:
	CRunButton(QWidget *parent, CDrawObject* pObj);	// 객체 생성자
	virtual void    Exec(bool check);
	// Button객체를 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//	    check : 사용( TRUE ), 정지( FALSE )
};

//////////////////////////////////////////////////////////////////////
// 문자 Animaiton 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunText : public CRunObject
{
protected:
	CRunText();						// 객체 생성자
	QString  m_Format;				// 표시문자 Format
	// 2005. 02. 16. (그래픽 버전2부터)
	bool		m_bComma;				// 콤마(,) 표시
	virtual ~CRunText();			// 객체 파괴자
	CfgText*	m_pCfgText;
	int			m_nSensorIndex;
	void			makeFormat();
    QString        makeComma(QString& str);
public:
	CRunText(QWidget *parent, CDrawObject* pObj, CfgText* pCfgText);		// 객체 생성자
	virtual void    Exec(CTag* pTag);
	virtual void    Exec(double value);
    virtual void    Exec(QString value);

	// 문자 객체를 처리하는 함수
	// Parameters :
	//		pView : View 주소
	//	    pTag  : Tag의 주소
};


//////////////////////////////////////////////////////////////////////
// Not Used
//////////////////////////////////////////////////////////////////////
class CRunUIButton : public CRunObject
{
protected:
	CRunUIButton();
	virtual ~CRunUIButton();
public:
	CRunUIButton(QWidget *parent, CDrawObject* pObj);
	virtual void    Exec(CTag* pTag);
};

//////////////////////////////////////////////////////////////////////
// Not Used
//////////////////////////////////////////////////////////////////////
class CRunUIDataD : public CRunObject
{
protected:
	CRunUIDataD();
	virtual ~CRunUIDataD();
	QPoint	m_point;
	CfgData*	m_pCfgData;
public:
	CTag* pTag2;
	CRunUIDataD(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData);
	virtual void    Exec(CTag* pTag);
	void OutputValue(bool val);
};

//////////////////////////////////////////////////////////////////////
// Not Used
//////////////////////////////////////////////////////////////////////
class CRunUIDataA : public CRunUIDataD
{
protected:
	CRunUIDataA();
	virtual ~CRunUIDataA();
public:
	CRunUIDataA(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData);
	virtual void    Exec(CTag* pTag);
};

class CRunUIDataS : public CRunUIDataD
{
protected:
    CRunUIDataS();
    virtual ~CRunUIDataS();
public:
    CRunUIDataS(QWidget *parent, CDrawObject* pObj, CfgData* pCfgData);
    virtual void    Exec(CTag* pTag);
};


//////////////////////////////////////////////////////////////////////
// User Graphic 화일 열고 닫기 Class
//////////////////////////////////////////////////////////////////////
class CRunOpen : public CRunObject
{
    Q_OBJECT
protected:
	CRunOpen();						// 객체 생성자
	virtual ~CRunOpen();			// 객체 파괴자
	Cfgfile*	m_pCfgfile;
signals:
	void	sigOpenWindow(const QString&, int);
	void	sigCloseWindow();
	void	sigTMSData();
	void	sigTMSInfo();
	void	sigSewer();
	void	sigU_City();
	void	sigRainSensor();
	void	sigCommMon();
    void	sigTrend();
    void	sigSysMenu();
    void	sigPointMon();
    void	sigAlarmSummay();
    void	sigLog();
    void	sigEpsTestSetup();
public:
	CRunOpen(QWidget *parent, CDrawObject* pObj, Cfgfile* pCfgfile);		// 객체 생성자
	virtual void    Exec(CTag* pTag);
	// 화일을 열고 닫는 함수
	// Parameters :
	//		pView : View 주소
	//	    pTag  : Tag의 주소
};

//////////////////////////////////////////////////////////////////////
// Animation 객체의 깜박임 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunBlink : public CRunObject
{
public:
	CRunBlink();					// 객체 생성자
	CfgBlink* m_pCfgBlink;
	double m_V1,m_V2,m_V3,m_V4;		// hihi, hi, lo, lolo
	virtual ~CRunBlink();			// 객체 파괴자
	double m_nCurrent;				// Tag의 값
	CTag* m_pTag;					// Tag의 주소
	QColor m_SaveColor;			// 원래의 색
	CRunBlink(QWidget *parent, CDrawObject* pObj, CfgBlink* pCfgBlink);		// 객체 생성자

	bool	SetCheck();
	// 사각영역을 검사하는 함수
	// Parameters :
	//		rect  : 사각영역
	//		pView : View 주소
	virtual void    Exec(bool bBlink);
	// 객체의 깜박임을 처리하는 함수
	// Parameters :
	//		pView  : View 주소
	//	    bBlink : 보이기( TRUE ), 숨기기( FALSE )
};

//////////////////////////////////////////////////////////////////////
// Animation 객체의 Frame 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunFrame : public CRunObject
{
protected:
	CRunFrame();					// 객체 생성자
	virtual ~CRunFrame();			// 객체 파괴자
	double	m_nCurrent;				// Tag의 값
	int		m_nFramePos;			// Frame의 현재 위치
	int		m_nSkipCnt;

//	QList	m_List;					// Frame Image를 보관하는 객체

public:
	CTag* m_pTag;				// Tag의 주소
	CRunFrame(QWidget* parent, CDrawObject* pObj);		// 객체 생성자

	virtual void Exec(double dwValue);
	virtual void Exec();
	// 객체의 Frame을 처리하는 함수
	// Parameters :
	//		pView  : View 주소
	bool SetCheck();
	// 객체의 Frame처리 여부 검사
};

//////////////////////////////////////////////////////////////////////
// Animation 객체의 Frame 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunPipeFlow : public CRunObject
{
protected:
	CRunPipeFlow();						// 객체 생성자
	virtual ~CRunPipeFlow();			// 객체 파괴자
	double	m_nCurrent;					// Tag의 값
	int		m_nPipeFlowPos;				// Frame의 현재 위치
	int		m_nSkipCnt;					// NotUse
	bool	m_bAnimation;				// 에니메이션이 동작 여부

public:
	CTag* m_pTag;					// Tag의 주소

//	CDIBSectionLite m_RenderTarget;
//	CPipeRoutine	m_Flow;
	QColor		m_clrPrimary,		// 처음 색깔
					m_clrSecondary;		// 마지막 색깔

	CRunPipeFlow(QWidget *parent, CDrawObject* pObj);		// 객체 생성자
	int GetSpeed();
	// 객체의 속도를 가져오는 함수
	void ShowObject();
	// 객체의 배경 이미지를 보이거나 감추는 함수
	void Draw();
	// 그림을 그리는 함수
	bool SetCheck();
	// 객체의 Flow처리 여부 검사
	virtual void Exec(CTag* pTag);
	// 객체의 Flow을 처리하는 함수
	// Parameters :
	//		pView  : View 주소
	virtual void Exec(double dwValue);
	// 객체의 Flow을 수식일 때 불리우는 함수
	// Parameters :
	//		pView  : View 주소
	virtual void Exec();
	// 객체의 Flow을 처리하는 함수
	// Parameters :
	//		pView  : View 주소
};
//////////////////////////////////////////////////////////////////////
// Gauge Animaiton 처리 Class
//////////////////////////////////////////////////////////////////////
class CRunGauge : public CRunObject
{
protected:
    CRunGauge();						// 객체 생성자
    QString  m_Format;				// 표시문자 Format
    bool		m_bComma;				// 콤마(,) 표시
    virtual ~CRunGauge();			// 객체 파괴자
    CfgGauge*	m_pCfgGauge;
    int			m_nSensorIndex;
    void			makeFormat();
    QString        makeComma(QString& str);
public:
    CRunGauge(QWidget *parent, CDrawObject* pObj, CfgGauge* pCfgText);		// 객체 생성자
    virtual void    Exec(CTag*);
    virtual void    Exec(double value);

    // 문자 객체를 처리하는 함수
    // Parameters :
    //		pView : View 주소
    //	    pTag  : Tag의 주소
};

#endif

