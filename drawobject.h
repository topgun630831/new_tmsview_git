#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H

#include <QtGui>
#include "srview.h"
#include "qcgaugewidget.h"
class Button2;

class  CfgObject : public QObject
{
public:								// 수정일자: 1997. 7. 23
	QString 	sName;
	CfgObject() {};			// 채우기 조건 or 태그명 
};

//////////////////////////////////////////////////////////////
//  Animation Percent Fill 객체
//
//
class  CfgFill : public CfgObject
{
public:								// 수정일자: 1997. 7. 23
//	QString 	sName;				// 채우기 조건 or 태그명 
	int			nPosition;			// 채우는 위치 (상하좌우) 0,1,2,3
	int			nDisplayValue;		// 채울때 표시여부 (값,%,Not)
    double		nVMax;				// 입력값    최대
    double		nVMin;				//		     최소
    double		nPMax;				// 채우기 %  최대
    double		nPMin;				//           최소
	QColor    cBack;				// 조건 칼라 (백,채우기,문자)
	QColor    cFill;				// 채우기 칼라 
	QColor    cText;				// 문자열 칼라 
	bool		m_bIndirect;		// 간접태그
	int			m_nTagType;

	CfgFill();

    virtual void    linkLoad(QJsonObject& jObj);
};

//////////////////////////////////////////////////////////////
//  Animation Move 객체
//
//
class  CfgMove : public CfgObject
{
public:
//	QString sName;				// 움직일 조건 or 태그명 
	bool    nTouch;				// Mouse Touch (TRUE/FALSE)
	bool	nLocate;			// Left / Right   --  Top / Bottom
    double	nVMax;				// 입력값		 최대값
    double	nVMin;				//				 최소값
    double	nPMax;				// Pixel 단위    최대값
    double	nPMin;				//				 최소값
	int     nCodition;			// 중심 (0,1,2) 앞,중간,뒤
	bool		m_bIndirect;		// 간접태그
	int			m_nTagType;
	int			m_nType;

	CfgMove();					// 생성자 
	~CfgMove();					// 소멸자
    virtual void    linkLoad(QJsonObject& jObj);
};


//////////////////////////////////////////////////////
// Animation Size 객체  
//
//
class  CfgSize : public CfgObject
{
public:
//	QString sName;					// 사이즈변동 조건 or 태그명 
    double	nVMax;					// 입력값         최대값
    double	nVMin;					//				  최소값
    double	nPMin;					// 퍼센트		  최소비율
    double	nPMax;					//				  최대비율
	int     nDefault;               // 현재 사용 안함 (꼭지점 변화) 
	bool	m_bIndirect;				// 간접태그
	int			m_nTagType;
	int			m_nType;

	CfgSize();						// 생성자 
	~CfgSize();						// 소멸자 
    virtual void    linkLoad(QJsonObject& jObj);
};


/////////////////////////////////////////////////////
// Animation Rotate 객체  
//
//
class  CfgRotate : public CfgObject
{
public:
//	QString sName;		// 수식또는 태그명 
	int     nVCCW;		// 입력값 최대값 
	int     nVCW;		//		  최소값 
	int     nRCCW;		// 각도   최대각도 
	int     nRCW;		//		  최소각도 
	QPoint  pt;			// (X,Y)
	bool	m_bIndirect;				// 간접태그
	int			m_nTagType;

	CfgRotate();		// 생성자 
	~CfgRotate();		// 소멸자
    virtual void    linkLoad(QJsonObject& jObj);
};


//////////////////////////////////////////////////////
// Animation Color 객체 
//
//
class  CfgColor : public CfgObject
{
public:
//	QString 	sName;				// 수식 또는 태그명 
	int 		btype;				// Analog, Digital
	int         bcheck;				// Analog 일때 (Value, Alarm 이냐 )
	int         nAlarm;				// Alarm  일때 (Value, Deviation, ROC 이냐)
	bool		bValue;
	bool		bDeviation;
	bool		bROC;

    double		vhihi_hi;			//  1 입력값에 위한 분류  5단칼라 경계값
    double		vhi_normal;			//	2
    double		vnormal_lo;			//  3
    double		vlo_lolo;			//  4

	QColor    cHiHi;				// ANALOG 칼라 (Value,Alarm 공용)
	QColor    cHi;				//
	QColor    cNormal;			//
	QColor    cLo;				//
	QColor    cLoLo;				//

	QColor    cOn;				// DIGITAL 칼라 (Value,Alarm 공용)
	QColor    cOff;				//
	bool		m_bIndirect;		// 간접태그
	int			m_nType;			// 0: Fill Color, 1:Line Color, 2: Text Color
	int			m_nTagType;
	
