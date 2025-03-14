// Parser.h: interface for the CParser class.
//
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include <QtGui>

#ifndef __PARSER_H__
#define __PARSER_H__

#define MAXLEN   256
#define MAXVAR   26

enum tok_type { UnknownTk, Function, Variable, Number, Operator, Rel_Op };
enum rel_op_type { LT1, GT1, LE1, GE1, EQ1, NE1 };
enum error { Ok, Divide_By_Zero , Rparen_Needed,
             Lparen_Needed, Quete_Needed, Syntax_Error, Token_Error };

class CTag;
class CRunObject;
class CSrView;
/////////////////////////////////////////////////////////////////
// 수식을 분석하는 Class
/////////////////////////////////////////////////////////////////
class CParser : public QObject  
{
	enum ParserMode {
		RunMode,
		TestMode,
		AddMode,
		PeriAddMode,
		PeriDeleteMode
	};
public:
	CParser();			// 객체 생성자
	CParser(QString& expression) {
		m_nRefCnt = 0; 
        m_bPopup = false;
		InitToken(expression);
		m_pTag			= NULL;
        m_bValidate		= false;
	};
	// 객체 생성자
	// Parameters :
	//		expression : 수식을 포함하는 문자열
	virtual ~CParser();	// 객체 파괴자
	int Run(double *result);
	// 수식을 분석하여 결과를 얻는 함수
	// Parameters :
	//		result : 결과를 받을 변수
	// Return Value :
	//		상태 값
	void InitToken(QString& expression);
	// 수식문자열을 분석용 문자열에 치환하는 함수
	// Parameters :
	//		expression : 수식 문자열
	void Release(void);
	void RemoveSelf(void);
	// 
	bool Validate(CRunObject* pRunObj);
	// 분석하여 결과를 얻는 함수
	// Parameters :
	// Return Value :
    //		성공( true ), 실패( false )
	void AddTagChain(CRunObject* pObj);
	// Tag에 연결된 수식을 분석하는 함수
	// Parameters :
	//		pObj      : Tag의 주소
	//		MapRunTag : Not Used
	//		Animation : Not Used
	//		pView     : View의 주소
	void Exec();
	// 수식을 분석하고 Tag의 변경내용을 수행하는 함수
	// Parameters :
	//		pView : View의 주소
	//		pTag  : Tag의 주소
	int GetErrorPos() { return (m_pos); };
    CSrView*		m_pView;

protected:
	QString	m_expression;				// 수식을 저장할 문자열
	char cur_token[MAXLEN];				// 예약된 문자를 저장할 배열
//	char *cur_pos;						// 분석중인 문자의 위치
//	char *org_pos;						// 원래 위치
	enum error status;					// Error Status
	enum tok_type token_type;			// 예약분자의 종류
	bool m_bInit;						// 초기화 
	bool m_bValidate;
	QByteArray m_sData; 

	int cur_pos;						// 분석중인 문자의 위치
	int org_pos;						// 원래 위치

	int				m_pos;				// 현 포지션
	int				m_length;			// 수식의 문자길이
	ParserMode		m_nMode;			// 분석 모드
	int				m_nRefCnt;
	CRunObject*		m_pRunObj;			// Tag의 주소
	CTag*			m_pTag;			// Tag의 주소
	bool			m_bPopup;			// View의 주소
	QString			m_sTagName;

	QString PrintError(enum error e);
	// 오류가 났을때의 문자열을 되돌리는 함수
	// Parameters :
	//		e : Error Code
	// Return Value :
	//		Error에 해당하는 문자열
	int	GetStatus(void);
	// 현재의 상태를 되돌리는 함수
	// Return Value :
	//		현재의 상태값
	int LookupVariable(char *s);
	// s가 Tag이름 인지 검사하고 List에 추가
	// Parameters :
	//		s : 분석 문자열
	// Return Value :
	//		성공( 0 ), 실패( -1 )
	double GetVariable(int i);
	// Tag의 값을 되돌리는 함수
	// Parameters :
	//		i : Not Used
	// Return Value :
	//		 Tag의 값
	void PutVariable(int i, double d);
	// Tag의 값을 지정하는 함수
	// Parameters :
	//		i : Not Used
	//		d : 지정할 값
	int LookupFunction(char *s);
	// s가 함수인지 검사하는 함수
	// Parameters :
	//		s : 분석 문자열
	// Return Value :
	//		함수이면( 함수번호 ) 아니면( -1 )
	const char *IsWhite(char c);
	// c가 Tab/CR/LF인지 검사하는 함수
	// Parameters :
	//		c : 검사할 문자
	// Return Value :
	//		성공( 위치 ) 실패( NULL )
	const char *IsDelimeter(char c);
	// c가 각종 Operator인지 검사하는 함수
	// Parameters :
	//		c : 검사할 문자
	// Return Value :
	//		성공( 위치 ) 실패( NULL )
	const char *IsDigit(char c);
	// c가 공학용 숫자를 포함한 숫자인지 검사하는 함수
	// Parameters :
	//		c : 검사할 문자
	// Return Value :
	//		성공( 위치 ) 실패( NULL )
	const char *IsDigitOnly(char c);
	// c가 소숫점을 포함한 숫자인지 검사하는 함수
	// Parameters :
	//		c : 검사할 문자
	// Return Value :
	//		성공( 위치 ) 실패( NULL )
	const char *IsAlpha(char c);
	// c가 밑줄을 포함한 영문자인지 검사하는 함수
	// Parameters :
	//		c : 검사할 문자
	// Return Value :
	//		성공( 위치 ) 실패( NULL )
	void PutBack(void);
	// 분석을 되돌리는 함수
	int GetToken(void);
	// 문자열을 분석하여 분리하는 함수
	// Return Value :
	//		성공( 1 ), 실패( 0 )
	void ProcExpAssign(double *result);
	// 치환/Operator를 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	int LookupRelOp(char *s);
	// s가 Operator인지 검사하는 함수
	// Parameters :
	//		s : 검사할 문자
	// Return Value :
	//		성공( 위치 ), 실패( -1 )
	void ProcExpRelOp(double *result);
	// 대소비교를 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExp2Plus(double *result);
	// 가감을 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExp2Mult(double *result);
	// 승제를 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExpPower(double *result);
	// n제곱을 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExpUnary(double *result);
	// 가감을 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExpParen(double *result);
	// 괄호를 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
	void ProcExpAtom(double *result);
	// 함수/변수/상수를 처리하는 함수
	// Parameters :
	//		result : 결과를 저장할 변수
};

#endif
