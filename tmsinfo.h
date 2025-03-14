/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
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

#ifndef TmsInfoDialog_H
#define TmsInfoDialog_H

#include <QDialog>
#include <QTimer>
#include <QtSql>
#include <QtGui>
#include <QTcpSocket>
#include "mylineedit.h"
#include <QTableWidgetItem>
#include <QListWidget>
#include <QCheckBox>
#include <QMessageBox>

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;
class QTableWidget;
class QDir;
class QLabel;
class QLineEdit;
class QGridLayout;
class QListWidget;
class QCheckBox;
class QComboBox;
class CWMINet;
class QListWidgetItem;
class QTableView;

extern int XMaxRes;
extern int YMaxRes;



class SystemTab : public QWidget
{
    Q_OBJECT

public:
    SystemTab(QWidget *parent = 0);
};


class ProgramTab : public QWidget
{
    Q_OBJECT

public:
 //   QDirModel model;
	QTableWidget *secTable;
	QListWidget		*listWidget;
    ProgramTab(QWidget *parent = 0);
	void showFiles(const QDir &directory, const QStringList &files);
	void InfoView();
};

class LogData10sTab : public QWidget
{
    Q_OBJECT

public:
 //   QDirModel model;
    QTableWidget *secTable;
	QListWidget		*listWidget;
    LogData10sTab(QWidget *parent = 0);
	void showFiles(const QDir &directory, const QStringList &files);
	void InfoView();
};

class LogData5mTab : public QWidget
{
    Q_OBJECT

public:
 //   QDirModel model;
    QTableWidget *minTable;
    LogData5mTab(QWidget *parent = 0);
	void showFiles(const QDir &directory, const QStringList &files);
	void InfoView();
};
class CTmsInfoDialog;
class DecimalDisplayTab : public QWidget
{
    Q_OBJECT

private slots:
	void change();
	void onTimer();
	void onDelayTimer();
    
public:

	DecimalDisplayTab(QWidget *parent = 0);
	void InfoView(const QString& ipAddr, int port);
	bool ExtractData( char *Buffer);
    QTableWidget	*decimalTable;
    QPushButton		*changeButton;
	bool			m_bPasswordOK;
	CWMINet			*m_pWMINet;
	QTimer			m_Timer;
	QTimer			m_DelayTimer;
	uint			m_nIdle;
	QString			m_ipAddr;
	int				m_nPort;
	CTmsInfoDialog	*m_pDlg;
	QMessageBox* waitMessageBox;

};

class CalibrationTab : public QWidget
{
    Q_OBJECT

private slots:
	void change();
	void onTimer();
    
public:

	CalibrationTab(QWidget *parent = 0);
	void InfoView(const QString& ipAddr, int port);
	bool ExtractData( char *Buffer);
	QTableWidget	* calTable;
	QSqlQueryModel	*model;
    QPushButton		*changeButton;
	bool			m_bPasswordOK;
	CWMINet			*m_pWMINet;
	QTimer			m_Timer;
	uint			m_nIdle;
	QString			m_ipAddr;
	int				m_nPort;
};

class CTmsInfoDialog : public QDialog
{
    Q_OBJECT

public:
    CTmsInfoDialog(const QString& ipAddr, int port, QWidget *parent,Qt::WindowFlags f);
	bool			m_bChanged;
	QPushButton *ok2Button;
private slots:
    void onTimer();
    void currentChanged(int index);

private:
	QTimer			m_Timer;
	uint			m_nIdle;
	SystemTab*		m_systemTab;
	ProgramTab*		m_programTab;
	LogData10sTab*	m_logData10sTab;
	LogData5mTab*	m_logData5mTab;
	DecimalDisplayTab* m_decimalDisplayTab;
	CalibrationTab* m_calibrationTab;
	bool			m_bSystem;
	bool			m_bProgram;
	bool			m_bLog10s;
	bool			m_bLog5m;
	bool			m_bDecimal;
	bool			m_bCal;
	QString			m_IpAddr;
	int				m_Port;
};

