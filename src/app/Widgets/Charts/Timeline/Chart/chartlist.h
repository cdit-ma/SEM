#ifndef CHARTLIST_H
#define CHARTLIST_H

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

namespace MEDEA {

class Chart;

/**
 * @brief The ChartList class holds and displays an ordered list of Charts.
 * This class holds the Chart widgets in order of insertion.
 * It also sends signals to the TimelineChartView whenever the user performs any action that attemps to change the displayed range.
 */
class ChartList : public QWidget
{
    Q_OBJECT

public:
    enum DRAG_MODE{NONE, PAN, RUBBERBAND};

    explicit ChartList(QWidget* parent = nullptr);

    void setAxisXVisible(bool visible);
    void setAxisYVisible(bool visible);
    void setAxisWidth(double width);

    void addChart(Chart* chart);
    void insertChart(int index, Chart* chart);
    void removeChart(Chart* chart);

    const QList<Chart*>& getCharts() const;
    const QRectF& getHoverLineRect() const;

    bool isPanning() const;

signals:
    void panning(bool _panning);

    void zoomed(int delta);
    void panned(double dx, double dy);
    void rubberbandUsed(double left, double right);

    void chartHovered(Chart* chart, bool hovered_);
    void hoverLineUpdated(bool visible, QPointF pos);

public slots:
    void themeChanged();

    void setChartHovered(Chart* chart, bool hovered_);
    void clearHovered();

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
    QRectF hoverLineRect_;

    QVBoxLayout* layout_ = nullptr;
    QList<Chart*> charts_;

    QPointF hoverCursor_;
};

}

#endif // CHARTLIST_H
