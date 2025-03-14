#include <QtGui>
#include <QtSql>
#include <QMessageBox>

#include "tmsinfo.h"
#include "calculator.h"
#include "wmiview.h"
#include <sys/vfs.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/in.h>



extern uint PopupScreenSaveCount;
extern bool	g_bDebug;
extern	bool g_bReboot;

bool GetItem(char *item, char *value)
{
    char buf[256];
    FILE *pFile;
    pFile = ::fopen("/etc/ether.conf", "r");
	value[0] = 0;
    int len = strlen(item);
	if(pFile)
	{
		while(::fgets(buf, 1024, pFile)){
			//printf("item=%s buf=%s\n", item, buf);
			if(!strncmp(item, buf, len)){
				int	len2 = strlen(buf+strlen(item)+1);
				if(len2 > 0 && len2 < 127)
				{
					memcpy(value, buf+strlen(item)+1, len2-1);
					value[len2-1] = 0;
					qDebug("[%s]", value);
					return true;
				}
			}
		}
		::fclose(pFile);
	}
	return false;
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


float folderSize(const char *folder, int &totalSize, int &freeSize)
{
    struct statfs folderInfo;
    statfs(folder, &folderInfo);
/*    qDebug("bsize=%d",folderInfo.f_bsize);
    qDebug("total blocks=%d",folderInfo.f_blocks);
    qDebug("free=%d",folderInfo.f_bfree);
    qDebug("avail=%d",folderInfo.f_bavail);
 */   // bfree?bavail옜 ?
	totalSize = folderInfo.f_blocks *folderInfo.f_bsize / (1024*1024);
    int usedSize =  (folderInfo.f_blocks - folderInfo.f_bavail)*folderInfo.f_bsize/(1024*1024);
    freeSize =  folderInfo.f_bavail *folderInfo.f_bsize / (1024*1024);
  //  qDebug("total=%d, free=%d",totalSize, freeSize);
    return ((float)usedSize / totalSize) * 100;

}



int memInfo(int &totalSize, int &freeSize)
{

    FILE *fp = NULL;
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        printf("Meminfo open error\n");
        return false;
    }

    int index =0;
//    int totalMem, freeMem;
    char line[128];
    char null[12];
    while(1)
    {
        fgets(line, 128, fp);
 //       printf("line=%s\n", line);
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
//    printf( "meminfo %d %d\n", totalMem, freeMem);
    return true;

}




CTmsInfoDialog::CTmsInfoDialog(QWidget *parent)
    : QDialog(parent)
{
///	QSize size = QSize(16,16);

   QTabWidget *tabWidget = new QTabWidget;
    tabWidget->addTab(new SystemTab(), tr("시스템 정보"));
    tabWidget->addTab(new ProgramTab(), tr("프로그램폴더"));
    tabWidget->addTab(new LogData10sTab(), tr("10초 데이터"));
   tabWidget->addTab(new LogData5mTab(), tr("5분/시간 데이터"));
    tabWidget->addTab(new DecimalDisplayTab(), tr("계측 표시"));

    QPushButton *okButton = new QPushButton(tr("확인"));
                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(tabWidget, SIGNAL(currentChanged(int index)), this, SLOT(currentChanged()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(okButton);
    setLayout(mainLayout);

//	tabWidget->setIconSize(size);
//	setWindowIcon(icon);
    setWindowTitle(tr("수질TMS DataLogger 정보"));
}

void CTmsInfoDialog::currentChanged()
{

}

void CTmsInfoDialog::onTimer()
{
	if(++m_nIdle > PopupScreenSaveCount)
	{
		reject();
	}
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
			qDebug("Password = %s", password.toAscii().data());
	}
	db.close();
	
	dbname =  "/flash/works/wmi.db";
    db.setDatabaseName(dbname);
	db.open();

	QSqlQuery q2("SELECT LocationCode FROM Exts_TmsExt");
	if (q2.next())
	{
		factoryCode = q2.value(0).toString();
		if(g_bDebug)
			qDebug("LocationCode = %s", password.toAscii().data());
	}
	db.close();
	QSqlDatabase::removeDatabase("QSQLITE");



    QLabel *cpuInfoLabel = new QLabel(tr("CPU 정보:"));
    QLabel *cpuInfoValueLabel = new QLabel(tr("CPU : S3C2440  Clock : 400MHz"));
    cpuInfoValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

#if 0
    QLabel *flashLabel = new QLabel(tr("Flash:"));
    QLabel *flashValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(64).arg(10));
    flashValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
#endif

	int total, used;
	folderSize("/sd", total, used);
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

    QLabel *passwordLabel = new QLabel(tr("암호화된 비밀번호:"));

	QLabel *passwordValueLabel = new QLabel(password);
    passwordValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(cpuInfoLabel);
    mainLayout->addWidget(cpuInfoValueLabel);
    //mainLayout->addWidget(flashLabel);
    //mainLayout->addWidget(flashValueLabel);
    mainLayout->addWidget(diskLabel);
    mainLayout->addWidget(diskValueLabel);
    mainLayout->addWidget(memoryLabel);
    mainLayout->addWidget(memoryValueLabel);
    mainLayout->addWidget(codeLabel);
    mainLayout->addWidget(codeValueLabel);
    mainLayout->addWidget(dirLabel);
    mainLayout->addWidget(dirValueLabel);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QStringList findFiles(const QDir &directory, const QStringList &files, bool Desc)
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

void ProgramTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
 //       fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
 //       sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
 //       sizeItem->setFlags(Qt::ItemIsEnabled);

//        int row = secTable->rowCount();
//        secTable->insertRow(row);
        secTable->setItem(i, 0, fileNameItem);
        secTable->setItem(i, 1, sizeItem);
//		QString str = files[i];
//		str += tr("%1 KB").arg(int((size + 1023) / 1024));
//		listWidget->addItem(str);
    
	
	}
}

