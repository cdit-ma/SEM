#include "chartlist.h"
#include "../../../../theme.h"
#include "chart.h"

#include <QPainter>
#include <QWheelEvent>
#include <QWidgetAction>
#include <QToolButton>

#define BACKGROUND_OPACITY 0.25
#define HOVER_LINE_WIDTH 2.0

using namespace MEDEA;

//QColor ChartList::backgroundColor_ = QColor(0,0,0);

/**
 * @brief ChartList::ChartList
 * @param parent
 */
ChartList::ChartList(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);

    layout_ = new QVBoxLayout(this);
    layout_->setMargin(0);
    layout_->setSpacing(0);

    hoverLineRect_ = QRectF(0, 0, HOVER_LINE_WIDTH, height());

    connect(Theme::theme(), &Theme::theme_Changed, this, &ChartList::themeChanged);
    themeChanged();
}


/**
 * @brief ChartList::setAxisXVisible
 * @param visible
 */
void ChartList::setAxisXVisible(bool visible)
{
    axisXVisible_ = visible;
}


/**
 * @brief ChartList::setAxisYVisible
 * @param visible
 */
void ChartList::setAxisYVisible(bool visible)
{
    axisYVisible_ = visible;
}


/**
 * @brief ChartList::setAxisWidth
 * @param width
 */
void ChartList::setAxisWidth(double width)
{
    axisWidth_ = width;
    axisLinePen_.setWidthF(width);
    setContentsMargins(width, 0, 0, 0);
}


/**
 * @brief ChartListWidget::addChart
 * @param chart
 */
void ChartList::addChart(Chart* chart)
{
    insertChart(-1, chart);
}


/**
 * @brief ChartList::insertChart
 * @param index
 * @param chart
 */
void ChartList::insertChart(int index, Chart* chart)
{
    if (chart) {
        charts_.append(chart);
        layout_->insertWidget(index, chart);
        chart->installEventFilter(this);
    }
}


/**
 * @brief ChartList::removeChart
 * @param chart
 */
void ChartList::removeChart(Chart* chart)
{
    if (chart) {
        layout_->removeWidget(chart);
        charts_.removeAll(chart);
    }
}


/**
 * @brief ChartList::getCharts
 * @return
 */
const QList<Chart*>& ChartList::getCharts() const
{
    return charts_;
}


/**
 * @brief ChartList::getHoverLineRect
 * @return
 */
const QRectF& ChartList::getHoverLineRect() const
{
    return hoverLineRect_;
}


/**
 * @brief ChartList::isPanning
 * @return
 */
bool ChartList::isPanning() const
{
    return dragMode_ == DRAG_MODE::PAN || dragMode_ == DRAG_MODE::RUBBERBAND;
}


/**
 * @brief ChartList::themeChanged
 */
void ChartList::themeChanged()
{
    Theme* theme = Theme::theme();
    highlightColor_ = theme->getHighlightColor();
    hoveredRectColor_ = theme->getBackgroundColor();

    backgroundColor_ = theme->getAltBackgroundColor();
    backgroundColor_.setAlphaF(BACKGROUND_OPACITY);

    cursorPen_ = QPen(theme->getTextColor(), 8);
    axisLinePen_ = QPen(theme->getAltTextColor(), axisWidth_);
    topLinePen_ = QPen(theme->getAltTextColor(), 1.5);
    hoverLinePen_ = QPen(theme->getTextColor(), HOVER_LINE_WIDTH, Qt::PenStyle::DotLine);
}


/**
 * @brief ChartList::setChartHovered
 * This is called when the cursor was moved over/off the provided chart.
 * It is also called when the provided chart's label was hovered on/off.
 * @param chart
 * @param hovered
 */
void ChartList::setChartHovered(Chart* chart, bool hovered)
{
    if (chart) {
        // the signal below is used to tell the TimelineChartView which chart was hovered on/off
        // it sets the corresponding ChartLabel's hovered state
        emit chartHovered(chart, hovered);
        chart->setHovered(hovered);
    }
    update();
}


void ChartList::clearHovered()
{
    hovered_ = false;
    update();
}



/**
 * @brief ChartList::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool ChartList::eventFilter(QObject* watched, QEvent *event)
{
    Chart* chart = qobject_cast<Chart*>(watched);
    if (event->type() == QEvent::Enter) {
        setChartHovered(chart, true);
    } else if (event->type() == QEvent::Leave) {
        setChartHovered(chart, false);
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief ChartList::mousePressEvent
 * @param event
 */
void ChartList::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        dragMode_ = PAN;
        panOrigin_ = event->pos();
        setCursor(Qt::ClosedHandCursor);
        // if the CTRL key is down, go into rubberband mode
        if (event->modifiers() & Qt::ControlModifier) {
            dragMode_ = RUBBERBAND;
            setCursor(Qt::CrossCursor);
        }
    }
    QWidget::mousePressEvent(event);
}


/**
 * @brief ChartList::mouseReleaseEvent
 * @param event
 */
