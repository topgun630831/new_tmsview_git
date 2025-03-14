#include <QDebug>
#include <QtGui>
#include <cmath>
#include <QToolButton>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include "infomationdialog.h"
#include "questiondialog.h"
#include "tmstrenddialog.h"
#include "ui_tmstrenddialog.h"
#include "datetimeinput.h"
#include "tmssetup.h"
#include "srsocket.h"

extern bool g_DebugDisplay;
extern CSrSocket *g_pSrSocket;
extern bool   g_Flag[TAG_MAX];
extern QString TmsTagNameList[TAG_MAX];
extern QString GraphicId;

void TmsTrendDialog::readFlightCurves(const QString &fileName)
{
    QVector<QPointF> data[8];
    double factX = 0.0013;
    double factY[8] = { 0.0008, 0.1, 0.2, 0.2, 0.1, 0.8, 0.2, 0.0 };
    double offsY[8] = { +500, -55, +309, +308, 0, 0 , 100, 100};
    int pos[8] = { 3, 6, 7, 8, 9, 10, 0, 2 };
    QFile file(fileName);
    double offsX = 0.0;

    double index = 0;
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList coords = line.split(' ',
                                            QString::SkipEmptyParts);
            if (coords.count() >= 8) {
                double x = factX * coords[0].toDouble()*90;
                if (data[0].isEmpty())
                    offsX = x;
                //qDebug() << "offsX:" << offsX << ", coords[0]:" << coords[0] << ", x:" << x;
                for (int i = 0; i < 8; ++i) {
                    double y = coords[pos[i]].toDouble()*300;
                    data[i].append(QPointF(x - offsX,
                    //                       data[i].append(QPointF(index,
                                           factY[i] * (y - offsY[i])));
                    //qDebug() << "i:" << i << ", x:" << x - offsX << ", y:" << factY[i] * (y - offsY[i]);
                }
                index++;
            }
        }
    }

    setCurveData(0, data[0]);
    setCurveData(1, data[1]);
    setCurveData(2, data[2]);
    setCurveData(3, data[3]);
    setCurveData(4, data[4]);
    setCurveData(5, data[5]);
    setCurveData(6, data[6]);
    setCurveData(7, data[7]);
}

TmsTrendDialog::TmsTrendDialog(QWidget *parent) :
    CMonDialog(parent),
    ui(new Ui::TmsTrendDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_AcceptTouchEvents);
//    setDragMode(ScrollHandDrag);

    PlotRect = ui->trendFrame->geometry();
    GraphAbsoluteRect.setRect(PlotRect.left()+leftMargin, PlotRect.top()+topMargin,
                                PlotRect.width()-leftMargin-rightMargin,
                                PlotRect.height()-topMargin-bottomMargin);
    GraphRelativeRect.setRect(leftMargin, topMargin,
                                PlotRect.width()-leftMargin-rightMargin,
                                PlotRect.height()-topMargin-bottomMargin);
   // readFlightCurves(":/in1.txt");

//    setBackgroundRole(QPalette::Dark);
//    setAutoFillBackground(true);
//    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    rubberBandIsShown = false;

    setPlotSettings(PlotSettings());

    m_TagLabel[0] = ui->tag1;
    m_TagLabel[1] = ui->tag2;
    m_TagLabel[2] = ui->tag3;
    m_TagLabel[3] = ui->tag4;
    m_TagLabel[4] = ui->tag5;
    m_TagLabel[5] = ui->tag6;
    m_DescLabel[0] = ui->desc1;
    m_DescLabel[1] = ui->desc2;
    m_DescLabel[2] = ui->desc3;
    m_DescLabel[3] = ui->desc4;
    m_DescLabel[4] = ui->desc5;
    m_DescLabel[5] = ui->desc6;
    m_Button[0] = ui->btnTag1;
    m_Button[1] = ui->btnTag2;
    m_Button[2] = ui->btnTag3;
    m_Button[3] = ui->btnTag4;
    m_Button[4] = ui->btnTag5;
    m_Button[5] = ui->btnTag6;

    for(int i = 0; i < 6; i++)
    {
        if(g_Flag[i])
        {
            m_bSelected[i] = true;
            LabelDisplay(m_TagLabel[i], i);
        }
        else
        {
            m_TagLabel[i]->setVisible(false);
            m_DescLabel[i]->setVisible(false);
            m_Button[i]->setVisible(false);
            m_bSelected[i] = false;
        }
    }
    mDateTime = QDateTime::currentDateTime();
    mDateTime = mDateTime.addDays(-1);

    SendCommand();
}