class CDecimalInput : public QDialog
{
    Q_OBJECT

public:
    CDecimalInput(QString& SensorName, double value, QWidget *parent = 0);
	~CDecimalInput();
	double m_nValue;
	bool	m_bOn;
	bool	m_bOff;
	
private slots:
    void accept();
    void onTimer();
private:
	QLabel *Sensor;
	QLabel *SensorValue;
	QLabel *Old;
	QLabel *OldValue;
	QLineEdit *display;
	QPushButton *ok;
	QPushButton *cancel;
	QGridLayout *subLayout;
	QString	m_sVal;
	QTimer				m_Timer;
	uint			m_nIdle;
	bool	eventFilter(QObject*, QEvent *e);
};

class CDevice : public QObject
{
    Q_OBJECT

public:
	CDevice() {};
	QString			m_deviceName;
	QString			m_deviceDesc;
	QList<QString*>	m_topicList;
	int				m_nIndex;

};


#define	INBUFSIZE	21000
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define ACK 0x06
#define CR  0x0d
#define NAK 0x15

class CSewerCommandTab;
class CSewerDialog;
class CU_CityDialog;
class CDumpTab;
class CSewerNet : public QObject
{
    Q_OBJECT

public:
    CSewerNet(const QString& ipAddr, int port, int type=0);
    virtual ~CSewerNet();
	void sendCommad(char *buf, int len);

private slots:
	void connected();
    void readyRead();
    void disconnected();
    void displayError(QAbstractSocket::SocketError socketError);
public slots:
    void onCommand(const char *cmd, const char *sub, int type);
	void onDumpCommand(char *date);
	void onTimeSync(char *date);
signals:
	void sigMsgPut(QString&);
	void sigDataPut(int, char*);
	void sigbtnEnabled(bool);
	void sigbtnEnabled2(bool);

private:

	enum ProtolState 
	{
		PROT_IDLE,
		PROT_WAIT_ACK,
		PROT_WAIT_RES,
		PROT_WAIT_HEAD,
		PROT_WAIT_EOT,
	};

	QTcpSocket			*m_ptcpSocket;
	int					m_nPort;
	QString				m_ipAddr;
	int					m_nblockSize;
	ProtolState			m_nState;					// 현재의 통신상태
	bool				m_bDumpInit;
	int					m_nTimer;
	bool				m_bEnabled;

	// 초기화 과정이 끝났는지의 여부
	int	dummy;
	char				m_RspBuffer[INBUFSIZE];		// 수신된 문자를 저장할 버퍼
	int					m_nWaitLen;
	quint32				m_nCurrmSec;
	quint32				m_dwLastMsec;
	quint32				m_nSendmSec;
	quint32				m_nLastconnectmSec;
	QString				m_sCommand;
	QString				m_sSubCommand;
	CSewerCommandTab*	m_CommandTab;
	CDumpTab*			m_DumpTab;
	int					m_nType;
	char				m_date[30];
	void				SendCommand();
	void				CheckSum(char *buf, int len, char* sum);
	void				msgPut(QString msg);
	void				msgDisp(QString head, char *msg);

public:

	CSewerDialog  *m_pSewerDlg;
	
	void linkEvent(CSewerCommandTab* commandTab, CDumpTab* dumpTab);
	void linkEventUCity(CU_CityDialog* ucityDlg);
	void connectToHost();
	void disconnect();
	void dumpInit();
	void onTimer();
	bool	m_bEnding;

};

class CSewerCommandTab : public QDialog
{
    Q_OBJECT

public:
    CSewerCommandTab( QWidget *parent = 0);
    ~CSewerCommandTab();
	QList<CDevice*>	m_deviceList;		
	QListWidget		*listWidget;
    void command(const char*cmd, const char*msg);
    void onConf(const char *title, const char *sub, bool type);

private slots:
    void onTime();
	void onPorr();
	void onRtur();
	void onFlmr();
	void onSamp();
	void onSama();
	void onSams();
	void onSamr();
	void onComp();
	void onComa();
	void onComs();
	void onConf1();
	void onConfc();
	void onConfv();

