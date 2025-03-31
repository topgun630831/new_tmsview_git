#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QDebug>
#include <QScrollBar>
#include "calendardlg.h"
#include "srsocket.h"
#include "tmsdata.h"
#include "ui_tmsdata.h"
#include "questiondialog.h"
#include "infomationdialog.h"
#include "tag.h"
#include "analogset.h"
extern QString gSoftwareModel;

extern QSqlDatabase gDb;
extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern QString GraphicId;
extern QMap<QString, CTag*> m_TagMap;
extern bool    g_bToc10;
extern bool m_bUseToc;
extern bool m_bGas;    //이송가스

int TmsColumnWidth[6][16] = {
    {
        80,    //순번
        120,    //시간
        80,    //종류
        140,    //T-N
        140,    //T-P
//        75,    //COD
        140,    //TOC
   //     75,    //BOD
        140,    //PH
        140,    //SS
        140,    //유량계
        140,    //채수기00
        140,    //채수기01
        100,    //채수기02
        90,    //채수기
        90,    //채수기
        100,    //채수기
        126,    //채수기
    },
    {
        60,    //순번
        120,    //시간
        60,    //종류
        150,    //T-N
        150,    //T-P
        150,    //COD
 //       75,    //BOD
        150,    //PH
        150,    //SS
        150,    //유량계
        130,    //채수기00
        130,    //채수기01
        100,    //채수기02
        90,    //채수기
        90,    //채수기
        100,    //채수기
        126,    //채수기
    },
    {
        100,    //순번
        160,    //시간
        60,    //종류
        220,    //값
        160,    //상태
        200,    //MSIG검출신호
        200,    //MTM1수온
        200,    //MTM2측정조온도
        370,    //MSAM희석비율
        236,    //AUXI외부입출력 8
    },
    {
        80,    //순번
        100,    //시간
        160,    //값
        130,    //상태
        110,    //MSIG검출신호
        110,    //MTM1수온
        110,    //MTM2측정조 온도
        150,    //MSAM희석비율
        110,    //ZERO영점 교정액 농도
        110,    //SPAN스팬 교정액 농도
        110,    //SLOP교정 기울기
        110,    //ICPT교정 절편
        110,    //FACT보정한 기울기
        110,    //OFST보정한 절편
        110,    //MAXR측정범위(한계치)
        186,    //AUXI외부입출력
    },
    {
        80,    //순번
        130,    //유량
        130,    //유량계1
        130,    //유량계2
        130,    //유량계3
        130,    //유량계4
        130,    //유량계5
    },
    {
        500,
        200,
        200,
        1006
    }
};


int TmsColumnMax[6] = { 16, 16, 10, 16, 7, 4 };
extern bool m_bUseToc;
QString InqCode;
bool bLoaded = false;

void fileCopy(const char* src, const char* dest, const char* filename, bool bMsg)
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
        CInfomationDialog dlg(QString("파일이 존재하지 않습니다(%1)").arg(srcF));
        dlg.exec();
        return;
    }
    QFile::remove(destF);
    bool disp = true;
    if(srcFile.copy(destF) == false)
    {
        //qDebug() << "src:" << srcF << " dest:" << destF;
        msg = QString("파일저장에 실패했습니다(%1)").arg(dest);
    }
    else
    {
        if(bMsg == false)
            disp = false;
        msg = QString("파일저장에 성공했습니다");
    }
    system("sync");
    if(disp)
    {
        CInfomationDialog dlg(msg);
        dlg.exec();
    }
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

