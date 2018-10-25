#ifndef TIMELINECHART_H
#define TIMELINECHART_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPen>
#include <math.h>
#include <float.h>

#include <QDebug>

enum class CHART_RENDER_TYPE{DEFAULT, COUNT, BAR_CODE, FLOATING_BAR, GROUNDED_BAR};

class EntityChart;
class TimelineChart : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NONE, PAN, RUBBERBAND};

    explicit TimelineChart(QWidget* parent = 0);

    void setMin(double min);
    void setMax(double max);
    void setRange(double min, double max);

    void setAxisWidth(double width);
    void setPointsWidth(double width);

    void addEntityChart(EntityChart* chart);
    void insertEntityChart(int index, EntityChart* chart);
    void removeEntityChart(EntityChart* chart);

    const QList<EntityChart*>& getEntityCharts();
    const QRectF& getHoverRect();

    QPair<double, double> getRange();

    void initialRangeSet();
    bool isRangeSet();
    bool isPanning();

    qint64 mapPixelToTime(double pixel_x);

signals:
    void zoomed(int delta);
    void panned(double dx, double dy);
    void changeDisplayedRange(double min, double max);
    void rangeChanged(double min, double max, bool updateDisplay = false);

    void entityChartHovered(EntityChart* chart, bool hovered);
    void hoverLineUpdated(bool visible, QPointF pos);

public slots:
    void themeChanged();

    void setEntityChartHovered(EntityChart* chart, bool hovered);
    void entityChartRangeChanged(double min, double max);

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
    void hoverRectUpdated();

    void clearDragMode();
    double mapToRange(double value);

    double _dataMin = DBL_MAX;
    double _dataMax = DBL_MIN;
    double _displayMin;
    double _displayMax;

    double axisWidth;
    double pointsWidth;

    DRAG_MODE dragMode = NONE;
    bool hovered = false;
    bool rangeSet = false;

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
