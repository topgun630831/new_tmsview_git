#include "sewerdialog2.h"
#include "ui_sewerdialog2.h"

#include "calendardlg.h"
#include "srsocket.h"
#include "infomationdialog.h"
#include <QDebug>

extern CSrSocket *g_pSrSocket;
int SewerColumnWidth[7] = {
    60,     //No
    200,    //일시
    130,    //유량
    130,    //수위
    130,    //유속
    130,    //적산
    180,    //상태표시
};

CSewerDialog2::CSewerDialog2(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CSewerDialog2)
{
    ui->setupUi(this);
    ui->tableDump->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    for(int c = 0; c < 7; c++)
        ui->tableDump->horizontalHeader()->resizeSection(c, SewerColumnWidth[c]);

    mFromDate = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString str = QString("%1").arg(time.hour(), 2, 10, QChar('0'));
    ui->editHour->setText(str);
    ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));

    mPage = -1;
    mTotalPage = 0;
}

CSewerDialog2::~CSewerDialog2()
{
    delete ui;
    foreach (const DUMP_TAB * history, mDumpList)
        delete history;
    mDumpList.clear();
}

void CSewerDialog2::on_btnFromDate_clicked()
{
    CalendarDlg dlg( mFromDate, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mFromDate = dlg.getDate();
        ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));
    }
}

void CSewerDialog2::on_btnInq_clicked()
{
    ui->tableDump->clearContents();
    foreach (const DUMP_TAB * history, mDumpList)
        delete history;
    mDumpList.clear();

    mStartPoint = mPage = -1;
    mTotalPage = mTotalPoint = 0;
    PageDisplay();

    QString cmd = "Ext_Hasu_Cmd";
    SendCommand(cmd);
}

void CSewerDialog2::on_btnFirst_clicked()
{
    int fromHour =  ui->editHour->text().toInt();
    fromHour--;
    if(fromHour < 0 || fromHour > 23)
    {
        fromHour = 23;
        mFromDate = mFromDate.addDays(-1);
    }
    QString str = QString("%1").arg(fromHour, 2, 10, QChar('0'));
    ui->editHour->setText(str);
    str = mFromDate.toString(tr("yyyy-MM-dd"));
    ui->btnFromDate->setText(str);

    on_btnInq_clicked();
}

void CSewerDialog2::on_btnPre_clicked()
{
    if(mPage > 0)
    {
        mPage--;
        mStartPoint = mPage * SEWER_MAX_ROW;
        DataDisplay();
    }
}

void CSewerDialog2::on_btnNext_clicked()
{
    if(mPage != -1 && mPage != mTotalPage-1)
    {
        mPage++;
        mStartPoint = mPage * SEWER_MAX_ROW;
        DataDisplay();
    }
}

void CSewerDialog2::on_btnLast_clicked()
{
    int fromHour =  ui->editHour->text().toInt();
    fromHour++;
    if(fromHour < 0 || fromHour > 23)
    {
        fromHour = 0;
        mFromDate = mFromDate.addDays(1);
    }
    QString str = QString("%1").arg(fromHour, 2, 10, QChar('0'));
    ui->editHour->setText(str);
    ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));

    on_btnInq_clicked();
}

void CSewerDialog2::SendCommand(QString cmd)
{
    QJsonObject argObject;
    QString arg = "DUMP";
    int fromHour =  ui->editHour->text().toInt();
    if(fromHour < 0 || fromHour > 23)
    {
        CInfomationDialog dlg("잘못된 시간입니다.");
        dlg.exec();
        return;
    }
    arg += mFromDate.toString("yyyyMMdd");
    QString str = QString("%1").arg(fromHour,2,10,QChar('0'));
    arg += str;

    arg += mFromDate.toString("0000~yyyyMMdd");
    arg += QString("%1").arg(fromHour,2,10,QChar('0'));
    arg += "5959";
    QJsonValue argValue(arg);

    g_pSrSocket->sendCommand((QWidget*)this, cmd, QString(""), argValue);
}

const char *codeStr[4] =  {
    "FVOLUM",   //유량
    "FLEVEL",   //수위
    "FLRATE",   //유속
    "FTOTAL"   //적산
};

int CSewerDialog2::CodeIndex(QString code)
{
    for(int i = 0; i < 4; i++)
    {
        if(code == codeStr[i])
            return i;
    }
    return -1;
}

void CSewerDialog2::onRead(QString& cmd, QJsonObject& jobject)
{
    QString result;
    result = jobject["Result"].toString();

    int index=1;
    int count = 0;
    if(cmd == "Ext_Hasu_Cmd")
    {
        while(1)
        {
            QString str;
            str = result.mid(index+14, 2);
            int len = str.toInt();
            if(len == -2)   // 미래
                break;
            if( len > 0)
            {
                DUMP_TAB *item = new DUMP_TAB;
                index += 8;    //yyyymmdd
                str = result.mid(index, 6); // hhmmss
                str.insert(2,QChar(':'));
                str.insert(5,QChar(':'));
                item->Item[0] = str;
                index += (6+2);     //항목수 포함
                for(int i = 0; i < len; i++ )
                {
                    str = result.mid(index, 6);
                    index += 6;
                    int sensor = CodeIndex(str);
                    if(sensor != -1)
                    {
                        str = result.mid(index, 10);
                        index += 10;
                        item->Item[sensor+1] = str;
                        str = result.mid(index, 2);
                        index += 2;
                        if(str == "00")
                            str = "정상";
                        else
                        if(str == "01")
                            str = "통신불량";
                        item->Item[5] = str;
                    }
                    else
                        index += 12;
                }
                mDumpList.append(item);
                count++;
                if(index >= result.length())
                    break;
            }
            else
            {
                index += 16;    //yyyymmdd00
                if(index >= result.length())
                    break;
            }
        }
        if(count == 0)
        {
            CInfomationDialog dlg("자료가 없습니다.");
            dlg.exec();
            return;
        }
        PageInit(count);
        DataDisplay();
    }
    else
        return;
}

void CSewerDialog2::PageInit(int total)
{
    mTotalPoint = total;
    mStartPoint = mPage = 0;
    mTotalPage = mTotalPoint / SEWER_MAX_ROW;
    if(mTotalPoint % SEWER_MAX_ROW)
        mTotalPage++;
}

void CSewerDialog2::DataDisplay()
{
    QTableWidget *table;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    table = ui->tableDump;
    table->clearContents();
    for(int row = mStartPoint; row < mStartPoint + SEWER_MAX_ROW; row++, cnt++)
    {
        if(row >= mDumpList.size())
            break;
        str = QString("%1").arg(row+1);
        item = new QTableWidgetItem(str);
        table->setItem(cnt, 0, item);
        for(int col = 1; col < 7; col++)
        {
            item = new QTableWidgetItem(mDumpList[row]->Item[col-1]);
            table->setItem(cnt, col, item);
        }
    }
    PageDisplay();
}

void CSewerDialog2::PageDisplay()
{
    QString str = QString("Page(%1/%2)").arg(mPage+1).arg(mTotalPage);
    ui->labelPage->setText(str);
}
