#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QJsonArray>
#include <QProgressBar>
#include <QScrollBar>
#include <QDateTime>
#include "networksetup.h"
#include "tmssetup.h"
#include "ui_tmssetup.h"
#include "sensorselect.h"
#include "monitorudp.h"
#include "diselect.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "srsocket.h"

#ifdef _WIN32
    #include <windows.h>
#else // linux stuff
    #include <sys/vfs.h>
    #include <sys/stat.h>
#endif // _WIN32

extern QSqlDatabase gDb;
extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern QString GraphicId;
extern QString TmsCheckTag[8];
extern QString TmsTagNameList[TAG_MAX];
extern QString gTmsVersion;
extern QString gHardwareRevision;
extern QString gPassword;
extern bool    m_bScreenSave;

#define SENSOR_ROW_HIGHT   80

struct TMS_TAG_TAB TagTab[38] = {
/* 0 */{"",          "Analog", "측정값",     "Tms",   "Value",      "R", "R32",      "",     "",        "㎎/L",0, "0"},
/* 1 */{"_COMM",     "Digital", "통신상태",   "Tms",   "Comm_Status", "R", "DIgital", "정상",  "통신이상", "",   0, "0"},
/* 2 */{"_CHECK",    "Digital", "점검중",    "System", "Check",       "RW", "DIgital", "점검중", "정상",    "",  1, "0"},
/* 3 */{"_EQUIP_STS", "Analog", "계측기 상태", "Tms",   "Status", "R", "S32",     "",      "",       "",  0,"0"},
/* 4 */{"_STS",       "Analog", "최종 상태",   "System","Status",      "R", "S32", "", "",    "",  0, "5"},
/* 5 */{"_CMD",       "String", "명령어",     "Tms",    "Cmd",         "RW","Text",  "", "",    "",  0, ""},
/* 6 */{"_INFO",      "String", "부가정보",    "Tms",    "Info",        "R","Text",  "", "",    "",  0, ""},
/* 7 */{"_TIME",      "String", "시간",       "Tms",    "Time",        "R","Text",  "", "",    "",  0, ""},
/* 8 */{"_PASS",      "String", "계측기암호(PASS)",  "Tms",    "PASS",        "R","Text",  "", "",    "",  0, ""},
/* 9 */{"_MSIG",      "Analog", "검출신호(MSIG)",    "Tms",   "MSIG",         "R", "R32",      "",     "",        "㎎/L",0, "0"},
/* 10*/{"_MTM1",      "Analog", "수온(MTM1)",       "Tms",   "MTM1",         "R", "R32",      "",     "",        "℃",0, "0"},// SS만 옵션
/* 11*/{"_MTM2",      "Analog", "측정조온도(MTM2)",  "Tms",   "MTM2",         "R", "R32",      "",     "",        "℃",0, "0"},
/* 12*/{"_MSAM",      "Analog", "희석비율(MSAM)",    "Tms",   "MSAM",         "R", "R32",      "",     "",        "",0, "0"},
/* 13*/{"_ZERO",      "Analog", "영점 교정액농도(ZERO)","Tms",   "ZERO",         "R", "R32",      "",     "",        "㎎/L",0, "0"},
/* 14*/{"_SPAN",      "Analog", "스팬 교정액농도(SPAN)","Tms",   "SPAN",         "R", "R32",      "",     "",        "㎎/L",0, "0"},
/* 15*/{"_SLOP",      "Analog", "교정 기울기(SLOP)",   "Tms",   "SLOP",         "R", "R32",      "",     "",        "",0, "0"},
/* 16*/{"_ICPT",      "Analog", "교정 절편(ICPT)",    "Tms",   "ICPT",         "R", "R32",      "",     "",        "",0, "0"}, // PH,SS
/* 17*/{"_FACT",      "Analog", "보정한 기울기(FACT)", "Tms",   "FACT",         "R", "R32",      "",     "",        "",0, "0"},
/* 18*/{"_OFST",      "Analog", "보정한 절편(OFST)",   "Tms",   "OFST",         "R", "R32",      "",     "",        "",0, "0"},
/* 19*/{"_MAXR",      "Analog", "측정범위(MAXR)",      "Tms",   "MAXR",         "R", "R32",      "",     "",        "㎎/L",0, "0"},
/* 20*/{"_AUXI",      "String", "외부 입출력(AUXI)",   "Tms",   "AUXI",         "R", "Text",      "",     "",        "",0, "0"},
/* 21*/{"",           "Analog", "온도",         "Tms",   "SampTemp",     "R", "R32",      "",     "",      "℃",0, "0"},
/* 22*/{"_RUN",       "Analog", "동작상태",     "Tms",   "SampRun",      "R", "S32",      "", "",      "",0, "0"},
/* 23*/{"_POS",       "Analog", "채수병 위치",    "Tms",   "SampPos",     "R", "R32",      "",     "",      "",0, "0"},
/* 24*/{"_DOOR",      "Digital","채수기 도어",    "Tms",   "SampDoor",     "R", "Digital", "열림",     "닫힘",      "",0, "0"},
/* 25*/{"DORON",      "Digital","출입문 개폐",    "System",   "di1",      "R", "Digital", "열림",     "닫힘",      "",0, "0"},
/* 26*/{"FMLON",      "Digital","채수펌프 (A)",   "System",   "di2",      "R", "Digital", "작동",     "정지",      "",0, "0"},
/* 27*/{"FMRON",      "Digital","채수펌프 (B)",   "System",   "di4",      "R", "Digital", "작동",     "정지",      "",0, "0"},
/* 28*/{"PWRON",      "Digital","입력전원 단절",    "System",   "di3",      "R", "Digital", "단절",     "정상",      "",0, "0"},
/* 29*/{"FLOW_DAYTOTAL","Analog","일 적산량",        "System",   "",      "R", "R32",    "",       "",      "",0, "0"},
/* 30*/{"TS_CONNECTED", "Digital","서버접속 상태",   "System",   "",      "R", "Digital", "연결",     "종료",      "",0, "0"},
/* 31*/{"INT_CONNECTED","Digital","인터럽트접속 상태","System",   "",      "R", "Digital", "연결",     "종료",      "",0, "0"},
/* 32*/{"TMS_MSG",      "String","메시지",           "System",   "",      "R", "Text",   "",      "",      "",0, ""},
/* 33*/{"DATETIME",     "String","시스템 날짜시간",   "System",   "DateTime yyyy-MM-dd HH:mm:ss",      "R", "Text",   "",      "",      "",0, ""},
/* 34 */{"_TIME",       "String", "최종채수시각",     "Tms",    "Time",         "R","Text",  "", "",    "",  0, ""},
/* 35 {"_DAYTOTAL",       "Analog", "일적산",  "System",    "",         "R","R32",  "", "",    "TON",  1, "0"},*/
/* 35 */{"_RATE",       "Analog", "순시값",     "System",    "",         "R","R32",  "", "",    "M/S",  0, "0"},
/* 36*/{"TMS_FATAL",      "String","오류 메시지",           "System",   "",      "R", "Text",   "",      "",      "",0, ""},
/* 37*/{"IO_POWER",      "Digital","IO전원 상태",    "System",   "di7",      "R", "Digital", "정상",     "비정상",      "",0, "0"},
       };
struct TMS_TAG_TAB FlowTagTab[3] = {
/* 0 */{"",          "Analog", "유량적산",     "Tms",   "Value",      "R", "R32",      "",     "",        "TON", 0, "0"},
/* 0 */{"_RATE",     "Analog", "순시유량",     "Tms",   "Flw00",      "R", "R32",      "",     "",        "M/S", 0, "0"},
/* 1 */{"_COMM",     "Digital", "통신상태",   "Tms",   "Comm_Status", "R", "DIgital", "정상",  "통신이상", "",   0, "0"},
};

int ITEM_SELECT[12][25] = {
/* TN      */    {0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20},
/* TP      */    {0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20},
/* TOC     */    {0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20}, // COD와 동일,
/* PH      */    {0,1,2,3,4,5,6,7,8,9,10,13,14,15,16,17,18,19,20},
/* SS      */    {0,1,2,3,4,5,6,7,8,9,13,14,15,16,17,18,19,20},
/* COD     */    {0,1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20},
/* TOX     */    {0,1,2,3,4,5,6,7,8,9,11,13,14,17,18,19,20},
/* FLOW    */    {0,1,2,3,4,6,7,29,35},
/* SAMPLER */    {21,22,1,2,4,5,23,24,6,34},
/* 구형     */    {0,1,2,3,4,5,6,7,13,14,15,16,17,18,19},
/* SS 수온포함 */  {0,1,2,3,4,5,6,7,8,9,10,13,15,14,16,17,18,19,20},
/* TOC     */    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20}, // MTM1을 MSAM 2번째 값으로 사용
};

const char *DESC_TAB[9] = {
    "TN",
    "TP",
    "TOC",
    "PH",
    "SS",
    "COD",
    "TOX",
    "FLOW",
    "SAMPLER",
};

const char *UNIT_TAB[9] = {
    "㎎/L",
    "㎎/L",
    "㎎/L",
    "pH",
    "㎎/L",
    "㎎/L",
    "㎎/L",
    "TON",
    "℃",
};

const char *CONST_TAB[12] = {
    "PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",      // TN
    "PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",      // TP
    "PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",      // TOC
    "PASS,MSIG,MTM1,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",           // PH
    "PASS,MSIG,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",                // SS
    "PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",      // COD
    "PASS,MSIG,MTM2,ZERO,SPAN,FACT,OFST,MAXR,AUXI",                     // TOX
    "",                                                                 // FLOW
    "",                                                                 // SAMPLER
    "ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR",                               // OLD
    "PASS,MSIG,MTM1,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",           // SS with 수온
    "PASS,MSIG,MTM1,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI",      // Gas
};

int ITEM_MAX[12] = {
    /* TN      */    20,
    /* TP      */    20,
    /* TOC     */    20,
    /* PH      */    19,
    /* SS      */    18,
    /* COD     */    20,
    /* TOX     */    20,
    /* FLOW    */    9,
    /* SAMPLER */    10,
    /* 구형     */    15,
    /* SS 수온포함  */ 19,
    /* TOC     */    21,
};
#define TN   0
#define TP   1
#define TOC  2
#define PH   3
#define SS   4
#define COD  5
#define BOD  6
#define FLOW 7
#define SAMP 8
#define OLD  9
#define SS_TEMP  10
#define TOC_GAS  11
#define DORON  25
#define FMLON  26
#define FMRON  27
#define PWRON  28

void displayData(QString dir, const char * filter, QTableWidget *table);
QStringList findFiles(const QDir &directory, const QStringList &files, bool Desc);
void fileCopy(const char* src, const char* dest, const char* filename, bool bMsg);

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

