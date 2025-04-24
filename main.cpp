//#include "mainwindow.h"
#include <QApplication>
#include <QtGlobal>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QSplashScreen>
#include <QTimer>
#include "srview.h"
#include "myapplication.h"
#include "srsocket.h"
#include "infomationdialog.h"
#include "testdialog.h"
#include "epsene.h"
#include "tmsdialog.h"

#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <sstream>

#ifdef __linux__
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/reboot.h>
#endif
CSrSocket *g_pSrSocket;
CMyApplication *gApp;
QSqlDatabase gDb;
QSplashScreen *splash;

double AdcCalibraion[6][2];
QString gSoftwareModel;
QString gHardwareModel;
QString gHardwareRevision;
int XMaxRes = 1920; //1024;
int YMaxRes = 1200; //600;


#define VERSION "1.0"
#define SVN_REVISION "1000"
bool g_DebugDisplay=false;
bool g_WindowsRun=false;
QString g_Ip = "127.0.0.1";
QString g_Password;
int g_Port=35000;
int g_bTrend = false;

QObject *g_View;

bool	g_bSamp=true;
int		g_HasuPort;

void printHelpMessage()
{
    qDebug()<<"srview - sunrise Web Based HMI Server\n"
//              "Usage: srview [options] [filename]\n\n"
              "Usage: srview [options]\n\n"
              "Options:\n"
              "\t-h, --help\t\tshow this help message and exit\n"
              "\t-v, --version\t\tshow program's version number and exit\n"
              "\t-d, --debug\t\tshow debug message\n";
}

void writeVersion()
{
    QFile file("/var/run/version/view");
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        QDate date = QDate::currentDate();
        QTime time = QTime::currentTime();
        const char * macro_date = __DATE__;
        const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

        char s_month[5] = {0,};
        int iyear = 0, iday = 0;

        sscanf(macro_date, "%s %d %d", s_month, &iday, &iyear);
        int imonth = (strstr(month_names, s_month) - month_names) / 3 + 1;
        QString sDate = QString("%1-%2-%3 ").arg(iyear, 4, 10, QChar('0'))
                .arg(imonth, 2, 10, QChar('0'))
                .arg(iday, 2, 10, QChar('0'));

        out << "View\n";                        // Kind
        out << "tmsview\n";                      // 이름
        out << VERSION << "," << SVN_REVISION "\n";                 // Version
//        out << "*" << __DATE__ << " " <<  __TIME__ << "\n";   // 빌드시간
        out << sDate <<  __TIME__ << "\n";   // 빌드시간
        out << QString("본체 UI 프로그램\n");                    // 설명
        out << date.toString("yyyy-MM-dd") << " " << time.toString("hh:mm:ss") << "\n";
        file.close();
    }
}

void startPageNameLoad(QString& name)
{

    gDb = QSqlDatabase::addDatabase("QSQLITE");
    QString dbname =  "/data/project2.db";
    gDb.setDatabaseName(dbname);
    if(gDb.open() != true)
    {
        QSqlError err = gDb.lastError();
        qDebug() << "QSqlError : " << err.databaseText();
    }

    QSqlQuery query("SELECT Kind, Item, Value FROM Project");
    while(query.next())
    {
        QString kind = query.value(0).toString();
        QString pname = query.value(1).toString();
        QString value = query.value(2).toString();
        if(kind == "StartPage" && pname == "TouchGraphic") {
            name = value;
            if(g_DebugDisplay)
                qDebug() << "StartPage:" << name;
            break;
        }
    }
    gDb.close();
}

#define TEST_DATA_OFFSET 112
#define I2C_ADDR 0x57;

void systeminfoLoad()
{
    QFile file("/var/run/systeminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString msg("SystemInfo를 열 수 없습니다.");
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
        QString msg = "SystemInfo Load JSon Error :(";
        msg += QString::number(err.offset);
        msg +=")";
        msg +=errstr;
        file.close();
        CInfomationDialog dlg(msg);
        dlg.exec();
        return;
    }
    QJsonObject jObj  = jDoc.object();
    gSoftwareModel    = jObj["SoftwareModel"].toString();
    gHardwareModel    = jObj["HardwareModel"].toString();
    gHardwareRevision = jObj["HardwareRevision"].toString();
    if(jObj["AdcCalibration"].isNull() != true)
    {
        QJsonArray array    = jObj["AdcCalibration"].toArray();
        for(int i = 0; i < 6; i++)
        {
            AdcCalibraion[i][0] = array[i*2].toDouble();    // Min
            AdcCalibraion[i][1] = array[i*2+1].toDouble();  // Max
        }
    }
    file.close();
}

