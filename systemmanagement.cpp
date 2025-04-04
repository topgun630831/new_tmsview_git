#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include <QFile>
#include <QScrollBar>
#include <QDateTime>
#include <QSqlQuery>
#include "systemmanagement.h"
#include "ui_systemmanagement.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "monitorudp.h"
#include "fileselect.h"
#include "epsene.h"
#include "srview.h"
#include "srsocket.h"

extern EpsEnE *g_pEps;
extern CSrView	*g_pMainView;
extern QString gSoftwareModel;
extern QString gHardwareModel;
extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern void fileCopy(const char* src, const char* dest, const char* filename, bool bMsg);
extern QString gSoftwareModel;
extern QSqlDatabase gDb;

extern bool    m_bScreenSave;

SystemManagement::SystemManagement(bool bAll, QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::SystemManagement)
{
    ui->setupUi(this);

    if(bAll == true)
    {
        ui->groupBox_5->setVisible(false);
        ui->groupBox_4->setVisible(false);
        ui->groupBox_7->setVisible(false);
        ui->groupBox_reset->setVisible(false);
        ui->DeleteInterrupt->setVisible(false);
    }
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    ui->btnSystemRun->setEnabled(false);

    m_watcher.addPath("/app/wwwroot/tmp/");
    connect(&m_watcher, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged(const QString&)));
    buttonEnable();
    QScrollBar *scroll  = ui->listWidget->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 20px; height: 30px}");
/*
    if(gSoftwareModel != "ORION_TMS")    // 수질 TMS
    {
        ui->btnInitTmsData->setVisible(false);
        ui->DeleteInterrupt->setVisible(false);
    }
*/
/*    if(gSoftwareModel != "ORION_TMS")    // 수질 TMS
    {
        ui->btnInitTmsData->setText("프로젝트초기화");
        ui->DeleteInterrupt->setVisible(false);
    }
*/    loadReverseProxyConfig();
    ui->groupBox_reset->setVisible(false);
    connect(&m_TimerStatus, SIGNAL(timeout()), this, SLOT(onTimer()));
    m_TimerStatus.start(1000);
}

SystemManagement::~SystemManagement()
{
    delete ui;
}

void SystemManagement::paintEvent(QPaintEvent *)
{
    QPainter	p(this);
    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::white);

    QSize size = frameSize();
    p.setPen(pen);
    p.drawLine(0, 0, size.rwidth(), 0);
    p.drawLine(0, 0, 0, size.rheight());
    pen.setColor(Qt::darkGray);
    p.setPen(pen);
    p.drawLine(size.rwidth(), 0, size.rwidth(), size.rheight());
    p.drawLine(0, size.rheight(), size.rwidth(), size.rheight());
}

void SystemManagement::RunStart(bool start)
{
    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);

    QString proc = "app";
    bool SaveStatus=false;
    argObject.insert(QString("Proc"),QJsonValue(proc));
    argObject.insert(QString("Enabled"),QJsonValue(start));
    argObject.insert(QString("SaveStatus"),QJsonValue(SaveStatus));

    CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Set"), argObject, vObject);
}