int diskInfo(int &totalSize, int &freeSize)
{
#ifdef __linux__
    system("df -m /data> /tmp/disk.txt");
#endif
    FILE *fp = NULL;
    fp = fopen("/tmp/disk.txt", "r");
    if (fp == NULL)
    {
        printf("/tmp/disk.txt open error\n");
        return false;
    }

    int index =0;
    char line[128];
    char null[80];
    while(1)
    {
        fgets(line, 128, fp);
//        if(index == 4) {
          if(index == 2) {
            sscanf(line, "%s %d %s %d %s %s", null, &totalSize, null, &freeSize, null, null);
            fclose(fp);
            break;
        }
        index++;
    }
    //totalSize /=1024;
    //freeSize /=1024;
    return true;

}
#define fMB (1024*1024)
bool getFreeTotalSpace(const QString& sDirPath,double& fTotal, double& fFree)
{
#ifdef _WIN32

    QString sCurDir = QDir::current().absolutePath();
    QDir::setCurrent( sDirPath );

    ULARGE_INTEGER free,total;
    bool bRes = ::GetDiskFreeSpaceExA( 0 , &free , &total , NULL );
    if ( !bRes ) return false;

    QDir::setCurrent( sCurDir );

    fFree = static_cast<double>( static_cast<__int64>(free.QuadPart) ) / fMB;
    fTotal = static_cast<double>( static_cast<__int64>(total.QuadPart) ) / fMB;

#else //linux

    struct stat stst;
    struct statfs stfs;

    if ( ::stat(sDirPath.toLocal8Bit(),&stst) == -1 ) return false;
    if ( ::statfs(sDirPath.toLocal8Bit(),&stfs) == -1 ) return false;

    fFree = stfs.f_bavail * ( stst.st_blksize / fMB );
    fTotal = stfs.f_blocks * ( stst.st_blksize / fMB );

#endif // _WIN32

    return true;
}
TmsSetup::TmsSetup(bool flag[9], bool checkFlag[9], QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TmsSetup)
{
    mPassword = gPassword;
    m_bIniting = true;
    m_timerId = startTimer(1000*60*5);
    for(int i = 0; i < 9; i++)
        mbFlag[i] = flag[i];

    ui->setupUi(this);

//    if(gHardwareRevision == "1")
        m_nDiCount = 6;
//    else
//    {   ui->groupBoxSampler->setVisible(false);
//        m_nDiCount = 4;
//    }
    QTableWidget *table = ui->tableSensor;
    table->horizontalHeader()->resizeSection(0, 120);    // 측정기기
    table->horizontalHeader()->resizeSection(1, 120);   // ID
    table->horizontalHeader()->resizeSection(2, 120);   // 항목코드
    table->horizontalHeader()->resizeSection(3, 200);   // 프로토콜
    table->horizontalHeader()->resizeSection(4, 100);   // 소수점
    table->horizontalHeader()->resizeSection(5, 120);   // 통신포트
    table->horizontalHeader()->resizeSection(6, 120);   // 통신주기
    table->horizontalHeader()->resizeSection(7, 120);   // 타임아웃
    table->horizontalHeader()->resizeSection(8, 120);   // 업로드
    table->horizontalHeader()->resizeSection(9, 120);   // STX/ETX
    table->horizontalHeader()->resizeSection(10, 120);   // CCHK 사용
    table->horizontalHeader()->resizeSection(11, 120);   // Analog출력
 //   table->setMinimumHeight(100);

    m_bUseToc = false;
    gDb.setDatabaseName(tr("/data/project2.db"));
    if(gDb.open() == true)
    {
        int row = 0;
        QSqlQuery query("SELECT Id, Code, Desc, Protocol, RoundUp, Port, ScanTime, Timeout, ConstList, Upload,UseCchk,Analog from Ext_Tms_Item");
        while (query.next())
        {
            TMS_ITEM_TAB *tmsitem = new TMS_ITEM_TAB;
            tmsitem->Id =  query.value(0).toString();
            tmsitem->Code =  query.value(1).toString();
            tmsitem->Desc =  query.value(2).toString();
            tmsitem->Protocol =  query.value(3).toString();
            tmsitem->RoundUp =  query.value(4).toInt();
            tmsitem->Port =  query.value(5).toString();
            tmsitem->ScanTime =  query.value(6).toInt();
            tmsitem->Timeout =  query.value(7).toInt();
            tmsitem->ConstList =  query.value(8).toString();
            tmsitem->Upload =  query.value(9).toInt();
            tmsitem->UseCchk =  query.value(10).toInt();
            tmsitem->Analog =  query.value(11).toInt();
            tmsitem->UseStxChecksum = 0;        // 채수기와 유량계 제외하고는 NewAnalyzer이고 STX Checksum은 0 UseStxSet에서 Set
            if(tmsitem->Code == "FLW01")
            {
                // ui->f_comPort1->setCurrentText(tmsitem->Port);
                FlowRow = row;
                tmsitem->Port = "";
            }
            if((tmsitem->Code == "TOC00" || tmsitem->Code == "TOC10") && tmsitem->Upload == 1)
                m_bUseToc = true;
            if(tmsitem->Protocol != "io")
            {
                TmsItemList.append(tmsitem);
                SetItem(row, tmsitem);
                ui->tableSensor->setRowHeight(row, SENSOR_ROW_HIGHT);
                row++;
            }
            else
                delete tmsitem;
        }

        if(m_bUseToc)
        {
            mbFlag[2] = 1;
            ui->groupBoxCOD->setTitle("TOC");
        }

        QSqlQuery query2("SELECT Name, UseStxInChecksum from Driver_Tms");
        while (query2.next())
            UseStxSet(query2.value(0).toString(), query2.value(1).toInt());
        QSqlQuery query3("SELECT Ip, Port from Port Where Name='Server'");
        query3.next();
        mServerIp = query3.value(0).toString();
        mServerPort = query3.value(1).toString();
    }
    m_nDORON = SetDi("DORON", ui->door, ui->doorReverse);
    m_nFMLON = SetDi("FMLON", ui->sampL, ui->sampLReverse);
    m_nFMRON = SetDi("FMRON", ui->sampR, ui->sampRReverse);
    m_nPWRON = SetDi("PWRON", ui->power, ui->powerReverse);
    m_nSampDoor = SetDi2("SAM00_DOOR");

    DispGeneral();
    SelectedRow = -1;
    ui->comPort->setEnabled(false);
    ui->round->setEnabled(false);
    ui->scanTime->setEnabled(false);
    ui->timeout->setEnabled(false);
    ui->protocol->setEnabled(false);
    ui->upload->setEnabled(false);
    ui->usecchk->setEnabled(false);
    ui->UseStxChecksum->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(2);
    ui->stackedWidget_2->setCurrentIndex(0);

//    QWidget *widget = ui->stackedWidget->widget(2);
//    ui->stackedWidget->removeWidget(widget);

    int total, free;
    //double fTotal, fFree;
    //QString sDirPath = "/data";
    diskInfo(total, free);
    //if(getFreeTotalSpace(sDirPath,fTotal, fFree))
    ui->disk->setText(tr("Total %1 MB, Free %2 MB").arg(total).arg(free));
    ui->label_message->setText(tr("%1일 저장 가능").arg(free/6));
    memInfo(total, free);
    ui->memory->setText(tr("Total %1 MB, Free %2 MB").arg(total).arg(free));

    displayData(tr("/app"), "*", ui->tableProgram);

    QScrollBar *scroll  = ui->tableProgram->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 20px;}");

    mCheckBox[0] = ui->chkTn;
    mCheckBox[1] = ui->chkTp;
    mCheckBox[2] = ui->chkToc;
    mCheckBox[3] = ui->chkPh;
    mCheckBox[4] = ui->chkSs;
    mCheckBox[5] = ui->chkCod;
    mCheckBox[6] = ui->chkTox;
    mCheckBox[7] = ui->chkFlow;
    mCheckBox[8] = ui->chkSampler;

    mCheckBox2[0] = ui->chkTn_2;
    mCheckBox2[1] = ui->chkTp_2;
    mCheckBox2[2] = ui->chkToc_2;
    mCheckBox2[3] = ui->chkPh_2;
    mCheckBox2[4] = ui->chkSs_2;
    mCheckBox2[5] = ui->chkCod_2;
    mCheckBox2[6] = ui->chkTox_2;
    mCheckBox2[7] = ui->chkFlow_2;
    mCheckBox2[8] = ui->chkSampler_2;

    mLineEdit[0] = ui->editTn;
    mLineEdit[1] = ui->editTp;
    mLineEdit[2] = ui->editToc;
    mLineEdit[3] = ui->editPh;
    mLineEdit[4] = ui->editSs;
    mLineEdit[5] = ui->editCod;
    mLineEdit[6] = ui->editTox;
    mLineEdit[7] = ui->editFlow;
    mLineEdit[8] = ui->editSampler;

    for(int i = 0; i < 9; i++)
    {
        if(flag[i] == true)
        {
            int pos = DigitGet(TmsTagNameList[i]);
            mLineEdit[i]->setText(QString("%1").arg(pos));
        }
    }
    ui->Version->setText(gTmsVersion);
    ui->programLabel->setVisible(false);
    ui->tableProgram->setVisible(false);
    mbProtocolEdit = false;

    checkLabel[0] = ui->label_tn;
    checkLabel[1] = ui->label_tp;
    checkLabel[2] = ui->label_toc;
    checkLabel[3] = ui->label_ph;
    checkLabel[4] = ui->label_ss;
    checkLabel[5] = ui->label_cod;
    checkLabel[6] = ui->label_tox;
    checkLabel[7] = ui->label_flow;
    checkLabel[8] = ui->label_sampler;
    for(int i = 0; i < 9; i++)
    {
        if(flag[i] == true)
            mbCheckFlag[i] = checkFlag[i];
        else
            mbCheckFlag[i] = false;
    }
    CheckDisp();
    for(int i = 0; i < 9; i++)
    {
        // if((mbFlag[2] == true && i == 5) || mbFlag[i] == false) yskim
        if(mbFlag[i] == false)
        {
            mCheckBox[i]->setVisible(false);
            mCheckBox2[i]->setVisible(false);
            mLineEdit[i]->setVisible(false);
            checkLabel[i]->setVisible(false);
        }
    }
//    if(gHardwareRevision == "1")
        DispSamplerDoor();


    CheckFlow();

    QSqlQuery query("select SamplerModel From Driver_Tms Where Name='Sampler'");
    if(query.next())
        m_sSamplerModel = query.value(0).toString();

    gDb.close();
    /*
    QFile file("/data/door");
    if (file.open(QIODevice::ReadOnly))
    {
        ui->chkExternalIO->setChecked(true);
        ui->SamplerAddress->setText("30011/5");
        ui->SamplerPort->setCurrentIndex(9);
        file.close();
    }
    ui->groupBoxSampler->setVisible(false);
    */

    for(int i = 0; i < ui->protocol->count(); i++)
        ui->protocol->model()->setData(ui->protocol->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);
    for(int i = 0; i < ui->comPort->count(); i++)
        ui->comPort->model()->setData(ui->comPort->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);
    for(int i = 0; i < ui->MonitorPort->count(); i++)
        ui->MonitorPort->model()->setData(ui->MonitorPort->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);
    for(int i = 0; i < ui->MonitorPortBaudrate->count(); i++)
        ui->MonitorPortBaudrate->model()->setData(ui->MonitorPortBaudrate->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);

    m_GroupBox[0] = ui->groupBoxTN;
    m_GroupBox[1] = ui->groupBoxTP;
    m_GroupBox[2] = ui->groupBoxCOD;
    m_GroupBox[3] = ui->groupBoxPH;
    m_GroupBox[4] = ui->groupBoxSS;
