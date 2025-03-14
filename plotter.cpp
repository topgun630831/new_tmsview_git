#include <QtGui>
#include <cmath>
#include <QToolButton>
#include <QStylePainter>
#include <QStyleOptionFocusRect>
#include "plotter.h"

Plotter::Plotter(QRect &rect, QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Dark);
    setAutoFillBackground(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    rubberBandIsShown = false;

    zoomInButton = new QToolButton(this);
    zoomInButton->setIcon(QIcon(":/images/zoomin.png"));
    zoomInButton->adjustSize();
    connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));

    zoomOutButton = new QToolButton(this);
    zoomOutButton->setIcon(QIcon(":/images/zoomout.png"));
    zoomOutButton->adjustSize();
    connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));

    PlotRect = rect;
    setFixedSize(PlotRect.width(), PlotRect.height());

    setPlotSettings(PlotSettings());
}

void Plotter::setPlotSettings(const PlotSettings &settings)
{
    zoomStack.clear();
    zoomStack.append(settings);
    curZoom = 0;
    zoomInButton->hide();
    zoomOutButton->hide();
    refreshPixmap();
}

void Plotter::zoomOut()
{
    if (curZoom > 0) {
        --curZoom;
        zoomOutButton->setEnabled(curZoom > 0);
        zoomInButton->setEnabled(true);
        zoomInButton->show();
        refreshPixmap();
    }
}

void Plotter::zoomIn()
{
    if (curZoom < zoomStack.count() - 1) {
        ++curZoom;
        zoomInButton->setEnabled(curZoom < zoomStack.count() - 1);
        zoomOutButton->setEnabled(true);
        zoomOutButton->show();
        refreshPixmap();
    }
}

void Plotter::setCurveData(int id, const QVector<QPointF> &data)
{
    curveMap[id] = data;
    refreshPixmap();
}

void Plotter::clearCurve(int id)
{
    curveMap.remove(id);
    refreshPixmap();
}

QSize Plotter::minimumSizeHint() const
{
    return QSize(6 * leftMargin, 4 * bottomMargin);
}

QSize Plotter::sizeHint() const
{
    return QSize(12 * leftMargin, 8 * bottomMargin);
}

void Plotter::paintEvent(QPaintEvent * /* event */)
{
    QStylePainter painter(this);
//    painter.drawPixmap(PlotRect.left(), PlotRect.right(), pixmap);
    painter.drawPixmap(100, 100, pixmap);

    if (rubberBandIsShown) {
        painter.setPen(palette().light().color());
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
}

void Plotter::resizeEvent(QResizeEvent * /* event */)
{
    int x = width() - (zoomInButton->width()
                       + zoomOutButton->width() + 10);
    zoomInButton->move(x, 5);
    zoomOutButton->move(x + zoomInButton->width() + 5, 5);
    refreshPixmap();
}

void Plotter::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Plotter::mousePressEvent";
    QRect rect(leftMargin, topMargin,
               width() - (leftMargin+rightMargin), height() - (topMargin+bottomMargin));
//    width() - 2 * Margin, height() - 2 * Margin);

    if (event->button() == Qt::LeftButton) {
        qDebug() << "1";
        if (rect.contains(event->pos())) {
            qDebug() << "2";
            rubberBandIsShown = true;
            rubberBandRect.setTopLeft(event->pos());
            rubberBandRect.setBottomRight(event->pos());
            updateRubberBandRegion();
            setCursor(Qt::CrossCursor);
        }
    }
}

void Plotter::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "Plotter::mouseMoveEvent";
    if (rubberBandIsShown) {
        updateRubberBandRegion();
        rubberBandRect.setBottomRight(event->pos());
        updateRubberBandRegion();
    }
}

void Plotter::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && rubberBandIsShown) {
        rubberBandIsShown = false;
        updateRubberBandRegion();
        unsetCursor();

        QRect rect = rubberBandRect.normalized();
        if (rect.width() < 4 || rect.height() < 4)
            return;
        rect.translate(-leftMargin, -bottomMargin);

        PlotSettings prevSettings = zoomStack[curZoom];
        PlotSettings settings;
//        double dx = prevSettings.spanX() / (width() - 2 * Margin);
  //      double dy = prevSettings.spanY() / (height() - 2 * Margin);
        double dx = prevSettings.spanX() / (width() - (leftMargin+rightMargin));
        double dy = prevSettings.spanY() / (height() - (topMargin+bottomMargin));
        settings.minX = prevSettings.minX + dx * rect.left();
        settings.maxX = prevSettings.minX + dx * rect.right();
        settings.minY = prevSettings.maxY - dy * rect.bottom();
        settings.maxY = prevSettings.maxY - dy * rect.top();
        settings.adjust();

        zoomStack.resize(curZoom + 1);
        zoomStack.append(settings);
        zoomIn();
    }
}

void Plotter::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
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

void Plotter::wheelEvent(QWheelEvent *event)
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

void Plotter::updateRubberBandRegion()
{
    QRect rect = rubberBandRect.normalized();
    qDebug() << rect;
    update(rect.left(), rect.top(), rect.width(), 1);
    update(rect.left(), rect.top(), 1, rect.height());
    update(rect.left(), rect.bottom(), rect.width(), 1);
    update(rect.right(), rect.top(), 1, rect.height());
}

