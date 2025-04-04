#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "monitorudp.h"
#include "versioninfo.h"
#include "ui_versioninfo.h"
int VColumnWidth[5] = {
        250,     //구분
        150,    //이름
        100,    //버전
        250,    //빌드날자
        500    //설명
};

extern bool    m_bScreenSave;
CVersionInfo::CVersionInfo(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::CVersionInfo)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    QTableWidget *table = ui->tableVersion;
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    for(int c = 0; c < 5; c++)
        table->horizontalHeader()->resizeSection(c, VColumnWidth[c]);
    QJsonObject argObject;
    QJsonValue vValue(QJsonValue::Null);
    CMonitorUdp::Instance()->sendCommand(this, QString("System_GetVersion"), argObject, vValue);
    startTimer(1000);
}

CVersionInfo::~CVersionInfo()
{
    delete ui;
}

void CVersionInfo::paintEvent(QPaintEvent *)
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

void CVersionInfo::onRead(QString& cmd, QJsonObject& jobject)
{
    const char *VersionItem[5] = {
        "Kind",
        "Item",
        "Version",
        "Builddate",
        "Desc"
    };
    QJsonArray array = jobject["Result"].toArray();
    if(cmd == "System_GetVersion")
    {
        QTableWidget *table = ui->tableVersion;
        table->setRowCount(array.size());
        QTableWidgetItem *item;
        int row = 0;
        foreach (const QJsonValue& value, array)
        {
            QJsonObject itemObject = value.toObject();
            for(int i = 0; i < 5; i++)
            {
                QString str = itemObject[VersionItem[i]].toString();
                item = new QTableWidgetItem(str);
                table->setItem(row, i, item);
            }
            row++;
        }
    }
}

void CVersionInfo::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