void displayData(QString dir, const char* filter, QTableWidget *table)
{
    QDir directory = QDir(dir);
    QStringList files;
    QString fileName = filter;
    files = directory.entryList(QStringList(fileName),
                                QDir::Files | QDir::NoSymLinks);

    files = findFiles(directory, files, true);

    table->setRowCount(files.size());
    for (int i = 0; i < files.size(); ++i) {
        QFile file(directory.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(QString("%1 KB").arg(int((size + 1023) / 1024)));
        table->setItem(i, 0, fileNameItem);
        table->setItem(i, 1, sizeItem);
    }
   // table->resizeColumnsToContents();
}

const char *AllHourHeader[16] =  {
    "순번",
    "시간",
    "종류",
    "TON00",
    "TOP00",
    "TOC00",
    "PHY00",
    "SUS00",
    "유량",
    "채수온도",
    "SAM01",
    "SAM02",
    "DOR",
    "PWR",
    "FML",
    "FMR",
};

const char *AllHourHeader2[16] =  {
    "순번",
    "시간",
    "종류",
    "TON00",
    "TOP00",
    "TOC10",
    "PHY00",
    "SUS00",
    "유량",
    "채수온도",
    "SAM01",
    "SAM02",
    "DOR",
    "PWR",
    "FML",
    "FMR",
};


TmsData::TmsData(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TmsData)
{
    bLoaded = false;
    mbInitOk = false;
    ui->setupUi(this);
    mPageIndex = 1;
    ui->stackedWidget->setCurrentIndex(mPageIndex);

    m_watcher.addPath("/app/wwwroot/tmp/");
    connect(&m_watcher, SIGNAL(directoryChanged(const QString&)),
                     this, SLOT(handleFileChanged(const QString&)));
    buttonEnable();

    mTableWidget[0] = ui->tableAllHour;
    mTableWidget[1] = ui->tableAll5Min;
    mTableWidget[2] = ui->tableIndividual5Min;
    mTableWidget[3] = ui->tableIndividual10Sec;
    mTableWidget[4] = ui->tableFlow;
    mTableWidget[5] = ui->tableSampler;
    for(int i = 0; i < 6; i++)
        for(int c = 0; c < TmsColumnMax[i]; c++)
            mTableWidget[i]->horizontalHeader()->resizeSection(c, TmsColumnWidth[i][c]);
    for(int c = 0; c < TmsColumnMax[4]; c++)
        ui->tableFlow_2->horizontalHeader()->resizeSection(c, TmsColumnWidth[4][c]);

    QStringList listAllHour;

    if(g_bToc10)
    {
        for(int i = 0; i < 16; i++)
            listAllHour.append(AllHourHeader2[i]);
    }
    else
    {
//        for(int i = 0; i < 16; i++)
//            listAllHour.append(AllHourHeader[i]);
    }
    if(m_bUseToc == true)
    {
        ui->tableAllHour->setHorizontalHeaderLabels(listAllHour);
        ui->tableAll5Min->setHorizontalHeaderLabels(listAllHour);

        ui->comboBox_indi5Min->setItemText(2, "TOC");
        ui->comboBox_indi10Sec->setItemText(2, "TOC");
    }

    ui->table5Min->horizontalHeader()->resizeSection(0, 406);
    ui->table5Min->horizontalHeader()->resizeSection(1, 130);
    ui->table10Sec->horizontalHeader()->resizeSection(0, 406);
    ui->table10Sec->horizontalHeader()->resizeSection(1, 130);

    ui->tableAllHour->verticalHeader()->setVisible(false);
    ui->tableAll5Min->verticalHeader()->setVisible(false);
    ui->tableIndividual5Min->verticalHeader()->setVisible(false);
    ui->tableIndividual10Sec->verticalHeader()->setVisible(false);
    ui->tableFlow->verticalHeader()->setVisible(false);
    ui->tableSampler->verticalHeader()->setVisible(true);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();
    int hour = time.hour();
    if(time.minute() < 5)
    {
        if(hour == 0)
        {
            date = date.addDays(-1);
            hour = 23;
        }
        else
            hour--;
    }
    ui->btnUsb->setVisible(false);
    mNoData[0] = ui->labelNoDataAllHour;
    mNoData[1] = ui->labelNoDataAll;
    mNoData[2] = ui->labelNoData5Min;
    mNoData[3] = ui->labelNoData10Sec;
    mNoData[4] = ui->labelNoDataFlow;
    ui->labelNoDataSampler->setVisible(false);
    for(int i = 0; i < 5; i++)
    {
        mFromHour[i] = hour;
        mDate[i] = date;
        mPage[i] = -1;
        mTotalPage[i] = 0;
        mbFirst[i] = true;
        mNoData[i]->setVisible(false);
    }
    QString str = QString("%1").arg(hour, 2, 10, QChar('0'));
    ui->editHour_all5Min->setText(str);
    ui->editHour_indi5Min->setText(str);
    ui->editHour_indi10Sec->setText(str);
    ui->editHour_allHour->setText(str);
    ui->btnDate_all5Min->setText(mDate[0].toString(tr("yyyy-MM-dd")));
    ui->btnDate_indi5Min->setText(mDate[1].toString(tr("yyyy-MM-dd")));
    ui->btnDate_indi10Sec->setText(mDate[2].toString(tr("yyyy-MM-dd")));
    ui->btnDate_allHour->setText(mDate[3].toString(tr("yyyy-MM-dd")));
    ui->btnDate_Flow->setText(mDate[4].toString(tr("yyyy-MM-dd")));

    mResendFromHour = 0;
    mResendToHour = hour;
    mResendFromDate = mResendToDate = mDate[0];
    ui->btnResendFromDate->setText(mResendFromDate.toString(tr("yyyy-MM-dd")));
    ui->btnResendToDate->setText(mResendToDate.toString(tr("yyyy-MM-dd")));
    ui->editResendFromHour->setText("0");
    ui->editResendToHour->setText(str);

/*
    QTableWidget *table = ui->table5Min;
    QScrollBar *scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px;background:#114060;}");
    scroll  = table->horizontalScrollBar();
    scroll->setStyleSheet("QScrollBar:horizontal { height: 60px;background:#114060}");

    table = ui->table10Sec;
    scroll  = table->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 60px; background:#114060}");
    scroll  = table->horizontalScrollBar();
    scroll->setStyleSheet("QScrollBar:horizontal { height: 60px;background:#114060}");
*/    ui->editHour_allHour->setVisible(false);

    mbInitOk = true;

    m_nFlowNum = 0;
    gDb.setDatabaseName(tr("/data/project2.db"));
    if(gDb.open() == true)
    {
        QSqlQuery query("select Name From Tag Where Name LIKE 'FLOW%'");
        while(query.next())
        {
            QString name = query.value(0).toString();
            if(name.length() != 5)
                continue;
            m_nFlowNum++;
        }
        gDb.close();
    }
    QStringList horizontalHeaders;
    horizontalHeaders.append("시간");
    horizontalHeaders.append("유량");

    if(m_nFlowNum > 5)
        m_nFlowNum = 5;
    if(m_nFlowNum > 1)
    {
        for(int i = 1 ; i <= m_nFlowNum; i++)
        {
            QString flow = QString("유량계%1").arg(i);
            ui->comboBox_indi5Min->addItem(flow);
            ui->comboBox_indi10Sec->addItem(flow);
            horizontalHeaders.append(flow);
        }
    }
    ui->tableFlow->setHorizontalHeaderLabels(horizontalHeaders);
    ui->tableFlow_2->setHorizontalHeaderLabels(horizontalHeaders);
    int num = 14;
    if(m_nFlowNum > 1)
        num += m_nFlowNum;
    for(int i = 0; i < num; i++)
        ui->comboBox_indi5Min->model()->setData(ui->comboBox_indi5Min->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);
    for(int i = 0; i < num; i++)
        ui->comboBox_indi10Sec->model()->setData(ui->comboBox_indi10Sec->model()->index(i, 0), QSize(50, 50), Qt::SizeHintRole);

    QString cmd = "Ext_Tms_Dump";
    InqCode = "All";

    ui->comboBox_indi5Min->setStyleSheet("QComboBox QAbstractItemView::item { height: 50px; }");
    ui->comboBox_indi10Sec->setStyleSheet("QComboBox QAbstractItemView::item { height: 50px; }");
    SendCommand(cmd);
    ui->allData->setChecked(true);
    ui->allData->setDefault(true);
}

TmsData::~TmsData()
{
    delete ui;
    for(int i = 0; i < 4; i++)
        foreach (const TMS_DUMP_TAB * history, mDumpList[i])
            delete history;
}

void TmsData::on_allDataHour_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnUsb->setVisible(false);
    mPageIndex = 0;
    if(mbFirst[mPageIndex])
    {
        InqClicked(ui->tableAllHour);
        mbFirst[mPageIndex] = false;
    }
}