ProgramTab::ProgramTab(QWidget *parent)
    : QWidget(parent)
{

    QDir directory = QDir(tr("/flash"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files, false);

    secTable = new QTableWidget(files.size(), 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
    secTable->setHorizontalHeaderLabels(labels);
    secTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    secTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    secTable->verticalHeader()->show();

	QVBoxLayout *layout = new QVBoxLayout;
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
 //       fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
 //       sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
 //       sizeItem->setFlags(Qt::ItemIsEnabled);

//        int row = secTable->rowCount();
//        secTable->insertRow(row);
        secTable->setItem(i, 0, fileNameItem);
        secTable->setItem(i, 1, sizeItem);
//		QString str = files[i];
//		str += tr("%1 KB").arg(int((size + 1023) / 1024));
//		listWidget->addItem(str);
    
	
	}
}

LogData10sTab::LogData10sTab(QWidget *parent)
    : QWidget(parent)
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
    secTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    secTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    secTable->verticalHeader()->show();

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(secTable);
	setLayout(layout);
    
	
	showFiles(directory, files);
}

LogData5mTab::LogData5mTab(QWidget *parent)
    : QWidget(parent)
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
    minTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    minTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    minTable->verticalHeader()->show();

	QVBoxLayout *layout = new QVBoxLayout;
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
//        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
//        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
//        sizeItem->setFlags(Qt::ItemIsEnabled);

//        int row = minTable->rowCount();
//        minTable->insertRow(row);
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

    m_bPasswordOK = false;
    changeButton = new QPushButton(tr("변경"));

    connect(changeButton, SIGNAL(clicked()), this, SLOT(change()));

    decimalTable = new QTableWidget(0, 2);
    QStringList labels;
    labels << tr("항목") << tr("소숫점 자리수");
    decimalTable->setHorizontalHeaderLabels(labels);
    decimalTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    decimalTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

	for(int i = 0; i < SensorNumber-1; i++)
	{
        decimalTable->insertRow(i);
        QTableWidgetItem *codeItem = new QTableWidgetItem(SensorCode[i]);
		QString format =  QString::number(SensorFormat[i]);
        QTableWidgetItem *formatItem = new QTableWidgetItem(format);
        decimalTable->setItem(i, 0, codeItem);
        decimalTable->setItem(i, 1, formatItem);
	}

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(decimalTable);
	layout->addWidget(changeButton);
	setLayout(layout);
}

bool CheckPassword(const char *userName, const char *password);