/*
    m_ConstEdit[0][0] = ui->tnZERO;
    m_ConstEdit[0][1] = ui->tnSPAN;
    m_ConstEdit[0][2] = ui->tnSLOP;
    m_ConstEdit[0][3] = ui->tnICPT;
    m_ConstEdit[0][4] = ui->tnFACT;
    m_ConstEdit[0][5] = ui->tnOFST;
    m_ConstEdit[0][6] = ui->tnMAXR;
    m_ConstDate[0]    = ui->tnDate;

    m_ConstEdit[1][0] = ui->tpZERO;
    m_ConstEdit[1][1] = ui->tpSPAN;
    m_ConstEdit[1][2] = ui->tpSLOP;
    m_ConstEdit[1][3] = ui->tpICPT;
    m_ConstEdit[1][4] = ui->tpFACT;
    m_ConstEdit[1][5] = ui->tpOFST;
    m_ConstEdit[1][6] = ui->tpMAXR;
    m_ConstDate[1]    = ui->tpDate;

    m_ConstEdit[2][0] = ui->codZERO;
    m_ConstEdit[2][1] = ui->codSPAN;
    m_ConstEdit[2][2] = ui->codSLOP;
    m_ConstEdit[2][3] = ui->codICPT;
    m_ConstEdit[2][4] = ui->codFACT;
    m_ConstEdit[2][5] = ui->codOFST;
    m_ConstEdit[2][6] = ui->codMAXR;
    m_ConstDate[2]    = ui->codDate;

    m_ConstEdit[3][0] = ui->phZERO;
    m_ConstEdit[3][1] = ui->phSPAN;
    m_ConstEdit[3][2] = ui->phSLOP;
    m_ConstEdit[3][3] = ui->phICPT;
    m_ConstEdit[3][4] = ui->phFACT;
    m_ConstEdit[3][5] = ui->phOFST;
    m_ConstEdit[3][6] = ui->phMAXR;
    m_ConstDate[3]    = ui->phDate;

    m_ConstEdit[4][0] = ui->ssZERO;
    m_ConstEdit[4][1] = ui->ssSPAN;
    m_ConstEdit[4][2] = ui->ssSLOP;
    m_ConstEdit[4][3] = ui->ssICPT;
    m_ConstEdit[4][4] = ui->ssFACT;
    m_ConstEdit[4][5] = ui->ssOFST;
    m_ConstEdit[4][6] = ui->ssMAXR;
    m_ConstDate[4]    = ui->ssDate;
*/
    for(int i = 0; i < 5; i++)
        m_GroupBox[i]->setEnabled(mbFlag[i]);
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        int index = CodePos(tmsitem->Code);
        if(tmsitem->Code == "COD00" && m_bUseToc == true)
            continue;
        if(index < 0 || index >= 5)
            continue;
//        if(tmsitem->UseCchk || tmsitem->Protocol == "NewAnalyzer")
        if(tmsitem->Protocol == "NewAnalyzer")
            m_GroupBox[index]->setEnabled(false);
    }

//    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(onTimer()));
//    m_Timer.start(1000);

    QJsonValue argValue;
//    g_pSrSocket->sendCommand(this, "Ext_Tms_GetConsts", GraphicId, argValue);
    g_pSrSocket->sendCommand(this, "Ext_Tms_GetTimeSyncConfig", GraphicId, argValue);
    m_bIniting = false;
    ui->DataDelete->setVisible(false);

    ui->check->setChecked(true);
    ui->sensor2->setChecked(true);
    ui->tableSensor->verticalHeader()->setDefaultSectionSize( SENSOR_ROW_HIGHT );
}

TmsSetup::~TmsSetup()
{
    delete ui;
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
        delete tmsitem;
    if(gDb.isOpen())
        gDb.close();
}

void TmsSetup::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == m_timerId)
        QDialog::reject();
}

void TmsSetup::CheckFlow()
{
    for(int i = 0; i < 5; i++)
    {
        m_bFlowRate[i] = false;
        m_Ratio[i] = 1;
    }
    QSqlQuery query("select Name, Device, Ratio From Tag Where Name LIKE 'FLOW%'");
    m_nFlowNum = 1;
    while(query.next())
    {
        QString name = query.value(0).toString();
        if(name.length() != 5)
            continue;
        int n = (name.toLocal8Bit())[4]-0x30;
        if(n < 1 || n >= 5)
            continue;
        if(n > m_nFlowNum)
            m_nFlowNum = n;
        QString device = query.value(1).toString();
        m_Ratio[n-1] = query.value(2).toFloat();
        QString str = QString("select Port From Driver_Tms Where Name='%1'").arg(device);
        QSqlQuery query2(str);
        if(query2.next())
        {
            QString port =  query2.value(0).toString();
            m_FlowPort[n-1] = port;
        }
    }
    QSqlQuery query2("select Name From Tag Where Name LIKE 'FLOW%_RATE'");
    while(query2.next())
    {
        QString name = query2.value(0).toString();
        int n = (name.toLocal8Bit())[4]-0x30;
        if(n <= 5)
        {
            m_bFlowRate[n-1] = true;
            //mFlowCheckBox[n-1]->setChecked(true);
        }
    }
//    m_nFlowRatio = 1;
//    QSqlQuery query3("select Ratio From Tag Where Name = 'FLOW1'");
//    if(query3.next())
//        m_nFlowRatio = query3.value(0).toFloat();
    QSqlQuery query4("select Name From Tag Where Name = 'SUS00_MTM1'");
    if(query4.next())
        m_bWaterTempSs = true;
    else
        m_bWaterTempSs = false;
}

void TmsSetup::DispSamplerDoor()
{
/*
    QSqlQuery query("select Driver,Address from Tag Where Name='SAM00_DOOR'");
    if(query.next())
    {
        if(query.value(0).toString() != "Tms")
        {
            ui->chkExternalIO->setChecked(true);
            ui->SamplerAddress->setText(query.value(1).toString());
            on_chkExternalIO_clicked(true);
            QSqlQuery query2("select Port from Driver_Modbus Where Name='io'");
            if(query2.next())
            {
                int index = ui->SamplerPort->findText(query2.value(0).toString());
                ui->SamplerPort->setCurrentIndex(index);
            }
        }
    }
    */
    m_bSamplerDoorExternal = false;
    QSqlQuery query("select Driver from Tag Where Name='SAM00_DOOR'");
    if(query.next())
    {
        if(query.value(0).toString() != "Tms")
            m_bSamplerDoorExternal = true;
//            ui->chkExternalIO->setChecked(true);
    }
}

void TmsSetup::SaveSamplerDoor()
{
    /*
    QString str;
    QString driver, device, address;
    if(ui->chkExternalIO->isChecked())
    {
        driver = "Modbus";
        device = "io";
        address = ui->SamplerAddress->text();
    }
    else
    {
        driver = "Tms";
        device = "Sampler";
        address = "SampDoor";
    }
    str = QString("UPDATE Tag SET Driver='%1', Device='%2',Address='%3' Where Name='SAM00_DOOR'").arg(driver).arg(device).arg(address);
    SqlExec(str);
    str = QString("UPDATE Driver_Modbus SET Port='%1' Where Name='io'").arg(ui->SamplerPort->currentText());
    SqlExec(str);
    */
    QString str;
    QString driver, device, address;
    int rev = 0;
//    if(ui->chkExternalIO->isChecked())
    if(m_bSamplerDoorExternal)
    {
        driver = "System";
        device = "io";
        address = QString("di%1").arg(m_nSampDoor);
//        if(ui->sampDoorReverse->isChecked())
        if(m_bDoorReversed)
            rev = 1;
    }
    else
    {
        driver = "Tms";
        device = "Sampler";
        address = "SampDoor";
    }
    str = QString("UPDATE Tag SET Driver='%1', Device='%2',Address='%3', Reversed=%4 Where Name='SAM00_DOOR'").arg(driver).arg(device).arg(address).arg(rev);
    SqlExec(str);
}

void TmsSetup::CheckDisp()
{
    for(int i = 0; i < 9; i++)
    {
        if(mbFlag[i] == true )
        {
            checkLabel[i]->setVisible(mbCheckFlag[i]);
        }
    }
}


void TmsSetup::UseStxSet(QString Name, int use)
{
    TMS_ITEM_TAB * tmsitem;
    if(Name == "Flow1")
        Name = "Flow";
    for(int i = 0; i < TmsItemList.size(); i++)
    {
        tmsitem = TmsItemList.at(i);
/*        if(Name == tmsitem->Desc || (Name == "TNTP" && (tmsitem->Desc == "TN" || tmsitem->Desc == "TP"))
                                 || (Name == "PHSS" && (tmsitem->Desc == "PH" || tmsitem->Desc == "SS"))
                                 || (Name == "TOCCOD" && (tmsitem->Desc == "TOC" || tmsitem->Desc == "COD")))
*/
        if(Name == tmsitem->Desc)
        {
            if(tmsitem->Desc == "Sampler" || tmsitem->Desc == "Flow")
            {
                tmsitem->UseStxChecksum = use;
                SetItem(QString::number(use), i, 9);
            }
        }
    }
}

void TmsSetup::DispGeneral()
{
    QSqlQuery query("select Location,Discharge,MonitorPort, Timeout from Ext_Tms");
    if(query.next())
    {
        ui->Location->setText(query.value(0).toString());
        ui->Discharge->setText(query.value(1).toString());
        ui->Timeout->setText(query.value(3).toString());
        QString monitor = query.value(2).toString();
        if(monitor != "")
        {
            int index = ui->MonitorPort->findText(monitor);
            ui->MonitorPort->setCurrentIndex(index);
            QString str = "select Baudrate from Port where Name = '" + monitor + "'";
            QSqlQuery query2(str);
            if(query2.next())
                ui->MonitorPortBaudrate->setCurrentText(query2.value(0).toString());
        }
        else
            ui->MonitorPortBaudrate->setEnabled(false);
    }
    QSqlQuery query2("select Ip, Port from Port where Name='Server'");
    if(query2.next())
    {
        ui->ServerIp->setText(query2.value(0).toString());
        ui->ServerPort->setText(query2.value(1).toString());
    }
    QSqlQuery query3("select Port from Port where Name='Listen'");
    if(query3.next())
    {
        ui->ListenPort->setText(query3.value(0).toString());
    }
}

int TmsSetup::SetDi(const char* name, QPushButton *btn, QCheckBox *chk)
{
    int pos = 0;
    QString str = QString("select Address, Reversed from Tag where Name = '%1'").arg(name);
    QSqlQuery query(str);
    if (query.next())
    {
        QString address = query.value(0).toString();
        address = address.mid(2);
        pos = address.toInt();
        if(pos)
        {
            address = QString("DI %1").arg(pos);
            btn->setText(address);
            if(query.value(1).toInt() == 1)
                chk->setChecked(true);
        }
    }
    return pos;
}

int TmsSetup::SetDi2(const char* name)
{
    int pos = 0;
    QString str = QString("select Address, Reversed from Tag where Name = '%1'").arg(name);
    QSqlQuery query(str);
    if (query.next())
    {
        QString address = query.value(0).toString();
        address = address.mid(2);
        pos = address.toInt();
        if(pos)
        {
            address = QString("DI %1").arg(pos);
            if(query.value(1).toInt() == 1)
              m_bDoorReversed = true;
            else
              m_bDoorReversed = false;
        }
    }
    return pos;
}

