#include <QtGui>
#include <QtSql>
#include <QMessageBox>

#include "tmsinfo.h"
#include "calculator.h"
#include "wmiview.h"
#include <sys/vfs.h>
extern uint PopupScreenSaveCount;
extern bool	g_bDebug;


float folderSize(const char *folder)
{
    struct statfs folderInfo;
    statfs(folder, &folderInfo);
    //printf("bsize=%d\n",folderInfo.f_bsize);
    //printf("total blocks=%d\n",folderInfo.f_blocks*4);
    //printf("free=%d\n",folderInfo.f_bfree*4);
    //printf("avail=%d\n",folderInfo.f_bavail*4);
    // bfree?bavail옜 ?
    int used =  folderInfo.f_blocks - folderInfo.f_bavail;
    return ((float)used / folderInfo.f_blocks) * 100;

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
    int totalMem, freeMem;
    char line[128];
    char null[12];
    while(1)
    {
        fgets(line, 128, fp);
        printf("line=%s\n", line);
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
    printf( "meminfo %d %d\n", totalMem, freeMem);
    return true;

}




CTmsInfoDialog::CTmsInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    tabWidget = new QTabWidget;
    tabWidget->addTab(new SystemTab(), tr("시스템 정보"));
    tabWidget->addTab(new LogData10sTab(), tr("10초 데이터"));
    tabWidget->addTab(new LogData5mTab(), tr("5분 데이터"));
    tabWidget->addTab(new DecimalDisplayTab(), tr("계측 표시"));

    okButton = new QPushButton(tr("확인"));
                                     // | QDialogButtonBox::Cancel);
	m_nIdle = 0;
    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	m_Timer.start(500);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(okButton);
    setLayout(mainLayout);

    setWindowTitle(tr("수질TMS DataLogger 정보"));
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
    QLabel *cpuInfoLabel = new QLabel(tr("CPU 정보:"));
    QLabel *cpuInfoValueLabel = new QLabel(tr("CPU : S3C2440  Clock : 400MHz"));
    cpuInfoValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *flashLabel = new QLabel(tr("Flash:"));
    QLabel *flashValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(64).arg(10));
    flashValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *diskLabel = new QLabel(tr("디스크:"));
    QLabel *diskValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(1024).arg(900));
    diskValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *memoryLabel = new QLabel(tr("메모리:"));
    QLabel *memoryValueLabel = new QLabel(tr("Total %1 MB, Free %2 MB").arg(64).arg(10));
    memoryValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *codeLabel = new QLabel(tr("사업장 코드:"));
    QLabel *codeValueLabel = new QLabel(tr("110200"));
    codeValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *dirLabel = new QLabel(tr("프로그램 위치:"));
    QLabel *dirValueLabel = new QLabel(tr("/flash/wmi"));
    dirValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

    QLabel *passwordLabel = new QLabel(tr("비밀번호:"));

	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE");
	QString dbname =  "/sd/database/tms.db";
    db.setDatabaseName(dbname);
	db.open();

	QSqlQuery q("SELECT Password FROM Password");
	QString password;
	if (q.next())
	{
		password = q.value(0).toString();
		qDebug("Password = %s", password.toAscii().data());
	}
	db.close();
	QSqlDatabase::removeDatabase("QSQLITE");
   
	QLabel *passwordValueLabel = new QLabel(password);
    passwordValueLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);


    QVBoxLayout *mainLayout = new QVBoxLayout;
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
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordValueLabel);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

QStringList findFiles(const QDir &directory, const QStringList &files)
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
    return foundFiles;
}

void LogData10sTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(Qt::ItemIsEnabled);

        int row = secTable->rowCount();
        secTable->insertRow(row);
        secTable->setItem(row, 0, fileNameItem);
        secTable->setItem(row, 1, sizeItem);
    }
}


