#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include<QFileDialog>
#include <QScrollBar>
#include "fileselect.h"
#include "ui_fileselect.h"

FileSelect::FileSelect(QString &dir, bool backup, bool bFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSelect)
{
    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    ui->filesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if(bFile)
    {
        FindFile(dir);
        ui->filesTable->horizontalHeader()->resizeSection(1, 100);
    }
    else
    {
        ui->filesTable->setColumnCount(1);
        ui->filesTable->horizontalHeaderItem(0)->setText("Directory");
        ui->select->setText("디렉토리 선택");
        QString filter;
        if(backup)
            filter = "backup_*";
        else
            filter = "setupbackup_*";
        FindDirectory(dir, filter);
    }
    QScrollBar *scroll  = ui->filesTable->verticalScrollBar();
    scroll->setStyleSheet("QScrollBar:vertical { width: 20px; height: 30px}");
    ui->filesTable->horizontalHeader()->resizeSection(0, 320);
}

FileSelect::~FileSelect()
{
    delete ui;
}

void FileSelect::paintEvent(QPaintEvent *)
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

void FileSelect::FindFile(QString &dir)
{
    currentDir = QDir(dir);
    QStringList files;
    files = currentDir.entryList(QStringList("*"),
                                 QDir::Files | QDir::NoSymLinks);

    for (int i = 0; i < files.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = ui->filesTable->rowCount();
        ui->filesTable->insertRow(row);
        ui->filesTable->setItem(row, 0, fileNameItem);
        ui->filesTable->setItem(row, 1, sizeItem);
    }
    ui->found->setText(tr("%1 file(s) found").arg(files.size()));
}

void FileSelect::FindDirectory(QString &dir, QString &filter)
{
    currentDir = QDir(dir);
    QStringList files;
    files = currentDir.entryList(QStringList(filter),
                                 QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    for (int i = 0; i < files.size(); ++i) {
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);

        int row = ui->filesTable->rowCount();
        ui->filesTable->insertRow(row);
        ui->filesTable->setItem(row, 0, fileNameItem);
    }
    ui->found->setText(tr("%1 dir(s) found").arg(files.size()));
}

void FileSelect::on_ok_clicked()
{
    QList<QTableWidgetItem *> list = ui->filesTable->selectedItems();
    if(list.size() >= 1)
    {
        mSelectedFile = list[0]->text();
        accept();
    }
}