void TmsSetup::SetItem(QString str, int row, int col)
{
    QTableWidgetItem *item = new QTableWidgetItem(str);
    ui->tableSensor->setItem(row, col, item);
}

void TmsSetup::SetItem(int row, const TMS_ITEM_TAB *tmsitem, bool bAdd)
{
    if(bAdd)
        ui->tableSensor->insertRow(row);
    ui->tableSensor->setRowHeight(row, SENSOR_ROW_HIGHT);
    int nAnalog = tmsitem->Analog;
    QString sId = tmsitem->Id;
    QString sCode = tmsitem->Code;
    QString tmp;
/*    if(tmsitem->Desc == "TOC")
    {
        if(tmsitem->Analog)
        {
            sCode = "TOC10";
            sId = "TOTALC1";
        }
        nAnalog = 0;
    }
*/    SetItem(tmsitem->Desc, row, 0);
    SetItem(sId, row, 1);
    SetItem(sCode, row, 2);
    SetItem(tmsitem->Protocol, row, 3);
    SetItem(QString::number(tmsitem->RoundUp), row, 4);
    SetItem(tmsitem->Port, row, 5);
    SetItem(QString::number(tmsitem->ScanTime), row, 6);
    SetItem(QString::number(tmsitem->Timeout), row, 7);
    SetItem(QString::number(tmsitem->Upload), row, 8);
    if(tmsitem->Desc == "Sampler" || tmsitem->Desc == "Flow")
    {
        SetItem(QString::number(tmsitem->UseStxChecksum), row, 9);
        SetItem(QString::number(tmsitem->UseCchk), row, 10);
    }
    else
    {
        SetItem(tmp, row, 9);
        SetItem(tmp, row, 10);
    }
    if(tmsitem->Desc != "TOC")
        SetItem(QString::number(nAnalog), row, 11);
    else
        SetItem(tmp, row, 11);
}

void TmsSetup::on_sensorSelect_clicked()
{
    SensorSelect dlg(m_nFlowNum, m_bWaterTempSs);
    dlg.sSamModel = m_sSamplerModel;
    for(int i = 0; i < 5; i++)
    {
        dlg.m_bFlowRate[i] = m_bFlowRate[i];
        dlg.m_FlowPort[i] = m_FlowPort[i];
//        dlg.m_FlowProtocol[i] = m_FlowProtocol[i];
        dlg.m_Ratio[i] = m_Ratio[i];
    }
    dlg.m_bSamplerDoorExternal = m_bSamplerDoorExternal;
    dlg.m_nSampDoor = m_nSampDoor;
    dlg.m_bDoorReversed = m_bDoorReversed;
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(tmsitem->Code == "TON00")
        {
            dlg.sTn = tmsitem->Protocol;
//            dlg.bTnCchk = tmsitem->UseCchk;
//            dlg.bStx_tn = (bool)tmsitem->UseStxChecksum;
//            dlg.bAnalog_tn = (bool)tmsitem->Analog;
            dlg.sTnComPort = tmsitem->Port;
             dlg.bTn = true;
        }
        else
        if(tmsitem->Code == "TOP00")
        {
            dlg.sTp = tmsitem->Protocol;
//            dlg.bTpCchk = tmsitem->UseCchk;
//            dlg.bStx_tp = (bool)tmsitem->UseStxChecksum;
//            dlg.bAnalog_tp = (bool)tmsitem->Analog;
            dlg.sTpComPort = tmsitem->Port;
            dlg.bTp = true;
        }
        else
        if(tmsitem->Code == "COD00")
        {
            dlg.sCod = tmsitem->Protocol;
//            dlg.bCodCchk = tmsitem->UseCchk;
//            dlg.bStx_cod = (bool)tmsitem->UseStxChecksum;
//            dlg.bAnalog_cod = (bool)tmsitem->Analog;
            dlg.sCodComPort = tmsitem->Port;
            dlg.bCod = true;
        }
        else
        if(tmsitem->Code == "PHY00")
        {
            dlg.sPh = tmsitem->Protocol;
//            dlg.bPhCchk = tmsitem->UseCchk;
//            dlg.bStx_ph = (bool)tmsitem->UseStxChecksum;
//            dlg.bAnalog_ph = (bool)tmsitem->Analog;
            dlg.sPhComPort = tmsitem->Port;
            dlg.bPh = true;
        }
        else
        if(tmsitem->Code == "SUS00")
        {
            dlg.sSs = tmsitem->Protocol;
//            dlg.bSsCchk = tmsitem->UseCchk;
//            dlg.bStx_ss = (bool)tmsitem->UseStxChecksum;
//            dlg.bAnalog_ss = (bool)tmsitem->Analog;
            dlg.sSsComPort = tmsitem->Port;
            dlg.bSs = true;
        }
        else
        if(tmsitem->Code == "FLW01")
        {
            dlg.sFlow = tmsitem->Protocol;
            dlg.bAnalog_flow = (bool)tmsitem->Analog;
            dlg.bStx_flow = (bool)tmsitem->UseStxChecksum;
            dlg.sFlowComPort = tmsitem->Port;
            dlg.bFlow = true;
        }
        else
        if(tmsitem->Code == "TOC00" || tmsitem->Code == "TOC10")
        {
            dlg.sToc = tmsitem->Protocol;
            dlg.bGas_toc = tmsitem->UseCchk;
//            dlg.bAnalog_toc = (bool)tmsitem->Analog;
//            dlg.bStx_toc = (bool)tmsitem->UseStxChecksum;
            dlg.sTocComPort = tmsitem->Port;
            dlg.bToc = true;
        }
        else
        if(tmsitem->Code == "SAM00")
        {
//            dlg.bSamplerCchk = tmsitem->UseCchk;
            dlg.bStx_sampler = (bool)tmsitem->UseStxChecksum;
            dlg.sSamplerComPort = tmsitem->Port;
            dlg.bSampler = true;
        }
    }
    dlg.disp();
    if(dlg.exec() == QDialog::Accepted)
    {
        for(int i = 0; i < 5; i++)
        {
            m_bFlowRate[i] = dlg.m_bFlowRate[i];
            m_FlowPort[i] = dlg.m_FlowPort[i];
//            m_FlowProtocol[i] = dlg.m_FlowProtocol[i];
            m_Ratio[i] = dlg.m_Ratio[i];
        }
        m_bSamplerDoorExternal = dlg.m_bSamplerDoorExternal;
        m_nSampDoor = dlg.m_nSampDoor;
        m_bDoorReversed = dlg.m_bDoorReversed;
        foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
            delete tmsitem;
        TmsItemList.clear();
        ui->tableSensor->setRowCount(0);
        mbFlag[0] = dlg.bTn;
        mbFlag[1] = dlg.bTp;
        mbFlag[2] = dlg.bToc;
        mbFlag[3] = dlg.bPh;
        mbFlag[4] = dlg.bSs;
        mbFlag[5] = dlg.bCod;
        mbFlag[7] = dlg.bFlow;
        mbFlag[8] = dlg.bSampler;
        m_nFlowNum  = dlg.nFlowNum;
 //       m_nFlowRatio  = dlg.nFlowRatio;
        m_sSamplerModel = dlg.sSamModel;
        m_bWaterTempSs = dlg.bWaterTemp_ss;
//        for(int i = 0; i < m_nFlowNum; i++)       // 유량계설정탭에서 설정하기 때문에 측정기 선택에서는 필요없음
//            m_bFlowRate[i] = dlg.bRate;
/*        for(int i = 0; i < 5; i++)
        {
            bool b = false;
            if(i < m_nFlowNum)
            {
                b = true;
                mFlowCheckBox[i]->setChecked(m_bFlowRate[i]);
            }
            mFlowLabel[i]->setVisible(b);
            mFlowCheckBox[i]->setVisible(b);
        }
*/
        if(dlg.bTn)
            SensorAdd(tr("TN"), tr("TOTALN0"), tr("TON00"), dlg.sTn, 3, // 총 질소
                         dlg.sTnComPort, 10000, 5000, tr("PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                         dlg.bTnCchk, (int)dlg.bAnalog_tn, (int)dlg.bStx_tn);
                           0, (int)dlg.bAnalog_tn, 0);
        if(dlg.bTp)
            SensorAdd(tr("TP"), tr("TOTALP0"), tr("TOP00"), dlg.sTp, 3, // 총 인
                         dlg.sTpComPort, 10000, 5000, tr("PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                         dlg.bTpCchk, (int)dlg.bAnalog_tp, (int)dlg.bStx_tp);
                           0, (int)dlg.bAnalog_tp, 0);
        if(dlg.bToc)
        {
            QString id = "TOTALC0";
            QString code = "TOC00";
//            if(dlg.bAnalog_toc)
//            {
//                id = "TOTALC1";
//                code = "TOC10";
//            }
            SensorAdd(tr("TOC"), id, code, dlg.sToc, 1, // 생물학적 산소요구량
                         dlg.sTocComPort, 10000, 5000, tr("PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
                         dlg.bGas_toc, (int)dlg.bAnalog_toc, 0);
        }
        if(dlg.bPh)
            SensorAdd(tr("PH"), tr("PHYCHM0"), tr("PHY00"), dlg.sPh, 1, // 수소이온 농도지수
                         dlg.sPhComPort, 5000, 5000, tr("PASS,MSIG,MTM1,ZERO,SPAN,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                         dlg.bPhCchk, (int)dlg.bAnalog_ph, (int)dlg.bStx_ph);
                           0, (int)dlg.bAnalog_ph, 0);
        if(dlg.bSs)
        {
            if(m_bWaterTempSs)
                SensorAdd(tr("SS"), tr("SUSSOL0"), tr("SUS00"), dlg.sSs, 1, // 수소이온 농도지수
                         dlg.sSsComPort, 10000, 5000, tr("PASS,MSIG,MTM1,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                         dlg.bSsCchk, (int)dlg.bAnalog_ss, (int)dlg.bStx_ss);
                         0, (int)dlg.bAnalog_ss, 0);
            else
                SensorAdd(tr("SS"), tr("SUSSOL0"), tr("SUS00"), dlg.sSs, 1, // 수소이온 농도지수
                     dlg.sSsComPort, 10000, 5000, tr("PASS,MSIG,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                     dlg.bSsCchk, (int)dlg.bAnalog_ss, (int)dlg.bStx_ss);
                      0, (int)dlg.bAnalog_ss, 0);
        }
        if(dlg.bSampler)
            SensorAdd(tr("Sampler"), tr("SAMPLR0"), tr("SAM00"), tr("Sampler"), 1, // 채수기
                         dlg.sSamplerComPort, 1000, 5000, tr(""), 1,
                         0,0,(int)dlg.bStx_sampler);
        if(dlg.bFlow)
        {
            SensorAdd(tr("Flow"), tr("FLWMTR0"), tr("FLW01"), dlg.sFlow, 0, // 유량계
                         dlg.sFlowComPort, 10000, 5000, tr(""), 1,
                         0, (int)dlg.bAnalog_flow, (int)dlg.bStx_flow);
            // ui->f_comPort1->setCurrentText(tr("Com7"));
        }
        if(dlg.bCod)
            SensorAdd(tr("COD"), tr("CHEMLC0"), tr("COD00"), dlg.sCod, 1, // 화학적 산소요구량
                         dlg.sCodComPort, 10000, 5000, tr("PASS,MSIG,MTM2,MSAM,ZERO,SPAN,SLOP,ICPT,FACT,OFST,MAXR,AUXI"), 1,
//                         dlg.bCodCchk, (int)dlg.bAnalog_cod, (int)dlg.bStx_cod);
                         0, (int)dlg.bAnalog_cod, 0);
        int row = 0;

        foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList) {
            SetItem(row, tmsitem);
            row++;
        }
    }
    bool bToc = false;
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(tmsitem->Desc == "TOC" && tmsitem->Upload == 1)
        {
            bToc = true;
            m_bUseToc = true;
            break;
        }
    }
    int row = 0;
    foreach (TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(bToc == true && tmsitem->Code == "COD00" && tmsitem->Upload == 1)
        {
            tmsitem->Upload = 0;            // TOC가 있으면 TOC만 upload
            SetItem(QString::number(tmsitem->Upload), row, 8);
            break;
        }
        ui->tableSensor->setRowHeight(row, SENSOR_ROW_HIGHT);
        row++;
    }
}

void TmsSetup::SqlExec(QString sql)
{
    if(g_DebugDisplay)
        qDebug() << "SQL:" << sql;
    QSqlQuery query(sql);
    query.exec();
}

void TmsSetup::SensorAdd(QString Desc, QString Id, QString Code, QString Protocol, int Round,
                         QString Port, int ScanTime, int Timeout, QString ConstList, int Upload, int UseCchk, int Analog, int Stx)
{
    TMS_ITEM_TAB *tmsitem = new TMS_ITEM_TAB;
    tmsitem->Desc = Desc;
    tmsitem->Id = Id;
    tmsitem->Code = Code;
    tmsitem->RoundUp = Round;
    tmsitem->Protocol = Protocol;
    tmsitem->Port = Port;
    tmsitem->ScanTime = ScanTime;
    tmsitem->Timeout = Timeout;
    tmsitem->ConstList = ConstList;
    tmsitem->Upload = Upload;
    tmsitem->UseCchk = UseCchk;
    tmsitem->Analog = Analog;
    tmsitem->UseStxChecksum = Stx;

    TmsItemList.append(tmsitem);
}

void TmsSetup::on_door_clicked()
{
    DiSelect dlg(m_nDORON, m_nDiCount);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nDORON = dlg.nDi;
        DiDisp(m_nDORON, ui->door);
    }
}

void TmsSetup::on_power_clicked()
{
    DiSelect dlg(m_nPWRON, m_nDiCount);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nPWRON = dlg.nDi;
        DiDisp(m_nPWRON, ui->power);
    }
}

void TmsSetup::on_sampL_clicked()
{
    DiSelect dlg(m_nFMLON, m_nDiCount);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nFMLON = dlg.nDi;
        DiDisp(m_nFMLON, ui->sampL);
    }
}

