#ifndef TIMELINECHART_H
#define TIMELINECHART_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPen>
#include <math.h>
#include <float.h>

#include <QDebug>

enum class CHART_RENDER_TYPE{DEFAULT, COUNT, BAR_CODE, FLOATING_BAR, GROUNDED_BAR};

static const QString DATE_TIME_FORMAT = "MMM d, hh:mm:ss.zzz";
static const QString TIME_FORMAT = "hh:mm:ss.zzz";
static const QString DATE_FORMAT = "MMM d";

class EntityChart;
class TimelineChart : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NONE, PAN, RUBBERBAND};

    explicit TimelineChart(QWidget* parent = 0);

    void setAxisXVisible(bool visible);
    void setAxisYVisible(bool visible);
    void setAxisWidth(double width);

    void addEntityChart(EntityChart* chart);
    void insertEntityChart(int index, EntityChart* chart);
    void removeEntityChart(EntityChart* chart);

    const QList<EntityChart*>& getEntityCharts() const;
    const QRectF& getHoverRect() const;

    bool isPanning() const;

signals:
    void panning(bool _panning);

    void zoomed(int delta);
    void panned(double dx, double dy);
    void rubberbandUsed(double left, double right);

    void entityChartHovered(EntityChart* chart, bool hovered_);
    void hoverLineUpdated(bool visible, QPointF pos);

public slots:
    void themeChanged();

    void setEntityChartHovered(EntityChart* chart, bool hovered_);

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

    double axisWidth_;

    bool axisXVisible_ = false;
    bool axisYVisible_ = false;

    DRAG_MODE dragMode_ = NONE;
    bool hovered_ = false;
    bool rangeSet_ = false;

    QColor backgroundColor_;
    QColor backgroundHighlightColor_;
    QColor highlightColor_;
    QColor hoveredRectColor_;

    QPen axisLinePen_;
    QPen topLinePen_;
    QPen hoverLinePen_;
    QPen cursorPen_;

    QPointF cursorPoint_;
    QPointF panOrigin_;
    QRectF rubberBandRect_;
    QRectF hoveredChartRect_;
    QRectF hoverRect_;

    QVBoxLayout* layout_;
    QList<EntityChart*> entityCharts_;

    QPointF hoverCursor_;

};

#endif // TIMELINECHART_H
