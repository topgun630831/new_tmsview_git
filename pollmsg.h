#ifndef POLLMSG_H
#define POLLMSG_H

#include <QtGui>
#include "wminet.h"
#include "tag.h"

class CPollMsg : public QObject
{
    Q_OBJECT

private:
    CWMINet			*m_pWMINet;				// 프로토콜 객체의 주소
    int				m_nMaxLen;				// 송/수신 버퍼의 최대 길이
    QList<QString>	m_TagList;
public:

    int				ScanTimer;				// 시간간격을 조사하는 변수
    int				ScanReload;				// 지정된 시간간격을 저장할 변수
    bool			Due;					// 지정된 시간이 되었음
    int				Retries;				// 반복 횟수
    char			Message[ MAXMSGLEN ];	// 송신할 문자를 저장할 버퍼
    int				m_nIndex;				// 송신할 문자를 저장할 버퍼의 현재 위치/크기
    quint32			m_nLast;				// Checksum 전까지의 크기
    bool			m_bDeleteAble;			// 현 객체를 삭제할수 있는지의 여부
    bool			m_bEnd;
    bool			m_bAdd;
    WMINET_HEADER	*m_pHeader;

    CPollMsg(CWMINet *pPro, quint32 seq, quint16 cmd, quint16 SubCmd, int Time, bool DeleteAble, bool bNoRetry);
    // 객체 생성자
    // Parameters :
    //		pPro       : 프로토콜 객체의 주소
    //		seq        : 프레임의 순서번호
    //		cmd        : 프레임의 명령
    //		Time       : 시간간격
    //		DeleteAble : 현 객체의 삭제여부
    //		bNoRetry   : 실패시 반복 여부
    ~CPollMsg();	// 객체 파괴자
    void SetSeq(quint32 Seq);
    // 프레임의 순서번호를 지정하고 Checksum을 구해서 프레임을 완료하는 함수
    // parameters :
    //		seq : 지정할 프레임의 순서번호로 기본값을 사용하면 자동으로 지정함
    void End( void );
    // Checksum을 만들기 전까지의 문자수를 지정하는 함수
    bool Add( char* str, int len );
    // 송신 버퍼에 내용을 추가하는 함수
    // Parameters :
    //		str : 추가할 내용의 버퍼
    //		len : 추가할 내용의 길이
    // Return Value :
    //		성공( TRUE ), 실패( FALSE )
    bool AddWrite( quint32 atom, bool type, double value, QString &str, bool ack );
    // 출력할 내용을 프레임으로 만드는 함수
    // Parameters :
    //		atom  : 출력할 태그의 식별코드
    //		type  : 출력할 값의 종류( 0:value, 1:str )
    //		value : 출력할 숫자 값
    //		str   : 출력할 문자 값
    //		ack   : 출력후 성공/실패의 검사 여부
    // Return Value :
    //		성공( TRUE ), 실패( FALSE )
    void TimeCheck( int MsecChange );
    // 지정된 시간이 되었는지를 검사하는 함수
    // Parameters :
    //		MsecChange : 경과된 시간
    bool AddTag( QString& TagName);
    // 태그이름과 식별코드를 리스트에 추가하는 함수
    // Parameters :
    //		TagName : 태그이름
    //		TagAddr : 태그식별코드
    bool ExtractData( char * Buffer);
    // 수신된 버퍼를 분석하여 처리하는 함수
    // Parameters :
    //		Buffer : 수신된 버퍼의 주소
    //		Length : 버퍼의 길이
    void ReMake( void );
    void SetTagAtom(QString& TagName,quint16 atom, quint16 status);
    void SetTagValue(quint16 atom, quint16 status, float value);
    void ScanReset();

};

#endif // POLLMSG_H