int main(int argc, char *argv[])
{
//    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

#ifdef __linux__
    qputenv("QT_SCALE_FACTOR", "0.5");
    qputenv("QT_SCREEN_SCALE_FACTORS", "2");
#endif
      CMyApplication app(argc, argv);
#ifdef __linux__
    // get this process pid
    pid_t pid = getpid();

    // compose a bash command that:
    //    check if another process with the same name as yours
    //    but with different pid is running
    std::stringstream command;
    command << "ps -eo pid,comm | grep tmsview | grep -v " << pid;
    int isRuning = system(command.str().c_str());
    if (isRuning == 0) {
        qDebug() << "Another process already running. exiting.";
        return 1;
    }
    char buf[20];
    int fd = ::open("/var/run/view.pid",  O_WRONLY);
    if (fd >= 0) {
        sprintf(buf, "%d", pid);
        ::write(fd, buf, strlen(buf));
        ::close(fd);
    }
#endif

    // Load an application style
//    QFile styleFile( ":/qss/Diplaytap.qss" );
//    styleFile.open( QFile::ReadOnly );

    // Apply the loaded stylesheet
//    QString style( styleFile.readAll() );
//    app.setStyleSheet( style );

    qDebug() << "Version : " << VERSION << "," << SVN_REVISION << "(" << __DATE__ << " " << __TIME__ << ")";
//    systeminfoLoad();

//    QPixmap pixmap(":/images/splash.png");
//    splash = new QSplashScreen;
//    splash->setPixmap(pixmap);
//    splash->show();
#if 1
    gApp = &app;
    QRegExp rxArgHelp("--help");
    QRegExp rxArgH("-h");
    QRegExp rxArgVersion("--version");
    QRegExp rxArgV("-v");
    QRegExp rxArgDebug("--debug");
    QRegExp rxArgD("-d");
    QRegExp rxArgW("-w");

    QFile file("/data/arg.ini");
    if(file.exists()) {
        if(file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
//                qDebug() << "arg:" << line;
                QString line = in.readLine();
//                qDebug() << "arg" << line;
                if (rxArgDebug.indexIn(line) != -1  ||
                        rxArgD.indexIn(line) != -1)
                {
                   g_DebugDisplay = true;
                }
                else
                if (rxArgW.indexIn(line) != -1)
                {
                   g_WindowsRun = true;
                }
                else
                {
                    int pos = line.indexOf(QChar(' '));
                    QString cmd = line.left(pos);
                    QString val = line.mid(pos+1);
                    if(cmd == "IP")
                    {
                        g_Ip = val;
                        qDebug() << "IP:" << g_Ip;
                    }
                    if(cmd == "Trend")
                    {
                        g_bTrend = true;
                        //qDebug() << "Trend:" << g_bTrend;
                     }
                }
            }
        }
    }
#endif
//    QThread::msleep(300);
    // QT_SCALE_FACTOR 환경 변수 설정


    g_pSrSocket = new CSrSocket();
    writeVersion();
/*
    QTranslator appTranslator;
    QString translationsPath("/usr/share/easypaint/translations/");
    QString appLanguage = DataSingleton::Instance()->getAppLanguage();
    if(appLanguage == "system")
    {
        appTranslator.load(translationsPath + "easypaint_" + QLocale::system().name());
    }
    else
    {
        appTranslator.load(translationsPath + appLanguage);
    }
    a.installTranslator(&appTranslator);

 */
    QString folder="/data", filename="start", ipAddr="localhost", port="30000";

    if(gSoftwareModel == "BESTEC_HDMI" || gSoftwareModel == "BESTEC_SS-100")    // HDMI 지원
    {
        XMaxRes = 1280;
        YMaxRes = 720;
    }
    startPageNameLoad(filename);
    qDebug() << "<<<<<<<<<<<<<<X-Max=" << XMaxRes << "Y-Max=" << YMaxRes << ">>>>>>>>>>>>>>>>>>>>>>>";
    qDebug() << "===============================================================================";
    qDebug() << "folder=" << folder << "IpAddr=" << g_Ip << "port=" << g_Port << "filename=" << filename << "Debug = " << g_DebugDisplay;
    qDebug() << "===============================================================================";

    QFont font;

    font.setPointSize(12);
#ifdef WIN32
    font.setFamily(QString("나눔고딕_코딩"));
#endif
    app.setFont(font);
//    app.setOverrideCursor(Qt::BlankCursor);
//    app.setOverrideCursor(Qt::PointingHandCursor);


    int ret;

/*    if(gSoftwareModel == "EPS_HANRIVER")    // 한강수계 표준RTU
    {
        EpsEnE *View = new EpsEnE;
        View->show();
        g_View = (QObject*)View;
    }
    else
    if(gSoftwareModel == "ORION_TMS")    // 수질 TMS
*/    {
        TmsDialog *View = new TmsDialog;
        View->show();
        g_View = (QObject*)View;
    }
 /*   else
    {
        CSrView *View = new CSrView(folder, filename, ipAddr, port.toInt());
        View->show();
        g_View = (QObject*)View;
//        splash->finish(&View);
    }
//    delete splash;
*/
    gSoftwareModel = "ORION_TMS";
    ret = app.exec();
    return ret;
}