	CfgColor();						// 생성자 
	~CfgColor();					// 소멸자
    virtual void    linkLoad(QJsonObject& jObj);
};


//////////////////////////////////////////////////////
// Animation Blink 객체 
//
//
class  CfgBlink : public CfgObject
{
public:
//	QString 	sName;				// 수식 또는 태그명
	bool		btype;				// Analog, Digital
	bool        bcheck;				// TRUE - visible , FALSE - color
	int			nspeed;				// 0 - 400m/s   1 - 800m/s   2 - 1200m/s

	QColor    cFill;				// 채우기 칼라 
	QColor    cLine;				// 라인 칼라 
	QColor    cText;				// 문자열 칼라 

    double		vhihi_hi;			//  1 입력값에 위한 분류  5단칼라 경계값
    double       vhi_normal;			//  2
    double       vnormal_lo;			//  3
    double		vlo_lolo;           //  4

	bool		cHiHi;				// HiHi -- ANALOG Alarm
	bool		cHi;				// Hi
	bool		cNormal;			// Normal (No Alarm)
	bool		cLo;				// Lo
	bool		cLoLo;				// LoLo

	bool		cOn;				// On    일때 ------ Digital or 수식값 (TRUE/FALSE)
	bool		cOff;				// Off   일때 
	bool		cAlarm;				// Alarm 일때 
	bool	m_bIndirect;				// 간접태그
	int			m_nTagType;

	CfgBlink();						// 생성자 
    virtual void    linkLoad(QJsonObject& jObj);
};


//////////////////////////////////////////////////////
// Animation Hide 객체 
//
//
class  CfgHide : public CfgObject
{
public:
//	QString 	sName;				// 수식 또는 태그명 

	bool		btype;				// Digital,Analog
	bool		bValue;				// Value,Alarm
	int 		nOn;				// Digital - (0-On,Alarm 1-Off,Normal)

    double		vhihi_hi;			//  1 입력값에 위한 분류  5단칼라 경계값
    double        vhi_normal;			//  2
    double        vnormal_lo;			//  3
    double        vlo_lolo;			//  4

	bool		bHiHi;			    // HiHi Alarm Blink On/Off 
	bool		bHi;				// Hi   Alarm Blink On/Off 
	bool		bNormal;			// No   Alarm Blink On/Off 
	bool		bLo;				// Low  Alarm Blink On/Off 
	bool		bLoLo;				// LoLo Alarm Blink On/Off 
	bool		m_bIndirect;			// 간접태그
	int			m_nTagType;

	CfgHide();						// 생성자 
    virtual void    linkLoad(QJsonObject& jObj);
};


/////////////////////////////////////////////////
// Animation 문자열 표시 
//
class  CfgText : public CfgObject
{
public:
//	QString 	sName;		// 태그명 
	bool		btype;		// TRUE (Value) , FALSE (Message)
	QString		sFormat;	// Anlaog 일때 보여줄 포맷
	
	bool	    bUnit;		// Analog 일때 (Unit Display - TRUE) (Only Value - FALSE)

	bool		bZero;		// Value 앞에 0 붙이기 
	QString     sOnMsg;		// Digital 일때  OnMsg
	QString     sOffMsg;    // Digital 일때  OffMsg
	bool		m_bIndirect;			// 간접태그
	// 2005. 02. 16. (그래픽 버전 2부터)
	bool		bComma;		// 3 자리마다 콤마(,) 찍기
	int			m_nTagType;

	CfgText();				// 생성자 
    virtual void    linkLoad(QJsonObject& jObj);
};


//////////////////////////////////////
// 데이타 환경설정 
//////////////////////////////////////
class  CfgData : public CfgObject
{
public:
//	QString 	sName;			// 태그명 1
	QString 	sName2;			// 태그명 2

	QString		HotKey;			// HotKey (단축키) 등록 
	
	int			nSecurity;		// 등급

	int			ntype;			// (Digital, Analog)
	int			nDigital;		// Data Output Digital Type
								// 0 - set
								// 1 - Reverse
								// 2 - Reset
								// 3 - Toggle
								// 4 - 1 Point Pulse
								//  확인 요망 
								// 5 - 2 Point MainTain
								// 6 - 2 Point Pulse
								
	int			nAnalog;		// Analog Output type
								// 0 - (=) 값 치환
								// 1 - (+) 현재값에 더하기 
								// 2 - (-) 현재값에 빼기 
								// 3 - (*) 현재값에 곱하기 
								// 4 - (/) 현재값에 나누기(0 제외)
	double 		nValue;			// 출력 내보내고자 하는 값 
    QString 	sValue;			// 출력 내보내고자 하는 값
    int			nTime;			// 지속시간