void SystemManagement::on_btnSystemStop_clicked()
{
    CQuestionDialog dlg(tr(" 시스템을 정지하면 더 이상 감시와 데이터 저장을 할 수 없습니다\n 시스템을 정지하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
    {
        RunStart(false);
        ui->btnSystemRun->setEnabled(true);
        ui->btnSystemStop->setEnabled(false);
    }
}

void SystemManagement::on_btnSystemRun_clicked()
{
    RunStart(true);
    ui->btnSystemRun->setEnabled(false);
    ui->btnSystemRun->setEnabled(false);
    ui->btnSystemStop->setEnabled(true);
}

void SystemManagement::on_btnReboot_clicked()
{
    CQuestionDialog dlg(tr("시스템을 다시 시작 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
        sendCOmmand(QString("Recovery_Reset"));
}

void SystemManagement::on_btnUpgrade_clicked()
{
    QString dir = "/app/wwwroot/tmp/usbdisk/upgrade";
    if(QDir(dir).exists() == false)
    {
        CInfomationDialog dlg("USB 메모리에 업그레이드 파일이 없습니다");
        dlg.exec();
        return;
    }
    FileSelect dlg(dir);
    if(dlg.exec() == QDialog::Accepted)
    {
        CQuestionDialog qDlg(tr("업그레이드는 중요한 작업으로 완료시 까지 전원 공급상태를  유지해야 합니다.\n %1 (으)로 업그레이드 하시겠습니까?").arg(dlg.mSelectedFile), true);
        if(qDlg.exec() == QDialog::Accepted)
        {
            mSelectedFile = dlg.mSelectedFile;
            m_bCheck = qDlg.m_bCheck;
            m_Timer = startTimer(1);
            QString msg = "업그레이드 중 입니다.";
            ui->listWidget->addItem(msg);
            msg = "1분정도 소요되고 업그레이드 후에는 리부팅됩니다.";
            ui->listWidget->addItem(msg);
            ui->listWidget->scrollToBottom();
            QCoreApplication::processEvents();
        }
    }
}

void SystemManagement::on_btnSetupSave_clicked()
{
    ui->btnSetupSave->setEnabled(false);
    dbSync();
    ui->listWidget->clear();
    ui->listWidget->addItem(QString(tr("설정데이터 Backup중입니다.........")));
    QDateTime date = QDateTime::currentDateTime();
    QString src,dest,file;
    dest = "/app/wwwroot/tmp/usbdisk/setupbackup_";
    dest += date.toString("yyyyMMddhhmmss");
    src = "/data";
    //dest = "/app/wwwroot/tmp/usbdisk";

    QDir destDir(dest);
    if(destDir.exists() == false)
    {
        ui->listWidget->addItem(QString("MkDir(%1)").arg(dest));
        destDir.mkpath(dest);
    }

    file = "project2.db";
    fileCopyS(src, dest, file);
    file = "system.db";
    fileCopyS(src, dest, file);

    mDest = dest + "/userweb";
    mSrc = "/data/userweb";
    QDir dir(mSrc);
    scanDir(dir);

    mDest = dest + "/windows";
    mSrc = "/data/windows";
    QDir dir2(mSrc);
    scanDir(dir2);

    system("sync");
    ui->listWidget->addItem(QString(tr("설정데이터 Backup을 완료했습니다.")));
    ui->listWidget->scrollToBottom();
    ui->btnSetupSave->setEnabled(true);
}

void SystemManagement::on_btnSetupRestore_clicked()
{
    ui->btnSetupRestore->setEnabled(false);
    ui->listWidget->clear();
    CQuestionDialog dlg(tr("설정데이터를 복원하려면 시스템을 잠시 정지하여야 합니다\n 복원 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
    {
        QString dir = "/app/wwwroot/tmp/usbdisk";
        FileSelect fileSelect(dir, false, false);
        if(fileSelect.exec() == QDialog::Accepted)
        {
            RunStart(false);
            ui->listWidget->addItem(QString(tr("설정데이터 Restore중입니다.........")));
            QCoreApplication::processEvents();
            mDest = "/data";
            mSrc = dir + "/" + fileSelect.mSelectedFile;
            RunStart(false);
            scanDir(mSrc);
            RunStart(true);
            ui->listWidget->addItem(QString(tr("설정데이터 Restore를 완료했습니다.")));
            ui->listWidget->scrollToBottom();
            system("sync");
            /*
            QString src,dest,file;
            dest = "/data";
            src = "/app/wwwroot/tmp/usbdisk";
            file = "project2.db";
            fileCopyS(src, dest, file);
            file = "system.db";
            fileCopyS(src, dest, file);
            */
            RunStart(true);
            ui->listWidget->addItem(QString(tr("설정데이터 Restore를 완료했습니다.")));
        }
    }
    ui->btnSetupRestore->setEnabled(true);
}
/*
void SystemManagement::fileCopy(const char* src, const char* dest, const char* filename, bool bMsg)
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists() == false)
    {
        CInfomationDialog dlg("USB메모리를 확인하여 주시기 바랍니다");
        dlg.exec();
        return;
    }
    QString msg;
    QString srcF = QString("%1/%2").arg(src).arg(filename);
    QString destF = QString("%1/%2").arg(dest).arg(filename);
    QFile srcFile(srcF);
    if(srcFile.exists() == false)
    {
        CInfomationDialog dlg(tr("파일이 존재하지 않습니다(%1)").arg(src));
        dlg.exec();
        return;
    }
    QFile::remove(destF);
    system("sync");
    bool disp = true;
    if(srcFile.copy(destF) == false)
        msg = tr("파일저장에 실패했습니다(%1)").arg(dest);
    else
    {
        if(bMsg == false)
            disp = false;
        msg = tr("파일저장에 성공했습니다");
    }
    if(disp)
    {
        CInfomationDialog dlg(msg);
        dlg.exec();
    }
}
*/
void SystemManagement::handleFileChanged(const QString& )
{
    buttonEnable();
}

void SystemManagement::buttonEnable()
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists() == false)
    {
        ui->btnDataSave->setEnabled(false);
        ui->btnDataRestore->setEnabled(false);
        ui->btnSetupSave->setEnabled(false);
        ui->btnSetupRestore->setEnabled(false);
        ui->btnUpgrade->setEnabled(false);
    }
    else
    {
        ui->btnDataSave->setEnabled(true);
        ui->btnDataRestore->setEnabled(true);
        ui->btnSetupSave->setEnabled(true);
        ui->btnSetupRestore->setEnabled(true);
        ui->btnUpgrade->setEnabled(true);
    }
}

void SystemManagement::dbSync()
{
    QByteArray ar;
    if(gSoftwareModel == "EPS_HANRIVER")    // 한강수계 표준RTU
        ar = g_pEps->makeCommand(tr("Data_DbSync"), QJsonValue (QJsonValue::Null));
    else
        ar = g_pMainView->makeCommand(tr("Data_DbSync"), QJsonValue (QJsonValue::Null));
    g_pSrSocket->sendCommand((QWidget*)g_pEps, tr("Data_DbSync"), QString(""), ar);
}

void SystemManagement::onRead(QString& cmd, QJsonObject& jobject)
{
    if(g_DebugDisplay)
        qDebug() << "SystemManagement::onRead : " << cmd;
    if(cmd == "Recovery_Upgrade")
    {
        QString result;
        QString error = jobject["Error"].toString();
        if(error == "Error")
        {
            result = jobject["Result"].toString();
            ui->listWidget->addItem(result);
            ui->listWidget->scrollToBottom();
        }
    }
}


void SystemManagement::on_btnFactoryReset_clicked()
{
    CQuestionDialog dlg(tr("이 명령은 모든 프로그램 및 데이터를 초기화합니다.\n공장 초기화 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
        sendCOmmand(QString("Recovery_FactoryReset"));
}

void SystemManagement::on_btnInitRootfs_clicked()   // 기능삭
{
    CQuestionDialog dlg(tr("이 명령은 모든 프로그램을 초기화합니다.\n프로그램 초기화 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
        sendCOmmand(QString("Recovery_InitRootfs"));
}

void SystemManagement::on_btnInitData_clicked()
{
    CQuestionDialog dlg(tr("이 명령은 모든 데이터를 초기화합니다.\n데이터 초기화 하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
        sendCOmmand(QString("Recovery_InitRuntimeData"));
//        sendCOmmand(QString("Recovery_InitData"));
}

void SystemManagement::sendCOmmand(QString cmd)
{
    QJsonObject argObject;
    QJsonValue vValue(QJsonValue(0));
    CMonitorUdp::Instance()->sendCommand(this, cmd, argObject, vValue);
}

void SystemManagement::on_btnDataSave_clicked()
{
    ui->btnDataSave->setEnabled(false);
    dbSync();
    ui->listWidget->clear();
    ui->listWidget->addItem(QString(tr("자료 Backup중입니다.........")));
    QCoreApplication::processEvents();
    QDateTime date = QDateTime::currentDateTime();
    QString name = "/app/wwwroot/tmp/usbdisk/backup_";
    name += date.toString("yyyyMMddhhmmss");
    mDest = name;
    mSrc = "/data";
    QDir dir(mSrc);
    scanDir(dir);
    ui->listWidget->addItem(QString(tr("자료 Backup을 완료했습니다.")));
    ui->listWidget->scrollToBottom();
    system("sync");
    ui->btnDataSave->setEnabled(true);
}

void SystemManagement::on_btnDataRestore_clicked()
{
    ui->btnDataRestore->setEnabled(false);
    ui->listWidget->clear();
    CQuestionDialog dlg(tr("자료를 복원하려면 시스템을 잠시 정지하여야 합니다\n 복원 하시겠습니까?"));
    {
        if(dlg.exec() == QDialog::Accepted)
        {
            QString dir = "/app/wwwroot/tmp/usbdisk";
            FileSelect fileSelect(dir, true, false);
            if(fileSelect.exec() == QDialog::Accepted)
            {
                ui->listWidget->addItem(QString(tr("자료 Restore중입니다.........")));
                QCoreApplication::processEvents();
                mDest = "/data";
                mSrc = dir + "/" + fileSelect.mSelectedFile;
                RunStart(false);
                scanDir(mSrc);
                RunStart(true);
                ui->listWidget->addItem(QString(tr("자료 Restore를 완료했습니다.")));
                ui->listWidget->scrollToBottom();
                system("sync");
            }
        }
    }
    ui->btnDataRestore->setEnabled(true);
}

void SystemManagement::scanDir(QDir dir)
{
    dir.setNameFilters(QStringList("*"));
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QString src = dir.path();
    QString dest = mDest + src.mid(mSrc.length());
    QDir destDir(dest);
    if(destDir.exists() == false)
    {
        ui->listWidget->addItem(QString("Make Directory %1").arg( src));
        destDir.mkpath(dest);
    }
    QStringList fileList = dir.entryList();
    for (int i=0; i<fileList.count(); i++)
    {
        if(fileList[i].contains(".db-shm") == false && fileList[i].contains(".db-wal") == false)
            fileCopyS(src, dest, fileList[i]);
        ui->listWidget->scrollToBottom();
        QCoreApplication::processEvents();
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirList = dir.entryList();
    for (int i=0; i<dirList.size(); ++i)
    {
        if(dirList.at(i) != "lost+found")
        {
            QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
            scanDir(QDir(newPath));
        }
    }
}

void SystemManagement::fileCopyS(QString &src, QString &dest, QString &filename)
{
    ui->listWidget->addItem(filename);
    QString srcF = QString("%1/%2").arg(src).arg(filename);
    QString destF = QString("%1/%2").arg(dest).arg(filename);
    QFile srcFile(srcF);
    if(srcFile.exists() == false)
    {
        ui->listWidget->addItem(QString(tr("파일이 존재하지 않습니다(%1)")).arg(srcF));
        return;
    }
    QFile::remove(destF);
    if(srcFile.copy(destF) == false)
        ui->listWidget->addItem(QString(tr("파일 저장에 실패했습니다(Src:%1 Dest:%2)")).arg(srcF).arg(destF));
}

void SystemManagement::timerEvent(QTimerEvent *)
{
    killTimer(m_Timer);

    QString dir = "/app/wwwroot/tmp/usbdisk/upgrade";
    //fileCopy("/app/wwwroot/tmp/usbdisk/upgrade", "/app/wwwroot/tmp", mSelectedFile.toLocal8Bit().data(), false);
    //system("sync");
    QJsonObject argObject;
    QString name2 = dir + "/" + mSelectedFile;
    QString name =mSelectedFile;
    QFile sel(name2);
    argObject.insert(QString("Filename"),QJsonValue(name2));
    argObject.insert(QString("Filesize"),QJsonValue((int)sel.size()));
    argObject.insert(QString("Peroid"),QJsonValue(0));
    argObject.insert(QString("Offset"),QJsonValue(0));

    if(m_bCheck)
    {
        QFile file("/data/__FORMAT_DATAFS__");
        file.open(QFile::WriteOnly);
        file.write(" ");
        file.close();
    }
    CMonitorUdp::Instance()->sendCommand(this,  QString("Recovery_Upgrade"), argObject, QJsonValue(QJsonValue::Null));
}

void SystemManagement::on_btnInitTmsData_clicked()
{
//    if(gSoftwareModel == "ORION_TMS")    // 수질 TMS
    {
        CQuestionDialog dlg("모든 데이터를 삭제하시겠습니까?");
        if(dlg.exec() == QDialog::Accepted)
        {
            RunStart(false);
            system("rm /data/data.*");
            system("rm /data/tms.*");
            system("rm /data/tmssec.*");
            system("rm /data/share.*");
            system("rm /tmp/share.*");
            system("rm /csv/*");
            RunStart(true);
        }
    }
#if 0
    else
    {
        CQuestionDialog dlg(tr("이 명령은 프로젝트 데이터를 초기화합니다.\n프로젝트 데이터를 초기화 하시겠습니까?"));
        if(dlg.exec() == QDialog::Accepted)
            sendCOmmand(QString("Recovery_InitRuntimeData"));
    }
#endif
}

void SystemManagement::on_DeleteInterrupt_clicked()
{
    CQuestionDialog dlg("인터럽트 데이터를 삭제하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        RunStart(false);
        gDb.setDatabaseName(tr("/data/tms.db"));
        if(gDb.open() == true)
        {
            QString sql = QString("delete from 'TmsInt'");
            if(g_DebugDisplay)
                qDebug() << "SQL:" << sql;
            QSqlQuery query(sql);
            query.exec();
            gDb.close();
        }
        RunStart(true);
    }
}

void SystemManagement::loadReverseProxyConfig()
{
    m_bLoadOkProxyIP = false;
    m_sReverseProxyIP = "211.238.253.248";
    m_sReverseProxyMode = "Client";
    m_nReverseProxyPort = 30001;
    QFile file("/app/ReverseProxy.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg("ReverseProxy.conf Open(Read)에 실패했습니다.");
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    QJsonParseError err;
    QString errstr;
    QJsonDocument jDoc = QJsonDocument().fromJson(file.readAll(),&err);
    if(err.error != QJsonParseError::NoError)
    {
        errstr = err.errorString();
        qDebug() << "\n\n\nError : (" << err.offset << ") " << errstr;
        QString msg = "ReverseProxy.conf Load JSon Error :(";
        msg += QString::number(err.offset);
        msg +=")";
        msg +=errstr;
        file.close();
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    QJsonObject jObj  = jDoc.object();
    m_sReverseProxyMode = jObj["Mode"].toString();
    m_sReverseProxyIP = jObj["ServerIp"].toString();
    m_nReverseProxyPort = jObj["ServerPort"].toInt();
    ui->ReverseProxyIP->setText(m_sReverseProxyIP);
    ui->ReverseProxyPort->setText(QString("%1").arg(m_nReverseProxyPort));
    m_bLoadOkProxyIP  = true;
    file.close();
}

void SystemManagement::saveReverseProxyConfig()
{
    QFile file("/app/ReverseProxy.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString msg("ReverseProxy.conf Open(Write)에 실패했습니다.");
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    QJsonObject jObj;
    jObj["Mode"] = m_sReverseProxyMode;
    jObj["ServerIp"] = m_sReverseProxyIP;
    jObj["ServerPort"] = m_nReverseProxyPort;
    QJsonDocument jDoc(jObj);
    file.write(jDoc.toJson());
    file.close();
}

void SystemManagement::on_btnRemoteRun_clicked()
{
    CQuestionDialog dlg(tr("원격지원을 실행하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
    {
        m_sReverseProxyIP = ui->ReverseProxyIP->text();
        m_nReverseProxyPort = ui->ReverseProxyPort->text().toInt();
        saveReverseProxyConfig();
        system("/dot/dotnet  /app/ReverseProxy.dll &");
    }
}


void SystemManagement::on_btnRemoteStop_clicked()
{
    CQuestionDialog dlg(tr("원격지원을 정지하시겠습니까?"));
    if(dlg.exec() == QDialog::Accepted)
    {
        QFile file("/var/run/ReverseProxy.pid");
        QString command;
        QString pid;
        if(file.exists() == true)
        {
            file.open(QIODevice::ReadOnly | QIODevice::Text);
            QByteArray ar;
            ar = file.readAll();
            pid = QString(ar.data());
            file.close();
            command = "kill " + pid;
            system(command.toLocal8Bit().constData());
        }
    }
}
void SystemManagement::onTimer()
{
    QString str;
    str = "Status : ";
    QFile file("/tmp/__ReverseProxy_status__");
    if(file.exists() == true)
    {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QByteArray ar;
        ar = file.readAll();
        str += QString(ar.data());
        file.close();
    }
    ui->remoteStatus->setText(str);

    if (m_bScreenSave == true)
        QDialog::reject();
}