void Plotter::refreshPixmap()
{
    pixmap = QPixmap(size());
    //pixmap.fill(this, 0, 0);

    QPainter painter(&pixmap);
    painter.initFrom(this);
    drawGrid(&painter);
    drawCurves(&painter);
    update();
}

void Plotter::drawGrid(QPainter *painter)
{
//    QRect rect(Margin, Margin,
//               width() - 2 * Margin, height() - 2 * Margin);
    QRect rect(leftMargin+PlotRect.left(), topMargin+PlotRect.top(),
               PlotRect.width() - (leftMargin+rightMargin), PlotRect.height() - (topMargin+bottomMargin));
    if (!rect.isValid())
        return;
    QColor color(Qt::black);
//    QBrush brush(color);
//    p.setBackground(brush);
//    p.eraseRect(m_FillRect);
    painter->fillRect(PlotRect,color);
#if 1
    PlotSettings settings = zoomStack[curZoom];
    QPen quiteDark = palette().dark().color().light();
    QPen light = palette().light().color();
    QString sLabel;

    quiteDark.setStyle(Qt::DotLine);
    for (int i = 0; i <= settings.numXTicks; ++i) {
        int x = rect.left() + (i * (rect.width() - 1)
                                 / settings.numXTicks);
        double label = settings.minX + (i * settings.spanX()
                                          / settings.numXTicks);

        char buf[20];

        ::snprintf(buf, 20, ":%02d", (int)label%60);
        sLabel = QString::number(label/60) + QString::fromUtf8(buf);

        painter->setPen(quiteDark);
        painter->drawLine(x, rect.top(), x, rect.bottom());
        painter->setPen(light);
        painter->drawLine(x, rect.bottom(), x, rect.bottom() + 5);
        painter->drawText(x - 50, rect.bottom() + 5, 100, 20,
                          Qt::AlignHCenter | Qt::AlignTop,
                          sLabel);
    }
    for (int j = 0; j <= settings.numYTicks; ++j) {
        int y = rect.bottom() - (j * (rect.height() - 1)
                                   / settings.numYTicks);
        double label = settings.minY + (j * settings.spanY()
                                          / settings.numYTicks);
        painter->setPen(quiteDark);
        painter->drawLine(rect.left(), y, rect.right(), y);
        painter->setPen(light);
        painter->drawLine(rect.left() - 5, y, rect.left(), y);
        painter->drawText(rect.left() - leftMargin, y - 10, leftMargin - 5, 20,
                          Qt::AlignRight | Qt::AlignVCenter,
                          QString::number((int)label));
    }
    painter->drawRect(rect.adjusted(0, 0, -1, -1));
   #endif
    painter->save();
    //painter->translate(-100,100);
    QFont TextFont;			// 객체 폰트
    TextFont.setBold(true);
    TextFont.setPointSize(15);
    painter->setFont (TextFont);

    //painter->drawText(200,rect.bottom()+40,"Demand Time");
    painter->drawText(rect.left(), rect.bottom()+20, rect.right()-rect.left(), 30,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      "Demand Time");

    painter->rotate(270);
    //painter->drawText(-300,20,"Demand Power[kW]");
    painter->drawText(-(rect.bottom()), 0, rect.bottom()-rect.top(), 30,
                      Qt::AlignHCenter | Qt::AlignVCenter,
                      "Demand Power[kW]");
    painter->restore();
}

void Plotter::drawCurves(QPainter *painter)
{
    static const QColor colorForIds[6] = {
        Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow
    };
    PlotSettings settings = zoomStack[curZoom];
//    QRect rect(Margin, Margin,
//               width() - 2 * Margin, height() - 2 * Margin);
    QRect rect(leftMargin+PlotRect.left(), topMargin+PlotRect.top(),
               PlotRect.width() - (leftMargin+rightMargin), PlotRect.height() - (topMargin+bottomMargin));
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
        for (int j = 0; j < data.count(); ++j) {
            double dx = data[j].x() - settings.minX;
            double dy = data[j].y() - settings.minY;
            double x = rect.left() + (dx * (rect.width() - 1)
                                         / settings.spanX());
            double y = rect.bottom() - (dy * (rect.height() - 1)
                                           / settings.spanY());
            polyline[j] = QPointF(x, y);
        }
        painter->setPen(colorForIds[uint(id) % 6]);
        painter->drawPolyline(polyline);
    }
  #endif
}

PlotSettings::PlotSettings()
{
    minX = 0.0;
    maxX = 900.0;   //10.0
    numXTicks = 15;

    minY = 0.0;
    maxY = 3000.0;  //10.0
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
    adjustAxis(minX, maxX, numXTicks);
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

void Plotter::draw(QPainter &painter /* event */)
{
//    QStylePainter painter(d);
    //QPainter painter(d);
  //  painter.drawPixmap(0, 0, pixmap);
        painter.drawPixmap(PlotRect.left(), PlotRect.right(), pixmap);

    setPlotSettings(PlotSettings());

    drawGrid(&painter);
    drawCurves(&painter);

/*
    if (rubberBandIsShown) {
        painter.setPen(palette().light().color());
        painter.drawRect(rubberBandRect.normalized()
                                       .adjusted(0, 0, -1, -1));
    }

    if (hasFocus()) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        option.backgroundColor = palette().dark().color();
        painter.drawPrimitive(QStyle::PE_FrameFocusRect, option);
    }
*/
}