    void onClear();
    void onCommand(const char *cmd, const char *sub);
	void msgPut(QString& str);
	void btnEnabled(bool);

signals:
    void sigCommand(const char*, const char*, int);
	void sigTimeSync(char*);
private:
	QPushButton *porrButton;
	QPushButton *timeButton;
	QPushButton *rturButton;
	QPushButton *flmrButton;

	QPushButton *sampButton;
	QPushButton *samaButton;
	QPushButton *samsButton;
	QPushButton *conf1Button;
	QPushButton *samrButton;

	QPushButton *compButton;
	QPushButton *comaButton;
	QPushButton *comsButton;
	QPushButton *comrButton;
	QPushButton *confcButton;

	QPushButton *confvButton;

};

class CDumpTab : public QDialog
{
    Q_OBJECT

public:
    CDumpTab(QWidget *parent = 0);
	~CDumpTab();

private slots:
    void dump();
    void rdat();
    void fromDateSelect();
    void toDateSelect();
	void dataPut(int, char*);
	void btnEnabled(bool);

signals:
    void sigCommand(const char*, const char*, int);
	void sigDumpCommand(char*);

private:
	bool			createConnection();
	QTimer			m_Timer;
	uint			m_nIdle;
	QDateTime		m_fromDate;
	QDateTime		m_toDate;
	QTableWidget	*view;
	QComboBox		*fromhourCombobox;
	QComboBox		*tohourCombobox;
	QPushButton		*fromDateSelectButton;
	QPushButton		*toDateSelectButton;
	QPushButton		*submitButton;
	QPushButton		*rdatButton;
};

class CSewerDialog : public QDialog
{
    Q_OBJECT

public:
    CSewerDialog(const QString& ipAddr, int port, QWidget *parent = 0);
	~CSewerDialog();
//	void onCommand(char *cmd, int type);
	void msgPut(QString str);
	CSewerNet			*m_pSewerNet;
private slots:
	void onTimer();
	void currentChanged(int index);
signals:
    void sigCommand(int, const char*);

private:
	QTimer				m_Timer;
	uint				m_nIdle;
	CSewerCommandTab*	m_CommandTab;
	CDumpTab*			m_DumpTab;

	QString				m_IpAddr;
	int					m_Port;
};

class CTimeSyncDialog : public QDialog
{
    Q_OBJECT

public:
		CTimeSyncDialog(QWidget *parent = 0);
		char	m_sDate[20];
private slots:
    void accept();
private:
		bool isNum(QString& str);
		MyLineEdit *m_ymdEdit;
		MyLineEdit *m_hmsEdit;
};

class CTimeInputDialog : public QDialog
{
    Q_OBJECT

public:
		CTimeInputDialog(QWidget *parent, QString title, bool type=true);
		QLineEdit *m_interval;
		QLineEdit *m_onTime;
private slots:
    void accept();
private:
		bool	eventFilter(QObject*, QEvent *e);
};

class CLanSetupDialog : public QDialog
{
    Q_OBJECT

public:
    CLanSetupDialog(QWidget *parent = 0);
	MyLineEdit *m_lan1IP;
	MyLineEdit *m_lan1Mask;
	MyLineEdit *m_lan2IP;
	MyLineEdit *m_lan2Mask;
	MyLineEdit *m_gateway;
	char onboot1[64], onboot2[64];
	char bootproto1[64], bootproto2[64], idaddr1[64], idaddr2[64];
	char netmask1[64], netmask2[64], Gateway[64], Dns1[64], Dns2[64], Mac1[64], Mac2[64];
private slots:
    void accept();

};

class CUcityNet;
class CU_CityDialog : public QDialog
{
    Q_OBJECT

public:
    CU_CityDialog(const QString& ipAddr, QWidget *parent = 0);
	~CU_CityDialog();
	CUcityNet			*m_pUCityNet;
private slots:
	void onTimer();
    void dump();
    void fromDateSelect();
    void toDateSelect();
	void dataPut(int, char*, char*);
	void viewList();
	void msgPut(QString& str);
signals:
	void sigDumpCommand(char*);

private:
	QTimer			m_Timer;
	uint			m_nIdle;
	QDateTime		m_fromDate;
	QDateTime		m_toDate;
	QTableWidget	*view;
	QComboBox		*fromhourCombobox;
	QComboBox		*tohourCombobox;
	QPushButton		*fromDateSelectButton;
	QPushButton		*toDateSelectButton;
	QPushButton		*submitButton;
    QPushButton		*okButton;
	QList<QTableWidgetItem *> itemList[4];