TmsTrendDialog::~TmsTrendDialog()
{
    delete ui;
}

void TmsTrendDialog::LabelDisplay(QLabel* label, int col)
{
    static const char *bcolor[8] = {
        "red", "green", "blue", "cyan", "magenta", "yellow", "darkCyan", "darkMagenta"
    };
    static const char *fcolor[8] = {
        "black", "black", "white", "black", "black", "black", "white", "white"
    };
    QString bColor;
    if(m_bSelected[col])
        bColor = bcolor[col];
    else
        bColor = "gray";
    QString style = tr("QLabel {background-color : %1;color : %2;}").arg(bColor).arg(fcolor[col]);
    label->setStyleSheet(style);
}

void TmsTrendDialog::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
    ui->btnZoomIn->setEnabled(false);
    ui->btnZoomOut->setEnabled(false);
    refreshPixmap();
}

void TmsTrendDialog::setCurveData(int id, const QVector<QPointF> &data)
{
    curveMap[id] = data;
   // refreshPixmap();
}

void TmsTrendDialog::clearCurve(int id)
{
    curveMap.remove(id);
    //refreshPixmap();
}

QSize TmsTrendDialog::minimumSizeHint() const
{
    return QSize(6 * leftMargin, 4 * bottomMargin);
}

QSize TmsTrendDialog::sizeHint() const
{
    return QSize(12 * leftMargin, 8 * bottomMargin);
}

void TmsTrendDialog::paintEvent(QPaintEvent *p /* event */)
{
    QStylePainter painter(this);
    painter.drawPixmap(PlotRect.left(), PlotRect.top(), pixmap);

    if (rubberBandIsShown) {
        painter.setPen(Qt::white);
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = Qt::black;
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void TmsTrendDialog::resizeEvent(QResizeEvent * /* event */)
{
    refreshPixmap();
}

void TmsTrendDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (PlotRect.contains(event->pos())) {
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
    }
}

void TmsTrendDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (rubberBandIsShown) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void TmsTrendDialog::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown) {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();

        QRect rect = rubberBandRect.normalized();
        if (rect.width() < 20 || rect.height() < 20)
        {
            CInfomationDialog dlg(tr("선택한 영역이 너무 작습니다"));
            dlg.exec();
            return;
        }
    //    rect.translate(-(leftMargin+rightMargin), -(bottomMargin+topMargin));

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;
//        double dx = prevSettings.spanX() / (width() - 2 * Margin);
  //      double dy = prevSettings.spanY() / (height() - 2 * Margin);

        double dx = (prevSettings.spanX()) / (GraphRelativeRect.width());// - (leftMargin+rightMargin));
        double dy = prevSettings.spanY() / (GraphRelativeRect.height());// - (topMargin+bottomMargin));
        settings.minX = prevSettings.minX + dx * (rect.left()-GraphAbsoluteRect.left());

        settings.maxX = prevSettings.minX + (dx * (rect.right()-GraphAbsoluteRect.left()));

        settings.minY = prevSettings.maxY - dy * (rect.bottom()-GraphAbsoluteRect.top());
        settings.maxY = prevSettings.maxY - dy * (rect.top()-GraphAbsoluteRect.top());
        settings.adjust();

        zoomStack.resize(curZoom + 1);
        zoomStack.append(settings);
        on_btnZoomIn_clicked();
    }
}

