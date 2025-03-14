#include "propertyrange.h"
#include "ui_propertyrange.h"
#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "questiondialog.h"
#include "infomationdialog.h"
#include "srsocket.h"
extern CSrSocket *g_pSrSocket;

PropertyRange::PropertyRange(POINT_TAB *pointTab, QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::PropertyRange)
{
    mPointTab = pointTab;
    QJsonArray array;
    array.append(QJsonValue(pointTab->Name));
    g_pSrSocket->sendCommand((QWidget*)this, QString("Tag_Property"), QString("Graphic"), QJsonValue(array));

    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

    QString name = pointTab->Name + "[" + pointTab->Desc + "]";
    ui->tagName->setText(name);
    ui->TagType->setText(pointTab->TagType);
    ui->Rw->setText(pointTab->Rw);

    mTimerId = startTimer(1000);
    ui->Value->setText(pointTab->Value);
    ui->RawValue->setText(pointTab->RawValue);
    if(pointTab->TagType == "Analog")
    {
        ui->stackedDetail->setCurrentIndex(0);
    }
    else
    if(pointTab->TagType == "Pulse")
        ui->stackedDetail->setCurrentIndex(1);
    else
    if(pointTab->TagType == "Digital")
        ui->stackedDetail->setCurrentIndex(2);

/*
    mProperty = p;
    RawMin = p->RawMin;
    RawMax = p->RawMax;
    EuMin  = p->EuMin;
    EuMax  = p->EuMax;
    Ratio  = p->Ratio;
    RoundUp = p->RoundUp;
    Deadband = p->Deadband;
    Reversed = p->Reversed;

    //Common
    QString name = p->Name + "[" + p->Desc + "]";
    ui->tagName->setText(name);
    ui->TagType->setText(p->TagType);
    ui->DataType->setText(p->DataType);
    ui->Rw->setText(p->Rw);
    ui->Driver->setText(p->Driver);
    ui->Device->setText(p->Device);
    ui->Address->setText(p->Address);
    ui->UseForcedValue->setChecked(p->UseForcedValue);
    ui->ForceValue->setText(QString("%1").arg(p->ForcedValue));
    ui->InitialValue->setText(QString("%1").arg(p->InitValue));
    ui->Linktags->setText(p->LinkTags);

    if(p->TagType == "Analog")
    {
        ui->stackedDetail->setCurrentIndex(0);
        ui->RawMin->setText(QString("%1").arg(p->RawMin));
        ui->RawMax->setText(QString("%1").arg(p->RawMax));
        ui->EuMin->setText(QString("%1").arg(p->EuMin));
        ui->EuMax->setText(QString("%1").arg(p->EuMax));
        ui->Ratio->setText(QString("%1").arg(p->Ratio));
        ui->RoundUp->setText(QString("%1").arg(p->RoundUp));
        ui->Deadband->setText(QString("%1").arg(p->Deadband));
        RatioEnableDisbale();
    }
    else
    if(p->TagType == "Pulse")
    {
        ui->stackedDetail->setCurrentIndex(1);
        ui->P_RawMax->setText(QString("%1").arg(p->RawMax));
        ui->P_EuMax->setText(QString("%1").arg(p->EuMax));
        ui->P_Ratio->setText(QString("%1").arg(p->Ratio));
    }
    else
    if(p->TagType == "Digital")
    {
        ui->stackedDetail->setCurrentIndex(2);
        ui->Reversed->setChecked(p->Reversed);
    }
    */
}

PropertyRange::~PropertyRange()
{
    if(mTimerId)
        killTimer(mTimerId);
    delete ui;
}

