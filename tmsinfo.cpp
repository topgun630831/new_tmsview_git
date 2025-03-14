#include <QtGui>
#include <QtSql>
#include <QTableWidget>

#include "tmsinfo.h"
#include "calendardlg.h"
#include "calculator.h"
#ifdef __linux__
#include <sys/vfs.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "calendardlg.h"
#include <sys/types.h>
#include "wminet.h"

#define	UCITY_PORT			6500
#define	RAIN_SENSOR_PORT	6500

void WriteValue(QString name,float value);
bool	g_bDebug = true;
extern	bool g_bReboot;
extern	bool g_bPowerOff;
//extern	QString g_siteCode;
extern	QString g_siteDesc;
extern bool	g_bSamp;
extern	int			m_wdtSockFd;
extern	int			m_wdtID;
void wdtHeartbeat(int fd, int id);

bool GetItem(const char *item, char *value)
{
    char buf[256];
    FILE *pFile;
    pFile = ::fopen("/etc/ether.conf", "r");
	value[0] = 0;
    int len = strlen(item);
	if(pFile)
	{
		while(::fgets(buf, 1024, pFile)){
			if(!strncmp(item, buf, len)){
				int	len2 = strlen(buf+strlen(item)+1);
				if(len2 > 0 && len2 < 63)
				{
					memcpy(value, buf+strlen(item)+1, len2-1);
					value[len2-1] = 0;
					::fclose(pFile);
					return true;
				}
			}
		}
		::fclose(pFile);
	}
	return false;
}

#ifdef __linux__
int GetNetworkInfo(const char *interface, char *sip, char *snetmask, char *smac)
{
	unsigned int ip, netmask;
//	char *mac;
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	struct ifreq ifrq;
    struct sockaddr_in *sin;
    strcpy(ifrq.ifr_name, interface);

	sip[0]=snetmask[0]=smac[0] = 0;
    if(ioctl(sockfd, SIOCGIFADDR, &ifrq) < 0){
        close(sockfd);
        return -1;
    }

    sin = (struct sockaddr_in *)&ifrq.ifr_addr;
    ip = ntohl(sin->sin_addr.s_addr);

    // get netmask
    if(ioctl(sockfd, SIOCGIFNETMASK, &ifrq) < 0) {
        close(sockfd);
        return -1;
    }

    sin = (struct sockaddr_in *)&ifrq.ifr_addr;
    netmask = ntohl(sin->sin_addr.s_addr);

    /*if(mac != NULL){
        if (ioctl(sockfd ,SIOCGIFHWADDR, &ifrq) < 0)   {
            close(sockfd);
            return -1;
        }
        mac = ifrq.ifr_hwaddr.sa_data;
    }*/
    close(sockfd);
	sprintf(sip, "%d.%d.%d.%d", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
	sprintf(snetmask, "%d.%d.%d.%d", (netmask >> 24) & 0xff, (netmask >> 16) & 0xff, (netmask >> 8) & 0xff, netmask & 0xff);
//	sprintf(smac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return 0;
}

int Connect(char *file)
{   
    sockaddr_un     addr;

    bzero(&addr, sizeof(addr));
    addr.sun_family = AF_LOCAL;
    ::strcpy(addr.sun_path, file);

    int fd = socket( AF_LOCAL, SOCK_STREAM, 0 );

    if ( fd < 0 ){
        return -1;
    }                               

    if ( ::connect(fd, (sockaddr *)&addr, sizeof(addr)) == -1){
        close(fd);
        return -1;
    }                                       
    return fd;
}
#endif
void SaveCalibrationTime(char *buf)
{
    char filename[128];
    snprintf(filename, 128, "/flash/.config/.%s",buf);

    FILE *pFile = fopen(filename, "w+");
    if(pFile == NULL)
        return;

    fprintf(pFile, "%d", (int)time(NULL));

    fclose(pFile);
}

int LoadCalibrationTime(char *buf)
{
    char filename[128];
	int	tm;
    snprintf(filename, 128, "/flash/.config/.%s",buf);

    FILE *pFile = fopen(filename, "r");
    if(pFile == NULL)
        return 0;

    fscanf(pFile, "%d", &tm);

    fclose(pFile);
	return tm;
}


#define INT(x)          (*(int *)((x)))
#define LEN_OFFSET      0
#define MAINCMD_OFFSET  4
#define SUBCMD_OFFSET   8
#define DATA_OFFSET     12
#define RESULT_OFFSET   12
#define HEADER_LEN      12

/*
int FileCmd(int , int , int len, void *data)
{
#ifdef __linux__
    int sockfd = Connect("/tmp/__mon");
    char packet[128];
    INT(packet + LEN_OFFSET) = len;
    INT(packet + MAINCMD_OFFSET) = mainCmd;
    INT(packet + SUBCMD_OFFSET) = subCmd;
    ::write(sockfd, packet, DATA_OFFSET);

    if(len > 0)
        ::write(sockfd, data, len);

    ::read(sockfd, packet, HEADER_LEN);
    if(INT(packet) > 0){
        ::read(sockfd, data , INT(packet));
    }
	::close(sockfd);
#endif
    return INT(data);
}
*/
/*
float folderSize(const char *, int &totalSize, int &freeSize)
{
 #ifdef __linux__
    struct statfs folderInfo;
    statfs(folder, &folderInfo);

	totalSize = folderInfo.f_blocks *folderInfo.f_bsize / (1024*1024);
    int usedSize =  (folderInfo.f_blocks - folderInfo.f_bavail)*folderInfo.f_bsize/(1024*1024);
    freeSize =  folderInfo.f_bavail *folderInfo.f_bsize / (1024*1024);
    return ((float)usedSize / totalSize) * 100;
#else
    return 0;
#endif
}
*/


extern int memInfo(int &totalSize, int &freeSize);
/*
{

    FILE *fp = NULL;
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        printf("Meminfo open error\n");
        return false;
    }

    int index =0;
    char line[128];
    char null[12];
    while(1)
    {
        fgets(line, 128, fp);
        if(index == 0) {
            sscanf(line, "%s %d", null, &totalSize);
        } else if(index == 1) {
            sscanf(line, "%s %d", null, &freeSize);
        } else {
            fclose(fp);
            break;
        }
        index++;
    }
	totalSize /=1024;
	freeSize /=1024;
    return true;

}
*/



CTmsInfoDialog::CTmsInfoDialog(const QString& ipAddr, int port, QWidget *parent,Qt::WindowFlags f)
    : QDialog(parent, f)
{

	m_bChanged = false;
	m_IpAddr = ipAddr;
	m_Port = port;
	QTabWidget *tabWidget = new QTabWidget(this);
	m_systemTab = new SystemTab(this);
	m_programTab = new ProgramTab(this);
	m_logData10sTab = new LogData10sTab(this);
	m_logData5mTab = new LogData5mTab(this);
	m_decimalDisplayTab = new DecimalDisplayTab(this);
	m_calibrationTab = new CalibrationTab(this);

	tabWidget->addTab(m_systemTab, tr("시스템 정보"));
	tabWidget->addTab(m_programTab, tr("프로그램폴더"));
	tabWidget->addTab(m_logData10sTab, tr("10초 데이터"));
	tabWidget->addTab(m_logData5mTab, tr("5분/시간 데이터"));
	tabWidget->addTab(m_decimalDisplayTab, tr("계측 표시"));
	tabWidget->addTab(m_calibrationTab, tr("교정값 설정"));

	QPushButton *okButton = new QPushButton(tr("확인"));
	ok2Button = okButton;
                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));

	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(okButton);
	setLayout(mainLayout);

	m_bSystem = m_bProgram = m_bLog10s = m_bLog5m = m_bDecimal = m_bCal = false;

    setWindowTitle(tr("수질TMS DataLogger 정보"));
}

void CTmsInfoDialog::currentChanged(int index)
{
	if(index == 1 && m_bProgram == false)
	{
		m_programTab->InfoView();
		m_bProgram = true;
	}
	else
	if(index == 2 && m_bLog10s == false)
	{
		m_logData10sTab->InfoView();
		m_bLog10s = true;
	}
	else
	if(index == 3 && m_bLog5m == false)
	{
		m_logData5mTab->InfoView();
		m_bLog5m = true;
	}
	else
	if(index == 4 && m_bDecimal == false)
	{
		m_decimalDisplayTab->InfoView(m_IpAddr, m_Port);
		m_bDecimal = true;
	}
	else
	if(index == 5 && m_bCal == false)
	{
		m_calibrationTab->InfoView(m_IpAddr, m_Port);
		m_bCal = true;
	}

}

void CTmsInfoDialog::onTimer()
{
}

SystemTab::SystemTab(QWidget *parent)
    : QWidget(parent)
{
	QString password;
	QString factoryCode;

	
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	QString dbname =  "/sd/database/tms.db";
    db.setDatabaseName(dbname);
	db.open();

	QSqlQuery q("SELECT Password FROM Password");
	if (q.next())
	{
		password = q.value(0).toString();
		password.replace(0, 6, tr("******"));
		if(g_bDebug)
            qDebug("Password = %s", password.toLatin1().data());
	}
	db.close();
	
	dbname =  "/flash/works/wmi.db";
    db.setDatabaseName(dbname);
	db.open();

	QSqlQuery q2("SELECT LocationCode FROM Exts_TmsExt2");
	if (q2.next())
	{
		factoryCode = q2.value(0).toString();
		if(g_bDebug)
            qDebug("LocationCode = %s", password.toLatin1().data());
	}
	db.close();
	QSqlDatabase::removeDatabase("QSQLITE");



    QLabel *cpuInfoLabel = new QLabel(tr("CPU 정보:"));
    QLabel *cpuInfoValueLabel = new QLabel(tr("CPU : S3C2440  Clock : 400MHz"));
    cpuInfoValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *flashLabel = new QLabel(tr("Flash:"));
    QLabel *flashValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(64).arg(10));
    flashValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	int total, used;

//	folderSize("/sd", total, used);
    QLabel *diskLabel = new QLabel(tr("디스크:"));
    QLabel *diskValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(total).arg(used));
    diskValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	memInfo(total, used);
    QLabel *memoryLabel = new QLabel(tr("메모리:"));
    QLabel *memoryValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(total+4).arg(used));
    memoryValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *codeLabel = new QLabel(tr("사업장 코드:"));
    QLabel *codeValueLabel = new QLabel(factoryCode);
    codeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *dirLabel = new QLabel(tr("프로그램 위치:"));
    QLabel *dirValueLabel = new QLabel(tr("/flash/wmi"));
    dirValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *dirDataLabel = new QLabel(tr("자료저장 위치:"));
    QLabel *dirDataValueLabel = new QLabel(tr("/sd/database"));
    dirDataValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *passwordLabel = new QLabel(tr("암호화된 비밀번호:"));

	QLabel *passwordValueLabel = new QLabel(password);
    passwordValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);


    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(cpuInfoLabel);
    mainLayout->addWidget(cpuInfoValueLabel);
    mainLayout->addWidget(flashLabel);
    mainLayout->addWidget(flashValueLabel);
    mainLayout->addWidget(diskLabel);
    mainLayout->addWidget(diskValueLabel);
    mainLayout->addWidget(memoryLabel);
    mainLayout->addWidget(memoryValueLabel);
    mainLayout->addWidget(codeLabel);
    mainLayout->addWidget(codeValueLabel);
    mainLayout->addWidget(dirLabel);
    mainLayout->addWidget(dirValueLabel);
    mainLayout->addWidget(dirDataLabel);
    mainLayout->addWidget(dirDataValueLabel);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

extern QStringList findFiles(const QDir &directory, const QStringList &files, bool Desc);
/*
{
    QStringList foundFiles;

    for (int i = 0; i < files.size(); ++i) {

        QFile file(directory.absoluteFilePath(files[i]));

        if (file.open(QIODevice::ReadOnly)) {
            QString line;
            QTextStream in(&file);
            while (!in.atEnd()) {
                line = in.readLine();
				{
                    foundFiles << files[i];
                    break;
                }
            }
        }
    }
	if(Desc)
		qSort(foundFiles.begin(), foundFiles.end(), qGreater<QString>());
	else
		qSort(foundFiles.begin(), foundFiles.end(), qLess<QString>());
    return foundFiles;
}
*/
void ProgramTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
		// fileNameItem->setFlags(nameItem->flags() ^ Qt::ItemIsEditable);  item별로 read only, 
		// sizeItem->setFlags(descItem->flags() ^ Qt::ItemIsEditable);      전체는 setEditTriggers(QAbstractItemView::NoEditTriggers);로 대채
        secTable->setItem(i, 0, fileNameItem);
        secTable->setItem(i, 1, sizeItem);
	}
}