	QString				m_IpAddr;
	int					m_Port;
};

class CRainSensorDlg : public QDialog
{
    Q_OBJECT

public:
    CRainSensorDlg(const QString& ipAddr, QWidget *parent = 0);
	~CRainSensorDlg();
	CUcityNet			*m_pUCityNet;
private slots:
	void onTimer();
	void dump();
	void onTimeSync();
	void onRset();
    void fromDateSelect();
    void toDateSelect();
	void dataPut(int, char*);
	void msgPut(QString& str);
signals:
	void sigDumpCommand(char*, char*);
	void sigTimeSync(char*);
	void sigRset();

private:
	QTimer			m_Timer;
	uint			m_nIdle;
	QDateTime		m_fromDate;
	QDateTime		m_toDate;
	QTableWidget	*view;
	QComboBox		*fromhourCombobox;
	QComboBox		*tohourCombobox;
	QPushButton		*fromDateSelectButton;
	QPushButton		*toDateSelectButton;
	QPushButton		*submitButton;
	QPushButton		*timeButton;
    QPushButton		*okButton;
	QPushButton		*rsetButton;

	QString				m_IpAddr;
	int					m_Port;
};

class CUcityNet : public QObject
{
    Q_OBJECT

public:
    CUcityNet(const QString& ipAddr, int port, int type=0);
    virtual ~CUcityNet();
	void sendCommad(char *buf, int len);

private slots:
	void connected();
    void readyRead();
    void disconnected();
    void displayError(QAbstractSocket::SocketError socketError);
public slots:
	void onDumpCommandUCity(char *date);
    void onCommandRainSensor(const char *cmd, const char *date);
	void onTimeSync(char*);
	void onRainRset();
signals:
	void sigMsgPut(QString&);
	void sigDataPutUCity(int, char*, char*);
	void sigDataPutRainSensor(int, char*);
	void sigViewList();

private:

	enum ProtolState 
	{
		PROT_IDLE,
		PROT_WAIT_STX,
		PROT_WAIT_RES,
		PROT_WAIT_HEAD,
		PROT_WAIT_EOT,
		PROT_WAIT_RAIN_HEAD,
		PROT_WAIT_RAIN_RES,
		PROT_WAIT_RAIN_TIME_HEAD,
		PROT_WAIT_RAIN_RSET,
	};

	QTcpSocket			*m_ptcpSocket;
	int					m_nPort;
	QString				m_ipAddr;
	int					m_nblockSize;
	ProtolState			m_nState;					// 현재의 통신상태
	bool				m_bDumpInit;
	int					m_nTimer;
	bool				m_bEnabled;
	quint32				m_nCount;

	
	// 초기화 과정이 끝났는지의 여부
	int	dummy;
	char				m_RspBuffer[INBUFSIZE];		// 수신된 문자를 저장할 버퍼
	int					m_nWaitLen;
	quint32				m_nCurrmSec;
	quint32				m_dwLastMsec;
	quint32				m_nSendmSec;
	quint32				m_nLastconnectmSec;
	QString				m_sCommand;
	QString				m_sSubCommand;
	CSewerCommandTab*	m_CommandTab;
	CDumpTab*			m_DumpTab;
	int					m_nType;
	char				m_date[30];
	char				m_rdate[30];
	void				CheckSum(char *buf, int len, char* sum);
	quint16				CheckSumUint(char *buf, int len);
	void				msgPut(QString msg);
	void				msgDisp(QString head, char *msg, int cnt);

public:

	void linkEventUCity(CU_CityDialog* ucityDlg);
	void linkEventRainSensor(CRainSensorDlg* rainSensorDlg);
	void connectToHost();
	void disconnect();
	void dumpInit();
	void onTimer();
	bool m_bEnding;
};

#endif