void TmsSetup::on_sampR_clicked()
{
    DiSelect dlg(m_nFMRON, m_nDiCount);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nFMRON = dlg.nDi;
        DiDisp(m_nFMRON, ui->sampR);
    }
}
/*
void TmsSetup::on_sampDoor_clicked()
{
    DiSelect dlg(m_nSampDoor, m_nDiCount);
    if(dlg.exec() == QDialog::Accepted)
    {
        m_nSampDoor = dlg.nDi;
        if(m_nSampDoor == 0)
            m_nSampDoor = 5;
        DiDisp(m_nSampDoor, ui->sampDoor);
    }
}
*/
void TmsSetup::DiDisp(int pos, QPushButton *btn)
{
    QString str = "사용안함";
    if(pos)
        str = QString("DI %1").arg(pos);
    btn->setText(str);
}

void TmsSetup::on_tableSensor_itemClicked(QTableWidgetItem *item)
{
    ui->comPort->setEnabled(true);
    ui->round->setEnabled(true);
    ui->scanTime->setEnabled(true);
    ui->timeout->setEnabled(true);
    ui->protocol->setEnabled(true);
    ui->upload->setEnabled(true);
    ui->UseStxChecksum->setEnabled(true);
    ui->usecchk->setEnabled(true);

    SelectedRow = item->row();
    ui->tableSensor->setRowHeight(SelectedRow, SENSOR_ROW_HIGHT);
    QTableWidgetItem *item2 = ui->tableSensor->item(SelectedRow,5);
    int index = ui->comPort->findText(item2->text());
    ui->comPort->setCurrentIndex(index);
    item2 = ui->tableSensor->item(SelectedRow,4);
    ui->round->setText(item2->text());
    item2 = ui->tableSensor->item(SelectedRow,6);
    ui->scanTime->setText(item2->text());
    item2 = ui->tableSensor->item(SelectedRow,7);
    ui->timeout->setText(item2->text());
    item2 = ui->tableSensor->item(SelectedRow,8);
    bool check = false;
    if(item2->text() == "1")
        check = true;
    ui->upload->setChecked(check);

    QString Desc = ui->tableSensor->item(SelectedRow,0)->text();
    QString code = ui->tableSensor->item(SelectedRow,2)->text();
    QString protocol = ui->tableSensor->item(SelectedRow,3)->text();

    if(code == "FLW01" || code == "SAM00")
    {
        item2 = ui->tableSensor->item(SelectedRow,9);
        check = false;
        if(item2->text() == "1")
            check = true;
        ui->UseStxChecksum->setChecked(check);
        item2 = ui->tableSensor->item(SelectedRow,10);
        check = false;
        if(item2->text() == "1")
            check = true;
        ui->usecchk->setChecked(check);
        ui->UseStxChecksum->setEnabled(true);
        ui->usecchk->setEnabled(true);
    }
    else
    {
        check = false;
        ui->UseStxChecksum->setChecked(check);
        ui->usecchk->setChecked(check);
        ui->UseStxChecksum->setEnabled(false);
        ui->usecchk->setEnabled(false);
    }
    mbProtocolEdit = true;
    ui->protocol->clear();
    if(code == "FLW01")
    {
        ui->protocol->addItem(tr("Flow"));
        ui->protocol->addItem(tr("NewFlow"));
        ui->comPort->setEnabled(false);
    }
    else
    if(code == "SAM00")
    {
        ui->protocol->addItem(tr("Sampler"));
        ui->comPort->setEnabled(true);
    }
    else
//    if(code == "TOC00")
    {
        ui->protocol->addItem(tr("NewAnalyzer"));
        SetItem("NewAnalyzer", SelectedRow, 3);
        ui->comPort->setEnabled(true);
    }
/*
    else
    {
        ui->protocol->addItem(tr("Analyzer"));
        ui->protocol->addItem(tr("NewAnalyzer"));
    }
*/

    for(int i = 0; i < ui->protocol->count(); i++)
        ui->protocol->model()->setData(ui->protocol->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);

    index = ui->protocol->findText(ui->tableSensor->item(SelectedRow,3)->text());
    ui->protocol->setCurrentIndex(index);
    mbProtocolEdit = false;
}

void TmsSetup::RunStart(bool start)
{
    QJsonObject argObject;
    QJsonValue vObject(QJsonValue::Null);

    bool SaveStatus=false;
    argObject.insert(QString("Proc"),QJsonValue(QString("Sun")));
    argObject.insert(QString("Enabled"),QJsonValue(start));
    argObject.insert(QString("SaveStatus"),QJsonValue(SaveStatus));

    CMonitorUdp::Instance()->sendCommand(0,  QString("ProcMonitor_Set"), argObject, vObject);
}

void TmsSetup::on_save_clicked()
{
    bool bError = false;

    QString monitor = ui->MonitorPort->currentText();
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if((tmsitem->Code != "FLW01" && tmsitem->Port == "") || tmsitem->Port == monitor)
        {
            bError = true;
            break;
        }
    }
    for(int i = 0; i < TmsItemList.length(); i++)
    {
        for(int j = 0; j < TmsItemList.length(); j++)
        {
            if(i != j)
            {
                if(TmsItemList[i]->Port == TmsItemList[j]->Port)
                {
                    if((TmsItemList[i]->Code == "TON00" && TmsItemList[j]->Code == "TOP00") ||
                            (TmsItemList[i]->Code == "TOP00" && TmsItemList[j]->Code == "TON00") ||
                            (TmsItemList[i]->Code == "PHY00" && TmsItemList[j]->Code == "SUS00") ||
                            (TmsItemList[i]->Code == "SUS00" && TmsItemList[j]->Code == "PHY00"))
                    {
                        continue;
                    }
                    bError = true;
                    break;
                }
            }
        }
    }
    for(int i = 0; i < TmsItemList.length(); i++)
    {
        if(TmsItemList[i]->Code != "FLW01")
        {
            for(int j = 0; j < m_nFlowNum; j++)
            {
                if(TmsItemList[i]->Port == m_FlowPort[j])
                {
                    bError = true;
                    break;
                }
            }
        }
    }
    if(bError == true)
    {
        CInfomationDialog dlg("잘못된 통신포트나 중복된 포트가 있습니다");
        dlg.exec();
        return;
    }

    QString serverIp = ui->ServerIp->text();
    QString serverPort = ui->ServerPort->text();

    QString msg = "변경된 내용을 저장하시겠습니까?\n";
    if(serverIp != mServerIp)
    {
        if(serverPort != mServerPort)
        {
            msg += "(IP:" + serverIp + ", Port:" + serverPort + ")\n\n";
        }
        else
        {
            msg += "(IP:" + serverIp + ")\n\n";
        }
    }
    else
    if(serverPort != mServerPort)
    {
        msg += "(Port:" + serverPort + ")\n\n";
    }
    else
        msg += "\n";
    msg += "저장하면 프로그램이 재기동합니다.";
    CQuestionDialog dlg(msg);
    if(dlg.exec() == QDialog::Accepted)
    {
        gDb.setDatabaseName(tr("/data/project2.db"));
        if(gDb.open() == true)
        {
            DbSave();
            gDb.close();
        }
        QDialog::accept();
    }
}

int TmsSetup::SensorIndex(QString desc)
{
    int ret = -1;
    for(int i = 0; i < 9; i++)
        if(desc == DESC_TAB[i])
            ret = i;
    return ret;
}

void TmsSetup::CheckMultiSensor(bool &tntp, bool &phss, bool &toccod)
{
    QString tn,tp,ph,ss, toc, cod;
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(tmsitem->Desc == "TN")
            tn = tmsitem->Port;
        else
        if(tmsitem->Desc == "TP")
            tp = tmsitem->Port;
        else
        if(tmsitem->Desc == "PH")
            ph = tmsitem->Port;
        else
        if(tmsitem->Desc == "SS")
            ss = tmsitem->Port;
        else
        if(tmsitem->Desc == "TOC")
            toc = tmsitem->Port;
        else
        if(tmsitem->Desc == "COD")
            cod = tmsitem->Port;
    }
    if(tn == tp)
        tntp = true;
    if(ph == ss)
        phss = true;
//    if(toc == cod)
//       toccod = true;
}