void PropertyRange::onRead(QString& cmd, QJsonObject& jobject)
{
    if(cmd == "Tag_Property")
    {
        QJsonArray array = jobject["Result"].toArray();
        foreach (const QJsonValue& value, array)
        {
            QJsonObject obj = value.toObject();
            mProperty.Name = obj["Name"].toString();
            mProperty.Desc = obj["Desc"].toString();
            mProperty.Rw   = obj["Rw"].toString();
            mProperty.Driver = obj["Driver"].toString();
            mProperty.Device = obj["Device"].toString();
            mProperty.Address = obj["Address"].toString();
            mProperty.InitValue = obj["InitValue"].toDouble();
            mProperty.UseForcedValue = obj["UseForcedValue"].toDouble();
            mProperty.ForcedValue = obj["ForcedValue"].toBool();
            mProperty.LinkTags = obj["LinkTags"].toString();
            mProperty.TagType = obj["TagType"].toString();
            mProperty.DataType = obj["DataType"].toString();
            mProperty.RawMin = obj["RawMin"].toDouble();
            mProperty.RawMax = obj["RawMax"].toDouble();
            mProperty.EuMin = obj["EuMin"].toDouble();
            mProperty.EuMax = obj["EuMax"].toDouble();
            mProperty.Ratio = obj["Ratio"].toDouble();
            mProperty.RoundUp = obj["RoundUp"].toInt();
            mProperty.Deadband = obj["Deadband"].toDouble();
            mProperty.Format = obj["Format"].toDouble();
            mProperty.UseLowLowAlarm = obj["UseLowLowAlarm"].toInt();
            mProperty.LowLowAlarmValue = obj["LowLowAlarmValue"].toDouble();
            mProperty.UseLowAlarm = obj["UseLowAlarm"].toInt();
            mProperty.LowAlarmValue = obj["LowAlarmValue"].toDouble();
            mProperty.UseHighAlarm = obj["UseHighAlarm"].toInt();
            mProperty.HighAlarmValue = obj["HighAlarmValue"].toDouble();
            mProperty.UseHighHighAlarm = obj["UseHighHighAlarm"].toInt();
            mProperty.HighHighAlarmValue = obj["HighHighAlarmValue"].toDouble();
            mProperty.OnMsg = obj["OnMsg"].toString();
            mProperty.OffMsg = obj["OffMsg"].toString();
            mProperty.Reversed = obj["Reversed"].toBool();
            mProperty.DigitalEvent = obj["DigitalEvent"].toString();
            mProperty.DigitalAlarm = obj["DigitalAlarm"].toString();
            mProperty.ClearCondition = obj["ClearCondition"].toString();
            mProperty.MaxValueChange = obj["MaxValueChange"].toDouble();
        }
        RawMin = mProperty.RawMin;
        RawMax = mProperty.RawMax;
        EuMin  = mProperty.EuMin;
        EuMax  = mProperty.EuMax;
        Ratio  = mProperty.Ratio;
        RoundUp = mProperty.RoundUp;
        Deadband = mProperty.Deadband;
        Reversed = mProperty.Reversed;

        //Common
        ui->DataType->setText(mProperty.DataType);
        ui->Driver->setText(mProperty.Driver);
        ui->Device->setText(mProperty.Device);
        ui->Address->setText(mProperty.Address);
        ui->UseForcedValue->setChecked(mProperty.UseForcedValue);
        ui->ForceValue->setText(QString("%1").arg(mProperty.ForcedValue));
        ui->InitialValue->setText(QString("%1").arg(mProperty.InitValue));
        ui->Linktags->setText(mProperty.LinkTags);

        if(mProperty.TagType == "Analog")
        {
            ui->RawMin->setText(QString("%1").arg(mProperty.RawMin));
            ui->RawMax->setText(QString("%1").arg(mProperty.RawMax));
            ui->EuMin->setText(QString("%1").arg(mProperty.EuMin));
            ui->EuMax->setText(QString("%1").arg(mProperty.EuMax));
            ui->Ratio->setText(QString("%1").arg(mProperty.Ratio));
            ui->RoundUp->setText(QString("%1").arg(mProperty.RoundUp));
            ui->Deadband->setText(QString("%1").arg(mProperty.Deadband));
            if(Ratio == 0)
                ui->UseRatio->setChecked(false);
            else
                ui->UseRatio->setChecked(true);
            RatioEnableDisbale();
        }
        else
        if(mProperty.TagType == "Pulse")
        {
            ui->P_RawMax->setText(QString("%1").arg(mProperty.RawMax));
            ui->P_EuMax->setText(QString("%1").arg(mProperty.EuMax));
            ui->P_Ratio->setText(QString("%1").arg(mProperty.Ratio));
        }
        else
        if(mProperty.TagType == "Digital")
        {
            ui->Reversed->setChecked(mProperty.Reversed);
        }
        ui->save->setEnabled(true);
    }
    else
    if(cmd == "Tag_ChangePropertyRange")
    {
  //      CInfomationDialog dlg(jobject["Result"].toString());
  //      dlg.exec();
    }
}

void PropertyRange::paintEvent(QPaintEvent *)
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

