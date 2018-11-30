#ifndef TIMELINECHART_H
#define TIMELINECHART_H

#include <QWidget>
#include <QVBoxLayout>
#include <QToolBar>
#include <QToolButton>
#include <QSpinBox>
#include <QAction>
#include <QMenu>
#include <QPen>
#include <math.h>
#include <float.h>

#include <QDebug>

static QString DATETIME_FORMAT = "MMMM d, hh:mm:ss:zzzzz";
static QString TIME_FORMAT = "hh:mm:ss:zzzzz";
static QString DATE_FORMAT = "MMMM d";

class EntityChart;
class TimelineChart : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NO_MODE, PAN_MODE, RUBBERBAND_MODE};

    explicit TimelineChart(QWidget* parent = 0);

    void setMin(double min);
    void setMax(double max);
    void setRange(double min, double max);

    const QRectF& getHoverRect();

    void setAxisWidth(double width);
    void setPointsWidth(double width);

    void addEntityChart(EntityChart* chart);
    void insertEntityChart(int index, EntityChart* chart);
    void removeEntityChart(EntityChart* chart);

    const QList<EntityChart*>& getEntityCharts();

    QPair<double, double> getRange();
    bool isPanning();

    qint64 mapPixelToTime(double pixel_x);

signals:
    void zoomed(int delta);
    void panned(double dx, double dy);
    void changeDisplayedRange(double min, double max);
    void rangeChanged(double min, double max, bool updateDisplay = false);
    void hoverLineUpdated(bool visible, QPointF globalPos);

public slots:
    void themeChanged();

    void entityChartHovered(EntityChart* chart, bool hovered);
    void entityChartRangeChanged(double min, double max);

private slots:
    void setRangeTriggered();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    void hoverRectUpdated(bool repaintRequired = false);

    void clearDragMode();
    double mapToRange(double value);

    double _dataMin = DBL_MAX;
    double _dataMax = DBL_MIN;
    double _displayMin;
    double _displayMax;

    double axisWidth;
    double pointsWidth;

    DRAG_MODE dragMode = NO_MODE;
    bool hovered = false;

    QToolBar* _toolbar;
    QToolBar* t1;
    QToolBar* t2;

    QSpinBox* h1;
    QSpinBox* h2;
    QSpinBox* m1;
    QSpinBox* m2;
    QSpinBox* s1;
    QSpinBox* s2;
    QSpinBox* ms1;
    QSpinBox* ms2;

    QMenu* _setMinMenu;
    QMenu* _setMaxMenu;
    QAction* _showSetMinMenu;
    QAction* _showSetMaxMenu;
    QToolButton* _setMinButton;
    QToolButton* _setMaxButton;

    QColor backgroundColor;
    QColor backgroundHighlightColor;
    QColor highlightColor;
    QColor hoveredRectColor;

    QPen axisLinePen;
    QPen topLinePen;
    QPen hoverLinePen;
    QPen cursorPen;

    QPointF cursorPoint;
    QPointF panOrigin;
    QRectF rubberBandRect;
    QRectF hoveredChartRect;
    QRectF hoverRect;

    QVBoxLayout* _layout;
    QList<EntityChart*> _entityCharts;

    QPointF hover_cursor;

};

#endif // TIMELINECHART_H
