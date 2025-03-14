#include <QPainter>
#include <QPixmap>
#include <QDesktopWidget>
#include "property.h"
#include "ui_property.h"
#include "questiondialog.h"
#include "srsocket.h"
extern CSrSocket *g_pSrSocket;

CProperty::CProperty(PROPERTY_TAB *p, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CProperty)
{
    mProperty = p;
    UseLowLowAlarm     = p->UseLowLowAlarm;
    LowLowAlarmValue   = p->LowLowAlarmValue;
    UseLowAlarm        = p->UseLowAlarm;
    LowAlarmValue      = p->LowAlarmValue;
    UseHighAlarm       = p->UseHighAlarm;
    HighAlarmValue     = p->HighAlarmValue;
    UseHighHighAlarm   = p->UseHighHighAlarm;
    HighHighAlarmValue = p->HighHighAlarmValue;
    Reversed           = p->Reversed;
    DigitalEvent       = p->DigitalEvent;
    DigitalAlarm       = p->DigitalAlarm;

    ui->setupUi(this);
    const QRect screen = QApplication::desktop()->screenGeometry();
    this->move( screen.center() - this->rect().center() );
    installEventFilter(this);

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

    QWidget *widget;
    if(p->TagType == "Digital")
    {
        ui->OnMsg->setText(p->OnMsg);
        ui->OffMsg->setText(p->OffMsg);
        ui->Reversed->setChecked(p->Reversed);
        //ui->DigitalEvent->setChecked(p->DigitalEvent);
        //ui->DigitalAlarm->setChecked(p->DigitalAlarm);
        widget = ui->stackedDetail->widget(2);
        ui->stackedDetail->removeWidget(widget);
        widget = ui->stackedDetail->widget(1);
        ui->stackedDetail->removeWidget(widget);
    }
    else
    if(p->TagType == "Analog")
    {
        ui->RawMin->setText(QString("%1").arg(p->RawMin));
        ui->RawMax->setText(QString("%1").arg(p->RawMax));
        ui->EuMin->setText(QString("%1").arg(p->EuMin));
        ui->EuMax->setText(QString("%1").arg(p->EuMax));
        ui->Ratio->setText(QString("%1").arg(p->Ratio));
        ui->RoundUp->setText(QString("%1").arg(p->RoundUp));
        ui->Deadband->setText(QString("%1").arg(p->Deadband));
        ui->Format->setText(QString("%1").arg(p->Format));
        ui->UseLowLowAlarm->setChecked(p->UseLowLowAlarm);
        ui->LowLowAlarmValue->setText(QString("%1").arg(p->LowLowAlarmValue));
        ui->UseLowAlarm->setChecked(p->UseLowAlarm);
        ui->LowAlarmValue->setText(QString("%1").arg(p->LowAlarmValue));
        ui->UseHighAlarm->setChecked(p->UseHighAlarm);
        ui->HighAlarmValue->setText(QString("%1").arg(p->HighAlarmValue));
        ui->UseHighHighAlarm->setChecked(p->UseHighHighAlarm);
        ui->HighHighAlarmValue->setText(QString("%1").arg(p->HighHighAlarmValue));
        widget = ui->stackedDetail->widget(2);
        ui->stackedDetail->removeWidget(widget);
        widget = ui->stackedDetail->widget(0);
        ui->stackedDetail->removeWidget(widget);
    }
    else
    if(p->TagType == "Pulse")
    {
        ui->pRawMax->setText(QString("%1").arg(p->RawMax));
        ui->pEuMax->setText(QString("%1").arg(p->EuMax));
        ui->pRatio->setText(QString("%1").arg(p->Ratio));
        ui->pFormat->setText(QString("%1").arg(p->Format));
        ui->ClearCondition->setText(p->ClearCondition);
        ui->MaxValueChange->setText(QString("%1").arg(p->MaxValueChange));
        if(p->UseHighAlarm == 1)
            ui->pUseHighAlarm->setChecked(true);
        ui->HighAlarmValue->setText(QString("%1").arg(p->HighAlarmValue));
        if(p->UseHighHighAlarm == 1)
            ui->pUseHighHighAlarm->setChecked(true);
        ui->pHighHighAlarmValue->setText(QString("%1").arg(p->HighHighAlarmValue));
        widget = ui->stackedDetail->widget(0);
        ui->stackedDetail->removeWidget(widget);
        widget = ui->stackedDetail->widget(0);
        ui->stackedDetail->removeWidget(widget);
    }
    else    // String
        ui->groupBoxDetail->setVisible(false);
}

CProperty::~CProperty()
{
    delete ui;
}

void CProperty::paintEvent(QPaintEvent *)
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