bool PropertyRange::changeCompare(QJsonObject& jObject)
{
    if(mProperty.TagType == "Analog")
    {
        RawMin   = ui->RawMin->text().toDouble();
        RawMax = ui->RawMax->text().toDouble();
        EuMin    = ui->EuMin->text().toDouble();
        EuMax  = ui->EuMax->text().toDouble();
        Ratio  = ui->Ratio->text().toDouble();
        RoundUp  = ui->RoundUp->text().toInt();
        Deadband = ui->Deadband->text().toDouble();
    }
    else
    if(mProperty.TagType == "Pulse")
    {
        RawMax = ui->P_RawMax->text().toDouble();
        EuMax  = ui->P_EuMax->text().toDouble();
        Ratio  = ui->P_Ratio->text().toDouble();
    }
    else
    if(mProperty.TagType == "Digital")
    {
        Reversed = ui->Reversed->isChecked();
    }

    bool        bChange=false;
    jObject.insert(QString("Tag"), QJsonValue(mProperty.Name));
    jObject.insert(QString("RawMin"), QJsonValue(RawMin));
    jObject.insert(QString("RawMax"), QJsonValue(RawMax));
    jObject.insert(QString("EuMin"), QJsonValue(EuMin));
    jObject.insert(QString("EuMax"), QJsonValue(EuMax));
    jObject.insert(QString("Ratio"), QJsonValue(Ratio));
    jObject.insert(QString("RoundUp"), QJsonValue(RoundUp));
    jObject.insert(QString("Deadband"), QJsonValue(Deadband));
    jObject.insert(QString("Reversed"), QJsonValue(Reversed));
    if(RawMin != mProperty.RawMin)
    {
        bChange = true;
        mProperty.RawMin   = RawMin;
    }
    if(RawMax != mProperty.RawMax)
    {
        bChange = true;
        mProperty.RawMax   = RawMax;
    }
    if(EuMin != mProperty.EuMin)
    {
        bChange = true;
        mProperty.EuMin   = EuMin;
    }
    if(EuMax != mProperty.EuMax)
    {
        bChange = true;
        mProperty.EuMax   = EuMax;
    }
    if(Ratio != mProperty.Ratio)
    {
        bChange = true;
        mProperty.Ratio   = Ratio;
    }
    if(RoundUp != mProperty.RoundUp)
    {
        bChange = true;
        mProperty.RoundUp   = RoundUp;
    }
    if(Deadband != mProperty.Deadband)
    {
        bChange = true;
        mProperty.Deadband   = Deadband;
    }
    if(Reversed != mProperty.Reversed)
    {
        bChange = true;
        mProperty.Reversed   = Reversed;
    }
    return bChange;
}

void PropertyRange::changeproperty(QJsonObject jObject)
{
    g_pSrSocket->sendCommand(this, QString("Tag_ChangePropertyRange"),QString(""),QJsonValue(jObject));
}

void PropertyRange::on_save_clicked()
{
    QJsonObject jObject;
    if(changeCompare(jObject) == true)
    {
        CQuestionDialog dlg(tr("변경된 내용을 저장하시겠습니까?"));
        if(dlg.exec() ==QDialog::Accepted)
            changeproperty(jObject);
        QDialog::accept();
    }
    else
    {
        CInfomationDialog dlg("변경된 내용이 없습니다.");
        dlg.exec();
    }
}

void PropertyRange::on_Ratio_textChanged(const QString &arg1)
{
    Ratio  = arg1.toDouble();
    if(Ratio == 0)
        ui->UseRatio->setChecked(false);
    else
        ui->UseRatio->setChecked(true);
    RatioEnableDisbale();
}

void PropertyRange::RatioEnableDisbale()
{
    if(Ratio != 0)
    {
        ui->RawMin->setEnabled(false);
        ui->RawMax->setEnabled(false);
        ui->EuMin->setEnabled(false);
        ui->EuMax->setEnabled(false);
        ui->Ratio->setEnabled(true);
    }
    else
    {
        ui->RawMin->setEnabled(true);
        ui->RawMax->setEnabled(true);
        ui->EuMin->setEnabled(true);
        ui->EuMax->setEnabled(true);
        ui->Ratio->setEnabled(false);
    }
}

void PropertyRange::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == mTimerId)
    {
        ui->Value->setText(mPointTab->Value);
        ui->RawValue->setText(mPointTab->RawValue);
    }
}

void PropertyRange::on_UseRatio_clicked()
{
    if(ui->UseRatio->isChecked())
    {
        if(Ratio == 0)
            Ratio = 1;
    }
    else
        Ratio = 0;
    ui->Ratio->setText(QString("%1").arg(Ratio));
    RatioEnableDisbale();
}
