#include "timelinechart.h"
#include "../../../../theme.h"
#include "entitychart.h"

#include <QPainter>
#include <QWheelEvent>
#include <QWidgetAction>
#include <QToolButton>

#define BACKGROUND_OPACITY 0.25
#define HOVER_LINE_WIDTH 2.0


/**
 * @brief TimelineChart::TimelineChart
 * @param parent
 */
TimelineChart::TimelineChart(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::WheelFocus);

    layout_ = new QVBoxLayout(this);
    layout_->setMargin(0);
    layout_->setSpacing(0);

    hoverRect_ = QRectF(0, 0, HOVER_LINE_WIDTH, height());

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimelineChart::themeChanged);
    themeChanged();
}


/**
 * @brief TimelineChart::setAxisXVisible
 * @param visible
 */
void TimelineChart::setAxisXVisible(bool visible)
{
    axisXVisible_ = visible;
}


/**
 * @brief TimelineChart::setAxisYVisible
 * @param visible
 */
void TimelineChart::setAxisYVisible(bool visible)
{
    axisYVisible_ = visible;
}


/**
 * @brief TimelineChart::setAxisWidth
 * @param width
 */
void TimelineChart::setAxisWidth(double width)
{
    axisWidth_ = width;
    axisLinePen_.setWidthF(width);
    setContentsMargins(width, 0, 0, 0);
}


/**
 * @brief TimelineChartWidget::addEntityChart
 * @param chart
 */
void TimelineChart::addEntityChart(EntityChart* chart)
{
    insertEntityChart(-1, chart);
}


/**
 * @brief TimelineChart::insertEntityChart
 * @param index
 * @param chart
 */
void TimelineChart::insertEntityChart(int index, EntityChart* chart)
{
    if (chart) {
        entityCharts_.append(chart);
        layout_->insertWidget(index, chart);
        chart->installEventFilter(this);
    }
}


/**
 * @brief TimelineChart::removeEntityChart
 * @param chart
 */
void TimelineChart::removeEntityChart(EntityChart* chart)
{
    if (chart) {
        layout_->removeWidget(chart);
        entityCharts_.removeAll(chart);
    }
}


/**
 * @brief TimelineChart::getEntityCharts
 * @return
 */
const QList<EntityChart*>& TimelineChart::getEntityCharts() const
{
    return entityCharts_;
}


/**
 * @brief TimelineChart::getHoverRect
 * @return
 */
const QRectF& TimelineChart::getHoverRect() const
{
    return hoverRect_;
}


/**
 * @brief TimelineChart::isPanning
 * @return
 */
bool TimelineChart::isPanning() const
{
    return dragMode_ == DRAG_MODE::PAN || dragMode_ == DRAG_MODE::RUBBERBAND;
}


/**
 * @brief TimelineChart::themeChanged
 */
void TimelineChart::themeChanged()
{
    Theme* theme = Theme::theme();
    highlightColor_ = theme->getHighlightColor();
    hoveredRectColor_ = theme->getBackgroundColor();

    backgroundColor_ = theme->getAltBackgroundColor();
    backgroundColor_.setAlphaF(BACKGROUND_OPACITY);
    backgroundHighlightColor_ = theme->getActiveWidgetBorderColor();
    backgroundHighlightColor_.setAlphaF(BACKGROUND_OPACITY * 2.0);

    cursorPen_ = QPen(theme->getTextColor(), 8);
    axisLinePen_ = QPen(theme->getAltTextColor(), axisWidth_);
    topLinePen_ = QPen(theme->getAltTextColor(), 1.5);
    hoverLinePen_ = QPen(theme->getTextColor(), HOVER_LINE_WIDTH, Qt::PenStyle::DotLine);
}


/**
 * @brief TimelineChart::setEntityChartHovered
 * This is called
 * @param chart
 * @param hovered
 */
void TimelineChart::setEntityChartHovered(EntityChart* chart, bool hovered)
{
    if (chart) {
        hoveredChartRect_ = hovered ? chart->rect() : QRectF();
        hoveredChartRect_.moveTo(chart->pos());
        chart->setHovered(hovered);
    } else {
        hoveredChartRect_ = QRectF();
    }
    update();
}


/**
 * @brief TimelineChart::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool TimelineChart::eventFilter(QObject* watched, QEvent *event)
{
    // the signal below is used to tell the entity axis which chart was hovered
    // it's not sent from the setEntityChartHovered to avoid duplicated signals
    // because the entity axis uses that slot to hover on a chart
    EntityChart* chart = qobject_cast<EntityChart*>(watched);
    if (event->type() == QEvent::Enter) {
        setEntityChartHovered(chart, true);
        emit entityChartHovered(chart, true);
    } else if (event->type() == QEvent::Leave) {
        setEntityChartHovered(chart, false);
        emit entityChartHovered(chart, false);
    }
    return QWidget::eventFilter(watched, event);
}


/**
 * @brief TimelineChart::mousePressEvent
 * @param event
 */