	QString		sMessage;		// 확인 Dialog에 질문 메세지 
	QString		sScript;		// 스크립트
	bool		bConfrm;
	bool		m_bIndirect;			// 간접태그1
	bool		m_bIndirect2;			// 간접태그2
	int			m_nTagType;


	CfgData();						// 생성자 1
	CfgData(const CfgData & cfg);	
    virtual void    linkLoad(QJsonObject& jObj);
};

//////////////////////////////////////
// Animation User Graphic File 오픈 / 클로스 
//
class  Cfgfile : public CfgObject
{
public:
	
	int			tOpen;			// Open Window Type-> UserGraphics,Trend,......
	int			nOpen;			// TRUE-Open   FALSE-No Open
	int			tClose;			// Close Window Type ->UserGraphics,Trend.......
								// Function Call 기능사용 부터 버전 관리용으로 전환(2003.07.23)
								// 0: Function Call 기능이 없는 버전, 1: Function Call 기능
	bool	    bClose;			// TRUE-Close  FALSE-No Close

	QString 	sOpen;			// Open  Window FileName
	QString		sClose;			// Close Window FileName

	bool		bCurrent_Close;	// 현재 윈도우 닫기 체크 (TRUE - Close, FALSE - 그대로 유지)
	bool		m_bIndirect;

	// Function Call 기능을 위한 변수 선언(2003.07.23)
	int			nOption;		// 0: Function Call, 1: Open Window, n: ...
	QString		strFuncName;	// Function Call 옵션 사용시 호출 할 함수이름(Moudule.Function(Topic, ...))

	// 윈도우 이름 재정의를 위한 변수 선언(2005. 02. 18)
	bool		bUseTitle;		// TRUE : 사용, FALSE : 사용하지 않음
	QString		strTagName;		// 윈도우 타이틀 이름
	QString		strStringValue;	// 윈도우 타이틀 이름
	int			m_nTagType;

	Cfgfile();					// 생성자
	~Cfgfile();
	Cfgfile(const Cfgfile & cfg);
    virtual void    linkLoad(QJsonObject& jObj);
};

//////////////////////////////////////////
//  Animation - Picture Animaiton 
//
class  CfgFrame : public CfgObject
{
public:
//	QString 	sName;			 // 태그명 또는 수식 
	QString 	sFrameName;		 // Frame 그룹명 
	int			nTime;			 // 애니메이션 대기시간 - 1/1000초)
	bool		btype;			 // Discrete, Real
	int 		nOn;			 // Discrete - (0-On, Alarm 1-Off,Normal)
	bool		m_bIndirect;	 // 간접태그
	int			m_nTagType;

	CfgFrame();					 // 생성자 
	~CfgFrame();				 // 소멸자 
	CfgFrame(const CfgFrame & cfg);
    virtual void    linkLoad(QJsonObject& jObj);
};

//////////////////////////////////////////
//  Animation - Pipe Flow Animaiton 
//
class  CfgPipeFlow : public CfgObject
{
public:
//	QString 	sName;				// 태그명 또는 수식 
	int			m_nSpeed;			// 애니메이션 대기시간 - 1/1000초)
	QColor	cStart;				// 시작 색
	QColor	cEnd;				// 마지막 색
	int			m_nDirection;		// 흐름 방향
	bool		m_bIndirect;		// Not Use		

	bool		m_bSwap;			// 시작 색과 마지막색을 바꿈
	int			m_nArray;			// 색 배열이 순차 / 대칭
	int			m_nThick;			// 색의 두께
	bool		m_bBackground;		// 배경 그리기
	int			m_nTagType;
	int			nNO;				// 번호 
//	QString 	sName;				// 객체명 
	QString		sFilename;			// 파일명
	bool		bTran;				// 투명칼라 선택 (TRUE -투명 FALSE - 불투명)
	QColor    cTran;				// 투명칼라 (RGB)

	CfgPipeFlow();					// 생성자 
	~CfgPipeFlow();					// 소멸자
	
	CfgPipeFlow(const CfgPipeFlow & cfg);
    virtual void    linkLoad(QJsonObject& jObj);
};
/*
//////////////////////////////////////////////////
//  Animation - 프레임오브젝트 (CfgFrame 종속)
//  각 객체의 구성 (Picture Animation Object)
class  CfgObject : public CfgObject
{
public:
	
	
	CfgObject();					// 생성자 
    virtual void    linkLoad(QJsonObject& jObj);

};

*/
//////////////////////////////////////////
//  Animation - ToolTip
//
class  CfgToolTip : public CfgObject
{
public:
	QString 	sToolTip;		// 툴팁 문자열 (총 69자까지 허용)