void TmsTrendDialog::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        on_btnZoomIn_clicked();
        break;
    case Qt::Key_Minus:
        on_btnZoomOut_clicked();
        break;
    case Qt::Key_Left:
        zoomStack[curZoom].scroll(-1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Right:
        zoomStack[curZoom].scroll(+1, 0);
        refreshPixmap();
        break;
    case Qt::Key_Down:
        zoomStack[curZoom].scroll(0, -1);
        refreshPixmap();
        break;
    case Qt::Key_Up:
        zoomStack[curZoom].scroll(0, +1);
        refreshPixmap();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void TmsTrendDialog::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numTicks = numDegrees / 15;

    if (event->orientation() == Qt::Horizontal) {
        zoomStack[curZoom].scroll(numTicks, 0);
    } else {
        zoomStack[curZoom].scroll(0, numTicks);
    }
    refreshPixmap();
}

void TmsTrendDialog::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void TmsTrendDialog::refreshPixmap()
{
    pixmap = QPixmap(PlotRect.width(), PlotRect.height());
    pixmap.fill(Qt::black);
    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}

void TmsTrendDialog::drawGrid(QPainter *painter)
{
//    QRect rect(Margin, Margin,
//               width() - 2 * Margin, height() - 2 * Margin);
//    QRect rect(leftMargin+PlotRect.left(), topMargin+PlotRect.top(),
//               PlotRect.width() - (leftMargin+rightMargin), PlotRect.height() - (topMargin+bottomMargin));
    QRect rect = GraphRelativeRect;
    if (!rect.isValid())
        return;
    QPen pencol(QColor(100,100,100));
    QPen light(QColor(200,200,200));
    QColor color(Qt::black);
//    QBrush brush(color);
//    p.setBackground(brush);
//    p.eraseRect(m_FillRect);
//    painter->fillRect(PlotRect,color);
#if 1
    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().dark().color().light();
//    QPen light = palette().light().color();
    QString sLabel;

    QFont TextFont;			// 객체 폰트
    TextFont.setPointSize(9);
    painter->setFont (TextFont);

    quiteDark.setStyle(Qt::DotLine);
    QDateTime dateTime = mDateTime;
    for (int i = 0; i <= settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1)
                                 / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                          / settings.numXTicks);

        dateTime = mDateTime.addSecs((int)label*60);
        char buf[20];

        ::snprintf(buf, 20, ":%02d", (int)label%60);
        sLabel = dateTime.toString("hh:mm");

        painter->setPen(pencol);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->setPen(light);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          sLabel);
    }
    for (int j = 0; j <= settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                   / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                          / settings.numYTicks);
        painter->setPen(pencol);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        painter->setPen(light);
        painter->drawText(rect.left() - leftMargin, y - 10, leftMargin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number((int)label));
    }
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
   #endif
    painter->save();
    //painter->translate(-100,100);
    TextFont.setBold(true);
    TextFont.setPointSize(11);
    painter->setFont (TextFont);

    //painter->drawText(200,rect.bottom()+40,"Demand Time");
    painter->drawText(rect.left(), rect.bottom()+20, rect.right()-rect.left(), 30,
                      Qt::AlignLeft | Qt::AlignVCenter,
                      mDateTime.toString("yyyy/MM/dd"));

 //   painter->rotate(270);
    painter->drawText(5, 20,"[kW]");
    //painter->drawText(-(rect.bottom()), PlotRect.left(), rect.bottom()-rect.top(), 30,
    //                  Qt::AlignLeft | Qt::AlignVCenter,
                    //  Qt::AlignHCenter | Qt::AlignVCenter,
    //                  "Demand Power[kW]");
    painter->restore();
}

void TmsTrendDialog::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[8] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkCyan, Qt::gray
    };
    PlotSettings settings = zoomStack[curZoom];
    QRect rect = GraphRelativeRect;
    if (!rect.isValid())
        return;

    painter->setClipRect(rect.adjusted(+1, +1, -1, -1));