void DecimalDisplayTab::change()
{
	QString sensorName;
	QList<QTableWidgetItem *> list =  decimalTable->selectedItems();
	if(list.size() >= 1)
	{
		if(m_bPasswordOK == false)
		{
			CPasswordDlg dlg(this);
			if(dlg.exec() == QDialog::Accepted)
			{
				QString password = dlg.GetPassword();
				if(CheckPassword("tms", password.toAscii().data()) ==false)
				{
					QMessageBox::information(this, tr("TMS"), tr("비밀번호가 올바르지 않습니다."));
					return;
				}
				m_bPasswordOK = true;
			}
			else
				return;
		}
		QTableWidgetItem* item = list[0];
		int row = item->row();
		sensorName = SensorCode[row];
		CDecimalInput Dlg(sensorName, SensorFormat[row]);

		if(Dlg.exec() == QDialog::Accepted)
		{
			if((int)Dlg.m_nValue > 8 || (int)Dlg.m_nValue < 0)
				QMessageBox::information(this, tr("TMS"), tr("값이 허용된 범위보다 너무 크거나 작습니다."));
			else
			{
				SensorFormat[row] = (int)Dlg.m_nValue;
				QString format =  QString::number(Dlg.m_nValue);
				QTableWidgetItem *formatItem = new QTableWidgetItem(format);
				decimalTable->setItem(row, 1, formatItem);
				QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
				QString filename = "/flash/works/wmi.db";
				db.setDatabaseName(filename);
				if(db.open()) {
					QString Desc, Unit; 
					QString str("UPDATE Analog SET Format = ");
					str += QString::number(SensorFormat[row]);
					str += " WHERE Name = '";
					str += sensorName;
					str += "';";
					QSqlQuery query(str);
					query.next();
					if(sensorName == tr("FLW01"))
					{
						QString str = tr("UPDATE Analog SET Format = ");
						QString name = QString::number(SensorFormat[row]);
						str += name;
						str += " WHERE Name = '";
						str += tr("FLW01_5MIN");
						str += "';";
						QSqlQuery query(str);
						query.next();
						SensorFormat[SensorNumber-1] = SensorFormat[row];
					}
					db.close();
				}
				QSqlDatabase::removeDatabase("QSQLITE");
			}
		}
	}
	else
		QMessageBox::information(this, tr("TMS"), tr("변경할 항목이 선택되지 않았습니다."));
}

CDecimalInput::CDecimalInput(QString& SensorName, int value, QWidget *parent)
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

	ok = new QPushButton();
	ok->setText(tr("확인"));
	connect(ok, SIGNAL(clicked()), this, SLOT(accept()));

	cancel = new QPushButton();
	cancel->setText(tr("취소"));
	connect(cancel, SIGNAL(clicked()), this, SLOT(reject()));

	subLayout = new QGridLayout;
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
	if(++m_nIdle > PopupScreenSaveCount*5)
	{
		reject();
	}
}

