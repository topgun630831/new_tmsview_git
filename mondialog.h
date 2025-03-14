#ifndef MONDIALOG_H
#define MONDIALOG_H

#include <QDialog>

struct TAG_WRITE_DELAY {
    QString     Cmd;
    QByteArray  Data;
    int         Delay;
};

class CMonDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CMonDialog(QWidget *parent = 0);
    virtual void     onRead(QString &cmd, QJsonObject&)=0;

signals:

public slots:

};

#endif // MONDIALOG_H