#if 1
    QMapIterator<int, QVector<QPointF> > i(curveMap);
    while (i.hasNext()) {
        i.next();

        int id = i.key();
        QVector<QPointF> data = i.value();
        QPolygonF polyline(data.count());
        if(m_bSelected[uint(id) % 8])
        {
            for (int j = 0; j < data.count(); ++j) {
                double dx = data[j].x() - settings.minX;
                double dy = data[j].y() - settings.minY;
                double x = rect.left() + (dx * (rect.width() - 1)
                                             / settings.spanX());
                double y = rect.bottom() - (dy * (rect.height() - 1)
                                               / settings.spanY());
                polyline[j] = QPointF(x, y);
            }
            painter->setPen(colorForIds[uint(id) % 8]);
            painter->drawPolyline(polyline);
        }
    }
  #endif
}

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 1440.0;   //10.0
    numXTicks = 12;

    minY = 0.0;
    maxY = 100.0;  //10.0
    numYTicks = 6;
}

void PlotSettings::scroll(int dx, int dy)
{
    double stepX = spanX() / numXTicks;
    minX += dx * stepX;
    maxX += dx * stepX;

    double stepY = spanY() / numYTicks;
    minY += dy * stepY;
    maxY += dy * stepY;
}

void PlotSettings::adjust()
{
//    adjustAxis(minX, maxX, numXTicks);
    adjustAxis(minY, maxY, numYTicks);
}

void PlotSettings::adjustAxis(double &min, double &max, int &numTicks)
{
    const int MinTicks = 4;
    double grossStep = (max - min) / MinTicks;
    double step = std::pow(10.0, std::floor(std::log10(grossStep)));

    if (5 * step < grossStep) {
        step *= 5;
    } else if (2 * step < grossStep) {
        step *= 2;
    }

    numTicks = int(std::ceil(max / step) - std::floor(min / step));
    if (numTicks < MinTicks)
        numTicks = MinTicks;
    min = std::floor(min / step) * step;
    max = std::ceil(max / step) * step;
}

bool TmsTrendDialog::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
        qDebug() << "QTouchEvent";
        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2) {
            // determine scale factor
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();
            qreal currentScaleFactor =
                    QLineF(touchPoint0.pos(), touchPoint1.pos()).length()
                    / QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();
            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) {
                // if one of the fingers is released, remember the current scale
                // factor so that adding another finger later will continue zooming
                // by adding new scale factor to the existing remembered value.
//                totalScaleFactor *= currentScaleFactor;
//                currentScaleFactor = 1;
            }
//            setTransform(QTransform().scale(totalScaleFactor * currentScaleFactor,
//                                            totalScaleFactor * currentScaleFactor));
        }
        return true;
    }
    default:
        break;
    }
    return TmsTrendDialog::viewportEvent(event);
}

void TmsTrendDialog::on_btnDateTime_clicked()
{
    DateTimeInput dlg(mDateTime);
    if(dlg.exec() == QDialog::Accepted)
    {
        mDateTime.setDate(dlg.mDate);
        QTime time(dlg.mHour, dlg.mMin);
        mDateTime.setTime(time);
        SendCommand();
    }
}

void TmsTrendDialog::on_btnZoomIn_clicked()
{
    if (curZoom < zoomStack.count() - 1) {
        ++curZoom;
        ui->btnZoomIn->setEnabled(curZoom < zoomStack.count() - 1);
        ui->btnZoomOut->setEnabled(true);
        refreshPixmap();
    }
}

void TmsTrendDialog::on_btnZoomOut_clicked()
{
    if (curZoom > 0) {
        --curZoom;
        ui->btnZoomOut->setEnabled(curZoom > 0);
        ui->btnZoomIn->setEnabled(true);
        refreshPixmap();
    }
}

void TmsTrendDialog::on_btnTag1_clicked()
{
    SetEnabled(0);
}

void TmsTrendDialog::on_btnTag2_clicked()
{
    SetEnabled(1);
}

void TmsTrendDialog::on_btnTag3_clicked()
{
    SetEnabled(2);
}