LogData10sTab::LogData10sTab(QWidget *parent)
    : QWidget(parent)
{
    secTable = new QTableWidget(0, 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
    secTable->setHorizontalHeaderLabels(labels);
    secTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    secTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    secTable->verticalHeader()->show();
    //secTable->setShowGrid(false);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(secTable);
	setLayout(layout);

    QDir directory = QDir(tr("/sd/tensec"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName), QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files);
    showFiles(directory, files);
}

LogData5mTab::LogData5mTab(QWidget *parent)
    : QWidget(parent)
{
    
    minTable = new QTableWidget(0, 2);
    QStringList labels;
    labels << tr("파일") << tr("크기");
    minTable->setHorizontalHeaderLabels(labels);
    minTable->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    minTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(minTable);
	setLayout(layout);

    QDir directory = QDir(tr("/sd/fivemin"));
    QStringList files;
	QString fileName = "*";
    files = directory.entryList(QStringList(fileName),
                                QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files);
    showFiles(directory, files);

}

void LogData5mTab::showFiles(const QDir &directory, const QStringList &files)
{
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(Qt::ItemIsEnabled);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB").arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(Qt::ItemIsEnabled);

        int row = minTable->rowCount();
        minTable->insertRow(row);
        minTable->setItem(row, 0, fileNameItem);
        minTable->setItem(row, 1, sizeItem);
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

	for(int i = 0; i < SensorNumber; i++)
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
		/*		if(CheckPassword("tms", password.toAscii().data()) ==false)
				{
					QMessageBox::information(this, tr("TMS"), tr("비밀번호가 올바르지 않습니다."));
					return;
				}
		*/		m_bPasswordOK = true;
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
				QMessageBox::information(this, tr("TMS"), tr("값의 범위가 너무 크거나 작습니다."));
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
				//	if(g_bDebug)
						qDebug("%s", str.toAscii().data());
					QSqlQuery query(str);
					query.next();
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

	deviceLabel = new QLabel(tr("디바이스:"));
	deviceComboBox = new QComboBox;

	topicLabel = new QLabel(tr("토픽:"));
	topicComboBox = new QComboBox;

	listWidget = new QListWidget;
	QString name = tr("ProggyClenTT");
	QFont font(name);
	listWidget->setFont(font);
	//listWidget->addItem(tr("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

    scrollLockCheckBox = new QCheckBox(tr("일시정지"));

	portLabel = new QLabel(tr(""));
	portLabel->setFont(font);
	portLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

	startStopButton = new QPushButton(tr("분석시작"));
	okButton = new QPushButton(tr("확인"));
	startStopButton->setEnabled(false);

    connect(scrollLockCheckBox, SIGNAL(clicked()), this, SLOT(onScrollLock()));
    connect(startStopButton, SIGNAL(clicked()), this, SLOT(onStartStop()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
//    connect(deviceComboBox, SIGNAL(currentTextChanged(const QString & currentText )), this, SLOT(onCurrentItemChanged(const QString & currentText )));
    connect(deviceComboBox,
            SIGNAL(currentIndexChanged(const QString&)),
            this, SLOT(changeDevice()));

	subLayout = new QGridLayout;
	subLayout->addWidget(deviceLabel, 0, 0, 1, 1);
	subLayout->addWidget(deviceComboBox, 0, 1, 1, 2);
	subLayout->addWidget(topicLabel, 0, 4, 1, 1);
	subLayout->addWidget(topicComboBox, 0, 5, 1, 2);
	subLayout->addWidget(listWidget, 1, 0, 10, 7);
	subLayout->addWidget(scrollLockCheckBox, 11, 0, 1, 1);
	subLayout->addWidget(portLabel, 11, 1, 1, 3);
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


}

CCommMonDialog::~CCommMonDialog()
{ 
	while(deviceComboBox->count())
		deviceComboBox->removeItem(0);
	while(topicComboBox->count())
		topicComboBox->removeItem(0);
	while(listWidget->count())
		delete listWidget->takeItem(0);
	delete m_pWMINet;
	delete deviceLabel;
	delete deviceComboBox;
	delete topicLabel;
	delete topicComboBox;

	delete listWidget;
	delete portLabel;
	delete startStopButton;
	delete okButton;
	delete subLayout;
}

void CCommMonDialog::changeDevice()
{
	QString deviceName = deviceComboBox->currentText(); 
	while(topicComboBox->count())
		topicComboBox->removeItem(0);
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
		listWidget->insertItem(0, tr("========분석정지========"));
		startStopButton->setEnabled(false);
	}
	else
	{
		while(listWidget->count() > 0)
			delete listWidget->takeItem(0);

		startStopButton->setText(tr("분석정지"));
		m_bStarted = true;
		QString device = deviceComboBox->currentText();
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
								device->m_deviceName = buf;
								delete [] buf;
								deviceComboBox->addItem(device->m_deviceName);
								m_deviceList.append(device);

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

									if(Buffer[idx] < 0x20 || Buffer[idx] > 0x7a)
									{	
										if(Buffer[idx] == 0x02)
											str += "<stx>";
										else
										if(Buffer[idx] == 0x03)
											str += "<etx>";
										else
										if(Buffer[idx] == 0x0d)
											str += "<cr>";
										else
										{
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