void TmsSetup::DbSave()
{
//    for(int i = 0; i < 5; i++)
//    {
//        m_bFlowRate[i] = mFlowCheckBox[i]->isChecked();
//    }

    QProgressBar *progressBar = new QProgressBar(this);
    progressBar->setGeometry(330, 250, 420, 30);
    progressBar->setMinimum(0);
    progressBar->setMaximum(TmsItemList.length()+8);

    progressBar->show();
    int cnt = 0;
    QString str;
    SqlExec(tr("delete from 'Driver_Tms'"));
    SqlExec(tr("delete from 'Ext_Tms_Item'"));
    SqlExec(tr("delete from 'Tag'"));
    SqlExec(tr("delete from 'TagGroup'"));
    tntp = false;
    phss = false;
    toccod = false;
    bool tntpSave = false;
    bool phssSave = false;
    bool toccodSave = false;

    CheckMultiSensor(tntp, phss, toccod);

    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        progressBar->setValue(++cnt);
        QString ConstList;
        if(tmsitem->Code != "FLW01" && tmsitem->Code != "SAM00")
        {
            int index = SensorIndex(tmsitem->Desc);
            if(index < 0)
                continue;
            ConstList = CONST_TAB[index];
        }
        if(tmsitem->Code != "TOC00" && tmsitem->Code != "TOC10" && tmsitem->Protocol == "Analyzer")
            ConstList = CONST_TAB[OLD];
        if(tmsitem->Code == "FLW01")
        {
            for(int i = 0; i < m_nFlowNum; i++)
            {
                QString name = QString("%1%2").arg(tmsitem->Desc).arg(i+1);
                str = QString("INSERT INTO `Driver_Tms` (Name,Desc,Enabled,Protocol,UseStxInChecksum,SamplerModel,"
                              "Port,ScanPeriod,ReadRetryCount,WriteRetryCount,Timeout,FrameDelay,Address) VALUES"
                              "('%1','%1',1,'%2',%6,'',"
                              "'%3',%4,3,3,%5,0,0)")
                        .arg(name).arg(tmsitem->Protocol).arg(m_FlowPort[i]).arg(tmsitem->ScanTime).arg(tmsitem->Timeout).arg(tmsitem->UseStxChecksum);
                SqlExec(str);
            }
            //ui->f_comPort1->setCurrentText(tmsitem->Port);
        }
        else
        {
            bool skip = false;
            QString device = tmsitem->Desc;
            if(device == "TN")
            {
                if(tntp) //muti sensor 이면
                {
                    if(tntpSave == true)
                        skip = true;
                    device = "TNTP";
                    tntpSave = true;
                }
            }
            else
            if(device == "TP")
            {
                if(tntp) //muti sensor 이면
                {
                    if(tntpSave == true)
                        skip = true;
                    device = "TNTP";
                    tntpSave = true;
                }
            }
            else
            if(device == "PH")
            {
                if(phss) //muti sensor 이면
                {
                    if(phssSave == true)
                        skip = true;
                    device = "PHSS";
                    phssSave = true;
                }
            }
            else
            if(device == "SS")
            {
                if(phss) //muti sensor 이면
                {
                    if(phssSave == true)
                        skip = true;
                    device = "PHSS";
                    phssSave = true;
                }
                if(m_bWaterTempSs)
                    ConstList = CONST_TAB[SS_TEMP];
            }
            else
            if(device == "TOC")
            {
                if(toccod) //muti sensor 이면
                {
                    if(toccodSave == true)
                        skip = true;
                    device = "TOCCOD";
                    toccodSave = true;
                }
                if(tmsitem->UseCchk == 1)  //이송가스
                    ConstList = CONST_TAB[TOC_GAS];
            }
            else
            if(device == "COD")
            {
                if(toccod) //muti sensor 이면
                {
                    if(toccodSave == true)
                        skip = true;
                    device = "TOCCOD";
                    toccodSave = true;
                }
            }

            QString SamplerModel;
            if(tmsitem->Code == "SAM00")
                SamplerModel = m_sSamplerModel;
            if(skip == false)
            {
                str = QString("INSERT INTO `Driver_Tms` (Name,Desc,Enabled,Protocol,UseStxInChecksum,SamplerModel,"
                              "Port,ScanPeriod,ReadRetryCount,WriteRetryCount,Timeout,FrameDelay,Address) VALUES"
                              "('%1','%1',1,'%2',%6,'%7',"
                              "'%3',%4,3,3,%5,0,0)")
                        .arg(device).arg(tmsitem->Protocol).arg(tmsitem->Port).arg(tmsitem->ScanTime).arg(tmsitem->Timeout).arg(tmsitem->UseStxChecksum).arg(SamplerModel);
                SqlExec(str);
            }
        }
        str = QString("INSERT INTO `Ext_Tms_Item` (Id,Code,Enabled,Desc,Protocol,Roundup,Port,ScanTime,"
                      "Timeout,ConstList,Upload, UseCchk, Analog) VALUES ("
                      "'%1','%2',1,'%3','%4',%5,'%6',%7,"
                      "%8,'%9',%10, %11, %12)")
                .arg(tmsitem->Id).arg(tmsitem->Code).arg(tmsitem->Desc).arg(tmsitem->Protocol)
                .arg(tmsitem->RoundUp).arg(tmsitem->Port).arg(tmsitem->ScanTime)
                .arg(tmsitem->Timeout).arg(ConstList).arg(tmsitem->Upload).arg(tmsitem->UseCchk).arg(tmsitem->Analog);
        SqlExec(str);

        int     max=0;
        int     *select;
        int     sensor=-1;
        bool    bOld = false;
        if(tmsitem->Protocol == "Analyzer") // 구형이면
            bOld = true;
//        else
        {
            if(tmsitem->Code == "COD00")
                sensor = COD;
            else
            if(tmsitem->Code == "TON00")
                sensor = TN;
            else
            if(tmsitem->Code == "TOP00")
                sensor = TP;
            else
            if(tmsitem->Code == "PHY00")
                sensor = PH;
            else
            if(tmsitem->Code == "SUS00")
                sensor = SS;
            else
            if(tmsitem->Code == "FLW01")
                sensor = FLOW;
            else
            if(tmsitem->Code == "SAM00")
                sensor = SAMP;
            else
            if(tmsitem->Code == "TOC00" || tmsitem->Code == "TOC10")
                sensor = TOC;
        }
        if(sensor != -1)
        {
            if(bOld)
            {
                select = ITEM_SELECT[OLD];
                max = ITEM_MAX[OLD];
            }
            else
            {
                select = ITEM_SELECT[sensor];
                max = ITEM_MAX[sensor];
            }
            if(sensor == SS && m_bWaterTempSs == true)
            {
                select = ITEM_SELECT[SS_TEMP];
                max = ITEM_MAX[SS_TEMP];
            }
            if(sensor == TOC && tmsitem->UseCchk == 1)  //이송가스
            {
                select = ITEM_SELECT[TOC_GAS];
                max = ITEM_MAX[TOC_GAS];
            }
            SensorSave(tmsitem, max, sensor, select, bOld);
        }

        str = QString("UPDATE Port SET Baudrate=9600 Where Name='%1'")
                .arg(tmsitem->Port);
        SqlExec(str);
    }

    int pos;
    int rev;
    char addr[4];
    addr[0] = 'd';
    addr[1] = 'i';
    addr[3] = 0;
    QString group;
    if(m_nDORON)
    {
        if(ui->doorReverse->isChecked())
            rev = 1;
        else
            rev = 0;
        pos = DORON;
        IoSave("DORON", tr("출입문 개폐"));
        addr[2] = m_nDORON + 0x30;
        TagAdd(TagTab[pos].suffix, "부대장비&상태", TagTab[pos].TagType, TagTab[pos].Desc, TagTab[pos].Driver,
               "io", addr, TagTab[pos].Rw, TagTab[pos].DataType,
               TagTab[pos].On, TagTab[pos].Off, rev, TagTab[pos].Unit, 0, 0, "0", 1);
        progressBar->setValue(++cnt);
        group = "DORON";
    }
    if(m_nPWRON)
    {
        if(ui->powerReverse->isChecked())
            rev = 1;
        else
            rev = 0;
        pos = PWRON;
        IoSave("PWRON", tr("입력전원 단절"));
        addr[2] = m_nPWRON + 0x30;
        TagAdd(TagTab[pos].suffix, "부대장비&상태", TagTab[pos].TagType, TagTab[pos].Desc, TagTab[pos].Driver,
                "io", addr, TagTab[pos].Rw, TagTab[pos].DataType,
               TagTab[pos].On, TagTab[pos].Off, rev, TagTab[pos].Unit, 0, 0, "0", 1);
        if(group == "")
            group = "PWRON";
        else
        {
            group += ",";
            group += "PWRON";
        }
    }
    progressBar->setValue(++cnt);
    if(m_nFMLON)
    {
        if(ui->sampLReverse->isChecked())
            rev = 1;
        else
            rev = 0;
        pos = FMLON;
        IoSave("FMLON", tr("채수펌프(A)"));
        addr[2] = m_nFMLON + 0x30;
        TagAdd(TagTab[pos].suffix, "부대장비&상태", TagTab[pos].TagType, TagTab[pos].Desc, TagTab[pos].Driver,
                "io", addr, TagTab[pos].Rw, TagTab[pos].DataType,
               TagTab[pos].On, TagTab[pos].Off, rev, TagTab[pos].Unit, 0, 0, "0", 1);
        if(group == "")
            group = "FMLON";
        else
        {
            group += ",";
            group += "FMLON";
        }
    }
    progressBar->setValue(++cnt);
    if(m_nFMRON)
    {
        if(ui->sampRReverse->isChecked())
            rev = 1;
        else
            rev = 0;
        pos = FMRON;
        IoSave("FMRON", tr("채수펌프(B)"));
        addr[2] = m_nFMRON + 0x30;
        TagAdd(TagTab[pos].suffix, "부대장비&상태", TagTab[pos].TagType, TagTab[pos].Desc, TagTab[pos].Driver,
                "io", addr, TagTab[pos].Rw, TagTab[pos].DataType,
               TagTab[pos].On, TagTab[pos].Off, rev, TagTab[pos].Unit, 0, 0, "0", 1);
        if(group == "")
            group = "FMRON";
        else
        {
            group += ",";
            group += "FMRON";
        }
    }
    progressBar->setValue(++cnt);
    QString device;
    for(int i = 29; i < 34; i++)
    {
        QString device;
        if(i == 33)
            device = "time";
        else
            device = "internal";
        progressBar->setValue(++cnt);
        TagAdd(TagTab[i].suffix, "부대장비&상태", TagTab[i].TagType, TagTab[i].Desc, TagTab[i].Driver,
                device, TagTab[i].Address, TagTab[i].Rw, TagTab[i].DataType,
               TagTab[i].On, TagTab[i].Off, 0, TagTab[i].Unit, 0, 0, TagTab[i].InitValue, 1);
        if(group == "")
            group = TagTab[i].suffix;
        else
        {
            group += ",";
            group += TagTab[i].suffix;
        }
    }

    device = "internal";
    for(int i = 36; i < 38; i++)
    {
        if(i == 37)
            device = "io";
        TagAdd(TagTab[i].suffix, "부대장비&상태", TagTab[i].TagType, TagTab[i].Desc, TagTab[i].Driver,
                device, TagTab[i].Address, TagTab[i].Rw, TagTab[i].DataType,
               TagTab[i].On, TagTab[i].Off, 0, TagTab[i].Unit, 0, 0, TagTab[i].InitValue, 1);
        if(group == "")
            group = TagTab[i].suffix;
        else
        {
            group += ",";
            group += TagTab[i].suffix;
        }
    }

    str = QString("INSERT INTO `TagGroup` (Name, Desc, Enabled, Tags) VALUES ('%1', '%1', 1, '%2')").arg(tr("부대장비&상태")).arg(group);
    SqlExec(str);
    QString monitor = ui->MonitorPort->currentText();
    QString baudrate = ui->MonitorPortBaudrate->currentText();
    if(monitor == "사용안함")
        monitor = "";
    else
    {
        str = QString("UPDATE Port SET Baudrate='%1' Where Name='%2'")
                .arg(baudrate).arg(monitor);
        SqlExec(str);
    }
    str = QString("UPDATE Ext_Tms SET Location='%1', Discharge=%2, MonitorPort='%3', Timeout=%4")
            .arg(ui->Location->text()).arg(ui->Discharge->text()).arg(monitor).arg(ui->Timeout->text());
    SqlExec(str);
    str = QString("UPDATE Port SET Ip='%1', Port=%2 Where Name='Server'")
            .arg(ui->ServerIp->text()).arg(ui->ServerPort->text());
    SqlExec(str);
    str = QString("UPDATE Port SET Ip='%1', Port=%2 Where Name='Listen'")
            .arg(ui->ServerIp->text()).arg(ui->ListenPort->text());
    SqlExec(str);
    SaveSamplerDoor();
}