bool CProperty::changeCompare(QJsonArray& array)
{
    if(mProperty->TagType == "Digital")
    {
        Reversed = ui->Reversed->isChecked();
      //  DigitalEvent = ui->DigitalEvent->isChecked();
      //  DigitalAlarm = ui->DigitalAlarm->isChecked();
    }
    else
    if(mProperty->TagType == "Analog")
    {
        UseLowLowAlarm = ui->UseLowLowAlarm->isChecked();
        UseLowAlarm = ui->UseLowAlarm->isChecked();
        UseHighAlarm = ui->UseHighAlarm->isChecked();
        UseHighHighAlarm = ui->UseHighHighAlarm->isChecked();
        LowLowAlarmValue = ui->LowLowAlarmValue->text().toDouble();
        LowAlarmValue = ui->LowAlarmValue->text().toDouble();
        HighAlarmValue = ui->HighAlarmValue->text().toDouble();
        HighHighAlarmValue = ui->HighHighAlarmValue->text().toDouble();
    }
    else
    if(mProperty->TagType == "Pulse")
    {
        UseHighAlarm = ui->pUseHighAlarm->isChecked();
        UseHighHighAlarm = ui->pUseHighHighAlarm->isChecked();
        HighAlarmValue = ui->pHighAlarmValue->text().toDouble();
        HighHighAlarmValue = ui->pHighHighAlarmValue->text().toDouble();
    }
    bool        bChange=false;
    PROPERTY_TAB *p = mProperty;
    if(UseLowLowAlarm != p->UseLowLowAlarm)
    {
        QJsonObject jObj;
        jObj.insert(QString("UseLowLowAlarm"), QJsonValue(UseLowLowAlarm));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->UseLowLowAlarm     = UseLowLowAlarm;
    }
    if(LowLowAlarmValue != p->LowLowAlarmValue)
    {
        QJsonObject jObj;
        jObj.insert(QString("LowLowAlarmValue"), QJsonValue(LowLowAlarmValue));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->LowLowAlarmValue   = LowLowAlarmValue;
    }
    if(UseLowAlarm != p->UseLowAlarm)
    {
        QJsonObject jObj;
        jObj.insert(QString("UseLowAlarm"), QJsonValue(UseLowAlarm));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->UseLowAlarm        = UseLowAlarm;
    }
    if(LowAlarmValue != p->LowAlarmValue)
    {
        QJsonObject jObj;
        jObj.insert(QString("LowAlarmValue"), QJsonValue(LowAlarmValue));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->LowAlarmValue      = LowAlarmValue;
    }
    if(UseHighAlarm != p->UseHighAlarm)
    {
        QJsonObject jObj;
        jObj.insert(QString("UseHighAlarm"), QJsonValue(UseHighAlarm));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->UseHighAlarm       = UseHighAlarm;
    }
    if(HighAlarmValue != p->HighAlarmValue)
    {
        QJsonObject jObj;
        jObj.insert(QString("HighAlarmValue"), QJsonValue(HighAlarmValue));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->HighAlarmValue     = HighAlarmValue;
    }
    if(UseHighHighAlarm != p->UseHighHighAlarm)
    {
        QJsonObject jObj;
        jObj.insert(QString("UseHighHighAlarm"), QJsonValue(UseHighHighAlarm));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->UseHighHighAlarm   = UseHighHighAlarm;
    }
    if(HighHighAlarmValue != p->HighHighAlarmValue)
    {
        QJsonObject jObj;
        jObj.insert(QString("HighHighAlarmValue"), QJsonValue(HighHighAlarmValue));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->HighHighAlarmValue = HighHighAlarmValue;
    }
    if(Reversed != p->Reversed)
    {
        QJsonObject jObj;
        jObj.insert(QString("Reversed"), QJsonValue(Reversed));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->Reversed           = Reversed;
    }
    if(DigitalEvent != p->DigitalEvent)
    {
        QJsonObject jObj;
        jObj.insert(QString("DigitalEvent"), QJsonValue(DigitalEvent));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->DigitalEvent       = DigitalEvent;
    }
    if(DigitalAlarm != p->DigitalAlarm)
    {
        QJsonObject jObj;
        jObj.insert(QString("DigitalAlarm"), QJsonValue(DigitalAlarm));
        array.append(QJsonValue(jObj));
        bChange = true;
        p->DigitalAlarm       = DigitalAlarm;
    }
    return bChange;
}

void CProperty::changeproperty(QJsonArray jArray)
{
    QJsonArray array;
    QJsonObject jObj;
    jObj.insert(QString("Tag"),QJsonValue(mProperty->Name));
    jObj.insert(QString("Properties"),QJsonValue(jArray));
    array.append(jObj);
    g_pSrSocket->sendCommand(0, QString("Tag_ChangeProperty"),QString(""),QJsonValue(array));
}

void CProperty::on_save_clicked()
{
    QJsonArray jArray;
    if(changeCompare(jArray) == true)
        changeproperty(jArray);
}

void CProperty::on_ok_clicked()
{
    QJsonArray jArray;
    if(changeCompare(jArray) == true)
    {
        CQuestionDialog dlg(tr("변경된 내용을 저장하시겠습니까?"));
        if(dlg.exec() ==QDialog::Accepted)
            changeproperty(jArray);
    }
    QDialog::accept();
}