void TmsData::on_allData_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(1);
    ui->btnUsb->setVisible(false);
    mPageIndex = 1;
}

void TmsData::on_selectedData_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(2);
    ui->btnUsb->setVisible(false);
    mPageIndex = 2;
//    if(mbFirst[mPageIndex])
    {
        InqClicked(ui->tableIndividual5Min);
        mbFirst[mPageIndex] = false;
    }
}

void TmsData::on_selectedData10Sec_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(3);
    ui->btnUsb->setVisible(false);
    mPageIndex = 3;
  //  if(mbFirst[mPageIndex])
    {
        Inq10Sec();
        mbFirst[mPageIndex] = false;
    }
}

void TmsData::on_sampler_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(5);
    ui->btnUsb->setVisible(false);
    mPageIndex = 5;
    if(mbFirst[mPageIndex])
    {
        on_btnInqSampler_clicked();
        mbFirst[mPageIndex] = false;
    }
}

void TmsData::on_fivemin_clicked()
{
    if(bLoaded == false)
    {
        displayData(tr("/data/csv"), "S*", ui->table10Sec);
        displayData(tr("/data/csv"), "M*", ui->table5Min);
        bLoaded = true;
    }
    ui->stackedWidget->setCurrentIndex(6);
    QList<QTableWidgetItem *> list = ui->table5Min->selectedItems();
    for(int i = 0; i < list.length(); i++)
        list[i]->setSelected(false);
    list = ui->table10Sec->selectedItems();
    for(int i = 0; i < list.length(); i++)
        list[i]->setSelected(false);
    ui->btnUsb->setVisible(true);
}

void TmsData::on_manage_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(7);
    ui->btnUsb->setVisible(false);
}

void TmsData::handleFileChanged(const QString& )
{
    buttonEnable();
}

void TmsData::buttonEnable()
{
    if(QDir("/app/wwwroot/tmp/usbdisk").exists() == false)
        ui->btnUsb->setEnabled(false);
    else
        ui->btnUsb->setEnabled(true);
}

void TmsData::on_btnUsb_clicked()
{
    const char *tensec = "/data/csv";
    const char *fivemin = "/data/csv";
    const char *dir;
    QTableWidget *table;

    if(nFile == 1)
//    if(ui->stackedWidget->currentIndex() == 3)
    {
        table = ui->table10Sec;
        dir = tensec;
    }
    else
    if(nFile == 0)
//    if(ui->stackedWidget->currentIndex() == 4)
    {
        table = ui->table5Min;
        dir = fivemin;
    }
    else
    {
        //qDebug() << "nFile:" << nFile;
        return;
    }
    QList<QTableWidgetItem *> list = table->selectedItems();
    if(list.size() <= 0)
    {
        CInfomationDialog dlg("선택된 파일이 없습니다");
        dlg.exec();
        return;
    }
    for(int i = 0; i < list.size(); i++)
    {
        QString name = table->item(list[i]->row(), 0)->text();
        fileCopy(dir, "/app/wwwroot/tmp/usbdisk", name.toLocal8Bit().data(), false);
    }
    CInfomationDialog dlg("작업을 완료했습니다");
    dlg.exec();
}

void TmsData::DateClicked(QPushButton *btn, QTableWidget *pTable)
{
    CalendarDlg dlg( mDate[mPageIndex], this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mDate[mPageIndex] = dlg.getDate();
        btn->setText(mDate[mPageIndex].toString(tr("yyyy-MM-dd")));
        if(mPageIndex == 2)
        {
            mDate[3] = mDate[mPageIndex];
            ui->btnDate_indi10Sec->setText(mDate[mPageIndex].toString(tr("yyyy-MM-dd")));
        }
        else
        if(mPageIndex == 3)
        {
            mDate[2] = mDate[mPageIndex];
            ui->btnDate_indi5Min->setText(mDate[mPageIndex].toString(tr("yyyy-MM-dd")));
        }
        InqClicked(pTable);
    }
}

void TmsData::InqClicked(QTableWidget *pTable)
{
    mNoData[mPageIndex]->setVisible(false);
    pTable->clearContents();
    foreach (const TMS_DUMP_TAB * history, mDumpList[mPageIndex])
        delete history;
    mDumpList[mPageIndex].clear();

    mStartPoint[mPageIndex] = mPage[mPageIndex] = -1;
    mTotalPage[mPageIndex] = mTotalPoint[mPageIndex] = 0;
    if(mPageIndex != 4)
        PageDisplay();

    if(mPageIndex != 3)     // 10초자료가 아니면
    {
        QString cmd = "Ext_Tms_Dump";
        if(mPageIndex == 4)
        {
            for(int i = 0; i < 6; i++)
                FlowTotal[i] = 0;
            for(int i = 0; i < 24; i++)
            {
                TMS_DUMP_TAB *item = new TMS_DUMP_TAB;
                mDumpList[mPageIndex].append(item);
            }
        }
        SendCommand(cmd);
    }
}

void TmsData::FirstClicked(QTableWidget *pTable, QLineEdit *pEdit, QPushButton *btnDate)
{
    mFromHour[mPageIndex] =  pEdit->text().toInt();
    mFromHour[mPageIndex]--;
    if(mFromHour[mPageIndex] < 0 || mFromHour[mPageIndex] > 23)
    {
        mFromHour[mPageIndex] = 23;
        mDate[mPageIndex] = mDate[mPageIndex].addDays(-1);
    }
    QString str = QString("%1").arg(mFromHour[mPageIndex], 2, 10, QChar('0'));
    pEdit->setText(str);
    str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_all5Min->setText(str);

    InqClicked(pTable);
}

void TmsData::PreClicked()
{
    if(mPage[mPageIndex] > 0)
    {
        mPage[mPageIndex]--;
        mStartPoint[mPageIndex] = mPage[mPageIndex] * TMS_MAX_ROW;
        DataDisplay();
    }
}

void TmsData::NextClicked()
{
    if(mPage[mPageIndex] != -1 && mPage[mPageIndex] != mTotalPage[mPageIndex]-1)
    {
        mPage[mPageIndex]++;
        mStartPoint[mPageIndex] = mPage[mPageIndex] * TMS_MAX_ROW;
        DataDisplay();
    }
}