	CfgToolTip();				// 생성자
    virtual void    linkLoad(QJsonObject& jObj);
};

/////////////////////////////////////////////////
// Animation 문자열 표시
//
class  CfgGauge : public CfgObject
{
public:
//	QString 	sName;		// 태그명
    bool		btype;		// TRUE (Value) , FALSE (Message)
    QString		sFormat;	// Anlaog 일때 보여줄 포맷

    bool	    bUnit;		// Analog 일때 (Unit Display - TRUE) (Only Value - FALSE)

    bool		bZero;		// Value 앞에 0 붙이기
    QString     sOnMsg;		// Digital 일때  OnMsg
    QString     sOffMsg;    // Digital 일때  OffMsg
    bool		m_bIndirect;			// 간접태그
    // 2005. 02. 16. (그래픽 버전 2부터)
    bool		bComma;		// 3 자리마다 콤마(,) 찍기
    int			m_nTagType;

    CfgGauge();				// 생성자
    virtual void    linkLoad(QJsonObject& jObj);
};



///////////////////////////////////////////////////////////////////////
// CDrawObject   Base/Object 
//
//       Draw 객체의 모체가 된다.
///////////////////////////////////////////////////////////////////////
class CDrawObject : public QObject
{
    Q_OBJECT
public:
        CDrawObject(CSrView* pSrView);					// 생성자
        virtual ~CDrawObject();			// 소멸자 
public:

// Attributes
        QString         m_sName;		// 현재 객체 이름
//		QString			m_sDesc;		// 보조 설명
		int				m_3dtype1;		// 현재 객체에 효과를 줄 수 있는 갯수 
		int				m_3dtype2;		// 3D 효과 번호 (현재 사각형만)
        QString         m_drawshape;	// Draw Mode
        QRect           m_position;		// 객체 위치와 크기 
 		int				m_nBkMode;		// BackGround Mode
        QPen		    m_logpen;		// Line Attrib
        QBrush	        m_logbrush;		// Brush Attrib 
		double			m_nValue;		// Percent Fill Value
		LINKLIST		m_LinkList;
		bool			m_bDelete;
		bool			m_bDrawPercent;		//	Percent Fill Draw Change Variable 
        double			m_fPercent;			//  Percent Value
        CSrView*		m_pSrView;
		bool			m_bText;

        virtual bool    objectLoad(QJsonObject jObj);
		void			addRunList(QString& Name, CRunObject* pRun, int nTagType);
        virtual bool	linkLoad(QWidget *parent, QJsonObject& jObj, RUNLIST& runList, RUNLIST& slowList, RUNLIST& normalList, RUNLIST& fastList, bool isButton=false);
        virtual void    Draw(QPainter& p);
		virtual bool	HitTest(QPoint& point);
};

class CMyRectObject : public CDrawObject
{
public:
        CMyRectObject(CSrView* pSrView);					// 생성자
public:

// Attributes

        virtual bool    objectLoad(QJsonObject jObj);
        virtual void    Draw(QPainter& p);
};

class CMyRoundRectObject : public CDrawObject
{
public:
        CMyRoundRectObject(CSrView* pSrView);					// 생성자
public:

// Attributes

        virtual bool    objectLoad(QJsonObject jObj);
        virtual void    Draw(QPainter& p);
		QPoint          m_corner; // for roundRect corners
};

class CMyEllipseObject : public CDrawObject
{
public:
        CMyEllipseObject(CSrView* pSrView);					// 생성자
public:

// Attributes

        virtual bool    objectLoad(QJsonObject jObj);
        virtual void    Draw(QPainter& p);
};

class CMyTextObject : public CDrawObject
{
public:
        CMyTextObject(CSrView* pSrView);					// 생성자
public:

	QString			m_Text;				// 객체이름 
	QColor			m_TextColor;		// 객체 칼라 
	QFont			m_TextFont;			// 객체 폰트 
	int				m_dwEditStyle;		// Edit Style (Align)
	bool			m_EditMode;			// 투명  / 불투명 
	int				wFormat;			// 정렬 방식 
	int				m_nPointSize;
	int				m_Align;

// Attributes

        virtual bool    objectLoad(QJsonObject jObj);
        virtual void    Draw(QPainter& p);
		void			objectLoadCommon(QStringList& list, int index);
		void			objectLoadNewLine(QStringList& list);
};

