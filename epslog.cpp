#include "epslog.h"
#include "ui_epslog.h"

#include "calendardlg.h"
#include "srsocket.h"
#include "infomationdialog.h"
#include <QDebug>

extern CSrSocket *g_pSrSocket;
extern QString GraphicId;
int EpsColumnWidth[10] = {
    50,     //No
    80,    //일시
    120,    //유량
    120,    //수위
    120,    //유속
    120,    //적산
    120,    //보조수위
    120,    //전도도
    120,    //강우설량계
//    80,    //상태표시
};

CEpsLog::CEpsLog(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CEpsLog)
{
    ui->setupUi(this);
    ui->tableDump->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    for(int c = 0; c < 9; c++)
        ui->tableDump->horizontalHeader()->resizeSection(c, EpsColumnWidth[c]);

    mFromDate = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString str = QString("%1").arg(time.hour(), 2, 10, QChar('0'));
    ui->editHour->setText(str);
    ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));

    mPage = -1;
    mTotalPage = 0;
}

CEpsLog::~CEpsLog()
{
    delete ui;
    foreach (const EPS_DUMP_TAB * history, mDumpList)
        delete history;
    mDumpList.clear();
}

void CEpsLog::on_btnFromDate_clicked()
{
    CalendarDlg dlg( mFromDate, this);

    if(dlg.exec() == QDialog::Accepted)
    {
        mFromDate = dlg.getDate();
        ui->btnFromDate->setText(mFromDate.toString(tr("yyyy-MM-dd")));
    }
}

void CEpsLog::on_btnInq_clicked()
{
    ui->tableDump->clearContents();
    foreach (const EPS_DUMP_TAB * history, mDumpList)
        delete history;
    mDumpList.clear();

    mStartPoint = mPage = -1;
    mTotalPage = mTotalPoint = 0;
    PageDisplay();

    QString cmd = "Ext_Eps_Cmd";
    SendCommand(cmd);
}

void CEpsLog::on_btnFirst_clicked()
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

void CEpsLog::on_btnPre_clicked()
{
    if(mPage > 0)
    {
        mPage--;
        mStartPoint = mPage * EPS_MAX_ROW;
        DataDisplay();
    }
}

void CEpsLog::on_btnNext_clicked()
{
    if(mPage != -1 && mPage != mTotalPage-1)
    {
        mPage++;
        mStartPoint = mPage * EPS_MAX_ROW;
        DataDisplay();
    }
}

void CEpsLog::on_btnLast_clicked()
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

void CEpsLog::SendCommand(QString cmd)
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

    arg += mFromDate.toString("0000yyyyMMdd");
    arg += QString("%1").arg(fromHour,2,10,QChar('0'));
    arg += "5959";
    QJsonValue argValue(arg);

    g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
}

const char *codeStrEps[7] =  {
    "FVOLUD",   //유량
    "FLEVED",   //수위
    "FLRATD",   //유속
    "FTOTAD",   //적산
    "FSUBLD",   //보조수위
    "CONRAD",   //전도도
    "RAIN1D",   //강우설량계
};

int CEpsLog::CodeIndex(QString code)
{
    for(int i = 0; i < 7; i++)
    {
        if(code == codeStrEps[i])
            return i;
    }
    return -1;
}

void CEpsLog::onRead(QString& cmd, QJsonObject& jobject)
{
    QJsonArray array = jobject["Result"].toArray();

    int count = 0;
    if(cmd == "Ext_Eps_Cmd")
    {
        foreach (const QJsonValue& value, array)
        {
            int index=0;
            QString str;
            QString result = value.toString();
            int len = result.length()/35;
            if( len > 0)
            {
                EPS_DUMP_TAB *item = new EPS_DUMP_TAB;
                for(int i = 0; i < len; i++ )
                {
                    index += 8;    //yyyymmdd
                    str = result.mid(index, 6); // hhmmss
                    str.insert(2,QChar(':'));
                    str.insert(5,QChar(':'));
                    item->Item[0] = str;
                    index += 6;     //시간
                    str = result.mid(index, 6);
                    index += 6;
                    int sensor = CodeIndex(str);
                    if(sensor != -1)
                    {
                        str = result.mid(index, 10);
                        index += 13;
                        double val = str.toDouble();
                        if(sensor != 3)
                            val /= 100;
                        str = QString::number(val, 'g', 10);
                        int i = str.lastIndexOf('.');
                        if(i <= 0)
                            i = str.length();
                        if(i >0)
                        {
                           i -= 3;
                           while(i > 0)
                           {
                             str.insert(i, ',');
                             i -= 3;
                           }
                        }
                        item->Item[sensor+1] = str;
//                        str = result.mid(index, 2);
                        index += 2;
//                        item->Item[8] = str;
                    }
                    else
                        index += 15;
                }
                mDumpList.append(item);
                count++;
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

void CEpsLog::PageInit(int total)
{
    mTotalPoint = total;
    mStartPoint = mPage = 0;
    mTotalPage = mTotalPoint / EPS_MAX_ROW;
    if(mTotalPoint % EPS_MAX_ROW)
        mTotalPage++;
}

void CEpsLog::DataDisplay()
{
    QTableWidget *table;
    QTableWidgetItem *item;
    QString str;
    int cnt = 0;
    table = ui->tableDump;
    table->clearContents();
    for(int row = mStartPoint; row < mStartPoint + EPS_MAX_ROW; row++, cnt++)
    {
        if(row >= mDumpList.size())
            break;
        str = QString("%1").arg(row+1);
        item = new QTableWidgetItem(str);
        table->setItem(cnt, 0, item);
        for(int col = 1; col < 9; col++)
        {
            item = new QTableWidgetItem(mDumpList[row]->Item[col-1]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            int flag;
            if(col >= 2 && col <= 7)
               flag = Qt::AlignRight | Qt::AlignVCenter;
            else
                flag = Qt::AlignHCenter | Qt::AlignVCenter;
            item->setTextAlignment(flag);
            table->setItem(cnt, col, item);
        }
    }
    PageDisplay();
}

void CEpsLog::PageDisplay()
{
    QString str = QString("Page(%1/%2)").arg(mPage+1).arg(mTotalPage);
    ui->labelPage->setText(str);
}
