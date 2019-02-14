#ifndef TIMELINECHART_H
#define TIMELINECHART_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPen>
#include <math.h>
#include <float.h>

#include <QDebug>

enum class CHART_RENDER_TYPE{DEFAULT, COUNT, BAR_CODE, FLOATING_BAR, GROUNDED_BAR};

static QString DATE_TIME_FORMAT = "MMM d, hh:mm:ss.zzz";
static QString TIME_FORMAT = "hh:mm:ss.zzz";
static QString DATE_FORMAT = "MMM d";

class EntityChart;
class TimelineChart : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NONE, PAN, RUBBERBAND};

    explicit TimelineChart(QWidget* parent = 0);

    // NOTE: The timeline chart doesn't know anything about the data range
    void setRange(double min, double max);

    void setAxisXVisible(bool visible);
    void setAxisYVisible(bool visible);
    void setAxisWidth(double width);

    void addEntityChart(EntityChart* chart);
    void insertEntityChart(int index, EntityChart* chart);
    void removeEntityChart(EntityChart* chart);

    const QList<EntityChart*>& getEntityCharts();
    const QRectF& getHoverRect();

    bool isPanning();

signals:
    void panning(bool _panning);

    void zoomed(int delta);
    void panned(double dx, double dy);
    void rubberbandUsed(double left, double right);

    void entityChartHovered(EntityChart* chart, bool hovered);
    void hoverLineUpdated(bool visible, QPointF pos);

public slots:
    void themeChanged();

    void setEntityChartHovered(EntityChart* chart, bool hovered);

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
    double mapLocalPixelToTime(double pixel);

    double axisWidth;

    bool axisXVisible = false;
    bool axisYVisible = false;

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