CCommMonDialog::CCommMonDialog(const QString& ipAddr, int port, QWidget *parent)
    : QDialog(parent)
{

	m_bFirst = true;

	QLabel *deviceLabel = new QLabel(tr("디바이스:"));
	QComboBox *deviceComboBox = new QComboBox;

	QLabel *topicLabel = new QLabel(tr("토픽:"));
	QComboBox *topicComboBox = new QComboBox;

	listWidget = new QListWidget;
	QFont font = QFont("mine");
	font.setStyleStrategy(QFont::NoAntialias);
	listWidget->setFont(font);

    scrollLockCheckBox = new QCheckBox(tr("일시정지"));
    dispCheckBox = new QCheckBox(tr("HEX 표시"));

	portLabel = new QLabel(tr(""));
	portLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	startStopButton = new QPushButton(tr("분석시작"));
	QPushButton *okButton = new QPushButton(tr("확인"));
	startStopButton->setEnabled(false);

    connect(scrollLockCheckBox, SIGNAL(clicked()), this, SLOT(onScrollLock()));
    connect(dispCheckBox, SIGNAL(clicked()), this, SLOT(onDisp()));
    connect(startStopButton, SIGNAL(clicked()), this, SLOT(onStartStop()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(deviceComboBox, SIGNAL(currentTextChanged(const QString & currentText )), this, SLOT(onCurrentItemChanged(const QString & currentText )));
    connect(deviceComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(changeDevice()));

	QGridLayout *subLayout = new QGridLayout(this);
	subLayout->addWidget(deviceLabel, 0, 0, 1, 1);
	subLayout->addWidget(deviceComboBox, 0, 1, 1, 2);
	subLayout->addWidget(topicLabel, 0, 4, 1, 1);
	subLayout->addWidget(topicComboBox, 0, 5, 1, 2);
	subLayout->addWidget(listWidget, 1, 0, 10, 7);
	subLayout->addWidget(scrollLockCheckBox, 11, 0, 1, 1);
	subLayout->addWidget(dispCheckBox, 11, 1, 1, 1);
	subLayout->addWidget(portLabel, 11, 2, 1, 2);
    subLayout->addWidget(startStopButton, 11, 4, 1, 2);
    subLayout->addWidget(okButton, 11, 6, 1, 1);
    setLayout(subLayout);

    setWindowTitle(tr("통신 분석"));

	m_pWMINet = new CWMINet;
	m_pWMINet->initialComm(this, ipAddr, port);
	m_pWMINet->StartConv();

	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(100);
	m_bStarted = false;
	m_bAutoDisp = false;


}

CCommMonDialog::~CCommMonDialog()
{ 
/*	while(deviceComboBox->count())
		deviceComboBox->removeItem(0);
	while(topicComboBox->count())
		topicComboBox->removeItem(0);
	while(listWidget->count())
		delete listWidget->takeItem(0);
*/	delete m_pWMINet;
	/* 
	delete deviceLabel;
	delete deviceComboBox;
	delete topicLabel;
	delete topicComboBox;

	delete listWidget;
	delete portLabel;
	delete startStopButton;
	delete okButton;
	delete subLayout;
	delete dispCheckBox;
	*/
}

void CCommMonDialog::changeDevice()
{
	int index = deviceComboBox->currentIndex(); 
	if(index < 0)
		return;
	while(topicComboBox->count())
		topicComboBox->removeItem(0);

	CDevice *device = m_deviceList[index];
	for (int j = 0; j < device->m_topicList.size(); ++j)
	{
		QString *str = device->m_topicList[j];
		topicComboBox->addItem(*str);
	}

/*	
	for (int i = 0; i < m_deviceList.size(); ++i)
	{
		CDevice *device = m_deviceList.at(i);
		if(device->m_deviceName == deviceName)
		{
			for (int j = 0; j < device->m_topicList.size(); ++j)
			{
				QString *str = device->m_topicList.at(j);
				topicComboBox->addItem(*str);
			}
		}
	}
*/
}
void CCommMonDialog::onTimer()
{
	if(++m_nIdle > PopupScreenSaveCount*5)
	{
		reject();
	}
	m_pWMINet->onTimer();
}

void CCommMonDialog::onStartStop()
{
	if(m_bStarted)
	{
		m_bStarted = false;
		m_pWMINet->disconnect();
		listWidget->insertItem(0, tr("========Stop========"));
		startStopButton->setEnabled(false);
	}
	else
	{
		while(listWidget->count() > 0)
			delete listWidget->takeItem(0);

		startStopButton->setText(tr("분석정지"));
		m_bStarted = true;
		int index = deviceComboBox->currentIndex(); 
		;
//		QString device = deviceComboBox->currentText();
		//CDevice *device = m_deviceList[index].m_deviceName;
		QString device = m_deviceList[index]->m_deviceName;
		QString topic = topicComboBox->currentText();
		if(g_bDebug)
			qDebug("<Device=%s, Topic=%s>", device.toAscii().data(), topic.toAscii().data());

		m_pWMINet->deviceSelect(device, topic);
		scrollLockCheckBox->setChecked(false);
		m_bScrollLock = false;
	}
}

void CCommMonDialog::onScrollLock()
{
	m_bScrollLock = false;
	if(scrollLockCheckBox->checkState() == Qt::Checked)
	{
		m_bScrollLock = true;
		listWidget->insertItem(0, tr("========Pause========"));
	}
}

void CCommMonDialog::onDisp()
{
	m_bAutoDisp = false;
	if(dispCheckBox->checkState() == Qt::Checked)
		m_bAutoDisp = true;
}

bool CCommMonDialog::ExtractData( char *Buffer)
{
	QString    str;
	char	   *buf;
	quint32	   len;
	quint32	   i;
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
			m_pWMINet->setInitEnd();
			delete [] buf;

			m_pWMINet->commandWrite(pRcvHeader->Seq, WMI_CMD_COMMMON, WMI_SCMD_TOPIC_READ);

			break;
		case WMI_CMD_COMMMON:
			{
				switch(pRcvHeader->SubCmd)
				{
					case WMI_SCMD_TOPIC_READ:
					{
						if(m_bFirst == true)
						{
							int cnt = deviceComboBox->count();
							while(deviceComboBox->count())
								deviceComboBox->removeItem(0);
							cnt = topicComboBox->count();
							while(topicComboBox->count())
								topicComboBox->removeItem(0);

							while (!m_deviceList.isEmpty())
							{
								CDevice *device = m_deviceList.takeFirst();
								delete device;
							}

							quint32 deviceCnt = *( quint32* )&Buffer[ idx ];
							idx += sizeof( quint32 );

							for(i = 0; i < deviceCnt; i++)
							{
								len = *( quint32* )&Buffer[ idx ];
								idx+=sizeof( quint32 );
								buf = new char[ len+1 ];
								memcpy( buf, &Buffer[ idx ], len );
								idx+= ((len + 3) & 0xfffffffc);
								buf[ len ] = 0;
								CDevice *device = new CDevice;
								QString tmp = buf;
								int index = tmp.indexOf(":");
								if(index == -1)
								{
									device->m_deviceName = tmp;
									device->m_deviceDesc = tmp;
								}
								else
								{
									QStringList list = tmp.split(":");
									device->m_deviceName = list[0];
									device->m_deviceDesc = list[1];
								}
								delete [] buf;
								device->m_nIndex = i;
								m_deviceList.append(device);
								deviceComboBox->addItem(device->m_deviceDesc);

								quint32 topicCnt = *( quint32* )&Buffer[ idx ];
								idx += sizeof( quint32 );

								for(quint32 j = 0; j < topicCnt; j++)
								{
									len = *( quint32* )&Buffer[ idx ];
									idx+=sizeof( quint32 );
									buf = new char[ len+1 ];
									memcpy( buf, &Buffer[ idx ], len );
									idx+=((len + 3) & 0xfffffffc);
									buf[ len ] = 0;
									QString *topic = new QString;
									*topic = buf;
									if(g_bDebug)
										qDebug("topic(%d)----%s", j, topic->toAscii().data());
									device->m_topicList.append(topic);
									delete [] buf;
									if(i == 0)
										topicComboBox->addItem(*topic);

								}
							}
							m_bFirst = false;
						}
						startStopButton->setText(tr("분석시작"));
						startStopButton->setEnabled(true);
						m_bStarted = false;
						break;
					}
					case WMI_SCMD_TOPIC_SELECT:
						{
						QString port;
						len = *( quint32* )&Buffer[ idx ];
						idx+=sizeof( quint32 );
						buf = new char[ len+1 ];
						memcpy( buf, &Buffer[ idx ], len );
						idx+= ((len + 3) & 0xfffffffc);
						buf[ len ] = 0;
						port = buf;
						port += "(";

						len = *( quint32* )&Buffer[ idx ];
						idx+=sizeof( quint32 );
						buf = new char[ len+1 ];
						memcpy( buf, &Buffer[ idx ], len );
						idx+= ((len + 3) & 0xfffffffc);
						buf[ len ] = 0;
						port += buf;
						port += ")";
						portLabel->setText(port);
						if(g_bDebug)
							qDebug("port = %s", port.toAscii().data());
						}
						break;

					case WMI_SCMD_FRAME_REQ:
						{
							if(m_bStarted == true && m_bScrollLock != true)
							{
								char buf[10];
								
								quint32 sec = *( quint32* )&Buffer[ idx ];	// sec
								QDateTime time;
								time.setTime_t(sec);
								idx += 4;
								i = *( quint32* )&Buffer[ idx ];	// usec
								idx += 4;

								i = *( quint32* )&Buffer[ idx ];	// writeSuccess
								idx += 4;
								i = *( quint32* )&Buffer[ idx ];	//readSuccess
								idx += 4;
								i = *( quint32* )&Buffer[ idx ];	//writeError
								idx += 4;
								i = *( quint32* )&Buffer[ idx ];	//readError
								idx += 4;
								i = *( quint32* )&Buffer[ idx ];	//currErr
								idx += 4;
								
								quint32 isWrite = *( quint32* )&Buffer[ idx ];
								idx += 4;
								quint32 isTx = *( quint32* )&Buffer[ idx ];
								idx += 4;
								

								quint32 dataLen = *( quint32* )&Buffer[ idx ];
								idx += 4;
								quint32 realDataLen = (dataLen+3) & ~0x03;

								QString str;

								if(isTx == 0)
									str = "Rx(";
								else
								if(isTx == 1)
									str = "Tx(";
								else
								if(isTx == 2)
									str = "Link(";
								else
								if(isTx == 3)
									str = "Unlink(";
								else
								if(isTx == 4)
									str = "Timeout(";
								else
								if(isTx == 5)
									str = "Close(";
								else
								if(isTx == 6)
									str = "Error(";
								else
								if(isTx == 7)
									str = "ETC(";
								else
								{
									str = QString::number(isTx);
									str += "(";
								}

								if(isWrite)
									str += "W)[";
								else
									str += "R)[";

								str += time.toString(tr("mm:ss"));
								sprintf(buf, "][%3d] ", dataLen);
								str += buf;

								for(i = 0; i < dataLen;i++, idx++)
								{

									if(m_bAutoDisp == true || Buffer[idx] <= 0x20 || Buffer[idx] > 0x7a)
									{	

/*										if(Buffer[idx] == 0x02)
											str += "<stx>";
										else
										if(Buffer[idx] == 0x03)
											str += "<etx>";
										else
										if(Buffer[idx] == 0x0d)
											str += "<cr>";
										else
										if(Buffer[idx] == 0x06)
											str += "<ack>";
										else
										if(Buffer[idx] == 0x15)
											str += "<nak>";
										else
										if(Buffer[idx] == 0x05)
											str += "<enq>";
										else
										if(Buffer[idx] == 0x04)
											str += "<eot>";
										else
*/
										{
											if(m_bAutoDisp)
												sprintf(buf, "%02x ", Buffer[idx]);
											else
												sprintf(buf, "<%02x>", Buffer[idx]);
											str += buf;
										}
									}
									else
										str += Buffer[idx];
								}
								idx += realDataLen;
								listWidget->insertItem(0, str);
								while(listWidget->count() > 50)
									delete listWidget->takeItem(50);
							}
							if(g_bDebug)
								qDebug("Send(%d)...", pRcvHeader->Seq);
							m_pWMINet->commandWrite(pRcvHeader->Seq, WMI_CMD_COMMMON, WMI_SCMD_FRAME_REQ);
						}
						break;
					case WMI_SCMD_IDLE:
						break;
				}
			}
	}
	
	return true;
}

CInfoDialog::CInfoDialog(QWidget *parent)
    : QDialog(parent)
{
///	QSize size = QSize(16,16);

   tabWidget = new QTabWidget;
    tabWidget->addTab(new SystemInfoTab(), tr("시스템 정보"));
    tabWidget->addTab(new SystemMangerTab(), tr("시스템 관리"));

    okButton = new QPushButton(tr("확인"));
                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(tabWidget, SIGNAL(currentChanged(int index)), this, SLOT(currentChanged()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(okButton);
    setLayout(mainLayout);

//	tabWidget->setIconSize(size);
//	setWindowIcon(icon);
    setWindowTitle(tr("시스템 정보"));
}

void CInfoDialog::onTimer()
{
	if(++m_nIdle > PopupScreenSaveCount)
		reject();
}

SystemInfoTab::SystemInfoTab(QWidget *parent)
    : QWidget(parent)
{
	int total, used;
	folderSize("/sd", total, used);
    QLabel *diskLabel = new QLabel(tr("디스크:"));
    QLabel *diskValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(total).arg(used));
    diskValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	memInfo(total, used);
    QLabel *memoryLabel = new QLabel(tr("메모리:"));
    QLabel *memoryValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(total+4).arg(used));
    memoryValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	char buf[256], bootproto[128], idaddr[128], netmask[128];

	//QString lan1=tr("static 192.168.123.60(255.255.255.0)"), lan2=tr("static 192.168.123.61(255.255.255.0)"), gateway=tr("192.168.123.254"), primaryDNS=tr("164.124.101.2"), secondaryDNS;

	GetItem("ETH0_BOOTPROTO", bootproto);
	GetItem("ETH0_IPADDR", idaddr);
	GetItem("ETH0_NETMASK", netmask);
	snprintf(buf, 256, "%s %s(%s)",bootproto, idaddr, netmask);
	qDebug("<%s>", buf);
	QString lan1 = buf;

	GetItem("ETH1_BOOTPROTO", bootproto);
	GetItem("ETH1_IPADDR", idaddr);
	GetItem("ETH1_NETMASK", netmask);
	snprintf(buf, 256, "%s %s(%s)",bootproto, idaddr, netmask);
	QString lan2 = buf;
	
	GetItem("ETH0_GATEWAY", bootproto);
	QString gateway = bootproto;
	GetItem("DNS", bootproto);
	QString primaryDNS = bootproto;
	GetItem("DNS2", bootproto);
	QString secondaryDNS = bootproto;


    QLabel *lan1Label = new QLabel(tr("LAN1:"));
    QLabel *lan1ValueLabel = new QLabel(lan1);
    lan1ValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *lan2Label = new QLabel(tr("LAN2:"));
    QLabel *lan2ValueLabel = new QLabel(lan2);
    lan2ValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *gatewayLabel = new QLabel(tr("게이트웨이:"));
    QLabel *gatewayValueLabel = new QLabel(gateway);
    gatewayValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *primaryDNSLabel = new QLabel(tr("기본 DNS서버:"));
    QLabel *primaryDNSPValueLabel = new QLabel(primaryDNS);
    primaryDNSPValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *secondaryDNSLabel = new QLabel(tr("보조 DNS서버:"));
    QLabel *secondaryDNSPValueLabel = new QLabel(secondaryDNS);
    secondaryDNSPValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(diskLabel);
    mainLayout->addWidget(diskValueLabel);
    mainLayout->addWidget(memoryLabel);
    mainLayout->addWidget(memoryValueLabel);
    mainLayout->addWidget(lan1Label);
    mainLayout->addWidget(lan1ValueLabel);

    mainLayout->addWidget(lan2Label);
    mainLayout->addWidget(lan2ValueLabel);

    mainLayout->addWidget(gatewayLabel);
    mainLayout->addWidget(gatewayValueLabel);
    mainLayout->addWidget(primaryDNSLabel);
    mainLayout->addWidget(primaryDNSPValueLabel);
    mainLayout->addWidget(secondaryDNSLabel);
    mainLayout->addWidget(secondaryDNSPValueLabel);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

SystemMangerTab::SystemMangerTab(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *viewResetButton = new QPushButton(tr("뷰 재기동"));
    connect(viewResetButton, SIGNAL(clicked()), this, SLOT(slotViewReset()));

    QPushButton *powerOffButton = new QPushButton(tr("시스템 정지"));
    connect(powerOffButton, SIGNAL(clicked()), this, SLOT(slotPowerOff()));

    QPushButton *rebootButton = new QPushButton(tr("시스템 재기동"));
    connect(rebootButton, SIGNAL(clicked()), this, SLOT(slotRebooting()));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(viewResetButton);
    mainLayout->addWidget(powerOffButton);
    mainLayout->addWidget(rebootButton);
    setLayout(mainLayout);

}

void SystemMangerTab::slotViewReset()
{
	if(QMessageBox::question(this, tr("뷰 재기동"),
					   tr(" 뷰 프로그램을 재기동하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{
		g_bReboot = true;
	}
}

void SystemMangerTab::slotPowerOff()
{
	if(QMessageBox::question(this, tr("시스템 정지"),
					   tr(" 시스템을 정지하면 더 이상 감시와 데이터 저장을 할 수 없습니다.\n 시스템을 정지하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{

		QMessageBox::information(this, tr("시스템 정지"), tr("전원을 끄셔도 안전합니다."));
	}
}

void SystemMangerTab::slotRebooting()
{
	if(QMessageBox::question(this, tr("시스템 재기동"),
					   tr(" 시스템을 재기동 하면 그동안 감시와 데이터 저장을 할 수 없습니다.\n 시스템을 재기동하시겠습니까?"),
					   QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
	{

	}
}