void TmsData::LastClicked(QTableWidget *pTable, QLineEdit *pEdit, QPushButton *btnDate)
{
    mFromHour[mPageIndex] =  pEdit->text().toInt();
    mFromHour[mPageIndex]++;
    if(mFromHour[mPageIndex] < 0 || mFromHour[mPageIndex] > 23)
    {
        mFromHour[mPageIndex] = 0;
        mDate[mPageIndex] = mDate[mPageIndex].addDays(1);
    }
    QString str = QString("%1").arg(mFromHour[mPageIndex], 2, 10, QChar('0'));
    pEdit->setText(str);
    ui->btnDate_all5Min->setText(mDate[mPageIndex].toString(tr("yyyy-MM-dd")));

    InqClicked(pTable);
}

const char *codeStrTms[18] =  {
    "TON00",   //T-N 0
    "TOP00",   //T-P 1
    "COD00",   //COD 2
//    "TOC00",   //TOC 2
 //   "BOD00",   //BOD 3
    "PHY00",   //PH  3
    "SUS00",   //SS  4
    "FLW01",   //유량계 5
    "SAM00",   //채수기 온도 6
    "SAM01",   //채수기 병위치 7
    "SAM02",   //채수기 도어 8
    "DORON",   //출입문  9
    "PWRON",   //채수기 10
    "FMLON",   //채수기 11
    "FMRON",   //채수기 12
    "FLOW1",   //유량계1 13
    "FLOW2",   //유량계2 14
    "FLOW3",   //유량계3
    "FLOW4",   //유량계4
    "FLOW5",   //유량계5
};

const char *codeStrTmsToc[18] =  {
    "TON00",   //T-N 0
    "TOP00",   //T-P 1
//    "COD00",   //COD 2
    "TOC00",   //TOC 2
 //   "BOD00",   //BOD 3
    "PHY00",   //PH  3
    "SUS00",   //SS  4
    "FLW01",   //유량계 5
    "SAM00",   //채수기 온도 6
    "SAM01",   //채수기 병위치 7
    "SAM02",   //채수기 도어 8
    "DORON",   //채수기 9
    "PWRON",   //채수기 10
    "FMLON",   //채수기 11
    "FMRON",   //채수기 12
    "FLOW1",   //유량계1 13
    "FLOW2",   //유량계2 14
    "FLOW3",   //유량계3
    "FLOW4",   //유량계4
    "FLOW5",   //유량계5
};


const char *codeStrTmsTocHighTemp[18] =  {
    "TON00",   //T-N 0
    "TOP00",   //T-P 1
//    "COD00",   //COD 2
    "TOC10",   //TOC 2
 //   "BOD00",   //BOD 3
    "PHY00",   //PH  3
    "SUS00",   //SS  4
    "FLW01",   //유량계 5
    "SAM00",   //채수기 온도 6
    "SAM01",   //채수기 병위치 7
    "SAM02",   //채수기 도어 8
    "DORON",   //채수기 9
    "PWRON",   //채수기 10
    "FMLON",   //채수기 11
    "FMRON",   //채수기 12
    "FLOW1",   //유량계1 13
    "FLOW2",   //유량계2 14
    "FLOW3",   //유량계3
    "FLOW4",   //유량계4
    "FLOW5",   //유량계5
};


const char *statusStrTms[7] =  {
    "정상",
    "일시정지",
    "유량없음",
    "교정중",
    "점검중",
    "통신불량",
    "동작불량",
};

void TmsData::SendCommand(QString cmd)
{
    if(cmd == "Ext_Tms_Dump")
    {
        QJsonObject argObject;
        QString strDate;
        QString kind = "A";

        QDate toDate = mDate[mPageIndex];
        if(mPageIndex == 0 || mPageIndex == 4)
        {
            toDate = toDate.addDays(1);
            strDate = QString("%1 00:00:00").arg(mDate[mPageIndex].toString("yyyy-MM-dd"));
            argObject.insert(QString("StartTime"),strDate);
            strDate = QString("%1 00:00:00").arg(toDate.toString("yyyy-MM-dd"));
            argObject.insert(QString("EndTime"),QJsonValue(strDate));
            kind = "H";
        }
        else
        {
            int toHour = mFromHour[mPageIndex]+1;
            if(toHour >= 24)
            {
                toHour = 0;
                toDate = toDate.addDays(1);
            }
            strDate = QString("%1 %2:00:00").arg(mDate[mPageIndex].toString("yyyy-MM-dd")).arg(mFromHour[mPageIndex],2,10,QChar('0'));
            argObject.insert(QString("StartTime"),strDate);
            strDate = QString("%1 %2:00:00").arg(toDate.toString("yyyy-MM-dd")).arg(toHour,2,10,QChar('0'));
            argObject.insert(QString("EndTime"),QJsonValue(strDate));
        }
        argObject.insert(QString("Kind"),QJsonValue(kind));
//        QString code = "All";
        InqCode = "All";
        if(mPageIndex == 2)
        {
            m_nIndex = ui->comboBox_indi5Min->currentIndex();
            if(m_bUseToc == true)
            {
                if(g_bToc10 == true)
                    InqCode = codeStrTmsTocHighTemp[ui->comboBox_indi5Min->currentIndex()];
                else
                    InqCode = codeStrTmsToc[ui->comboBox_indi5Min->currentIndex()];
            }
            else
                InqCode = codeStrTms[ui->comboBox_indi5Min->currentIndex()];
        }
        else
        if(mPageIndex == 3)
        {
            m_nIndex = ui->comboBox_indi10Sec->currentIndex();
            if(m_bUseToc == true)
            {
                if(g_bToc10 == true)
                   InqCode = codeStrTmsTocHighTemp[ui->comboBox_indi10Sec->currentIndex()];
                else
                   InqCode = codeStrTmsToc[ui->comboBox_indi10Sec->currentIndex()];
            }
            else
               InqCode = codeStrTms[ui->comboBox_indi10Sec->currentIndex()];
        }
        argObject.insert(QString("Code"),QJsonValue(InqCode));
        QJsonValue argValue(argObject);
        g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
    }
    else
    if(cmd == "Ext_Tms_SampDump")
    {
        QJsonValue argValue(TMS_MAX_ROW);//  argValue(QJsonObject());
        g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
    }
    else
    if(cmd == "Ext_Tms_ResendToIns")
    {
        QJsonObject argObject;
        argObject.insert(QString("StartTime"),QJsonValue(QString("%1 %2:00:00")
                                                         .arg(mResendFromDate.toString("yyyy-MM-dd"))
                                                         .arg(mResendFromHour,2,10,QChar('0'))));
        argObject.insert(QString("EndTime"),QJsonValue(QString("%1 %2:00:00")
                                                       .arg(mResendToDate.toString("yyyy-MM-dd"))
                                                       .arg(mResendToHour,2,10,QChar('0'))));
        QJsonValue argValue(argObject);
        g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
    }
}