void ChartList::mouseReleaseEvent(QMouseEvent* event)
{
    if (dragMode_ == RUBBERBAND && !rubberBandRect_.isNull()) {
        // send a signal to update the axis' displayed range
        emit rubberbandUsed(rubberBandRect_.left(), rubberBandRect_.right());
    }

    // this is only here to demo that the hover axis dislay's position can be set manually
    //emit hoverLineUpdated(hoverRect.isValid(), QPointF(0, 0));

    emit panning(false);
    clearDragMode();
    QWidget::mouseReleaseEvent(event);
}


/**
 * @brief ChartList::mouseMoveEvent
 * @param event
 */
void ChartList::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    cursorPoint_ = mapFromGlobal(cursor().pos());
    hoverLineRect_.moveCenter(cursorPoint_);
    hoverRectUpdated(true);

    switch (dragMode_) {
    case PAN: {
        QPointF delta = cursorPoint_ - panOrigin_;
        panOrigin_ = cursorPoint_;
        emit panning(true);
        emit panned(-delta.x(), 0);
        break;
    }
    case RUBBERBAND:
        rubberBandRect_ = QRectF(panOrigin_.x(), 0, event->pos().x() - panOrigin_.x(), height());
        break;
    default:
        break;
    }
}


/**
 * @brief ChartList::wheelEvent
 * @param event
 */
void ChartList::wheelEvent(QWheelEvent *event)
{
    // need to accept the event here so that it doesn't scroll the timeline chart
    emit zoomed(event->delta());
    event->accept();
}


/**
 * @brief ChartList::keyReleaseEvent
 * @param event
 */
void ChartList::keyReleaseEvent(QKeyEvent *event)
{
    if (dragMode_ == RUBBERBAND) {
        if (event->key() == Qt::Key_Control) {
            clearDragMode();
        }
    }
    QWidget::keyReleaseEvent(event);
}


/**
 * @brief ChartList::enterEvent
 * @param event
 */
void ChartList::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setCursor(Qt::BlankCursor);
    hovered_ = true;

    hoverLineRect_ = visibleRegion().boundingRect();
    hoverLineRect_.setWidth(HOVER_LINE_WIDTH);
    hoverRectUpdated();
}


/**
 * @brief ChartList::leaveEvent
 * @param event
 */
void ChartList::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    hovered_ = false;
    hoverLineRect_ = QRectF();
    hoverRectUpdated();
    cursorPoint_.setX(-10);
}


/**
 * @brief ChartList::paintEvent
 * @param event
 */
void ChartList::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    QRect visibleRect = visibleRegion().boundingRect();
    painter.fillRect(visibleRect, backgroundColor_);

    painter.setPen(axisLinePen_);
    if (axisXVisible_) {
        QLineF axisX(visibleRect.bottomLeft(), visibleRect.bottomRight());
        painter.drawLine(axisX);
    }
    if (axisYVisible_) {
        QLineF axisY(visibleRect.topLeft(), visibleRect.bottomLeft());
        axisY.translate(axisWidth_ / 2.0, 0);
        painter.drawLine(axisY);
    }

    switch (dragMode_) {
    case RUBBERBAND: {
        painter.setOpacity(0.25);
        painter.setPen(QPen(highlightColor_.darker(), 2));
        painter.setBrush(highlightColor_);
        painter.drawRect(rubberBandRect_);
        break;
    }
    default:
        if (hovered_) {
            // paint the hover line
            painter.setPen(hoverLinePen_);
            painter.drawLine(cursorPoint_.x(), rect().top(), cursorPoint_.x(), rect().bottom());
        }
        break;
    }
}


/**
 * @brief ChartList::hoverRectUpdated
 * @param repaintRequired
 */
void ChartList::hoverRectUpdated(bool repaintRequired)
{
    /*
    if (hoverLineRect_.isNull()) {
        for (Chart* chart : charts_) {
            chart->setHoveredRect(hoverLineRect_);
        }
    } else {
        for (Chart* chart : charts_) {
            if (!chart->isVisible()) {
                continue;
            }
            QRect childRect(chart->x(), chart->y(), chart->width(), chart->height());
            if (visibleRegion().contains(childRect)) {
                chart->setHoveredRect(hoverLineRect_);
            }
        }
    }
     */

    for (auto chart : charts_) {
        if (chart->isVisible()) {
            if (visibleRegion().intersects(chart->geometry())) {
                chart->setHoveredRect(hoverLineRect_);
            }
        }
    }

    if (repaintRequired) {
        // NOTE: This repaint is required instead of an update whenever there's a moveEvent
        //  The hoveredSeriesTimeRange_ is populated in the children charts' paintEvent and needs to happen before the signal below is sent
        //  The slot connected to the signal in the TimelineChartView is what updates the hover display
        repaint();
    } else {
        update();
    }

    // Send a signal to the AxisWidget and the TimelineChartVIew notifying them of the change in hover line
    emit hoverLineUpdated(hoverLineRect_.isValid(), mapToGlobal(hoverLineRect_.center().toPoint()));
}


/**
 * @brief ChartList::clearDragMode
 */
void ChartList::clearDragMode()
{
    dragMode_ = NONE;
    rubberBandRect_ = QRectF();
    setCursor(Qt::BlankCursor);
    update();
}