void TmsTrendDialog::on_btnTag4_clicked()
{
    SetEnabled(3);
}

void TmsTrendDialog::on_btnTag5_clicked()
{
    SetEnabled(4);
}

void TmsTrendDialog::on_btnTag6_clicked()
{
    SetEnabled(5);
}

void TmsTrendDialog::SetEnabled(int tag)
{
    m_bSelected[tag] = m_Button[tag]->isChecked();
    LabelDisplay(m_TagLabel[tag], tag);
/*    for(int i = 0; i < 6; i++)
    {
        if(tag == i)
            m_bSelected[tag] = true;
        else
            m_bSelected[i] = false;
        LabelDisplay(m_TagLabel[i], i);
    }
*/    refreshPixmap();
}

const char *codeStrTrend[6] =  {
    "TON00",   //T-N
    "TOP00",   //T-P
    "COD00",   //COD
    "PHY00",   //PH
    "SUS00",   //SS
    "COD00",   //
};

int TmsTrendDialog::CodeIndex(QString code)
{
    for(int i = 0; i < 6; i++)
    {
        if(code == codeStrTrend[i])
            return i;
    }
    return -1;
}

void TmsTrendDialog::onRead(QString& cmd, QJsonObject& jobject)
{
    QJsonArray array = jobject["Result"].toArray();

    if(g_DebugDisplay)
        qDebug() << "cmd:" << cmd << "  len:" << array.size();
    if(cmd == "Ext_Tms_GetTrend")
    {
        QVector<QPointF> data[6];
        foreach (const QJsonValue& trend, array)
        {
            QJsonObject itemObject = trend.toObject();
            QString code = itemObject["Code"].toString();
            int index = CodeIndex(code);
            if(index < 0 || g_Flag[index] == false)
                continue;
            QString date = itemObject["StartDate"].toString();
            mDateTime.fromString(date);
            qDebug() << "Date:" << date << " code:" << code << " Date:" << mDateTime;
           // qDebug() << "Values:" << itemObject["Values"];
            QJsonArray values = itemObject["Values"].toArray();
            double x = 0;
            foreach (const QJsonValue& value, values)
            {
                data[index].append(QPointF(x, value.toString().toDouble()));
                x += 5;
            }
            setCurveData(index, data[index]);
        }
        refreshPixmap();
    }
}

void TmsTrendDialog::on_btnSelectAll_clicked()
{
    for(int i = 0; i < 6; i++)
    {
        m_bSelected[i] = true;
        LabelDisplay(m_TagLabel[i], i);
        m_Button[i]->setChecked(true);
    }
    refreshPixmap();
}

void TmsTrendDialog::on_btnNext_clicked()
{
    QDateTime date = mDateTime.addDays(2);
    if(date < QDateTime::currentDateTime())
    {
        mDateTime = mDateTime.addDays(1);
        SendCommand();
    }
}

void TmsTrendDialog::on_btnPrev_clicked()
{
    mDateTime = mDateTime.addDays(-1);
    SendCommand();
}

void TmsTrendDialog::SendCommand()
{
    QString cmd = "Ext_Tms_GetTrend";
    QJsonObject argObject;
    QString strDate;
    QString kind = "M";
    QDateTime toDate = mDateTime.addDays(1);
qDebug() << "Start:" << mDateTime << " End:" << toDate;
    strDate = QString("%1").arg(mDateTime.toString("yyyy-MM-dd hh:mm:ss"));
    argObject.insert(QString("StartTime"),strDate);
    strDate = QString("%1").arg(toDate.toString("yyyy-MM-dd hh:mm:ss"));
    argObject.insert(QString("EndTime"),QJsonValue(strDate));
    argObject.insert(QString("Kind"),QJsonValue(kind));
    QString code = "All";
    argObject.insert(QString("Code"),QJsonValue(code));
    QJsonValue argValue(argObject);
    g_pSrSocket->sendCommand((QWidget*)this, cmd, GraphicId, argValue);
    for(int index=0; index < 6; index++)
        curveMap.remove(index);
}