int TmsData::CodeIndex(QString code)
{
    for(int i = 0; i < 18; i++)
    {
        if(m_bUseToc == true)
        {
            if(g_bToc10 == true)
            {
                if(code == codeStrTmsTocHighTemp[i])
                    return i;
            }
            else
            {
                if(code == codeStrTmsToc[i])
                    return i;
            }
        }
        else
        {
            if(code == codeStrTms[i])
                return i;
        }
    }
    return -1;
}

void TmsData::onRead(QString& cmd, QJsonObject& jobject)
{
    QJsonArray array = jobject["Result"].toArray();

    int count = 0;
    if(array.size() == 0)
    {
        mNoData[mPageIndex]->setVisible(true);
//        CInfomationDialog dlg("자료가 습니다.");
//        dlg.exec();
        return;
    }
    if(g_DebugDisplay)
        qDebug() << "cmd:" << cmd << "  len:" << array.size();
    if(cmd == "Ext_Tms_Dump")
    {
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            if(mPageIndex == 0 || mPageIndex == 1)
            {
                QString time = itemObject["Date"].toString();
                time = time.mid(11, 9);
                QString code = itemObject["Code"].toString();
                QString kind = itemObject["Kind"].toString();
                QString value;// = QString("%1(%2)").arg(itemObject["Value"].toString())
                              //.arg((int)itemObject["Status"].toDouble());
                int pos = CodeIndex(code);
                if(pos > 12)
                    continue;
                int status = (int)itemObject["Status"].toDouble();
//                if(pos < 9 && status != 0)
                if(pos >= 8)
                    value = QString("%1").arg(status);
                else
                {
                    if(status != 0)
                        value = QString("%1(%2)").arg(itemObject["Value"].toString())
                                .arg(status);
                    else
                        value = QString("%1").arg(itemObject["Value"].toString());
                }
                if(pos == -1)
                {
                    qDebug() << "Code Error:" << code;
                    continue;
                }
                int index;
                int col;
                if((index = FindData(time, kind)) == -1)
                {
                    col = 2+pos;
                    TMS_DUMP_TAB *item = new TMS_DUMP_TAB;
                    item->Item[0] = time;
                    item->Item[1] = kind;

                    item->Item[col] = value;
                    mDumpList[mPageIndex].append(item);
                    count++;
                }
                else
                {
                    col = 2+pos;
                    mDumpList[mPageIndex][index]->Item[col] = value;
                }
            }
            else
            if(mPageIndex == 4)
            {
                QString time = itemObject["Date"].toString();
                time = time.mid(11, 2);
                int hour = time.toInt();
                QString code = itemObject["Code"].toString();
                int col;
                if(code == "FLW01")
                    col = 0;
                else
                if(code == "FLOW1")
                    col = 1;
                else
                if(code == "FLOW2")
                    col = 2;
                else
                if(code == "FLOW3")
                    col = 3;
                else
                if(code == "FLOW4")
                    col = 4;
                else
                if(code == "FLOW5")
                    col = 5;
                else
                    continue;

                int nValue = itemObject["Value"].toString().toInt();
                QString value;// = QString("%1(%2)").arg(itemObject["Value"].toString())
                              //.arg((int)itemObject["Status"].toDouble());
                int status = (int)itemObject["Status"].toDouble();
                if(status != 0)
                    value = QString("%1(%2)").arg(itemObject["Value"].toString())
                            .arg(status);
                else
                    value = QString("%1").arg(itemObject["Value"].toString());
                mDumpList[mPageIndex][hour]->Item[col] = value;
                FlowTotal[col] += nValue;
            }
            else
            {
                TMS_DUMP_TAB *item = new TMS_DUMP_TAB;
                QString time = itemObject["Date"].toString();
                time = time.mid(11, 9);
                item->Item[0] = time;
                item->Item[1] = itemObject["Kind"].toString();
                item->Item[2] = itemObject["Value"].toString();
                if(m_nIndex >= 8 && m_nIndex < 14)
                    item->Item[3] = QString("%1").arg(itemObject["Status"].toDouble());
                else
                    item->Item[3] = statusStrTms[(int)itemObject["Status"].toDouble()];
                item->Item[4] = itemObject["MSIG"].toString();
                QString mtm1 = itemObject["MTM1"].toString();
//                if((InqCode == "TOC00" || InqCode =="TOC10") && m_bUseToc == true && m_bGas == true)    //이송가스
//                if(bMtm1Null == false)
//                    item->Item[5] = "X";
//                else
                    item->Item[5] = mtm1;
                if(InqCode == "TOC00" || InqCode =="TOC10")
                    item->Item[5] = "X";
                item->Item[6] = itemObject["MTM2"].toString();
                QString msam = itemObject["MSAM"].toString();
//                if((InqCode == "TOC00" || InqCode =="TOC10") && m_bUseToc == true && m_bGas == true)    //이송가스
                if((InqCode == "TOC00" || InqCode =="TOC10") && mtm1 != "X")    //이송가스
                    item->Item[7] = msam + "," + mtm1;
                else
                    item->Item[7] = msam;
                item->Item[8] = itemObject["AUXI"].toString();
                mDumpList[mPageIndex].append(item);
                count++;
            }
        }
        PageInit(count);
        DataDisplay();
    }
    else
    if(cmd == "Ext_Tms_SampDump")
    {
        QTableWidgetItem *item;
        ui->tableSampler->setRowCount(0);
        ui->tableSampler->setRowCount(array.size());
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            item = new QTableWidgetItem(itemObject["Date"].toString());
            ui->tableSampler->setItem(count, 0, item);
            bool remote = itemObject["Remoted"].toBool();
            QString str;
            if(remote == true)
                str = "원격";
            else
                str = "현장";
            item = new QTableWidgetItem(str);
            ui->tableSampler->setItem(count, 1, item);
            item = new QTableWidgetItem(QString("%1").arg(itemObject["BottlePos"].toDouble()));
            ui->tableSampler->setItem(count, 2, item);
            count++;
        }
    }
    else
        return;
}