ProgramTab::ProgramTab(QWidget *parent)
    : QWidget(parent)
{
}
void ProgramTab::InfoView()
{

    QDir directory = QDir(tr("/flash"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files, false);

    secTable = new QTableWidget(files.size(), 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
/*    secTable->setHorizontalHeaderLabels(labels);
    secTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    secTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    secTable->verticalHeader()->show();
*/
//    secTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//    secTable->horizontalHeader()->setResizeMode(1);
//    secTable->verticalHeader()->show();

	secTable->setSelectionBehavior(QAbstractItemView::SelectRows);	// row 전체선택
	secTable->setSelectionMode(QAbstractItemView::SingleSelection);	
	secTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(secTable);
	setLayout(layout);
    
	
	showFiles(directory, files);
}



void LogData10sTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
        secTable->setItem(i, 0, fileNameItem);
        secTable->setItem(i, 1, sizeItem);
	}
}

LogData10sTab::LogData10sTab(QWidget *parent)
    : QWidget(parent)
{
}

void LogData10sTab::InfoView()
{

    QDir directory = QDir(tr("/sd/tensec"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files, true);

    secTable = new QTableWidget(files.size(), 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
    secTable->setHorizontalHeaderLabels(labels);
//    secTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    secTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
//    secTable->verticalHeader()->show();

	secTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	secTable->setSelectionMode(QAbstractItemView::SingleSelection);	
	secTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(secTable);
	setLayout(layout);
    
	
	showFiles(directory, files);
}

LogData5mTab::LogData5mTab(QWidget *parent)
    : QWidget(parent)
{  
}

void LogData5mTab::InfoView()
{

    QDir directory = QDir(tr("/sd/fivemin"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName),
                                QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files, true);

    minTable = new QTableWidget(files.size(), 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
    minTable->setHorizontalHeaderLabels(labels);
//    minTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    minTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
//    minTable->verticalHeader()->show();

	minTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	minTable->setSelectionMode(QAbstractItemView::SingleSelection);	
	minTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(minTable);
	setLayout(layout);

    showFiles(directory, files);

}

void LogData5mTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
        minTable->setItem(i, 0, fileNameItem);
        minTable->setItem(i, 1, sizeItem);
    }
}


extern char SensorCode[15][10];
extern int SensorFormat[15];
extern int	SensorNumber;

DecimalDisplayTab::DecimalDisplayTab(QWidget *parent)
    : QWidget(parent)
{
	m_pDlg = (CTmsInfoDialog*)parent;
//	m_pWMINet = NULL;
}

void DecimalDisplayTab::InfoView(const QString& ipAddr, int port)
{
	m_ipAddr = ipAddr;
	m_nPort	 = port;

    m_bPasswordOK = false;
    changeButton = new QPushButton(tr("변경"));

    connect(changeButton, SIGNAL(clicked()), this, SLOT(change()));

    decimalTable = new QTableWidget(0, 3, this);
    QStringList labels;
    labels << tr("항목") << tr("소숫점 자리수") << tr("교정값 변경일자");
    decimalTable->setHorizontalHeaderLabels(labels);
//    decimalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    decimalTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
//    decimalTable->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);

	decimalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	decimalTable->setSelectionMode(QAbstractItemView::SingleSelection);	
	decimalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
/*
	for(int i = 0; i < SensorNumber-1; i++)
	{
        decimalTable->insertRow(i);
        QTableWidgetItem *codeItem = new QTableWidgetItem(SensorCode[i]);
		QString format =  QString::number(SensorFormat[i]);
        QTableWidgetItem *formatItem = new QTableWidgetItem(format);
        decimalTable->setItem(i, 0, codeItem);
        decimalTable->setItem(i, 1, formatItem);

        int tm = LoadCalibrationTime(SensorCode[i]);
		
		if(tm != 0)
		{
			QString time;
			QDateTime datetime;
			datetime.setTime_t(tm);
			QString code = SensorCode[i];
			QString code2 = code.left(3);
			if(code2 != tr("FLW") &&  code2 != tr("SAM"))
			{
				QTableWidgetItem *dateItem = new QTableWidgetItem(datetime.toString("yy/M/d hh:mm:ss"));
				decimalTable->setItem(i, 2, dateItem);
			}
		}
	}

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(decimalTable);
	layout->addWidget(changeButton);
	setLayout(layout);
*/
	waitMessageBox = new QMessageBox( QMessageBox::Information,
                                            QObject::tr("TMS"),
                                            QObject::tr("처리중입니다. 잠시만 기다리십시오"));
                                            //0,
                                            //0,
                                            //Qt::Dialog | Qt::WindowStaysOnTopHint );
	waitMessageBox->setModal(false);
	
//	m_pWMINet = new CWMINet;
//	m_pWMINet->initialComm(this, ipAddr, port, 1);
//	m_pWMINet->StartConv();

	m_nIdle = 0;
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(100);
    connect(&m_DelayTimer, SIGNAL(timeout()), this, SLOT(onDelayTimer()));

}

bool CheckPassword(char *userName, char *password);

void DecimalDisplayTab::change()
{
/*	QString sensorName;
	QList<QTableWidgetItem *> list =  decimalTable->selectedItems();
	if(list.size() >= 1)
	{
		if(m_bPasswordOK == false)
		{
//			CPasswordDlg dlg(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
//			if(dlg.exec() == QDialog::Accepted)
			{
//				QString password = dlg.GetPassword();
                if(CheckPassword("tms", password.toLocal8Bit().data()) ==false)
				{
					QMessageBox::information(this, tr("TMS"), tr("비밀번호가 올바르지 않습니다."));
					return;
				}
                m_bPasswordOK = true;
			}
//			else
//				return;
		}

        QTableWidgetItem* item = list[0];
		int row = item->row();
        sensorName = SensorCode[row];
		CDecimalInput Dlg(sensorName, (double)SensorFormat[row]);

		if(Dlg.exec() == QDialog::Accepted)
		{
			if((int)Dlg.m_nValue > 8 || (int)Dlg.m_nValue < 0)
				QMessageBox::information(this, tr("TMS"), tr("값이 허용된 범위보다 너무 크거나 작습니다."));
			else
			if(Dlg.m_nValue == SensorFormat[row])
				QMessageBox::information(this, tr("TMS"), tr("입력한 값이 이전값과 동일합니다."));
			else
			{
				QString format =  QString::number(Dlg.m_nValue);
				QTableWidgetItem *formatItem = new QTableWidgetItem(format);
				decimalTable->setItem(row, 1, formatItem);
				QString param = "Format";
//				m_pWMINet->parameterChange(sensorName, param, format);
				SensorFormat[row] = (int)Dlg.m_nValue;
				if(sensorName == tr("FLW01"))
				{
					QString name = tr("FLW01_5MIN");
//					m_pWMINet->parameterChange(name, param, format);
					SensorFormat[SensorNumber-1] = SensorFormat[row];
				}
				m_pDlg->m_bChanged = true;
				changeButton->setEnabled(false);
				m_pDlg->ok2Button->setEnabled(false);
				m_DelayTimer.start(5000);

//				waitMessageBox->show();
			}
		}
	}
//	else
//		QMessageBox::information(this, tr("TMS"), tr("변경할 항목이 선택되지 않았습니다."));
*/
}
void DecimalDisplayTab::onTimer()
{
//	if(m_pWMINet)
//		m_pWMINet->onTimer();

}

void DecimalDisplayTab::onDelayTimer()
{
	changeButton->setEnabled(true);
	m_pDlg->ok2Button->setEnabled(true);
	m_DelayTimer.stop();
}

bool DecimalDisplayTab::ExtractData( char *Buffer)
{
	QString    str;
	char	   *buf;
	quint32	   len;
	quint16	   idx = sizeof(WMINET_HEADER);
	WMINET_HEADER *pRcvHeader = (WMINET_HEADER *)Buffer;

	if(pRcvHeader->CmdGroup & 0x8000 || pRcvHeader->SubCmd & 0x8000)
	{
		len = *( quint32* )&Buffer[ idx ];
		idx+=sizeof( quint32 );
		buf = new char[ len+1 ];
		memcpy( buf, &Buffer[ idx ], len );
		idx+=len;
		buf[ len ] = 0;
		QString str = tr("WMINet Err(%s) %1").arg(buf);
		QMessageBox::information(this, tr("통신"), str);
		delete [] buf;
		return false;
	}

	switch(pRcvHeader->CmdGroup)
	{
		case WMI_CMD_CONNECT:
			if(g_bDebug)
				qDebug("WMI_CMD_CONNECT");
			len = *( quint32* )&Buffer[ idx ];
			idx+=sizeof( quint32 );
			buf = new char[ len+1 ];
			memcpy( buf, &Buffer[ idx ], len );
			idx+=len;
			buf[ len ] = 0;
//			m_pWMINet->setInitEnd();
			delete [] buf;

			break;
		case WMI_CMD_RTDATA:
			{
//				if(pRcvHeader->SubCmd & 0x4000)
//				{
//					changeButton->setEnabled(true);
//					m_pDlg->ok2Button->setEnabled(true);
//					waitMessageBox->hide();
//					break;
//				}
				switch(pRcvHeader->SubCmd)
				{
					case WMI_SCMD_PARAMETER:
					{
						if(g_bDebug)
							qDebug("WMI_SCMD_PARAMETER");
						len = *( quint32* )&Buffer[ idx ];
						idx+=sizeof( quint32 );
						buf = new char[ len+1 ];
						memcpy( buf, &Buffer[ idx ], len );
						idx+=len;
						buf[ len ] = 0;
						if(g_bDebug)
							qDebug("RCV:%s", buf);
						delete [] buf;
	//					changeButton->setEnabled(true);
	//					m_pDlg->ok2Button->setEnabled(true);
						//waitMessageBox->hide();
						break;
					}
					case WMI_SCMD_IDLE:
					case WMI_SCMD_VALREQ:
						break;
					default:
						//QString str = tr("Sub Command %1").arg(pRcvHeader->SubCmd);
						//QMessageBox::information(this, tr("TMS"), str);
//						changeButton->setEnabled(true);
//						m_pDlg->ok2Button->setEnabled(true);
						//waitMessageBox->hide();
						break;
					
				}
			}
			break;
		default:
			//QString str = tr("Main Command %1").arg(pRcvHeader->CmdGroup);
			//QMessageBox::information(this, tr("TMS"), str);
			//changeButton->setEnabled(true);
			//m_pDlg->ok2Button->setEnabled(true);
			//waitMessageBox->hide();
			break;
}
	
	return true;
}

CDecimalInput::CDecimalInput(QString& SensorName, double value, QWidget *parent)
    : QDialog(parent)
{
	setWindowTitle(tr("소숫점 입력"));

	Sensor = new QLabel(tr("계측기"));
	SensorValue = new QLabel(SensorName);
	SensorValue->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QString strOld = tr("%1").arg(value);
	Old = new QLabel(tr("기존값"));
	OldValue = new QLabel(strOld);
	OldValue->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    display = new QLineEdit(strOld);

	ok = new QPushButton(this);
	ok->setText(tr("확인"));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

	cancel = new QPushButton();
	cancel->setText(tr("취소"));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	subLayout = new QGridLayout();
	subLayout->setSizeConstraint(QLayout::SetFixedSize);
	subLayout->addWidget(Sensor, 0, 0, 1, 1);
	subLayout->addWidget(SensorValue, 0, 1, 1, 1);
	subLayout->addWidget(Old, 1, 0, 1, 1);
	subLayout->addWidget(OldValue, 1, 1, 1, 1);
	subLayout->addWidget(display, 2, 0, 1, 2);
	subLayout->addWidget(ok, 3, 0, 1, 1);
	subLayout->addWidget(cancel, 3, 1, 1, 1);
	setLayout(subLayout);
	display->installEventFilter(this);

	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(100);
}

CDecimalInput::~CDecimalInput()
{
}

void CDecimalInput::accept()
{
	m_nValue = display->text().toFloat();
    QDialog::accept();
}

bool CDecimalInput::eventFilter(QObject* pObj, QEvent *e)
{
	pObj = pObj;
	if(e->type() == QEvent::MouseButtonRelease)
	{
		QString value = display->text();
	    Calculator calc(value);
		if(calc.exec() == QDialog::Accepted)
		{
			display->setText(calc.m_Value);
		}
	}
	return false;
}

void CDecimalInput::onTimer()
{
}

CalibrationTab::CalibrationTab(QWidget *parent)
    : QWidget(parent)
{
//	m_pWMINet = NULL;
}

void CalibrationTab::InfoView(const QString& ipAddr, int port)
{
	m_ipAddr = ipAddr;
	m_nPort	 = port;
    m_bPasswordOK = false;
    changeButton = new QPushButton(tr("변경"));

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("/flash/works/wmi.db");
    if (!db.open()) {
		QMessageBox::information(this, tr("TMS"), tr("DB OPEN에 실패했습니다."));
      return;
	}

    connect(changeButton, SIGNAL(clicked()), this, SLOT(change()));

//	QString query = "SELECT Name, Description, InitialValue from Analog where Name like '______CHK_' and External = 0 and IoType = 2";
	QSqlQuery query("SELECT Name, Description, InitialValue from Analog where Name like '______CHK_' and External = 0 ORDER BY Name ASC;");

    calTable = new QTableWidget(0, 3, this);
    QStringList labels;
    labels << tr("태그") << tr("항목")<< tr("교정값") << tr("변경일자");
    calTable->setHorizontalHeaderLabels(labels);

	calTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	calTable->setSelectionMode(QAbstractItemView::SingleSelection);	
	calTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

	int i = 0;
	while (query.next())
	{
		calTable->insertRow(i);
		QTableWidgetItem *nameItem = new QTableWidgetItem(query.value(0).toString());
		QTableWidgetItem *descItem = new QTableWidgetItem(query.value(1).toString());
		QTableWidgetItem *valueItem = new QTableWidgetItem(query.value(2).toString());
		calTable->setItem(i, 0, nameItem);
		calTable->setItem(i, 1, descItem);
		calTable->setItem(i, 2, valueItem);
	}
	calTable->setColumnWidth(0, 0);		// 안보이게
	calTable->setColumnWidth(1, 20*10);
	calTable->setColumnWidth(2, 10*10);
	calTable->setColumnWidth(3, 20*10);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(calTable);
	layout->addWidget(changeButton);
	setLayout(layout);
	db.close();
	QSqlDatabase::removeDatabase("QSQLITE");

//	m_pWMINet = new CWMINet;
//	m_pWMINet->initialComm(this, ipAddr, port, 2);
//	m_pWMINet->StartConv();

	m_nIdle = 0;
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(100);
}


void CalibrationTab::change()
{
	QString sensorName;


//    QModelIndexList indexes = model->selectedIndexes();
//    QList<QTableWidgetItem*> items;
 //   for (int i = 0; i < indexes.count(); ++i) {
   //     QModelIndex index = indexes.at(i);
     //   if (isIndexHidden(index))
       //     continue;
	
	
	QList<QTableWidgetItem *> list =  calTable->selectedItems();
	if(list.size() >= 1)
	{
		if(m_bPasswordOK == false)
		{
//			CPasswordDlg dlg(this, Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
//			if(dlg.exec() == QDialog::Accepted)
			{
//				QString password = dlg.GetPassword();
/*                if(CheckPassword("tms", password.toLatin1().data()) ==false)
				{
					QMessageBox::information(this, tr("TMS"), tr("비밀번호가 올바르지 않습니다."));
					return;
				}
*/				m_bPasswordOK = true;
			}
//			else
//				return;
		}

		
		QTableWidgetItem* item = list[0];
		int row = item->row();
		QString name=calTable->item(row,0)->text(), desc=calTable->item(row,1)->text();
        double val = atof(calTable->item(row,2)->text().toLatin1().data());
		CDecimalInput Dlg(desc, val);

		if(Dlg.exec() == QDialog::Accepted)
		{
			QString init =  QString::number(Dlg.m_nValue);

//			WriteValue(name, Dlg.m_nValue);

			// Change Event......
			QString code = name.mid(0, 5);
            SaveCalibrationTime(code.toLatin1().data());

			QTableWidgetItem *formatItem = new QTableWidgetItem(init);
			calTable->setItem(row, 2, formatItem);
	
			QString param = "InitialValue";
//			m_pWMINet->parameterChange(name, param, init);

		}
	}
	else
		QMessageBox::information(this, tr("TMS"), tr("변경할 항목이 선택되지 않았습니다."));
		
}

void CalibrationTab::onTimer()
{
//	if(m_pWMINet)
//		m_pWMINet->onTimer();
}

bool CalibrationTab::ExtractData( char *Buffer)
{
	QString    str;
	char	   *buf;
	quint32	   len;
	quint16	   idx = sizeof(WMINET_HEADER);
	WMINET_HEADER *pRcvHeader = (WMINET_HEADER *)Buffer;

	if(pRcvHeader->CmdGroup & 0x8000 || pRcvHeader->SubCmd & 0x8000)
	{
		len = *( quint32* )&Buffer[ idx ];
		idx+=sizeof( quint32 );
		buf = new char[ len+1 ];
		memcpy( buf, &Buffer[ idx ], len );
		idx+=len;
		buf[ len ] = 0;
		QString str = tr("WMINet Err(%s) %1").arg(buf);
		QMessageBox::information(this, tr("통신"), str);
		delete [] buf;
		return false;
	}

	switch(pRcvHeader->CmdGroup)
	{
		case WMI_CMD_CONNECT:
			if(g_bDebug)
				qDebug("WMI_CMD_CONNECT");
			len = *( quint32* )&Buffer[ idx ];
			idx+=sizeof( quint32 );
			buf = new char[ len+1 ];
			memcpy( buf, &Buffer[ idx ], len );
			idx+=len;
			buf[ len ] = 0;
//			m_pWMINet->setInitEnd();
			delete [] buf;

			break;
		case WMI_CMD_RTDATA:
			{
				switch(pRcvHeader->SubCmd)
				{
					case WMI_SCMD_PARAMETER:
					{
						if(g_bDebug)
							qDebug("WMI_SCMD_PARAMETER");
						len = *( quint32* )&Buffer[ idx ];
						idx+=sizeof( quint32 );
						buf = new char[ len+1 ];
						memcpy( buf, &Buffer[ idx ], len );
						idx+=len;
						buf[ len ] = 0;
						if(g_bDebug)
							qDebug("RCV:%s", buf);
						delete [] buf;
						break;
					}
					case WMI_SCMD_IDLE:
						break;
				}
			}
	}
	
	return true;
}

CSewerDialog::CSewerDialog(const QString& ipAddr, int port, QWidget *parent)
    : QDialog(parent)
{
	m_IpAddr = ipAddr;
	m_Port = port;		// Exts_Hasu_Server Table과 Port Table 이용 Main.cpp에서 초기화
//	m_IpAddr = "192.168.123.100";

//	qDebug("iP=%s, Port = %d",  m_IpAddr.toLatin1().data(), m_Port);

	m_pSewerNet = new CSewerNet(m_IpAddr, m_Port);


	QTabWidget *tabWidget = new QTabWidget(this);
	m_CommandTab = new CSewerCommandTab(this);
	m_DumpTab = new CDumpTab(this);

    tabWidget->addTab(m_CommandTab, tr("현장 명령"));
    tabWidget->addTab(m_DumpTab, tr("자료 조회"));

    QPushButton *okButton = new QPushButton(tr("확인"));
                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(currentChanged(int)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(okButton);
    setLayout(mainLayout);

	m_pSewerNet->linkEvent(m_CommandTab, m_DumpTab);

    setWindowTitle(tr("하수관거 RTU 원격명령"));

}

CSewerDialog::~CSewerDialog()
{
	delete m_pSewerNet;
}

void CSewerDialog::currentChanged(int index)
{
	if(index == 1)
		m_pSewerNet->dumpInit();
}

void CSewerDialog::onTimer()
{
}
/*
void CSewerDialog::onCommand(char *cmd, int type)
{
	m_pSewerNet->onCommand(cmd, type);
}
*/
CSewerCommandTab::CSewerCommandTab(QWidget *parent)
    : QDialog(parent)
{

/*	QLabel *siteLabel = new QLabel(g_siteDesc);
	siteLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	porrButton = new QPushButton(tr("펌프 재가동"));
    timeButton = new QPushButton(tr("정 교정"));
	rturButton = new QPushButton(tr("RTU 재기동"));
	flmrButton = new QPushButton(tr("유량계 재기동"));

	sampButton = new QPushButton(tr("채수펌프 수동가동"));
	samaButton = new QPushButton(tr("채수펌프 자동가동"));
	samsButton = new QPushButton(tr("채수펌프 자동멈춤"));
	conf1Button = new QPushButton(tr("채수펌프 간격설정"));
	samrButton = new QPushButton(tr("채수 Reset"));

	if(g_bSamp == false)
	{
		sampButton->setEnabled(false);
		samaButton->setEnabled(false);
		samsButton->setEnabled(false);
		conf1Button->setEnabled(false);
		samrButton->setEnabled(false);
	}

	compButton = new QPushButton(tr("세정펌프 수동가동"));
	comaButton = new QPushButton(tr("세정펌프 자동가동"));
	comsButton = new QPushButton(tr("세정펌프 자동멈춤"));
	confcButton = new QPushButton(tr("세정펌프 간격설정"));

	confvButton = new QPushButton(tr("유효범위 설정"));

	QPushButton *clearButton = new QPushButton(tr("화면지우기"));

	listWidget = new QListWidget();
	QFont font = QFont("mine");
	font.setStyleStrategy(QFont::NoAntialias);
	listWidget->setFont(font);

    connect(porrButton, SIGNAL(clicked()), this, SLOT(onPorr()));
	connect(timeButton, SIGNAL(clicked()), this, SLOT(onTime()));
	connect(rturButton, SIGNAL(clicked()), this, SLOT(onRtur()));
	connect(flmrButton, SIGNAL(clicked()), this, SLOT(onFlmr()));

	connect(sampButton, SIGNAL(clicked()), this, SLOT(onSamp()));
	connect(samaButton, SIGNAL(clicked()), this, SLOT(onSama()));
	connect(samsButton, SIGNAL(clicked()), this, SLOT(onSams()));
	connect(conf1Button, SIGNAL(clicked()), this, SLOT(onConf1()));
	connect(samrButton, SIGNAL(clicked()), this, SLOT(onSamr()));

	connect(compButton, SIGNAL(clicked()), this, SLOT(onComp()));
	connect(comaButton, SIGNAL(clicked()), this, SLOT(onComa()));
	connect(comsButton, SIGNAL(clicked()), this, SLOT(onComs()));
	connect(confcButton, SIGNAL(clicked()), this, SLOT(onConfc()));

	connect(confvButton, SIGNAL(clicked()), this, SLOT(onConfv()));

	connect(clearButton, SIGNAL(clicked()), this, SLOT(onClear()));

    connect(this, SIGNAL(sigCommand(char*, char*, int)), ((CSewerDialog*)parent)->m_pSewerNet, SLOT(onCommand(const char*, const char*, int)));
	connect(this, SIGNAL(sigTimeSync(char*)), ((CSewerDialog*)parent)->m_pSewerNet, SLOT(onTimeSync(char*)));

	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(siteLabel, 0, 0, 1, 3);		//  QWidget * widget, int fromRow, int fromColumn, int rowSpan, int columnSpan, Qt::Alignment alignment = 0 
    subLayout->addWidget(timeButton, 1, 5, 1, 1);
    subLayout->addWidget(porrButton, 1, 6, 1, 1);
    subLayout->addWidget(rturButton, 2, 5, 1, 1);

    subLayout->addWidget(flmrButton, 2, 6, 1, 1);
    subLayout->addWidget(compButton, 3, 5, 1, 1);
    subLayout->addWidget(comaButton, 3, 6, 1, 1);
    subLayout->addWidget(comsButton, 4, 5, 1, 1);
    subLayout->addWidget(confcButton, 4, 6, 1, 1);
   
	subLayout->addWidget(sampButton, 5, 5, 1, 1);
	subLayout->addWidget(samaButton, 5, 6, 1, 1);
	subLayout->addWidget(samsButton, 6, 5, 1, 1);
	subLayout->addWidget(samrButton, 6, 6, 1, 1);
	subLayout->addWidget(conf1Button, 7, 5, 1, 1);

	subLayout->addWidget(confvButton, 7, 6, 1, 1);

    subLayout->addWidget(clearButton, 9, 5, 1, 2);

	subLayout->addWidget(listWidget, 1, 0, 12, 4);
    setLayout(subLayout);
*/
}

CSewerCommandTab::~CSewerCommandTab()
{ 
}

void CSewerCommandTab::msgPut(QString& str)
{
	listWidget->addItem(str);
//	qDebug("%s", str.toLatin1().data());
}	

void CSewerCommandTab::onCommand(const char *cmd, const char* sub)
{
	listWidget->addItem("===========================================");
	QString str = "Command : ";
	str += cmd;
	listWidget->addItem(str);
	emit sigCommand(cmd, sub, 0);
}

void CSewerCommandTab::command(const char* cmd, const char* msg)
{
    if(QMessageBox::question(this, tr("하수관거 RTU"),
					   msg,
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		if(strcmp(cmd, "SMPA") == 0)
			onCommand("SMPS", "0");			//30 버젼에서는 "1"
		else
		if(strcmp(cmd, "SMPS") == 0)
			onCommand("SMPE", "0");
		else
		if(strcmp(cmd, "CMPA") == 0)		//30 버젼에서는 "1"
			onCommand("CMPS", "0");
		else
		if(strcmp(cmd, "CMPS") == 0)
			onCommand("CMPE", "0");
		else
			onCommand(cmd, "");
	}
}

void CSewerCommandTab::onPorr()
{
	command("PORR", " 펌프를 재작동 하시겠습니까?");
}

void CSewerCommandTab::onRtur()
{
	command("RTUR", " RTU를 재기동 하시겠습니까?");
}
void CSewerCommandTab::onFlmr()
{
	command("FLMR", " 유량계를 재기동 하시겠습니까?");
}
void CSewerCommandTab::onSamp()
{
	command("SAMP", " 채수장치를 수동가동 하시겠습니까?");
}
void CSewerCommandTab::onSama()
{
	command("SAMA", " 채수장치를 자동가동 하시겠습니까?");
}
void CSewerCommandTab::onSams()
{
	command("SAMS", " 채수장치를 자동멈춤 하시겠습니까?");
}
void CSewerCommandTab::onSamr()
{
	command("SAMR", " 채수장치를 Reset 하시겠습니까?");
}
void CSewerCommandTab::onComp()
{
	command("COMP", " 세정장치를 수동가동 하시겠습니까?");
}
void CSewerCommandTab::onComa()
{
	command("COMP", " 세정장치를 자동가동 하시겠습니까?");
}
void CSewerCommandTab::onComs()
{
	command("COMP", " 세정장치를 자동멈춤 하시겠습니까?");
}

void CSewerCommandTab::onConf(const char *title, const char *sub, bool type=true)
{
	CTimeInputDialog dlg(this, title, type);

	if(dlg.exec() == QDialog::Accepted)
	{
		char buf[30];

		buf[0] = *sub;
		sprintf(buf+1,  "%14d", dlg.m_interval->text().toInt());
		sprintf(buf+15,  "%14d", dlg.m_onTime->text().toInt());
		buf[29] = 0;
		onCommand("CONF", buf);
	}
}

void CSewerCommandTab::onConf1()
{
	onConf("채수펌프 가동시간", "1");
}

void CSewerCommandTab::onConfc()
{
	onConf("세정펌프 가동시간", "C");

}

void CSewerCommandTab::onConfv()
{
	onConf("유효범위 설정", "V", false);
}

void CSewerCommandTab::onTime()
{
	CTimeSyncDialog dlg(this);


	if(dlg.exec() == QDialog::Accepted)
	{
		QString str = "Command : ";
		str += "TIME(";
		str += dlg.m_sDate;
		str += ")";
		listWidget->addItem(str);
		emit sigTimeSync(dlg.m_sDate);
	}
}

void CSewerCommandTab::btnEnabled(bool enable)
{
	porrButton->setEnabled(enable);
	timeButton->setEnabled(enable);
	rturButton->setEnabled(enable);
	flmrButton->setEnabled(enable);

	if(g_bSamp == true)
	{
		sampButton->setEnabled(enable);
		samaButton->setEnabled(enable);
		samsButton->setEnabled(enable);
		conf1Button->setEnabled(enable);
		samrButton->setEnabled(enable);
	}
	compButton->setEnabled(enable);
	comaButton->setEnabled(enable);
	comsButton->setEnabled(enable);
	confcButton->setEnabled(enable);

	confvButton->setEnabled(enable);
}

void CSewerCommandTab::onClear()
{
	listWidget->clear();
}

CDumpTab::CDumpTab(QWidget *parent)
    : QDialog(parent)
{
	m_fromDate = QDateTime::currentDateTime();
	m_toDate = QDateTime::currentDateTime();
	m_fromDate = m_toDate.addSecs(-3*60*60);

	view = new QTableWidget(this);
    QStringList headerLabels;
    headerLabels << "No" << "일시" << "항목코드" << "측정값" << "단위" << "상태표시";

	view-> setColumnCount(6);
    view->setHorizontalHeaderLabels(headerLabels);

	view->setColumnWidth(0, 5*10);
	view->setColumnWidth(1, 20*10);
	view->setColumnWidth(2, 10*10);
	view->setColumnWidth(3, 20*10);
	view->setColumnWidth(4, 5*10);
	view->setColumnWidth(5, 10*10);

	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);	
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);

	fromDateSelectButton = new QPushButton(m_fromDate.toString(tr("yyyy년MM월dd일")), this);

	fromhourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		fromhourCombobox->addItem(tr("%1 시").arg(i));
	QTime time = m_fromDate.time();
	fromhourCombobox->setCurrentIndex(time.hour());

	QLabel *fromLabel = new QLabel(tr("부터"));

	toDateSelectButton = new QPushButton(m_toDate.toString(tr("yyyy년MM월dd일")), this);

	tohourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		tohourCombobox->addItem(tr("%1 시").arg(i));
	time = m_toDate.time();
	tohourCombobox->setCurrentIndex(time.hour());

	QLabel *toLabel = new QLabel(tr("까지"));

	submitButton = new QPushButton(tr("자료 재전송"), this);
	submitButton->setDefault(true);
	rdatButton = new QPushButton(tr("순시값 요청"), this);

	connect(fromDateSelectButton, SIGNAL(clicked()), this, SLOT(fromDateSelect()));
	connect(toDateSelectButton, SIGNAL(clicked()), this, SLOT(toDateSelect()));
	connect(submitButton, SIGNAL(clicked()), this, SLOT(dump()));
	connect(rdatButton, SIGNAL(clicked()), this, SLOT(rdat()));

    connect(this, SIGNAL(sigCommand(char*, char*, int)), ((CSewerDialog*)parent)->m_pSewerNet, SLOT(onCommand(const char*, const char*, int)));
//	connect(this, SIGNAL(sigCommand(char*, int)), ((CSewerDialog*)parent)->m_pSewerNet, SLOT(onCommand(char*, int)));
	connect(this, SIGNAL(sigDumpCommand(char*)), ((CSewerDialog*)parent)->m_pSewerNet, SLOT(onDumpCommand(char*)));


	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(fromDateSelectButton, 0, 0, 1, 2);		// row, col, hight, width
	subLayout->addWidget(fromhourCombobox,     0, 2, 1, 1);
	subLayout->addWidget(fromLabel,            0, 3, 1, 1);
	subLayout->addWidget(toDateSelectButton,   1, 0, 1, 2);
	subLayout->addWidget(tohourCombobox,       1, 2, 1, 1);
	subLayout->addWidget(toLabel,              1, 3, 1, 1);

	subLayout->addWidget(submitButton,         1, 4, 1, 1);
	subLayout->addWidget(rdatButton,           1, 5, 1, 1);

	subLayout->addWidget(view,		           2, 0, 12, 6);

	setLayout(subLayout);
}

CDumpTab::~CDumpTab()
{
}

void CDumpTab::fromDateSelect()
{
	QDate date = m_fromDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_fromDate.setDate(dlg.getDate());
		fromDateSelectButton->setText(m_fromDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CDumpTab::toDateSelect()
{
	QDate date = m_toDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_toDate.setDate(dlg.getDate());
		toDateSelectButton->setText(m_toDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CDumpTab::dump()
{
	QTime fromTime(fromhourCombobox->currentIndex(), 0, 0);
	QTime toTime(tohourCombobox->currentIndex(), 0, 0);

	m_fromDate.setTime(fromTime);
	m_toDate.setTime(toTime);

	QString str;

	if(m_fromDate.toTime_t() >= m_toDate.toTime_t()) 
	{
		QMessageBox::information(this, tr("하수관거 RTU"), tr("시작시간이 끝시간 보다 크거나 같습니다.\n시간을 다시 선택하여 주십시요."));
		return;
	}

	if(QMessageBox::question(this, tr("하수관거 RTU"),
					   tr(" 자료재전송을 요청 하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		char buf[30];
		QString str;
		str = m_fromDate.toString(tr("yyyyMMddhh0000"));
        memcpy(buf, str.toLatin1().data(), 14);
		str = m_toDate.toString(tr("yyyyMMddhh0000"));
        memcpy(buf+14, str.toLatin1().data(), 14);
		buf[28] = 0;
	
		emit sigDumpCommand(buf);
	}
}

void CDumpTab::rdat()
{
	if(QMessageBox::question(this, tr("하수관거 RTU"),
					   tr(" 순시값을 요청 하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
		emit sigCommand("RDAT", "", 1);
}

void CDumpTab::dataPut(int cnt, char *data)
{
	int pos = 0;
	view-> setRowCount(cnt);
	if(cnt == 0)
		return;
	for(int row = 0; row < cnt; row++)
	{
		QString str;
		char	buf[30];
		
		str.setNum(row+1);
		QTableWidgetItem *item0 = new QTableWidgetItem(str);

		memcpy(buf, data+pos, 4);
		buf[4] = '/';
		pos += 4;
		memcpy(buf+5, data+pos, 2);
		buf[7] = '/';
		pos += 2;
		memcpy(buf+8, data+pos, 2);
		buf[10] = ' ';
		pos += 2;

		memcpy(buf+11, data+pos, 2);
		buf[13] = ':';
		pos += 2;
		memcpy(buf+14, data+pos, 2);
		buf[16] = ':';
		pos += 2;
		memcpy(buf+17, data+pos, 2);
		buf[19] = 0;
		pos += 2;
		str = buf;
		QTableWidgetItem *item1 = new QTableWidgetItem(str);

		memcpy(buf, data+pos, 6);
		buf[6] = 0;
		str = buf;
		pos += 6;
		QTableWidgetItem *item2 = new QTableWidgetItem(str);

		memcpy(buf, data+pos, 10);
		buf[10] = 0;
		str = buf;
		pos += 10;
		QTableWidgetItem *item3 = new QTableWidgetItem(str);

		memcpy(buf, data+pos, 3);
		buf[3] = 0;
		str = buf;
		pos += 3;
		QTableWidgetItem *item4 = new QTableWidgetItem(str);

		memcpy(buf, data+pos, 2);
		buf[2] = 0;
		str = buf;
		pos += 2;
		QTableWidgetItem *item5 = new QTableWidgetItem(str);

        view->setItem(row, 0, item0);
        view->setItem(row, 1, item1);
        view->setItem(row, 2, item2);
        view->setItem(row, 3, item3);
        view->setItem(row, 4, item4);
        view->setItem(row, 5, item5);
	}
    view->resizeColumnsToContents();
    view->show();
}

void CDumpTab::btnEnabled(bool enable)
{
	submitButton->setEnabled(enable);
	rdatButton->setEnabled(enable);
}

CSewerNet::CSewerNet(const QString& ipAddr, int port, int type)
{
	m_ptcpSocket = NULL;
	m_nLastconnectmSec = 0;
	m_bEnding = false;

	m_nPort			= port;
	m_ipAddr		= ipAddr;
	m_nType			= type;
	m_bDumpInit		= false;
	m_nTimer		= 0;
	m_bEnabled		= true;

	m_ptcpSocket = new QTcpSocket(this);

	connect(m_ptcpSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_ptcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_ptcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		 this, SLOT(displayError(QAbstractSocket::SocketError)));

}

CSewerNet::~CSewerNet()
{ 
	if(m_ptcpSocket)
		delete m_ptcpSocket;
}

void CSewerNet::onTimer()
{
//	qDebug("Enable : %d, m_nTimer : %d", m_bEnabled, m_nTimer);
	if(m_bEnabled == false && ++m_nTimer > 6)	// 6*500ms=3S
	{
		emit sigbtnEnabled(true);
		if(m_bDumpInit)
			emit sigbtnEnabled2(true);
		m_bEnabled = true;
	}
}

void CSewerNet::linkEvent(CSewerCommandTab* commandTab, CDumpTab* dumpTab)
{
	connect(this, SIGNAL(sigMsgPut(QString&)), commandTab, SLOT(msgPut(QString&)));
	connect(this, SIGNAL(sigDataPut(int, char*)), dumpTab, SLOT(dataPut(int, char*)));

	connect(this, SIGNAL(sigbtnEnabled(bool)), commandTab, SLOT(btnEnabled(bool)));
	connect(this, SIGNAL(sigbtnEnabled2(bool)), dumpTab, SLOT(btnEnabled(bool)));
}

void CSewerNet::disconnect()
{
	m_ptcpSocket->disconnectFromHost();
}

void CSewerNet::msgPut(QString msg)
{
	emit sigMsgPut(msg);
}

void CSewerNet::msgDisp(QString head, char *msg)
{
	QString tmp, str = head;
	while(*msg)
	{
		if(*msg < 0x20 || *msg > 'z')
		{	
			tmp.sprintf("<%02X>", *msg);
			str += tmp;
		}
		else
			str += *msg;
		msg++;
	}

	emit sigMsgPut(str);
}


void CSewerNet::connectToHost()
{
	msgPut("connectToHost");
 	if(m_ptcpSocket->state() != QAbstractSocket::ConnectedState)
		m_ptcpSocket->connectToHost(m_ipAddr, m_nPort);
	else
	{
		while(int len = m_ptcpSocket->bytesAvailable())
			m_ptcpSocket->read(m_RspBuffer, len > INBUFSIZE ? INBUFSIZE : len);
		connected();
	}
}

void CSewerNet::connected()
{
	msgPut("Connected");
	m_nState = PROT_WAIT_ACK;
	m_dwLastMsec = m_nCurrmSec;
	char data = ENQ;
	m_ptcpSocket->write(&data, 1);
	m_nWaitLen = 1;
	msgPut("Send : ENQ");

}

void CSewerNet::sendCommad(char *buf, int len)
{
	m_bEnabled = false;
	emit sigbtnEnabled(false);
	if(m_bDumpInit)
		emit sigbtnEnabled2(false);
	m_nTimer = 0;

	if(m_ptcpSocket->state() != QAbstractSocket::ConnectedState)
		connectToHost();
	else
		m_ptcpSocket->write(buf, len);
}

void CSewerNet::readyRead()
{
	QString msg;
	char buf[10];
	qint32 len;
	while(( len = m_ptcpSocket->bytesAvailable()) != 0)
	{
		if(len < m_nWaitLen )
			return;
		if(len > INBUFSIZE)
			len = INBUFSIZE;
		
		switch( m_nState ) 
		{
			case PROT_IDLE:
		 		m_ptcpSocket->read(m_RspBuffer, 1);
				break;
			case PROT_WAIT_ACK:
			 		m_ptcpSocket->read(m_RspBuffer, 1);
					if(m_RspBuffer[0] == ACK)
					{
						msgPut("Recv : ACK");
						m_nState = PROT_WAIT_HEAD;
						SendCommand();
						m_nWaitLen = 11;
					}
					else
					{
						msg.sprintf("Wait ACK Recv :[%02X] ", m_RspBuffer[0]);
						msgPut(msg);
						m_nState = PROT_IDLE;
					}
					break;
			case PROT_WAIT_HEAD:
			 		m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
					msg.sprintf("Recv Head [%d] ", m_nWaitLen);
					msgPut(msg);
					m_RspBuffer[m_nWaitLen] = 0;
					msgDisp("Head : ", m_RspBuffer);

					if(m_RspBuffer[0] == STX)
					{
						memcpy(buf, &m_RspBuffer[7], 4);
						buf[4] = 0;
						m_nWaitLen = atoi(buf) - 11;	//head 제외
						m_nState = PROT_WAIT_RES;
					}
					else
					{
						while(int len = m_ptcpSocket->bytesAvailable())
							m_ptcpSocket->read(m_RspBuffer, len > INBUFSIZE ? INBUFSIZE : len);
					}
					break;
			case PROT_WAIT_RES:
				{
					msg.sprintf("Recv Count [%d]", m_nWaitLen);
					msgPut(msg);
					m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
					m_RspBuffer[m_nWaitLen] = 0;
					msgDisp("Recv : ", m_RspBuffer);
					int cnt = (m_nWaitLen-4)/35;
					msg.sprintf("Recv[%d,%d] :", m_nWaitLen, cnt);
					msgDisp(msg, m_RspBuffer);
					if(m_sCommand == "RDAT" || m_sCommand == "DUMP")
						emit sigDataPut(cnt, m_RspBuffer);
					char data = ACK;
					m_ptcpSocket->write(&data, 1);
					msgPut("Send : ACK");
					m_nWaitLen = 1;
					m_nState = PROT_WAIT_EOT;
				}
				break;
			case PROT_WAIT_EOT:
				m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
				if(m_RspBuffer[0] == EOT)
				{
					m_nState = PROT_IDLE;
					msgPut("Recv : EOT");
					emit sigbtnEnabled(true);
					if(m_bDumpInit)
						emit sigbtnEnabled2(true);
					m_bEnabled = true;
					m_nTimer = 0;
	//				disconnect();
				}
				break;
			default:
				break;
		}
	}
}

void CSewerNet::CheckSum(char *buf, int len, char* sum)
{
	unsigned int result = 0;
	for(int i = 0; i < len; i++)
		result += buf[i];
	sprintf(sum, "%02X",result & 0xff);
}

void CSewerNet::onCommand(const char *cmd, const char *sub, int type)
{
	m_nType = type;
	connectToHost();
	m_sCommand = cmd;
	m_sSubCommand = sub;
}

void CSewerNet::onDumpCommand(char *date)
{
	m_nType = 1;
	m_sCommand = "DUMP";
	memcpy(m_date, date, 28);
	connectToHost();
}

void CSewerNet::onTimeSync(char *date)
{
	m_nType = 0;
	m_sCommand = "TIME";
	memcpy(m_date, date, 14);
	m_date[14] = 0;
	connectToHost();
}

void CSewerNet::SendCommand()
{
/*	char sendBuf[80];
	char sum[5];
	int pos = 0;
	int siteLen = g_siteCode.length();

	sendBuf[pos++] = STX;
	memset(&sendBuf[pos], 0x20, 6);
    memcpy(&sendBuf[pos], g_siteCode.toLatin1().data(), siteLen);
	pos += 6;
    memcpy(&sendBuf[pos],  m_sCommand.toLatin1().data(), 4);
	pos += 4;
	if(m_sCommand == "DUMP")
	{
		memcpy(&sendBuf[pos],  m_date, 28);
		pos += 28;
	}
	else
	if(m_sCommand == "TIME")
	{
		memcpy(&sendBuf[pos],  m_date, 14);
		pos += 14;
	}
	else
	if(m_sCommand == "CONF")
	{
        memcpy(&sendBuf[pos], m_sSubCommand.toLatin1().data(), 29);
		pos += 29;
	}
*	else	//SMPS, SMPE, CMPS, CMPE
	if(m_sCommand == "SMPS" || m_sCommand == "CMPS")
	{
        memcpy(&sendBuf[pos], m_sSubCommand.toLatin1().data(), 1);
		pos += 1;
	}
*	sendBuf[pos++] = ETX;
	CheckSum(sendBuf, pos, sum);
	memcpy(&sendBuf[pos], sum, 2);
	pos += 2;
	sendBuf[pos++] = CR;
	sendBuf[pos] = 0;
	msgDisp("Send : ", sendBuf);

    sendCommad(sendBuf, pos);
    */
}

void CSewerNet::disconnected()
{
	msgPut("Disonnected");
}

void CSewerNet::displayError(QAbstractSocket::SocketError socketError)
{
	 QString msg;
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
			msg = tr("The host was not found. Please check the " 
                                     "host name and port settings.");
         break;
     case QAbstractSocket::ConnectionRefusedError:
			msg = tr("The connection was refused by the peer. "
                                     "settings are correct.");
         break;
     default:
		 msg = tr("The following error occurred: %1.") 
                                  .arg(m_ptcpSocket->errorString());
     }
	 //if(m_nType == 0)
		msgPut(msg);
}

void CSewerNet::dumpInit()
{
	m_bDumpInit = true;
}

CTimeSyncDialog::CTimeSyncDialog(QWidget *parent)
    : QDialog(parent)
{
	QDate date = QDate::currentDate();
	QTime time = QTime::currentTime();

	setWindowTitle(tr("시간 교정"));

	QLabel *ymdLabel = new QLabel(tr("년월일[yyyymmdd]"), this);

	m_ymdEdit = new MyLineEdit(this);

	m_ymdEdit->setMaxLength(10);
	m_ymdEdit->setMinimumWidth(100);
	m_ymdEdit->setText(date.toString("yyyyMMdd"));
	
	QLabel *hmsLabel = new QLabel(tr("시분초[hhmmss]"), this);

	m_hmsEdit = new MyLineEdit(this);

	m_hmsEdit->setMaxLength(10);
	m_hmsEdit->setMinimumWidth(100);
	m_hmsEdit->setText(time.toString("hhmmss"));

	QPushButton *ok = new QPushButton(this);
	ok->setText(tr("확인"));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

	QPushButton *cancel = new QPushButton(this);
	cancel->setText(tr("취소"));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(ymdLabel, 0, 0, 1, 2);		// row, col, hight, width
    subLayout->addWidget(m_ymdEdit, 0, 2, 1, 2);
    subLayout->addWidget(hmsLabel, 1, 0, 1, 2);
    subLayout->addWidget(m_hmsEdit, 1, 2, 1, 2);
    subLayout->addWidget(ok, 2, 1, 1, 1);
    subLayout->addWidget(cancel, 2, 2, 1, 1);
    setLayout(subLayout);
}

bool CTimeSyncDialog::isNum(QString& str)
{
	for(int i = 0; i < str.length(); i++)
		if(str[i] > '9' || str[i] < '0')
			return false;
	return true;
}

void CTimeSyncDialog::accept()
{
	QString date = m_ymdEdit->text();
	QString time = m_hmsEdit->text();
	if(date.length() == 8 && time.length() == 6 && isNum(date) && isNum(time))
	{
        memcpy(m_sDate, date.toLatin1().data(), 8);
        memcpy(m_sDate+8, time.toLatin1().data(), 6);
		m_sDate[14] = 0;
		QDialog::accept();
	}
	else
		QMessageBox::information(this, tr("시간 교정"), tr("입력한 일시가 올바르지 않습니다. 다시 입력하십시요."));
}

CTimeInputDialog::CTimeInputDialog(QWidget *parent, QString title, bool type)
    : QDialog(parent)
{
	setWindowTitle(title);

	QLabel *intervalLabel = new QLabel(tr("간격"), this);

	if(type == false)
		intervalLabel->setText(tr("최소값"));

	m_interval = new QLineEdit(this);
	m_interval->setMaxLength(14);
	m_interval->setMinimumWidth(100);
	
	QLabel *interMinLabel = new QLabel(tr("분"), this);

	QLabel *onTimeLabel = new QLabel(tr("가동시간"), this);

	if(type == false)
		onTimeLabel->setText(tr("최대값"));
	m_onTime = new QLineEdit(this);
	m_onTime->setMaxLength(14);
	m_onTime->setMinimumWidth(100);
	
	QLabel *onTimeMinLabel = new QLabel(tr("분"), this);

	QPushButton *ok = new QPushButton(this);
	ok->setText(tr("확인"));

	QPushButton *cancel = new QPushButton(this);
	cancel->setText(tr("취소"));

	m_interval->installEventFilter(this);
	m_onTime->installEventFilter(this);

	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(intervalLabel, 0, 0, 1, 1);		// row, col, hight, width
	subLayout->addWidget(m_interval, 0, 1, 1, 2);
	if(type)
		subLayout->addWidget(interMinLabel, 0, 3, 1, 1);
	else
		interMinLabel->setVisible(false);
	subLayout->addWidget(onTimeLabel, 1, 0, 1, 1);
	subLayout->addWidget(m_onTime, 1, 1, 1, 2);
	if(type)
		subLayout->addWidget(onTimeMinLabel, 1, 3, 1, 1);
	else
		onTimeMinLabel->setVisible(false);

	subLayout->addWidget(ok, 2, 1, 1, 1);
	subLayout->addWidget(cancel, 2, 2, 1, 1);
	setLayout(subLayout);
}

bool CTimeInputDialog::eventFilter(QObject* pObj, QEvent *e)
{
	if(e->type() == QEvent::MouseButtonRelease)
	{
		QString value;
		if(pObj == m_interval)
			value = m_interval->text();
		else
			value = m_onTime->text();
	    Calculator calc(value);
		if(calc.exec() == QDialog::Accepted)
		{
			if(pObj == m_interval)
				m_interval->setText(calc.m_Value);
			else
				m_onTime->setText(calc.m_Value);
		}
	}
	return false;
}

void CTimeInputDialog::accept()
{
    QDialog::accept();
}

CLanSetupDialog::CLanSetupDialog(QWidget *parent)
    : QDialog(parent)
{

	setWindowTitle(tr("LAN 설정"));

	GetItem("ETH0_ONBOOT", onboot1);
	GetItem("ETH0_BOOTPROTO", bootproto1);
	GetItem("ETH0_IPADDR", idaddr1);
	GetItem("ETH0_NETMASK", netmask1);
	QString lan1IP = idaddr1;
	QString lan1Mask = netmask1;

	GetItem("ETH1_ONBOOT", onboot2);
	GetItem("ETH1_BOOTPROTO", bootproto2);
	GetItem("ETH1_IPADDR", idaddr2);
	GetItem("ETH1_NETMASK", netmask2);
	QString lan2IP = idaddr2;
	QString lan2Mask = netmask2;
	
	GetItem("ETH0_GATEWAY", Gateway);
	QString	gateway = Gateway;
	GetItem("DNS", Dns1);
	QString primaryDNS = Dns1;
	GetItem("DNS2", Dns2);
	QString secondaryDNS = Dns2;

	GetItem("ETH0_MAC", Mac1);
	GetItem("ETH1_MAC", Mac2);

	QGroupBox *lan1GroupBox = new QGroupBox(tr("LAN-1"));

	m_lan1IP = new MyLineEdit(this);
	m_lan1IP->setMaxLength(15);
	m_lan1IP->setMinimumWidth(100);
	m_lan1IP->setText(lan1IP);
	m_lan1Mask = new MyLineEdit(this);
	m_lan1Mask->setMaxLength(15);
	m_lan1Mask->setMinimumWidth(100);
	m_lan1Mask->setText(lan1Mask);
	m_lan2IP = new MyLineEdit(this);
	m_lan2IP->setMaxLength(15);
	m_lan2IP->setText(lan2IP);
	m_lan2IP->setMinimumWidth(100);
	m_lan2Mask = new MyLineEdit(this);
	m_lan2Mask->setMaxLength(15);
	m_lan2Mask->setMinimumWidth(100);
	m_lan2Mask->setText(lan2Mask);
	m_gateway = new MyLineEdit(this);
	m_gateway->setMaxLength(15);
	m_gateway->setMinimumWidth(100);
	m_gateway->setText(gateway);

	QLabel *lan1IPLabel = new QLabel(tr("I P:"));

	QLabel *lan1IPValLabel = new QLabel(lan1IP);
	lan1IPValLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	QLabel *lan1MaskLabel = new QLabel(tr("NetMask:"));
	QLabel *lan1MaskValLabel = new QLabel(lan1Mask);
	lan1MaskValLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	QVBoxLayout *vbox1 = new QVBoxLayout;
	vbox1->addWidget(lan1IPLabel);
	vbox1->addWidget(m_lan1IP);
	vbox1->addWidget(lan1MaskLabel);
	vbox1->addWidget(m_lan1Mask);
	lan1GroupBox->setLayout(vbox1);

	QGroupBox *lan2GroupBox = new QGroupBox(tr("LAN-2"));

	QLabel *lan2IPLabel = new QLabel(tr("I P:"));
	QLabel *lan2MaskLabel = new QLabel(tr("NetMask:"));

	QVBoxLayout *vbox2 = new QVBoxLayout;
	vbox2->addWidget(lan2IPLabel);
	vbox2->addWidget(m_lan2IP);
	vbox2->addWidget(lan2MaskLabel);
	vbox2->addWidget(m_lan2Mask);
	lan2GroupBox->setLayout(vbox2);

	QLabel *gatewayLabel = new QLabel(tr("Gateway:"));

    QPushButton *okButton = new QPushButton(tr("확인"));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QPushButton *cancelButton = new QPushButton(tr("취소"));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addWidget(okButton);
	buttonLayout->addWidget(cancelButton);
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(lan1GroupBox);
    mainLayout->addWidget(lan2GroupBox);
    mainLayout->addWidget(gatewayLabel);
    mainLayout->addWidget(m_gateway);
	mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void CLanSetupDialog::accept()
{
	QString msg;
	msg = "LAN 설정을 다음의 정보로 변경 하시겠습니까?\n";
	msg += "Lan1 IP:";
	msg += m_lan1IP->text();
	msg += ", NetMask:";
	msg += m_lan1Mask->text();
	msg += "\n";
	msg += "Lan2 IP:";
	msg += m_lan2IP->text();
	msg += ", NetMask:";
	msg += m_lan2Mask->text();
	msg += "\n";
	msg += "Gateway:";
	msg += m_gateway->text();

	if(QMessageBox::question(this, tr("LAN 설정"),
					   msg,
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		QString str;
		FILE *pFile;
		pFile = ::fopen("/etc/ether.conf", "w+");
		if(pFile)
		{
			::fputs("#eth0\n", pFile);
			::fputs("ETH0_ONBOOT=", pFile);
			::fputs("y\n", pFile);
			::fputs("ETH0_BOOTPROTO=static\n", pFile);
			::fputs("ETH0_IPADDR=", pFile);
			str = m_lan1IP->text();
            ::fputs(str.toLatin1().data(), pFile);
			::fputs("\n", pFile);
			::fputs("ETH0_NETMASK=", pFile);
			str = m_lan1Mask->text();
            ::fputs(str.toLatin1().data(), pFile);
			::fputs("\n", pFile);
			::fputs("ETH0_GATEWAY=", pFile);
			str = m_gateway->text();
            ::fputs(str.toLatin1().data(), pFile);
			::fputs("\n", pFile);
			::fputs("ETH0_MAC=", pFile);
			::fputs(Mac1, pFile);
			::fputs("\n", pFile);

			::fputs("\n#eth1\n", pFile);
			::fputs("ETH1_ONBOOT=", pFile);
			::fputs(onboot2, pFile);
			::fputs("\n", pFile);
			::fputs("ETH1_BOOTPROTO=", pFile);
			::fputs(bootproto2, pFile);
			::fputs("\n", pFile);
			::fputs("ETH1_IPADDR=", pFile);
			str = m_lan2IP->text();
            ::fputs(str.toLatin1().data(), pFile);
			::fputs("\n", pFile);
			::fputs("ETH1_NETMASK=", pFile);
			str = m_lan2Mask->text();
            ::fputs(str.toLatin1().data(), pFile);
			::fputs("\n", pFile);
			::fputs("ETH1_MAC=", pFile);
			::fputs(Mac2, pFile);
			::fputs("\n", pFile);

			::fputs("DNS=", pFile);
			::fputs(Dns1, pFile);
			::fputs("\n", pFile);
			::fputs("DNS2=", pFile);
			::fputs(Dns2, pFile);
			::fputs("\n", pFile);
			::fclose(pFile);

			if(system("/etc/rc.d/rc.network restart >/dev/null") < 0)
				QMessageBox::information(this, tr("Network"), tr("네트워크 정보변경에 실패했습니다."));
		}
	}
	QDialog::accept();
}

CUcityNet::CUcityNet(const QString& ipAddr, int port, int type)
{
	m_ptcpSocket = NULL;
	m_nLastconnectmSec = 0;
	m_bEnding = false;

	m_nPort			= port;
	m_ipAddr		= ipAddr;
	m_nType			= type;
	m_bDumpInit		= false;
	m_nTimer		= 0;
	m_bEnabled		= true;

	m_ptcpSocket = new QTcpSocket(this);

	connect(m_ptcpSocket, SIGNAL(connected()), this, SLOT(connected()));
	connect(m_ptcpSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	connect(m_ptcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		 this, SLOT(displayError(QAbstractSocket::SocketError)));

}

CUcityNet::~CUcityNet()
{ 
	disconnect();
	if(m_ptcpSocket)
		delete m_ptcpSocket;
}

void CUcityNet::onTimer()
{
}

void CUcityNet::linkEventUCity(CU_CityDialog* ucityDlg)
{
	connect(this, SIGNAL(sigDataPutUCity(int, char*, char*)), ucityDlg, SLOT(dataPut(int, char*, char*)));
	connect(this, SIGNAL(sigViewList()), ucityDlg, SLOT(viewList()));
	connect(this, SIGNAL(sigMsgPut(QString&)), ucityDlg, SLOT(msgPut(QString&)));
}

void CUcityNet::linkEventRainSensor(CRainSensorDlg* rainSensorDlg)
{
	connect(this, SIGNAL(sigDataPutRainSensor(int, char*)), rainSensorDlg, SLOT(dataPut(int, char*)));
}

void CUcityNet::disconnect()
{
	m_ptcpSocket->disconnectFromHost();
}

void CUcityNet::msgPut(QString )
{
//	printf("%s\n",  msg.toLatin1().data());
	//	emit sigMsgPut(msg);
}

void CUcityNet::msgDisp(QString head, char *msg, int cnt)
{

	QString tmp, str = head;
	while(cnt--)
	{
		if(*msg <= 0x20 || *msg > 'z')
		{	
			tmp.sprintf("<%02X>", *msg);
			str += tmp;
		}
		else
			str += *msg;
		msg++;
	}
    //qDebug(str.toLatin1().data());
//	printf("%s\n",  str.toLatin1().data());
//	emit sigMsgPut(str);
}


void CUcityNet::connectToHost()
{
	msgPut("connectToHost");
 	if(m_ptcpSocket->state() != QAbstractSocket::ConnectedState)
		m_ptcpSocket->connectToHost(m_ipAddr, m_nPort);
	else
	{
		while(int len = m_ptcpSocket->bytesAvailable())
			m_ptcpSocket->read(m_RspBuffer, len > INBUFSIZE ? INBUFSIZE : len);
		connected();
	}
}

void CUcityNet::connected()
{
/*	msgPut("Connected");
	char sendBuf[100];
	int pos = 0;
	if(m_nType == 1)
	{
		char sum[5];
		sendBuf[pos++] = STX;
		memset(&sendBuf[pos], 0x20, 21);
		pos += 21;
        memcpy(&sendBuf[pos],  m_sCommand.toLatin1().data(), m_sCommand.length());
		pos += m_sCommand.length();
		memcpy(&sendBuf[pos],  m_date, 24);
		pos += 24;
		sendBuf[pos++] = ETX;
		CheckSum(sendBuf, pos, sum);
		memcpy(&sendBuf[pos], sum, 2);
		pos += 2;
		sendBuf[pos++] = CR;
		sendBuf[pos] = 0;
		msgDisp("Send : ", sendBuf, pos);

		sendCommad(sendBuf, pos);
		m_nState = PROT_WAIT_STX;
		m_nWaitLen = 1;
	}
	else
	if(m_nType == 2)
	{
		memcpy(sendBuf+pos,  "RAIN", 4);
		pos += 4;
		if(m_sCommand == "DUMP")
		{
			quint32 len = 4+ 24 + 4;
            *( quint32* )&sendBuf[pos] = len;
			pos += 4;

            memcpy(sendBuf+pos,  m_sCommand.toLatin1().data(), m_sCommand.length());
			pos += m_sCommand.length();
			memcpy(sendBuf+pos,  m_date, 24);
			pos += 24;
			m_nState = PROT_WAIT_RAIN_HEAD;
			m_nWaitLen = 8;
		}
		else
		if(m_sCommand == "TIME")
		{
			quint32 len = 4 + 12 + 4;
			*( quint32* )&sendBuf[pos] = len;
			pos += 4;

            memcpy(&sendBuf[pos],  m_sCommand.toLatin1().data(), m_sCommand.length());
			pos += m_sCommand.length();
			memcpy(&sendBuf[pos],  m_date, 12);
			pos += 12;
			m_nState = PROT_WAIT_RAIN_TIME_HEAD;
			m_nWaitLen = 12+4;
		}
		else
		if(m_sCommand == "RSET")
		{
			quint32 len = 4 + 4;
			*( quint32* )&sendBuf[pos] = len;
			pos += 4;

            memcpy(&sendBuf[pos],  m_sCommand.toLatin1().data(), m_sCommand.length());
			pos += m_sCommand.length();
//			memcpy(&sendBuf[pos],  m_date, 12);
//			pos += 12;
			m_nState = PROT_WAIT_RAIN_RSET;
//			m_nWaitLen = 12+4;
			m_nWaitLen = 4;
		}
		else
		{
			qDebug("Commad Error");
			return;
		}
		quint32 crc = (quint16)CheckSumUint(sendBuf+4, pos-4);
		*( quint32* )&sendBuf[pos] = crc;
		pos += 4;
		sendBuf[pos] = 0;
		msgDisp("Send : ", sendBuf, pos);

		sendCommad(sendBuf, pos);
    }*/
}

void CUcityNet::onTimeSync(char *date)
{
	m_nType = 2;
	m_sCommand = "TIME";
	memcpy(m_date, date, 12);
	m_date[12] = 0;
	connectToHost();
}

void CUcityNet::sendCommad(char *buf, int len)
{
	m_nTimer = 0;

	if(m_ptcpSocket->state() != QAbstractSocket::ConnectedState)
		connectToHost();
	else
		m_ptcpSocket->write(buf, len);
}

void CUcityNet::readyRead()
{
/*	QString msg;
	char buf[10];
	qint32 len;
	while(( len = m_ptcpSocket->bytesAvailable()) != 0)
	{
		if(len < m_nWaitLen )
			return;
		if(len > INBUFSIZE)
			len = INBUFSIZE;
		
		switch( m_nState ) 
		{
			case PROT_IDLE:
		 		m_ptcpSocket->read(m_RspBuffer, 1);
				break;
			case PROT_WAIT_STX:
			 	m_ptcpSocket->read(m_RspBuffer, 1);
				if(m_RspBuffer[0] == STX)
				{
					msgPut("Recv : STX\n");
					m_nState = PROT_WAIT_HEAD;
					m_nWaitLen = 40;
				}
				else
				if(m_RspBuffer[0] == EOT)
				{
					m_nState = PROT_IDLE;
					msgPut("Recv : EOT\n");
					m_nTimer = 0;
					emit sigViewList();
				}
				else
				{
					m_nState = PROT_IDLE;
					emit sigViewList();
					m_nTimer = 0;
				}
				break;
			case PROT_WAIT_HEAD:
			 	m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
				msg.sprintf("Recv Head [%d] ", m_nWaitLen);
				msgPut(msg);
				m_RspBuffer[m_nWaitLen] = 0;
				msgDisp("Head : ", m_RspBuffer, m_nWaitLen);

				memcpy(buf, &m_RspBuffer[38], 2);
				memcpy(m_rdate, &m_RspBuffer[26], 12);
				m_rdate[12] = 0;

				buf[2] = 0;
				m_nWaitLen = atoi(buf) * 15+4;	//ID(2)+측정값(10)+상태(2)
				m_nState = PROT_WAIT_RES;
				break;
			case PROT_WAIT_RES:
				{
					msg.sprintf("Recv Count [%d]", m_nWaitLen);
					msgPut(msg);
					m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
					m_RspBuffer[m_nWaitLen] = 0;
					int cnt = (m_nWaitLen/15);
					msg.sprintf("Recv[%d,%d] : ", m_nWaitLen, cnt);
					msgDisp(msg, m_RspBuffer, m_nWaitLen);
					char data = ACK;
					m_ptcpSocket->write(&data, 1);
					m_nWaitLen = 1;
					m_nState = PROT_WAIT_STX;
					msgPut("Send : ACK");
					if(m_sCommand == "DUMP")
						emit sigDataPutUCity(cnt, m_rdate, m_RspBuffer);
				}
				break;
			case PROT_WAIT_RAIN_HEAD:
			 	m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
				msg.sprintf("Recv Head [%d] ", m_nWaitLen);
				msgPut(msg);
				m_RspBuffer[m_nWaitLen] = 0;
				msgDisp("Head : ", m_RspBuffer, m_nWaitLen);

				m_nWaitLen = *(quint32*)&m_RspBuffer[4];
				m_nCount = (m_nWaitLen - 12) / (6*4 + 12);	// +CRC포함
				msg.sprintf("Recv Count %d, Wait=%d", m_nCount, m_nWaitLen);
				msgPut(msg);
				m_nState = PROT_WAIT_RAIN_RES;
				break;
			case PROT_WAIT_RAIN_RES:
				{
					msg.sprintf("Recv Count [%d]", m_nWaitLen);
					msgPut(msg);
					m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
					m_RspBuffer[m_nWaitLen] = 0;
					msg.sprintf("Recv[%d,%d] : ", m_nWaitLen, m_nCount);
					msgDisp(msg, m_RspBuffer, m_nWaitLen);
					m_nState = PROT_IDLE;
					if(m_sCommand == "DUMP")
						emit sigDataPutRainSensor(m_nCount, m_RspBuffer+8);
				}
			case PROT_WAIT_RAIN_TIME_HEAD:
			 	m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
				msg.sprintf("Recv Head [%d] ", m_nWaitLen);
				msgPut(msg);
				m_RspBuffer[m_nWaitLen] = 0;
				msgDisp("Head : ", m_RspBuffer, m_nWaitLen);
				m_nState = PROT_IDLE;
				break;
			case PROT_WAIT_RAIN_RSET:
			 	m_ptcpSocket->read(m_RspBuffer, m_nWaitLen);
				msg.sprintf("Recv Head [%d] ", m_nWaitLen);
				msgPut(msg);
				m_RspBuffer[m_nWaitLen] = 0;
				msgDisp("Head : ", m_RspBuffer, m_nWaitLen);
				m_nState = PROT_IDLE;
				break;
			default:
				break;
			wdtHeartbeat(m_wdtSockFd, m_wdtID);
		}
    }*/
}

void CUcityNet::CheckSum(char *buf, int len, char* sum)
{
	quint16 result = CheckSumUint(buf, len);
	sprintf(sum, "%02X",result & 0xff);
}

quint16 CUcityNet::CheckSumUint(char *buf, int len)
{
	int	i, j;
	quint16 crc = 0x0ffff;

	for( i = 0; i < len; i++ )
	{	
		crc = crc ^ buf[i];
		for( j = 0; j < 8; j++ )
		{	
			if( crc & 0x01 )
				crc = (crc >> 1) ^ 0xa001;
			else
				crc = crc >> 1;
		}
	}
	return crc;
}

void CUcityNet::onDumpCommandUCity(char *date)
{
	m_nType = 1;
	m_sCommand = "DUMP";
	memcpy(m_date, date, 24);
	connectToHost();
}

void CUcityNet::onCommandRainSensor(const char *cmd, const char *date)
{
	m_nType = 2;
	m_sCommand = cmd;
	memcpy(m_date, date, 24);
	connectToHost();
}

void CUcityNet::onRainRset()
{
	m_nType = 2;
	m_sCommand = "RSET";
	connectToHost();
}

void CUcityNet::disconnected()
{
	msgPut("Disonnected");
}

void CUcityNet::displayError(QAbstractSocket::SocketError socketError)
{
	 QString msg;
     switch (socketError) {
     case QAbstractSocket::RemoteHostClosedError:
         break;
     case QAbstractSocket::HostNotFoundError:
			msg = tr("The host was not found. Please check the " 
                                     "host name and port settings.");
         break;
     case QAbstractSocket::ConnectionRefusedError:
			msg = tr("The connection was refused by the peer. "
                                     "settings are correct.");
         break;
     default:
		 msg = tr("The following error occurred: %1.") 
                                  .arg(m_ptcpSocket->errorString());
     }
	 //if(m_nType == 0)
     qDebug("%s\n",msg.toLatin1().data());
	//	msgPut(msg);
}

void CUcityNet::dumpInit()
{
	m_bDumpInit = true;
}

CU_CityDialog::CU_CityDialog(const QString& ipAddr, QWidget *parent)
    : QDialog(parent)
{
	m_IpAddr = ipAddr;
	m_Port = UCITY_PORT;
//	m_IpAddr = "192.168.123.230";

//	qDebug("iP=%s, Port = %d",  m_IpAddr.toLatin1().data(), m_Port);

	m_pUCityNet = new CUcityNet(m_IpAddr, m_Port);


	m_fromDate = QDateTime::currentDateTime();
	m_toDate = QDateTime::currentDateTime();
	m_fromDate = m_toDate.addSecs(-1*60*60);

	view = new QTableWidget(this);
    QStringList headerLabels;
    headerLabels << "일시" << "항목코드" << "측정값" << "상태표시";

	view-> setColumnCount(4);
    view->setHorizontalHeaderLabels(headerLabels);

	view->setColumnWidth(0, 20*10);
	view->setColumnWidth(1, 10*10);
	view->setColumnWidth(2, 10*10);
	view->setColumnWidth(3, 10*10);

	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);	
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);

	fromDateSelectButton = new QPushButton(m_fromDate.toString(tr("yyyy년MM월dd일")), this);

	fromhourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		fromhourCombobox->addItem(tr("%1 시").arg(i));
	QTime time = m_fromDate.time();
	fromhourCombobox->setCurrentIndex(time.hour());

	QLabel *fromLabel = new QLabel(tr("부터"));

	toDateSelectButton = new QPushButton(m_toDate.toString(tr("yyyy년MM월dd일")), this);

	tohourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		tohourCombobox->addItem(tr("%1 시").arg(i));
	time = m_toDate.time();
	tohourCombobox->setCurrentIndex(time.hour());

	QLabel *toLabel = new QLabel(tr("까지"));

	submitButton = new QPushButton(tr("자료조회"), this);
	submitButton->setDefault(true);

    okButton = new QPushButton(tr("확인"));

	connect(fromDateSelectButton, SIGNAL(clicked()), this, SLOT(fromDateSelect()));
	connect(toDateSelectButton, SIGNAL(clicked()), this, SLOT(toDateSelect()));
	connect(submitButton, SIGNAL(clicked()), this, SLOT(dump()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	connect(this, SIGNAL(sigDumpCommand(char*)), m_pUCityNet, SLOT(onDumpCommandUCity(char*)));


	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(fromDateSelectButton, 0, 0, 1, 2);		// row, col, hight, width
	subLayout->addWidget(fromhourCombobox,     0, 2, 1, 1);
	subLayout->addWidget(fromLabel,            0, 3, 1, 1);
	subLayout->addWidget(toDateSelectButton,   1, 0, 1, 2);
	subLayout->addWidget(tohourCombobox,       1, 2, 1, 1);
	subLayout->addWidget(toLabel,              1, 3, 1, 1);

	subLayout->addWidget(submitButton,         1, 4, 1, 1);

	subLayout->addWidget(view,		           2, 0, 12, 6);

    subLayout->addWidget(okButton,			   14, 2, 1, 2);
	setLayout(subLayout);

                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	m_pUCityNet->linkEventUCity(this);

    setWindowTitle(tr("U-City 자료조회"));

}

CU_CityDialog::~CU_CityDialog()
{
	delete m_pUCityNet;
}

void CU_CityDialog::onTimer()
{
}

void CU_CityDialog::fromDateSelect()
{
	QDate date = m_fromDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_fromDate.setDate(dlg.getDate());
		fromDateSelectButton->setText(m_fromDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CU_CityDialog::toDateSelect()
{
	QDate date = m_toDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_toDate.setDate(dlg.getDate());
		toDateSelectButton->setText(m_toDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CU_CityDialog::dump()
{
	QTime fromTime(fromhourCombobox->currentIndex(), 0, 0);
	QTime toTime(tohourCombobox->currentIndex(), 0, 0);

	m_fromDate.setTime(fromTime);
	m_toDate.setTime(toTime);

	QString str;

	if(m_fromDate.toTime_t() >= m_toDate.toTime_t()) 
	{
		QMessageBox::information(this, tr("자료조회"), tr("시작시간이 끝시간 보다 크거나 같습니다.\n시간을 다시 선택하여 주십시요."));
		return;
	}

	if(QMessageBox::question(this, tr("자료조회"),
					   tr(" 자료 조회를 하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		char buf[30];
		QString str;

		str = m_fromDate.toString(tr("yyyyMMddhh00"));
        memcpy(buf, str.toLatin1().data(), 12);
		str = m_toDate.toString(tr("yyyyMMddhh00"));
        memcpy(buf+12, str.toLatin1().data(), 12);
		buf[24] = 0;

		view->setRowCount(0);
		submitButton->setEnabled(false);
		emit sigDumpCommand(buf);
	}
}

void CU_CityDialog::dataPut(int cnt, char *date, char*data)
{
	int pos = 0;
	if(cnt == 0)
		return;
	QString sDate;
	char	buf[30];

	memcpy(buf, date, 4);
	buf[4] = '/';
	pos += 4;
	memcpy(buf+5, date+pos, 2);
	buf[7] = '/';
	pos += 2;
	memcpy(buf+8, date+pos, 2);
	buf[10] = ' ';
	pos += 2;

	memcpy(buf+11, date+pos, 2);
	buf[13] = ':';
	pos += 2;
	memcpy(buf+14, date+pos, 2);
	buf[16] = 0;
	sDate = buf;

	pos = 0;
	for(int i = 0; i < cnt; i++)
	{
		QString str;

		QTableWidgetItem *item1 = new QTableWidgetItem(sDate);
		itemList[0].append(item1);

		memcpy(buf, data+pos, 3);
		buf[3] = 0;
		str = buf;
		pos += 3;
		QTableWidgetItem *item2 = new QTableWidgetItem(str);
		itemList[1].append(item2);

		memcpy(buf, data+pos, 10);
		buf[10] = 0;
		str = buf;
		pos += 10;
		QTableWidgetItem *item3 = new QTableWidgetItem(str);
		itemList[2].append(item3);

		memcpy(buf, data+pos, 2);
		buf[2] = 0;
		str = buf;
		pos += 2;
		QTableWidgetItem *item4 = new QTableWidgetItem(str);
		itemList[3].append(item4);
	}
}

void CU_CityDialog::viewList()
{
	view->setRowCount(itemList[0].count());
	int row = 0;
	while (!itemList[0].isEmpty())
	{
		view->setItem(row, 0, itemList[0].takeFirst());
		view->setItem(row, 1, itemList[1].takeFirst());
		view->setItem(row, 2, itemList[2].takeFirst());
		view->setItem(row, 3, itemList[3].takeFirst());
		row++;
	}
	view->resizeColumnsToContents();
	view->show();

	submitButton->setEnabled(true);

}

void CU_CityDialog::msgPut(QString&)
{
//	listWidget->addItem(str);
//	qDebug("%s", str.toLatin1().data());
}	

CRainSensorDlg::CRainSensorDlg(const QString& ipAddr, QWidget *parent)
    : QDialog(parent)
{
	m_IpAddr = ipAddr;
	m_Port = RAIN_SENSOR_PORT;
//	m_IpAddr = "192.168.123.230";

	m_pUCityNet = new CUcityNet(m_IpAddr, m_Port);

	m_fromDate = QDateTime::currentDateTime();
	m_toDate = QDateTime::currentDateTime();
	m_fromDate = m_toDate.addSecs(-3*60*60);

	view = new QTableWidget(this);
    QStringList headerLabels;
    headerLabels << "일시" << "PULSE" << "10분" << "시간" << "일" << "월" << "년";

	view-> setColumnCount(7);
    view->setHorizontalHeaderLabels(headerLabels);

	view->setColumnWidth(0, 20*10);
	view->setColumnWidth(1, 8*10);
	view->setColumnWidth(2, 8*10);
	view->setColumnWidth(3, 8*10);
	view->setColumnWidth(5, 8*10);
	view->setColumnWidth(6, 8*10);

	view->setSelectionBehavior(QAbstractItemView::SelectRows);
	view->setSelectionMode(QAbstractItemView::SingleSelection);	
	view->setEditTriggers(QAbstractItemView::NoEditTriggers);

	fromDateSelectButton = new QPushButton(m_fromDate.toString(tr("yyyy년MM월dd일")), this);

	fromhourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		fromhourCombobox->addItem(tr("%1 시").arg(i));
	QTime time = m_fromDate.time();
	fromhourCombobox->setCurrentIndex(time.hour());

	QLabel *fromLabel = new QLabel(tr("부터"));

	toDateSelectButton = new QPushButton(m_toDate.toString(tr("yyyy년MM월dd일")), this);

	tohourCombobox  = new QComboBox(this);
	for(int i = 0; i < 24; i++)
		tohourCombobox->addItem(tr("%1 시").arg(i));
	time = m_toDate.time();
	tohourCombobox->setCurrentIndex(time.hour());

	QLabel *toLabel = new QLabel(tr("까지"));

	submitButton = new QPushButton(tr("자료조회"), this);
	submitButton->setDefault(true);

	timeButton = new QPushButton(tr("시간동기"), this);
	rsetButton = new QPushButton(tr("초기화"), this);

	okButton = new QPushButton(tr("확인"));

	connect(fromDateSelectButton, SIGNAL(clicked()), this, SLOT(fromDateSelect()));
	connect(toDateSelectButton, SIGNAL(clicked()), this, SLOT(toDateSelect()));
	connect(submitButton, SIGNAL(clicked()), this, SLOT(dump()));
	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(timeButton, SIGNAL(clicked()), this, SLOT(onTimeSync()));
	connect(rsetButton, SIGNAL(clicked()), this, SLOT(onRset()));

    connect(this, SIGNAL(sigDumpCommand(const char*, const char*)), m_pUCityNet, SLOT(onCommandRainSensor(const char*, const char*)));
	connect(this, SIGNAL(sigTimeSync(char*)), m_pUCityNet, SLOT(onTimeSync(char*)));
	connect(this, SIGNAL(sigRset()), m_pUCityNet, SLOT(onRainRset()));


	QGridLayout *subLayout = new QGridLayout();
	subLayout->addWidget(fromDateSelectButton, 0, 0, 1, 2);		// row, col, hight, width
	subLayout->addWidget(fromhourCombobox,     0, 2, 1, 1);
	subLayout->addWidget(fromLabel,            0, 3, 1, 1);
	subLayout->addWidget(toDateSelectButton,   1, 0, 1, 2);
	subLayout->addWidget(tohourCombobox,       1, 2, 1, 1);
	subLayout->addWidget(toLabel,              1, 3, 1, 1);

	subLayout->addWidget(submitButton,         1, 4, 1, 1);
	subLayout->addWidget(timeButton,           1, 5, 1, 1);

	subLayout->addWidget(view,		           2, 0, 12, 6);

    subLayout->addWidget(okButton,			   14, 2, 1, 2);
	subLayout->addWidget(rsetButton,	           14, 5, 1, 1);
	setLayout(subLayout);

                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
	connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

	connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

	m_pUCityNet->linkEventRainSensor(this);

	setWindowTitle(tr("강우량계"));

}

CRainSensorDlg::~CRainSensorDlg()
{
	delete m_pUCityNet;
}

void CRainSensorDlg::onTimer()
{
}

void CRainSensorDlg::fromDateSelect()
{
	QDate date = m_fromDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_fromDate.setDate(dlg.getDate());
		fromDateSelectButton->setText(m_fromDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CRainSensorDlg::toDateSelect()
{
	QDate date = m_toDate.date();
    CalendarDlg dlg( date, this);

	if(dlg.exec() == QDialog::Accepted)
	{
		m_toDate.setDate(dlg.getDate());
		toDateSelectButton->setText(m_toDate.toString(tr("yyyy년MM월dd일")));
	}
}

void CRainSensorDlg::dump()
{
	QTime fromTime(fromhourCombobox->currentIndex(), 0, 0);
	QTime toTime(tohourCombobox->currentIndex(), 0, 0);

	m_fromDate.setTime(fromTime);
	m_toDate.setTime(toTime);

	QString str;

	if(m_fromDate.toTime_t() >= m_toDate.toTime_t()) 
	{
		QMessageBox::information(this, tr("강우량계"), tr("시작시간이 끝시간 보다 크거나 같습니다.\n시간을 다시 선택하여 주십시요."));
		return;
	}

	if(m_fromDate.toTime_t()+12*60*60 < m_toDate.toTime_t()) 
	{
		QMessageBox::information(this, tr("강우량계"), tr("12시간 이내로 조회바랍니다.\n시간을 다시 선택하여 주십시요."));
		return;
	}

	if(QMessageBox::question(this, tr("강우량계 자료조회"),
					   tr(" 자료 조회를  하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		char buf[30];
		QString str;

		str = m_fromDate.toString(tr("yyMMddhh0000"));
        memcpy(buf, str.toLatin1().data(), 12);
		str = m_toDate.toString(tr("yyMMddhh0000"));
        memcpy(buf+12, str.toLatin1().data(), 12);
		buf[24] = 0;

		view->setRowCount(0);
		char cmd[30];
		strcpy(cmd, "DUMP");
		emit sigDumpCommand(cmd, buf);
		submitButton->setEnabled(false);
		timeButton->setEnabled(false);
	}
}

void CRainSensorDlg::dataPut(int cnt, char*data)
{
//	if(cnt == 0)
//		return;
	int pos = 0;
	int row = 0;
	char	buf[30];

	view->setRowCount(cnt);
	for(int i = 0; i < cnt; i++)
	{
		QString str;

		memcpy(buf, data, 2);
		buf[2] = '/';
		pos += 2;
		memcpy(buf+3, data+pos, 2);
		buf[5] = '/';
		pos += 2;
		memcpy(buf+6, data+pos, 2);
		buf[8] = ' ';
		pos += 2;

		memcpy(buf+9, data+pos, 2);
		buf[11] = ':';
		pos += 2;
		memcpy(buf+12, data+pos, 2);
		buf[14] = ':';
		pos += 2;
		memcpy(buf+15, data+pos, 2);
		buf[17] = 0;
		pos += 2;
		str = buf;

		QTableWidgetItem *item = new QTableWidgetItem(str);
		view->setItem(row, 0, item);

		QTableWidgetItem *itemV[6];
		for(int i = 0; i < 6; i++)
		{
			quint32	value = *( quint32* )&data[pos];
			pos += 4;
			str.setNum(value);
			itemV[i] = new QTableWidgetItem(str);
			view->setItem(row, i+1, itemV[i]);
		}
		row++;
	}
	submitButton->setEnabled(true);
	timeButton->setEnabled(true);
}

void CRainSensorDlg::msgPut(QString& )
{
//	listWidget->addItem(str);
//	qDebug("%s", str.toLatin1().data());
}	

void CRainSensorDlg::onTimeSync()
{
	CTimeSyncDialog dlg(this);

	if(dlg.exec() == QDialog::Accepted)
	{
		emit sigTimeSync(&dlg.m_sDate[2]);
	}
}

void CRainSensorDlg::onRset()
{
	if(QMessageBox::question(this, tr("강우량계"),
					   tr(" 초기화 하면 저장된 모든 데이터가 손실됩니다.\n  강우량계 데이터를 초기화 하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		emit sigRset();
	}
}