void TmsSetup::IoSave(const char *Id, QString Desc)
{
    QString str = QString("INSERT INTO `Ext_Tms_Item` (Id,Code,Enabled,Desc,Protocol,Roundup,Port,ScanTime,"
                  "Timeout,ConstList,Upload,UseCchk,Analog) VALUES ("
                  "'MONITOR','%1',1,'%2','io',0,NULL,1000,NULL,NULL,1,NULL,0)")
            .arg(Id).arg(Desc);
    SqlExec(str);
}

void TmsSetup::SensorSave(const TMS_ITEM_TAB *item, int max, int sensor, int *select, bool bOld)
{
    QString name,device, group,unit, driver, address;
    for(int i=0; i < max; i++)
    {
        name = item->Code;
        if(select[i] == 29)     // FLOW_DAYTOTAL
            name = TagTab[select[i]].suffix;
        else
            name += TagTab[select[i]].suffix;
        driver = TagTab[select[i]].Driver;
        device = TagTab[select[i]].Driver;
        address = TagTab[select[i]].Address;
        if(device == "System")
            device = "internal";
        else
        {
            device = item->Desc;
            if(tntp && (item->Desc == "TN" || item->Desc == "TP"))
                device = "TNTP";
            if(phss && (item->Desc == "PH" || item->Desc == "SS"))
                device = "PHSS";
            if(toccod && (item->Desc == "TOC" || item->Desc == "BOD"))
                device = "TOCCOD";
        }
        if(sensor == FLOW)
        {
            if(select[i] == 0 || select[i] == 1 || select[i] == 4)
            {
                driver = "System";
                device = "internal";
                address = "";
            }
            else
            if(driver == "Tms")
            {
                device = "Flow1";
            }
        }
        if(select[i] == 0)
            unit = UNIT_TAB[sensor];
        else
            unit = TagTab[select[i]].Unit;
        int round = item->RoundUp;
        if((select[i] >= 2 && select[i] <= 8) || (select[i] > 21 && select[i] <= 23))   // _POS
            round = 0;
        else
        if(select[i] >= 9 && select[i] < 21)
            round = -1;
        else
        if(select[i] == 35) //_RATE
            round = 2;
        int save = TagTab[select[i]].Save;
        if(name == "SAMP00" && select[i] == 7)  // SAMP00_TIME
            save = 1;
        if(bOld == true && item->UseCchk == 0 && (sensor < FLOW || bOld == true) && (select[i] >= 13 && select[i] <= 19))
        {
            driver = "System";
            device = "internal";
        }
        TagAdd(name, item->Desc, TagTab[select[i]].TagType, TagTab[select[i]].Desc, driver,
                device, address, TagTab[select[i]].Rw, TagTab[select[i]].DataType,
               TagTab[select[i]].On, TagTab[select[i]].Off, 0, unit, round, save, TagTab[select[i]].InitValue, 1);
        if(select[i] != 5 && select[i] != 8) // _CMD, _PASS 제외
        {
            if(group == "")
                group = name;
            else
            {
                group += ",";
                group += name;
            }
        }
    }
    if(sensor == FLOW)
    {
        QString desc;
        for(int i = 0; i < m_nFlowNum; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                int round = item->RoundUp;
                if(j == 1 && m_bFlowRate[i] == false)
                    continue;
                if(j == 1)
                    round = 2;
                name = QString("FLOW%1").arg(i+1);
                name += FlowTagTab[j].suffix;
                desc = QString("%1%2").arg(FlowTagTab[j].Desc).arg(i+1);
                device = QString("Flow%1").arg(i+1);
                float ratio = 1;
                if(j == 0)
                    ratio = m_Ratio[i];
                TagAdd(name, item->Desc, FlowTagTab[j].TagType, desc, FlowTagTab[j].Driver,
                    device, FlowTagTab[j].Address, FlowTagTab[j].Rw, FlowTagTab[j].DataType,
                       FlowTagTab[j].On, FlowTagTab[j].Off, 0, FlowTagTab[j].Unit, round, FlowTagTab[j].Save, FlowTagTab[j].InitValue, ratio);
                if(m_nFlowNum > 1)
                {
                    group += ",";
                    group += name;
                }
            }
        }
    }
    name = QString("INSERT INTO `TagGroup` (Name, Desc, Enabled, Tags) VALUES ('%1', '%1', 1, '%2')").arg(item->Desc).arg(group);
    SqlExec(name);
}

void TmsSetup::TagAdd(QString name, QString group, const char *tagtype, QString desc, QString driver,
                      QString device, QString addr, const char * rw, const char *data, const char *on, const char *off,
                      int rev, QString unit, int round, int save, const char * InitValue, float ratio)
{
    QString str = QString("INSERT INTO `Tag` (Name,GroupName,TagType,Desc,Enabled,Driver,Device,Address,"
                          "RwMode,DataType,InitValue,UseForcedValue,ForcedValue,OnMsg,OffMsg,Reversed,"
                          "DigitalAlarm,DigitalEvent,Deadband,RawMin,RawMax,EuMin,EuMax,Unit,Ratio,"
                          "RoundUp,UseLowLowAlarm,LowLowAlarmValue,UseLowAlarm,LowAlarmValue,"
                          "UseHighAlarm,HighAlarmValue,UseHighHighAlarm,HighHighAlarmValue,LinkTags,"
                          "ClearCondition,MaxValueChange,SaveValue,ControlLog) VALUES ("
                          "'%1','%2','%3','%4',1,'%5','%6','%7',"
                          "'%8','%9','%16',0,'0','%10','%11',%12,"
                          "'None','None',0.0,0.0,0.0,0.0,0.0,'%13',%17,"
                          "%14,0,0.0,0,0.0,"
                          "0,0.0,0,0.0,'',"
                          "'Null',0.0,%15,0)")
            .arg(name).arg(group).arg(tagtype).arg(desc).arg(driver).arg(device).arg(addr)
            .arg(rw).arg(data).arg(on).arg(off).arg(rev)
            .arg(unit).arg(round+1).arg(save).arg(InitValue).arg(ratio);

            SqlExec(str);
}

void TmsSetup::on_program2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void TmsSetup::on_general2_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void TmsSetup::on_check_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void TmsSetup::on_timeSync_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void TmsSetup::on_setup_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

extern bool g_bExit;
void TmsSetup::on_networkSet_clicked()
{
    CNetworkSetup dlg;
    dlg.exec();
    if(g_bExit)
        accept();
}

void TmsSetup::on_sensor2_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void TmsSetup::on_flow_setup_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}

void TmsSetup::on_general2_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
}