class CMyButtonObject : public CDrawObject
{
    Q_OBJECT
public:
        CMyButtonObject(CSrView* pSrView);					// 생성자
        ~CMyButtonObject();

public:

	QString			m_Text;				// 객체이름 
	int				m_dwEditStyle;		// Edit Style (Align)
	int    m_wType;        // ON/OFF, ON
	QString m_sActOn;		// On 문자열 
	QString m_sActOff;		// Off 문자열 
	bool    m_bOnOff;       // On : TRUE
	bool    m_bDown;		// Down, Up
	QColor			m_TextColor;		// 객체 칼라 
	QFont			m_TextFont;			// 객체 폰트 
	bool			m_EditMode;			// 투명  / 불투명 
	int				wFormat;			// 정렬 방식 
	int				m_nPointSize;
    Button2 *button;
// Attributes

    virtual bool    objectLoad(QJsonObject jObj);
    virtual void    Draw(QPainter& ) {};
};

class CMyBitmapObject : public CDrawObject
{
public:
        CMyBitmapObject(CSrView* pSrView);					// 생성자
		bool		m_bSrc;
		QColor		m_trans_color;
//		QImage		m_Image;
		QPixmap		m_Image;
public:

// Attributes

    virtual bool    objectLoad(QJsonObject jObj);
	virtual void    Draw(QPainter& p);
};


class CMyPolyObject : public CDrawObject
{
public:
	bool    m_rectmode;
    CMyPolyObject(CSrView* pSrView);					// 생성자
	virtual ~CMyPolyObject();

    virtual bool    objectLoad(QJsonObject jObj);
	virtual void    Draw(QPainter& p);

public:
	bool    m_symbreak;
	int     m_nCount;
	QPoint* m_points;
	QPolygon m_Polygon;
protected:
	QSize   m_size;
	QPoint  m_basep;
	int     m_nAllocPoints;
};

/*-----------------------------------------------------------------------*/
/*------ CMyPolygonObject  ----------------------------------------------*/
/*-----------------------------------------------------------------------*/
class CMyPolygonObject : public CMyPolyObject
{
public:
    CMyPolygonObject(CSrView* pSrView);
	virtual ~CMyPolygonObject();
	
	virtual void    Draw(QPainter& p);
};

class CMyLineObject : public CDrawObject
{
public:
        CMyLineObject(CSrView* pSrView);					// 생성자
public:

// Attributes

    virtual bool    objectLoad(QJsonObject jObj);
    virtual void    Draw(QPainter& p);
};

/*-----------------------------------------------------------------------*/
/*------ CMyPieObject  --------------------------------------------------*/
/*-----------------------------------------------------------------------*/
class CMyPieObject : public CMyPolyObject
{
public:
    CMyPieObject(CSrView* pSrView);
	virtual ~CMyPieObject();
	
    virtual void    Draw(QPainter& p);
};

/*-----------------------------------------------------------------------*/
/*------ CMyArcObject  --------------------------------------------------*/
/*-----------------------------------------------------------------------*/
class CMyArcObject : public CMyPieObject
{
public:
    CMyArcObject(CSrView* pSrView);
	virtual ~CMyArcObject();
	
    virtual void    Draw(QPainter& p);
};

/*-----------------------------------------------------------------------*/
/*------ CMyChordObject  ------------------------------------------------*/
/*-----------------------------------------------------------------------*/
class CMyChordObject : public CMyPieObject
{
protected:
	CMyChordObject();
public:
    CMyChordObject(CSrView* pSrView);
	virtual ~CMyChordObject();
	
    virtual void    Draw(QPainter& p);
};

/*-----------------------------------------------------------------------*/
/*------ CMyBezierObject  -----------------------------------------------*/
/*-----------------------------------------------------------------------*/

class CMyBezierObject : public CMyPolyObject
{
public:
    CMyBezierObject(CSrView* pSrView);
	virtual ~CMyBezierObject();
	
	int     m_count;
    virtual void    Draw(QPainter& p);
};

/*-----------------------------------------------------------------------*/
/*------ CGaugeObject  -----------------------------------------------*/
/*-----------------------------------------------------------------------*/

class CMyGaugeObject : public CDrawObject
{
public:
    CMyGaugeObject(CSrView* pSrView);
    virtual ~CMyGaugeObject();

    int     m_nType;
    bool objectLoad(QJsonObject jObj);
    virtual void    Draw(QPainter& p);
    void setCurrentValue(double value);
private:
    QcGaugeWidget * mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;
};
#endif