void TimelineChart::mousePressEvent(QMouseEvent *event)
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
 * @brief TimelineChart::mouseReleaseEvent
 * @param event
 */
void TimelineChart::mouseReleaseEvent(QMouseEvent* event)
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
 * @brief TimelineChart::mouseMoveEvent
 * @param event
 */
void TimelineChart::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    cursorPoint_ = mapFromGlobal(cursor().pos());
    hoverRect_.moveCenter(cursorPoint_);
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
 * @brief TimelineChart::wheelEvent
 * @param event
 */
void TimelineChart::wheelEvent(QWheelEvent *event)
{
    // need to accept the event here so that it doesn't scroll the timeline chart
    emit zoomed(event->delta());
    event->accept();
}


/**
 * @brief TimelineChart::keyReleaseEvent
 * @param event
 */
void TimelineChart::keyReleaseEvent(QKeyEvent *event)
{
    if (dragMode_ == RUBBERBAND) {
        if (event->key() == Qt::Key_Control) {
            clearDragMode();
        }
    }
    QWidget::keyReleaseEvent(event);
}


/**
 * @brief TimelineChart::enterEvent
 * @param event
 */
void TimelineChart::enterEvent(QEvent *event)
{
    QWidget::enterEvent(event);
    setCursor(Qt::BlankCursor);
    hovered_ = true;

    hoverRect_ = visibleRegion().boundingRect();
    hoverRect_.setWidth(HOVER_LINE_WIDTH);
    hoverRectUpdated();
}


/**
 * @brief TimelineChart::leaveEvent
 * @param event
 */
void TimelineChart::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    hovered_ = false;
    hoverRect_ = QRectF();
    hoverRectUpdated();
}


/**
 * @brief TimelineChart::paintEvent
 * @param event
 */
void TimelineChart::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect visibleRect = visibleRegion().boundingRect();
    painter.fillRect(visibleRect, backgroundColor_);

    visibleRect = visibleRect.adjusted(axisWidth_ / 2.0, 0, 0, 0);

    painter.setPen(axisLinePen_);
    if (axisXVisible_) {
        QLineF axisX(visibleRect.bottomLeft(), visibleRect.bottomRight());
        painter.drawLine(axisX);
    }
    if (axisYVisible_) {
        QLineF axisY(visibleRect.topLeft(), visibleRect.bottomLeft());
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
            if (!hoveredChartRect_.isNull()) {
                // highlight the hovered child rect's background
                painter.setPen(axisLinePen_);
                painter.setBrush(backgroundHighlightColor_);
                painter.drawRect(hoveredChartRect_.adjusted(-axisLinePen_.widthF(), 0, axisLinePen_.widthF(), 0));
            }
            // paint the hover line
            painter.setPen(hoverLinePen_);
            painter.drawLine(cursorPoint_.x(), rect().top(), cursorPoint_.x(), rect().bottom());
        }
        break;
    }
}


/**
 * @brief TimelineChart::hoverRectUpdated
 * @param repaintRequired
 */
void TimelineChart::hoverRectUpdated(bool repaintRequired)
{
    if (hoverRect_.isNull()) {
        for (EntityChart* chart : entityCharts_) {
            chart->setHoveredRect(hoverRect_);
        }
    } else {
        for (EntityChart* chart : entityCharts_) {
            if (!chart->isVisible()) {
                continue;
            }
            QRect childRect(chart->x(), chart->y(), chart->width(), chart->height());
            if (visibleRegion().contains(childRect)) {
                chart->setHoveredRect(hoverRect_);
            }
        }
    }

    if (repaintRequired) {
        // this repaint is required instead of an update whenever there's a moveEvent
        // the hovered series ranges are being calculated in the children charts' paint event
        // and it needs to happen before the signal below is sent to the timeline view
        repaint();
    } else {
        update();
    }

    emit hoverLineUpdated(hoverRect_.isValid(), mapToGlobal(hoverRect_.center().toPoint()));
}


/**
 * @brief TimelineChart::clearDragMode
 */
void TimelineChart::clearDragMode()
{
    dragMode_ = NONE;
    rubberBandRect_ = QRectF();
    setCursor(Qt::BlankCursor);
    update();
}