void TmsSetup::on_checkSet_clicked()
{
    CQuestionDialog dlg("선택된 계측기를 점검중으로 변경하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
        CheckControl(1);
}

void TmsSetup::on_checkReset_clicked()
{
    CQuestionDialog dlg("선택된 계측기 점검중 상태를 해제하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
        CheckControl(0);
}
void TmsSetup::on_digitChange_clicked()
{
    CQuestionDialog dlg("소수점 자리수를 변경하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        QJsonArray array;
        bool selected = false;
        for(int i = 0; i < 9; i++)
        {
            if(mCheckBox2[i]->isChecked())
            {
                QJsonObject jObject;
                int pos = mLineEdit[i]->text().toInt();
                jObject.insert(QString("Code"),QJsonValue(TmsTagNameList[i]));
                jObject.insert(QString("Pos"),QJsonValue(pos));
                array.append(QJsonValue(jObject));
                DigitChange(TmsTagNameList[i], pos, true);
                TableChange(TmsTagNameList[i], pos);
                selected = true;
                if(i == 7)
                {
                    QString name;
                    qDebug() << "m_nFlowNum:" << m_nFlowNum;
                    for(int j = 1; j <= m_nFlowNum; j++)
                    {
                        name = QString("FLOW%1").arg(j);
                        DigitChange(name, pos, false);
                        qDebug() << "name:" << name;
                        jObject.insert(QString("Code"),QJsonValue(name));
                        jObject.insert(QString("Pos"),QJsonValue(pos));
                        array.append(QJsonValue(jObject));
                    }
                    name = QString("FLW01_RATE");
                    DigitChange(name, pos, false);
                }
            }
        }
        if(selected)
        {
            QJsonValue argValue(array);
            g_pSrSocket->sendCommand(0, "Ext_Tms_ChangeFloatPos", GraphicId, argValue);
        }
        else
        {
            CInfomationDialog dlg("선택된 계측기가 없습니다");
            dlg.exec();
        }
    }
}

void TmsSetup::DigitChange(QString name, int pos, bool bExtTms)
{
    gDb.setDatabaseName(tr("/data/project2.db"));
    if(gDb.open() == true)
    {
        QString str = QString("UPDATE Tag SET RoundUp=%1 Where Name = '%2'")
                .arg(pos+1).arg(name);
        SqlExec(str);
        if(bExtTms)
        {
            str = QString("UPDATE Ext_Tms_Item SET RoundUp=%1 Where Code = '%2'")
                    .arg(pos).arg(name);
        }
        SqlExec(str);
        gDb.close();
    }
}

void TmsSetup::TableChange(QString name, int pos)
{
    int i = 0;
    foreach (TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(tmsitem->Code == name)
        {
            tmsitem->RoundUp = pos;
            SetItem(QString::number(pos), i, 4);
            break;
        }
        i++;
    }
}

int TmsSetup::DigitGet(QString name)
{
    QString str = QString("SELECT RoundUp from Tag Where Name='%1'").arg(name);
    QSqlQuery query(str);
    int pos = 0;
    if(query.next())
        pos =  query.value(0).toInt();
    return pos-1;
}

void TmsSetup::on_chkAll_clicked()
{
    bool flag = ui->chkAll->isChecked();
    for(int i = 0; i < 9; i++)
        mCheckBox[i]->setChecked(flag);
}

void TmsSetup::CheckControl(int value)
{
    QJsonArray array;
    bool selected = false;
    for(int i = 0; i < 9; i++)
    {
        if(mCheckBox[i]->isVisible() && mCheckBox[i]->isChecked())
        {
            QJsonObject jObject;
            jObject.insert(QString("Tag"),QJsonValue(TmsCheckTag[i]));
            jObject.insert(QString("Value"),QJsonValue(value));
            array.append(QJsonValue(jObject));
            selected = true;
            mbCheckFlag[i] = (bool)value;
        }
    }
    if(selected)
    {
        QJsonValue argValue(array);
        g_pSrSocket->sendCommand(0, "Tag_Write", GraphicId, argValue);
    }
    else
    {
        CInfomationDialog dlg("선택된 계측기가 없습니다");
        dlg.exec();
    }
    CheckDisp();
}

void TmsSetup::on_DataDelete_clicked()
{
    CQuestionDialog dlg("모든 데이터를 삭제하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        RunStart(false);
        system("rm /data/data.db");
        system("rm /data/tms.db");
        system("rm /data/tmssec.db");
        system("rm /data/share.db");
        system("rm /tmp/share.db");
        system("rm /csv/*");
        RunStart(true);
    }
}

void TmsSetup::on_protocol_currentIndexChanged(const QString &protocol)
{
    if(mbProtocolEdit)
        return;
    if(SelectedRow < 0)
        return;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->Protocol = protocol;
    SetItem(tmsitem->Protocol, SelectedRow, 3);
    if(protocol == "NewFlow" || protocol == "NewAnalyzer")
    {
        ui->UseStxChecksum->setChecked(false);
        tmsitem->UseStxChecksum = 0;
        SetItem(QString::number(tmsitem->UseStxChecksum), SelectedRow, 9);
    }
}

void TmsSetup::on_comPort_currentIndexChanged(const QString &arg1)
{
    if(SelectedRow < 0)
        return;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->Port = arg1;
    SetItem(tmsitem->Port, SelectedRow, 5);
//    if(tmsitem->Desc == "Flow")
//        ui->f_comPort1->setCurrentText(arg1);
}

void TmsSetup::on_round_textChanged(const QString &arg1)
{
    if(SelectedRow < 0)
        return;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->RoundUp = arg1.toInt();
    SetItem(arg1, SelectedRow, 4);
}

void TmsSetup::on_scanTime_textChanged(const QString &arg1)
{
    if(SelectedRow < 0)
        return;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->ScanTime = arg1.toInt();
    SetItem(arg1, SelectedRow, 6);
}

void TmsSetup::on_timeout_textChanged(const QString &arg1)
{
    if(SelectedRow < 0)
        return;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->Timeout = arg1.toInt();
    SetItem(arg1, SelectedRow, 7);
}

void TmsSetup::on_UseStxChecksum_stateChanged(int)
{
    if(SelectedRow < 0)
        return;
    int check = 0;
    if(ui->UseStxChecksum->isChecked())
        check = 1;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->UseStxChecksum = check;
    SetItem(QString::number(tmsitem->UseStxChecksum), SelectedRow, 9);
}
/*
void TmsSetup::on_chkExternalIO_clicked(bool checked)
{
    if(m_nSampDoor == 0)
        m_nSampDoor = 5;
    DiDisp(m_nSampDoor, ui->sampDoor);
    ui->sampDoor->setEnabled(checked);
    ui->sampDoorReverse->setEnabled(checked);
}
*/
void TmsSetup::on_f_comPort1_currentIndexChanged(const QString &arg1)
{
    foreach (TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(tmsitem->Code == "FLW01")
        {
            tmsitem->Port = arg1;
            SetItem(tmsitem->Port, FlowRow, 5);
            if(SelectedRow == FlowRow)
                ui->comPort->setCurrentText(arg1);
        }
    }
}

void TmsSetup::on_usecchk_stateChanged(int)
{
    if(SelectedRow < 0)
        return;
    int check = 0;
    if(ui->usecchk->isChecked())
        check = 1;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->UseCchk = check;
    SetItem(QString::number(tmsitem->UseCchk), SelectedRow, 10);
}
/*
void TmsSetup::on_const_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void TmsSetup::on_changeConsts_clicked()
{
    CQuestionDialog dlg("측정상수를 변경하시겠습니까?");
    if(dlg.exec() != QDialog::Accepted)
        return;
    QJsonArray array;
    QDateTime date = QDateTime::currentDateTime();
    QString dateStr = date.toString("yyyy-MM-dd hh:mm:ss");
    int index;
    for(int i = 0; i < 5; i++)
    {
        index = i;
        if(i == 2 && m_bUseToc == true)
            index = 7;
        if(mbFlag[index])
        {
            QJsonObject jObject;
            jObject.insert(QString("Code"),QJsonValue(TmsTagNameList[index]));
            jObject.insert(QString("Date"),QJsonValue(dateStr));
            jObject.insert(QString("ZERO"),QJsonValue(m_ConstEdit[i][0]->text().toDouble()));
            jObject.insert(QString("SPAN"),QJsonValue(m_ConstEdit[i][1]->text().toDouble()));
            jObject.insert(QString("SLOP"),QJsonValue(m_ConstEdit[i][2]->text().toDouble()));
            jObject.insert(QString("ICPT"),QJsonValue(m_ConstEdit[i][3]->text().toDouble()));
            jObject.insert(QString("FACT"),QJsonValue(m_ConstEdit[i][4]->text().toDouble()));
            jObject.insert(QString("OFST"),QJsonValue(m_ConstEdit[i][5]->text().toDouble()));
            jObject.insert(QString("MAXR"),QJsonValue(m_ConstEdit[i][6]->text().toDouble()));
            array.append(QJsonValue(jObject));
        }
    }
    QJsonValue argValue(array);
    g_pSrSocket->sendCommand(0, "Ext_Tms_SetConsts", GraphicId, argValue);
}
*/
int TmsSetup::CodePos(QString code)
{
    for(int i = 0; i < 8; i++)
    {
        if(TmsTagNameList[i] == code)
        {
            if(m_bUseToc == true && (code == "TOC00" || code == "TOC10"))
                i = 2;
            return i;
        }
    }
    return -1;
}

void TmsSetup::onRead(QString& cmd, QJsonObject& jobject)
{
    if(g_DebugDisplay)
        qDebug() << "TmsSetup::onRead : " << cmd;
  /*
    if(cmd == "Ext_Tms_GetConsts")
    {
        QJsonArray ar = jobject["Result"].toArray();
        if(g_DebugDisplay)
            qDebug() << "Count:" << ar.count();
        QString item;
        foreach (const QJsonValue& val, ar)
        {
            QJsonObject jObj = val.toObject();
            QString code = jObj["Code"].toString();
            int pos = CodePos(code);
            if (pos < 0 || pos >= 5)
                continue;
            item = QString("%1").arg(jObj["ZERO"].toDouble());
            m_ConstEdit[pos][0]->setText(item);
            item = QString("%1").arg(jObj["SPAN"].toDouble());
            m_ConstEdit[pos][1]->setText(item);
            item = QString("%1").arg(jObj["SLOP"].toDouble());
            m_ConstEdit[pos][2]->setText(item);
            item = QString("%1").arg(jObj["ICPT"].toDouble());
            m_ConstEdit[pos][3]->setText(item);
            item = QString("%1").arg(jObj["FACT"].toDouble());
            m_ConstEdit[pos][4]->setText(item);
            item = QString("%1").arg(jObj["OFST"].toDouble());
            m_ConstEdit[pos][5]->setText(item);
            item = QString("%1").arg(jObj["MAXR"].toDouble());
            m_ConstEdit[pos][6]->setText(item);
            item = jObj["Date"].toString();
            m_ConstDate[pos]->setText(item);
        }
    }
    else
  */  if(cmd == "Ext_Tms_GetTimeSyncConfig")
    {
        QJsonObject jObject = jobject["Result"].toObject();

        ui->chkSensorTimesync->setChecked(jObject["Enabled"].toBool());
        QString str;
        str = QString("%1").arg(jObject["Hour"].toInt());
        ui->TimeSyncHour->setText(str);
        str = QString("%1").arg(jObject["Min"].toInt());
        ui->TimeSyncMin->setText(str);
    }
}

void TmsSetup::on_MonitorPort_currentTextChanged(const QString &monitor)
{
    if(m_bIniting)
        return;
    gDb.setDatabaseName(tr("/data/project2.db"));
    if(gDb.open() == true)
    {
        if(monitor != "사용안함")
        {
            ui->MonitorPortBaudrate->setEnabled(true);
            QString str = QString("select Baudrate from Port where Name = '%1'").arg(monitor);
            QSqlQuery query2(str);
            if(query2.next())
                ui->MonitorPortBaudrate->setCurrentText(query2.value(0).toString());
        }
        else
            ui->MonitorPortBaudrate->setEnabled(false);
        gDb.close();
    }
}
/*
void TmsSetup::onTimer()
{
    if(mPassword != gPassword)
        QDialog::reject();
    if (m_bScreenSave == true)
        QDialog::reject();
}
*/

void TmsSetup::on_TimeSyncSet_clicked()
{
    CQuestionDialog dlg("변경된 내용을 적용하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        int hour = ui->TimeSyncHour->text().toInt();
        int min = ui->TimeSyncMin->text().toInt();
        if(((hour >= 0 && hour < 23) && (min >= 0 && min < 59)) != true)
        {
            CInfomationDialog dlg("계측기로 보내는 시간 동기화 시간이나 분이 올바르지 않습니다.");
            dlg.exec();
            return;
        }
        QJsonObject jObject;
        bool enable = ui->chkSensorTimesync->isChecked();
        jObject.insert(QString("Enabled"),QJsonValue(enable));
        jObject.insert(QString("Hour"),QJsonValue(hour));
        jObject.insert(QString("Min"),QJsonValue(min));
        g_pSrSocket->sendCommand(this, "Ext_Tms_SetTimeSyncConfig", GraphicId, jObject);
    }
}

void TmsSetup::on_upload_clicked()
{
    if(SelectedRow < 0)
        return;
    int check = 0;
    if(ui->upload->isChecked())
        check = 1;
    TMS_ITEM_TAB *tmsitem = TmsItemList[SelectedRow];
    tmsitem->Upload = check;
    SetItem(QString::number(tmsitem->Upload), SelectedRow, 8);

    bool bToc = false;
    foreach (const TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if((tmsitem->Code == "TOC00" || tmsitem->Code == "TOC10") && tmsitem->Upload == 1)
        {
            m_bUseToc = true;
            bToc = true;
            break;
        }
    }
    int row = 0;
    foreach (TMS_ITEM_TAB * tmsitem, TmsItemList)
    {
        if(bToc && tmsitem->Code == "COD00" && tmsitem->Upload == 1)
        {
            tmsitem->Upload = 0;            // TOC가 있으면 TOC만 upload
            SetItem(QString::number(tmsitem->Upload), row, 8);
            if(SelectedRow == row)
                ui->upload->setChecked(false);
            break;
        }
        row++;
    }

}