int TmsData::FindData(QString time, QString kind)
{
    TMS_DUMP_TAB *p;
    for(int i = 0; i < mDumpList[mPageIndex].size(); i++)
    {
        p = mDumpList[mPageIndex][i];
        if(p->Item[0] == time && p->Item[1] == kind)
            return i;
    }
    return -1;
}

void TmsData::PageInit(int total)
{
    if(mPageIndex == 4)
        return;
    mTotalPoint[mPageIndex] = total;
    mStartPoint[mPageIndex] = mPage[mPageIndex] = 0;
    mTotalPage[mPageIndex] = mTotalPoint[mPageIndex] / TMS_MAX_ROW;
    if(mTotalPoint[mPageIndex] % TMS_MAX_ROW)
        mTotalPage[mPageIndex]++;
}

void TmsData::DataDisplay()
{
    QTableWidget *table;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    if(mPageIndex == 4)
    {
        ui->tableFlow->clearContents();
        ui->tableFlow_2->clearContents();
        int total[6];
        int flag;
        flag = Qt::AlignHCenter | Qt::AlignVCenter;
        str = QString("합계");
        item = new QTableWidgetItem(str);
        ui->tableFlow_2->setItem(12, 0, item);
        for(int i = 0; i < 12; i++)
        {
            str = QString("%1").arg(i);
            item = new QTableWidgetItem(str);
            ui->tableFlow->setItem(i, 0, item);
            str = QString("%1").arg(i+12);
            item = new QTableWidgetItem(str);
            ui->tableFlow_2->setItem(i, 0, item);
            for(int c = 0; c <= m_nFlowNum; c++)
            {
                if(mDumpList[mPageIndex][i]->Item[c] != "")
                {
                    item = new QTableWidgetItem(mDumpList[mPageIndex][i]->Item[c]);
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    item->setTextAlignment(flag);
                    ui->tableFlow->setItem(i, c+1, item);
                    total[c] += mDumpList[mPageIndex][i]->Item[c].toInt();
                }
                if(mDumpList[mPageIndex][i+12]->Item[c] != "")
                {
                    item = new QTableWidgetItem(mDumpList[mPageIndex][i+12]->Item[c]);
                    item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                    item->setTextAlignment(flag);
                    ui->tableFlow_2->setItem(i, c+1, item);
                    total[c] += mDumpList[mPageIndex][i+12]->Item[c].toInt();
                }
            }
            for(int c = 0; c <= m_nFlowNum; c++)
            {
                str = QString("%1").arg(FlowTotal[c]);
                item = new QTableWidgetItem(str);
                item->setTextAlignment(flag);
                ui->tableFlow_2->setItem(12, c+1, item);
            }
        }
        return;
    }
    if(mPageIndex == 0)
        table = ui->tableAllHour;
    else
    if(mPageIndex == 1)
        table = ui->tableAll5Min;
    else
    if(mPageIndex == 2)
        table = ui->tableIndividual5Min;
    else
    if(mPageIndex == 3)
        table = ui->tableIndividual10Sec;
    table->clearContents();
    for(int row = mStartPoint[mPageIndex]; row < mStartPoint[mPageIndex] + TMS_MAX_ROW; row++, cnt++)
    {
        if(row >= mDumpList[mPageIndex].size())
            break;
        str = QString("%1").arg(row+1);
        item = new QTableWidgetItem(str);
        table->setItem(cnt, 0, item);
        int max = 10;
        if(mPageIndex == 0 || mPageIndex == 1 )
            max = 17;
        else
        if(mPageIndex == 3)
            max = 16;
        for(int col = 1; col < max; col++)
        {
            item = new QTableWidgetItem(mDumpList[mPageIndex][row]->Item[col-1]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            int flag;
//            if(col >= 2 && col <= 7)
//               flag = Qt::AlignRight | Qt::AlignVCenter;
//            else
                flag = Qt::AlignHCenter | Qt::AlignVCenter;
            item->setTextAlignment(flag);
            table->setItem(cnt, col, item);
        }
    }
    PageDisplay();
}

void TmsData::PageDisplay()
{
    QString str = QString("Page(%1/%2)").arg(mPage[mPageIndex]+1).arg(mTotalPage[mPageIndex]);
    if(mPageIndex == 0)
        ui->labelPage_4->setText(str);
    else
    if(mPageIndex == 1)
        ui->labelPage_all5Min->setText(str);
    else
    if(mPageIndex == 2)
        ui->labelPage_indi5Min->setText(str);
    else
        ui->labelPage_indi10Sec->setText(str);
}

void TmsData::on_btnDate_allHour_clicked()
{
    DateClicked(ui->btnDate_allHour, ui->tableAllHour);
}
/*
void TmsData::on_btnInq_4_clicked()
{
    InqClicked(ui->tableAllHour, ui->editHour_allHour);
}
*/
void TmsData::on_btnFirst_allHour_clicked()
{
    mDate[mPageIndex] = mDate[mPageIndex].addDays(-1);
    QString str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_allHour->setText(str);

    InqClicked(ui->tableAllHour);
}

void TmsData::on_btnPre_allHour_clicked()
{
    PreClicked();
}

void TmsData::on_btnNext_allHour_clicked()
{
    NextClicked();
}

void TmsData::on_btnLast_allHour_clicked()
{
    mDate[mPageIndex] = mDate[mPageIndex].addDays(1);
    QString str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_allHour->setText(str);

    InqClicked(ui->tableAllHour);
}

void TmsData::on_btnDate_all5Min_clicked()
{
    DateClicked(ui->btnDate_all5Min, ui->tableAll5Min);
}
/*
void TmsData::on_btnInq_clicked()
{
    InqClicked(ui->tableAll5Min, ui->editHour_all5Min);
}
*/
void TmsData::on_btnFirst_all5Min_clicked()
{
    FirstClicked(ui->tableAll5Min, ui->editHour_all5Min, ui->btnDate_all5Min);
}

void TmsData::on_btnPre_all5Min_clicked()
{
    PreClicked();
}

void TmsData::on_btnNext_all5Min_clicked()
{
    NextClicked();
}

void TmsData::on_btnLast_all5Min_clicked()
{
    LastClicked(ui->tableAll5Min, ui->editHour_all5Min, ui->btnDate_all5Min);
}

void TmsData::on_btnDate_indi5Min_clicked()
{
    DateClicked(ui->btnDate_indi5Min, ui->tableIndividual5Min);
}

void TmsData::on_btnInq10Sec_clicked()
{
    Inq10Sec();
}

void TmsData::on_btnFirst_indi5Min_clicked()
{
    FirstClicked(ui->tableIndividual5Min, ui->editHour_indi5Min, ui->btnDate_indi5Min);
}

void TmsData::on_btnPre_indi5Min_clicked()
{
    PreClicked();
}

void TmsData::on_btnNext_indi5Min_clicked()
{
    NextClicked();
}

void TmsData::on_btnLast_indi5Min_clicked()
{
    LastClicked(ui->tableIndividual5Min, ui->editHour_indi5Min, ui->btnDate_indi5Min);
}

void TmsData::on_btnInqSampler_clicked()
{
    ui->labelNoDataSampler->setVisible(false);
    QString cmd = "Ext_Tms_SampDump";
    SendCommand(cmd);
}

void TmsData::on_btnDate_indi10Sec_clicked()
{
    DateClicked(ui->btnDate_indi10Sec, ui->tableIndividual10Sec);
}

void TmsData::Inq10Sec()
{
    InqClicked(ui->tableIndividual10Sec);
    gDb.setDatabaseName(tr("/data/tmssec.db"));
    if(gDb.open() == true)
    {
        int count = 0;
        QDateTime datetime(mDate[mPageIndex], QTime(mFromHour[mPageIndex], 0));
        uint time = datetime.toTime_t();
        int index = ui->comboBox_indi10Sec->currentIndex();
        QString code;

        if(m_bUseToc == true)
        {
            if(g_bToc10 == true)
                code = codeStrTmsTocHighTemp[index];
            else
                code = codeStrTmsToc[index];
        }
        else
            code = codeStrTms[index];
        QString str;
        if(code == "TOC00" || code == "TOC10")
        {
            str = QString("SELECT Date, Value, Status, msig, mtm1, mtm2, msam, slop, icpt,"
                                    "zero, span, fact, ofst, maxr, auxi from TmsSec"
                                        " Where Code Like 'TOC%' AND (Date >= %1 AND Date < %2)").arg(time).arg(time+3600);
        }
        else
        {
            str = QString("SELECT Date, Value, Status, msig, mtm1, mtm2, msam, slop, icpt,"
                        "zero, span, fact, ofst, maxr, auxi from TmsSec"
                            " Where Code = '%1' AND (Date >= %2 AND Date < %3)").arg(code).arg(time).arg(time+3600);
        }
        qDebug() << "TMSSEC : " << str;
        QSqlQuery query(str);
        while (query.next())
        {
            TMS_DUMP_TAB *item = new TMS_DUMP_TAB;
            QDateTime time = QDateTime::fromTime_t(query.value(0).toUInt());
            item->Item[0] = time.toString("mm:ss");
            item->Item[1] = query.value(1).toString();
//            if(index > 8 && index < 14)
             if(index >= 8)
                item->Item[2] = query.value(2).toString();
            else
                item->Item[2] = statusStrTms[query.value(2).toUInt()];
            QVariant var;
            QString mtm1;
            bool bMtm1Null = true;
            for(int i = 3; i < 14; i++)
            {
                var = query.value(i);
                if(var.isNull())
                {
                   item->Item[i] = "X";
                }
                else
                {
//                    if((code == "TOC00" || code == "TOC10") && i == 4 && m_bUseToc == true && m_bGas == true)    //이송가스
                    if((code == "TOC00" || code == "TOC10") && i == 4)    //이송가스
                    {
                        item->Item[i] = "X";
                        mtm1 = QString("%1").arg(var.toFloat());
                        bMtm1Null = false;
                    }
                    else
                        item->Item[i] = QString("%1").arg(var.toFloat());
                }
            }
//            if((code == "TOC00" || code == "TOC10") && m_bUseToc == true && m_bGas == true)    //이송가스
            if((code == "TOC00" || code == "TOC10") && bMtm1Null == false)    //이송가스
            {
                item->Item[6] = item->Item[6] + "," + mtm1;
            }
            var = query.value(14);
            if(var.isNull())
                item->Item[14] = "X";
            else
                item->Item[14] = var.toString();
            mDumpList[mPageIndex].append(item);
            count++;
        }
        if(count)
        {
            PageInit(count);
            DataDisplay();
        }
        else
        {
            mNoData[3]->setVisible(true);
            return;
        }

        gDb.close();
    }
}

void TmsData::on_btnFirst_indi10Sec_clicked()
{
    mFromHour[mPageIndex] =  ui->editHour_indi10Sec->text().toInt();
    mFromHour[mPageIndex]--;
    if(mFromHour[mPageIndex] < 0 || mFromHour[mPageIndex] > 23)
    {
        mFromHour[mPageIndex] = 23;
        mDate[mPageIndex] = mDate[mPageIndex].addDays(-1);
    }
    QString str = QString("%1").arg(mFromHour[mPageIndex], 2, 10, QChar('0'));
    ui->editHour_indi10Sec->setText(str);
    str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_indi10Sec->setText(str);

    Inq10Sec();
}

void TmsData::on_btnPre_indi10Sec_clicked()
{
    PreClicked();
}

void TmsData::on_btnNext_indi10Sec_clicked()
{
    NextClicked();
}

void TmsData::on_btnLast_indi10Sec_clicked()
{
    mFromHour[mPageIndex] =  ui->editHour_indi10Sec->text().toInt();
    mFromHour[mPageIndex]++;
    if(mFromHour[mPageIndex] < 0 || mFromHour[mPageIndex] > 23)
    {
        mFromHour[mPageIndex] = 0;
        mDate[mPageIndex] = mDate[mPageIndex].addDays(1);
    }
    QString str = QString("%1").arg(mFromHour[mPageIndex], 2, 10, QChar('0'));
    ui->editHour_indi10Sec->setText(str);
    ui->btnDate_indi10Sec->setText(mDate[mPageIndex].toString(tr("yyyy-MM-dd")));

    Inq10Sec();
}

void TmsData::on_editHour_all5Min_textChanged(const QString &arg1)
{
    if(mbInitOk == false)
        return;
    mFromHour[mPageIndex] = arg1.toInt();
    InqClicked(ui->tableAll5Min);
}

void TmsData::on_editHour_indi5Min_textChanged(const QString &arg1)
{
    if(mbInitOk == false)
        return;
    ui->editHour_indi10Sec->setText(arg1);
    mFromHour[3] = mFromHour[mPageIndex] = arg1.toInt();
    InqClicked(ui->tableIndividual5Min);
}

void TmsData::on_editHour_indi10Sec_textChanged(const QString &arg1)
{
    if(mbInitOk == false)
        return;
    ui->editHour_indi5Min->setText(arg1);
    mFromHour[2] = mFromHour[mPageIndex] = arg1.toInt();
    Inq10Sec();
}

void TmsData::on_comboBox_indi5Min_currentIndexChanged(const QString &arg1)
{
    if(mbInitOk == false)
        return;
    ui->comboBox_indi5Min->setCurrentText(arg1);
    InqClicked(ui->tableIndividual5Min);
}

void TmsData::on_comboBox_indi10Sec_currentIndexChanged(const QString &arg1)
{
    if(mbInitOk == false)
        return;
    ui->comboBox_indi10Sec->setCurrentText(arg1);
    Inq10Sec();
}

void TmsData::on_table5Min_clicked(const QModelIndex &)
{
//    nFile = 0;
//    QList<QTableWidgetItem *> list = ui->table10Sec->selectedItems();
//    for(int i = 0; i < list.length(); i++)
//        list[i]->setSelected(false);
}

void TmsData::on_table10Sec_clicked(const QModelIndex &)
{
//    nFile = 1;
//    ui->stackedWidget->setCurrentIndex(5);
//    QList<QTableWidgetItem *> list = ui->table5Min->selectedItems();
//    for(int i = 0; i < list.length(); i++)
//        list[i]->setSelected(false);
}

void TmsData::on_btnResend_clicked()
{
    CQuestionDialog dlg("INS 자료를 재전송 하시겠습니까?");
    if(dlg.exec() == QDialog::Accepted)
    {
        mResendFromHour = ui->editResendFromHour->text().toInt();
        mResendToHour = ui->editResendToHour->text().toInt();
        QDateTime From(mResendFromDate);
        QTime time;
        time.setHMS(mResendFromHour,0,0);
        From.setTime(time);
        QDateTime To(mResendToDate);
        time.setHMS(mResendToHour,0,0);
        To.setTime(time);
        // qDebug() << "from:" << From.toTime_t() <<  "   to:" << To.toTime_t();
        if(To.toTime_t()-From.toTime_t() > 24*60*60)
        {
            CInfomationDialog dlg("기간은 24시간이내 이어야 합니다");
            dlg.exec();
        }
        else
        if(To.toTime_t() <= From.toTime_t())
        {
            CInfomationDialog dlg("시작시간과 끝 시간이 올바르지 않습니다");
            dlg.exec();
        }
        else
        {
            QString cmd = "Ext_Tms_ResendToIns";
            SendCommand(cmd);
        }
    }
}

void TmsData::on_btnDaySet_clicked()
{
    QString msg = "일 적산 유량";
    CTag *pTag = m_TagMap["FLOW_DAYTOTAL"];
    CAnalogSet Dlg(msg, pTag, this);
    if(Dlg.exec() == QDialog::Accepted)
        pTag->writeValue(Dlg.m_sValue);
}

void TmsData::on_btnResendFromDate_clicked()
{
    CalendarDlg dlg( mResendFromDate, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        mResendFromDate = dlg.getDate();
        ui->btnResendFromDate->setText(mResendFromDate.toString(tr("yyyy-MM-dd")));
    }
}

void TmsData::on_btnResendToDate_clicked()
{
    CalendarDlg dlg( mResendToDate, this);
    if(dlg.exec() == QDialog::Accepted)
    {
        mResendToDate = dlg.getDate();
        ui->btnResendToDate->setText(mResendToDate.toString(tr("yyyy-MM-dd")));
    }
}

void TmsData::on_table5Min_pressed(const QModelIndex &index)
{
    nFile = 0;
    QList<QTableWidgetItem *> list = ui->table10Sec->selectedItems();
    for(int i = 0; i < list.length(); i++)
        list[i]->setSelected(false);
}

void TmsData::on_table10Sec_pressed(const QModelIndex &index)
{
    nFile = 1;
    ui->stackedWidget->setCurrentIndex(5);
    QList<QTableWidgetItem *> list = ui->table5Min->selectedItems();
    for(int i = 0; i < list.length(); i++)
        list[i]->setSelected(false);
}

void TmsData::on_btnDate_Flow_clicked()
{
    DateClicked(ui->btnDate_Flow, ui->tableFlow);
}

void TmsData::on_btnFirst_Flow_clicked()
{
    mDate[mPageIndex] = mDate[mPageIndex].addDays(-1);
    QString str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_Flow->setText(str);

    InqClicked(ui->tableFlow);
}

void TmsData::on_btnLast_Flow_clicked()
{
    mDate[mPageIndex] = mDate[mPageIndex].addDays(1);
    QString str = mDate[mPageIndex].toString(tr("yyyy-MM-dd"));
    ui->btnDate_Flow->setText(str);

    InqClicked(ui->tableFlow);
}

void TmsData::on_flowData_clicked()
{
    nFile = -1;
    ui->stackedWidget->setCurrentIndex(4);
    ui->btnUsb->setVisible(false);
    mPageIndex = 4;
    if(mbFirst[mPageIndex])
    {
        InqClicked(ui->tableFlow);
        mbFirst[mPageIndex] = false;
    }
}
