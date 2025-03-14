#ifndef TMSTRENDDIALOG_H
#define TMSTRENDDIALOG_H

#include <QMap>
#include <QPixmap>
#include <QVector>
#include <QDialog>
#include <QtGui>
#include <QLabel>
#include <QDateTime>
#include <QJsonValue>
#include "mondialog.h"
//#include "plotter.h"
class QToolButton;
class PlotSettings;

namespace Ui {
class TmsTrendDialog;
}

class TmsTrendDialog : public CMonDialog
{
    Q_OBJECT

public:
    explicit TmsTrendDialog(QWidget *parent = 0);
    ~TmsTrendDialog();
    void setPlotSettings(const PlotSettings &settings);
    void setCurveData(int id, const QVector<QPointF> &data);
    void clearCurve(int id);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    bool viewportEvent(QEvent *event);
    void onRead(QString& cmd, QJsonObject& jobject);

public slots:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void readFlightCurves(const QString &fileName);

private slots:
    void on_btnDateTime_clicked();
    void on_btnZoomIn_clicked();
    void on_btnZoomOut_clicked();
    void on_btnTag1_clicked();
    void on_btnTag2_clicked();
    void on_btnTag3_clicked();
    void on_btnTag4_clicked();
    void on_btnTag5_clicked();
    void on_btnTag6_clicked();
    void on_btnSelectAll_clicked();
    void on_btnNext_clicked();
    void on_btnPrev_clicked();

private:
    Ui::TmsTrendDialog          *ui;
    void updateRubberBandRegion();
    void refreshPixmap();
    void drawGrid(QPainter *painter);
    void drawCurves(QPainter *painter);
    void LabelDisplay(QLabel* label, int col);
    void SetEnabled(int tag);
    void SendCommand();
    int CodeIndex(QString code);

    enum { leftMargin = 70, rightMargin = 20,topMargin = 20,bottomMargin = 50 };

    QMap<int, QVector<QPointF> > curveMap;
    QVector<PlotSettings> zoomStack;
    int curZoom;
    bool rubberBandIsShown;
    QRect rubberBandRect;
    QPixmap pixmap;
    QRect PlotRect;
    QRect GraphAbsoluteRect;
    QRect GraphRelativeRect;
    bool   m_bSelected[8];
    QLabel *m_TagLabel[6];
    QLabel *m_DescLabel[6];
    QToolButton *m_Button[6];
    QDateTime mDateTime;
};

class PlotSettings
{
public:
    PlotSettings();

    void scroll(int dx, int dy);
    void adjust();
    double spanX() const { return maxX - minX; }
    double spanY() const { return maxY - minY; }

    double minX;
    double maxX;
    int numXTicks;
    double minY;
    double maxY;
    int numYTicks;

private:
    static void adjustAxis(double &min, double &max, int &numTicks);
};

#endif // TMSTRENDDIALOG_H
