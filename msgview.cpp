#include <QScrollBar>
#include "msgview.h"
#include "ui_msgview.h"

extern bool    m_bScreenSave;

MsgView::MsgView(QStringList list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MsgView)
{
    ui->setupUi(this);

    ui->listWidget->setStyleSheet("QListWidget { "
                               "margin:20px;"
                               "padding:20px;}");
    ui->listWidget->setSpacing(5);

    for(int i = 0; i < list.size(); i++)
        ui->listWidget->insertItem(i, list[i]);
    startTimer(1000);
}

MsgView::~MsgView()
{
    delete ui;
}

void MsgView::timerEvent(QTimerEvent *)
{
    if (m_bScreenSave == true)
        QDialog::reject();
}
