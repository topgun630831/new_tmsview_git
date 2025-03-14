#ifndef WMINET_H
#define WMINET_H

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

#define MAXMSGLEN	20000
#define	INBUFSIZE	21000

#include "pollmsg.h"
#include "tag.h"

/////////////////////////////////////////////////////////////////////////
// 수신된 프레임을 분석하고 응답 프레임을 만들어 관리하는 Class
/////////////////////////////////////////////////////////////////////////
class CPollMsg;
class CWMINet : public QObject
{
    Q_OBJECT

public:
    CWMINet();
    virtual ~CWMINet();
    void onTimer();

private slots:
    void connected();
    void readyRead();
    void disconnected();
    void displayError(QAbstractSocket::SocketError socketError);

private:

    enum ProtolState
    {
        PROT_IDLE,
        PROT_WAIT_RES,
        PROT_ERR_TIMEOUT,
        PROT_ERR_RCV,
        PROT_ERR_SEND,
        PROT_WAIT_QUIT
    };

//	QTimer				m_Timer;
    QTcpSocket			*m_ptcpSocket;
    int					m_nPort;
    QString				m_ipAddr;
    int					m_nblockSize;
    QList<CPollMsg*>	m_listInit;					// 클라이언트로 부터 요구된 태그이름을 저장하는 List
    QList<CPollMsg*>	m_listWrite;					// 클라이언트로 부터 요구된 태그이름을 저장하는 List
    CPollMsg*			m_pEventReqMsg;				// 상태 송신 프레임의 정보를 가리키는 주소
    CPollMsg*			m_pCurPollMsg;				// 현재 송신한 프레임의 정보를 가리키는 주소
    ProtolState			m_nState;					// 현재의 통신상태
    bool				m_bInitEnd;

    // 초기화 과정이 끝났는지의 여부
    int	dummy;
    char				m_RspBuffer[INBUFSIZE];		// 수신된 문자를 저장할 버퍼
    char				m_TempBuffer[INBUFSIZE];	// 수신된 문자를 저장할 버퍼
    int					m_nRspIndex;
    QMap<CTag*, quint16>		m_TagMap;
    quint32				m_cSeq;					// 프레임의 순서번호
    QString				m_strName;			// 드라이버를 검색할때 사용되는 이름
    quint32				m_nCurrmSec;
    quint32				m_dwLastMsec;
    quint32				m_nSendmSec;
    quint32				m_nLastconnectmSec;

    void connectToHost();
    void AllEnd( void );
    void MakeEventReq( void );
    bool SendQuery( void );
    bool ProtPoll( void );
    void ConnectProc( void );
    int DoProtocol(char* lpszBlock, quint32 nLength );
    void ProcessResponse( CPollMsg *poll );
    void AddWritePoll(CPollMsg   *pPollMsg);
public:

    QWidget  *m_pCommMon;

    void initial(const QString& ipAddr, int port);
    void initialComm(QWidget *pCommMon, const QString& ipAddr, int port, int type=0);
    void MakeInitMsg( QString& TagName);
    void setInitEnd(bool end=true);
    bool isInitEnd();
    void StartConv();
    void AddWritePoll( QString strTagName, bool type, double value, QString& str, bool ack );
    void InitDelete( void );
    void ReMake( void );
    void commandWrite(quint32 seq, quint16 Cmd, quint16 SubCmd);
    void deviceSelect(QString& device, QString& topic);
    void parameterChange(QString& tagname, QString& parameter, QString& value);
    void disconnect();
    bool	m_bEnding;
    int		m_nType;

};

#endif // WMINET_H
