#include <QScrollBar>
#include "msgview.h"
#include "ui_msgview.h"

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
}

MsgView::~MsgView()
{
    delete ui;
}
