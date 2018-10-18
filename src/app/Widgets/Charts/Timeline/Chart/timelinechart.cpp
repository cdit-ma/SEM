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
    setFocusPolicy(Qt::StrongFocus);

    _layout = new QVBoxLayout(this);
    _layout->setMargin(0);
    _layout->setSpacing(0);

    _toolbar = new QToolBar(this);
    _toolbar->setWindowFlags(_toolbar->windowFlags() | Qt::Popup);
    _showSetMinMenu = _toolbar->addAction("Set minimum time");
    _showSetMaxMenu = _toolbar->addAction("Set maximum time");

    ((QToolButton*)_toolbar->widgetForAction(_showSetMinMenu))->setPopupMode(QToolButton::InstantPopup);
    ((QToolButton*)_toolbar->widgetForAction(_showSetMaxMenu))->setPopupMode(QToolButton::InstantPopup);

    /*
     * SET UP MENUS AND SPIN BOXES
     */

    h1 = new QSpinBox(this);
    h1->setSuffix(" h");
    h1->setRange(0, 23);
    h2 = new QSpinBox(this);
    h2->setSuffix(" h");
    h2->setRange(0, 23);
    m1 = new QSpinBox(this);
    m1->setSuffix(" m");
    h1->setRange(0, 59);
    m2 = new QSpinBox(this);
    m2->setSuffix(" m");
    m2->setRange(0, 59);
    s1 = new QSpinBox(this);
    s1->setSuffix(" s");
    s1->setRange(0, 59);
    s2 = new QSpinBox(this);
    s2->setSuffix(" s");
    s2->setRange(0, 59);
    ms1 = new QSpinBox(this);
    ms1->setSuffix(" ms");
    ms1->setRange(0, 999);
    ms2 = new QSpinBox(this);
    ms2->setSuffix(" ms");
    ms2->setRange(0, 999);

    /*
    h1->setFocusProxy(this);
    h1->setFocusPolicy(Qt::StrongFocus);
    m1->setFocusProxy(this);
    m1->setFocusPolicy(Qt::StrongFocus);

    /*h1->setFocusPolicy(Qt::StrongFocus);
    m1->setFocusPolicy(Qt::StrongFocus);
    s1->setFocusPolicy(Qt::StrongFocus);
    ms1->setFocusPolicy(Qt::StrongFocus);*/

    _setMinButton = new QToolButton(this);
    _setMinButton->setToolTip("Set time");
    t1 = new QToolBar(this);
    t1->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    t1->addWidget(_setMinButton);

    _setMaxButton = new QToolButton(this);
    _setMaxButton->setToolTip("Set time");
    t2 = new QToolBar(this);
    t2->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    t2->addWidget(_setMaxButton);

    connect(_setMinButton, &QToolButton::clicked, this, &TimelineChart::setRangeTriggered);
    connect(_setMaxButton, &QToolButton::clicked, this, &TimelineChart::setRangeTriggered);

    QWidget* widget1 = new QWidget(this);
    QHBoxLayout* layout1 = new QHBoxLayout(widget1);
    layout1->setMargin(5);
    layout1->setSpacing(5);
    layout1->addWidget(h1);
    layout1->addWidget(m1);
    layout1->addWidget(s1);
    layout1->addWidget(ms1);
    layout1->addWidget(t1);

    QWidget* widget2 = new QWidget(this);
    QHBoxLayout* layout2 = new QHBoxLayout(widget2);
    layout2->setMargin(5);
    layout2->setSpacing(5);
    layout2->addWidget(h2);
    layout2->addWidget(m2);
    layout2->addWidget(s2);
    layout2->addWidget(ms2);
    layout2->addWidget(t2);

    QWidgetAction* minAction = new QWidgetAction(this);
    minAction->setDefaultWidget(widget1);
    QWidgetAction* maxAction = new QWidgetAction(this);
    maxAction->setDefaultWidget(widget2);

    _setMinMenu = new QMenu(this);
    _setMinMenu->addAction(minAction);
    _showSetMinMenu->setMenu(_setMinMenu);

    _setMaxMenu = new QMenu(this);
    _setMaxMenu->addAction(maxAction);
    _showSetMaxMenu->setMenu(_setMaxMenu);

    hoverRect = QRectF(0, 0, HOVER_LINE_WIDTH, height());

    connect(Theme::theme(), &Theme::theme_Changed, this, &TimelineChart::themeChanged);
    themeChanged();
}


/**
 * @brief TimelineChart::setMin
 * @param min
 */
void TimelineChart::setMin(double min)
{
    if (min != _displayMin) {
        QDateTime dt; dt.setMSecsSinceEpoch(min);
        for (EntityChart* chart : _entityCharts) {
            chart->setMin(min);
        }
        _displayMin = min;
        emit changeDisplayedRange(_displayMin, _displayMax);
    }
}


/**
 * @brief TimelineChart::setMax
 * @param max
 */
void TimelineChart::setMax(double max)
{
    if (max != _displayMax) {
        QDateTime dt; dt.setMSecsSinceEpoch(max);
        for (EntityChart* chart : _entityCharts) {
            chart->setMax(max);
        }
        _displayMax = max;
        emit changeDisplayedRange(_displayMin, _displayMax);
    }
}


/**
 * @brief TimelineChart::setRange
 * @param min
 * @param max
 */
void TimelineChart::setRange(double min, double max)
{
    for (EntityChart* chart : _entityCharts) {
        chart->setRange(min, max);
    }
    _displayMin = min;
    _displayMax = max;
    emit changeDisplayedRange(_displayMin, _displayMax);
}


/**
 * @brief TimelineChart::setAxisWidth
 * @param width
 */
void TimelineChart::setAxisWidth(double width)
{
    axisWidth = width;
    axisLinePen.setWidthF(width);
    setContentsMargins(width, 0, 0, 0);
}


/**
 * @brief TimelineChart::setPointsWidth
 * @param width
 */
void TimelineChart::setPointsWidth(double width)
{
    for (EntityChart* chart : _entityCharts) {
        chart->setPointWidth(width);
    }
    pointsWidth = width;
}


/**
 * @brief TimelineChartWidget::addEntityChart
 * @param chart
 */
void TimelineChart::addEntityChart(EntityChart* chart)
{
    insertEntityChart(-1, chart);
}


const QRectF& TimelineChart::getHoverRect()
{
    return hoverRect;
}


quint64 TimelineChart::mapPixelToTime(double pixel_x)
{
    auto offset = pixel_x / width();
    auto delta = _displayMax - _displayMin;
    return _displayMin + (delta * offset);
}


/**
 * @brief TimelineChart::insertEntityChart
 * @param index
 * @param chart
 */
void TimelineChart::insertEntityChart(int index, EntityChart* chart)
{
    if (chart) {
        _entityCharts.append(chart);
        _layout->insertWidget(index, chart);
        chart->setPointWidth(pointsWidth);
        chart->setRange(_displayMin, _displayMax);
        chart->installEventFilter(this);
        entityChartRangeChanged(chart->getRangeX().first, chart->getRangeX().second);
        connect(chart, &EntityChart::dataRangeXChanged, this, &TimelineChart::entityChartRangeChanged);
    }
}


/**
 * @brief TimelineChart::removeEntityChart
 * @param chart
 */
void TimelineChart::removeEntityChart(EntityChart* chart)
{
    if (chart) {
        _layout->removeWidget(chart);
        _entityCharts.removeAll(chart);
    }
}


/**
 * @brief TimelineChart::getRange
 * @return
 */
QPair<double, double> TimelineChart::getRange()
{
    return {_displayMin, _displayMax};
}


/**
 * @brief TimelineChart::initialRangeSet
 */
void TimelineChart::initialRangeSet()
{
    rangeSet = true;
}


/**
 * @brief TimelineChart::isRangeSet
 * @return
 */
bool TimelineChart::isRangeSet()
{
    return rangeSet;
}


/**
 * @brief TimelineChart::isPanning
 * @return
 */
bool TimelineChart::isPanning()
{
    return dragMode == DRAG_MODE::PAN || dragMode == DRAG_MODE::RUBBERBAND;
}


/**
 * @brief TimelineChart::themeChanged
 */
void TimelineChart::themeChanged()
{
    Theme* theme = Theme::theme();
    highlightColor = theme->getHighlightColor();
    hoveredRectColor = theme->getBackgroundColor();

    backgroundColor = theme->getAltBackgroundColor();
    backgroundColor.setAlphaF(BACKGROUND_OPACITY);
    backgroundHighlightColor = theme->getActiveWidgetBorderColor();
    backgroundHighlightColor.setAlphaF(BACKGROUND_OPACITY * 2.0);
    //backgroundHighlightColor = theme->getAltBackgroundColor();

    cursorPen = QPen(theme->getTextColor(), 8);
    axisLinePen = QPen(theme->getAltTextColor(), axisWidth);
    topLinePen = QPen(theme->getAltTextColor(), 2);
    hoverLinePen = QPen(theme->getTextColor(), HOVER_LINE_WIDTH, Qt::PenStyle::DotLine);

    _toolbar->setIconSize(theme->getLargeIconSize());
    _toolbar->setStyleSheet(theme->getToolBarStyleSheet() +
                           "QToolBar {"
                           "background:" + theme->getBackgroundColorHex() + ";"
                           "border-radius:" + theme->getSharpCornerRadius() + ";"
                           "}"
                           "QToolButton::checked:!hover{ background:" + theme->getAltBackgroundColorHex() + ";}"
                           + theme->getToolTipStyleSheet());

    QString toolbarStyle = theme->getToolBarStyleSheet() +
                           "QToolButton:!hover{"
                           "border: 1px solid " + theme->getAltTextColorHex() + ";"
                           "background:" + theme->getBackgroundColorHex() + ";}";
    t1->setIconSize(theme->getIconSize());
    t1->setStyleSheet(theme->getToolTipStyleSheet() + toolbarStyle);
    t2->setIconSize(theme->getIconSize());
    t2->setStyleSheet(theme->getToolTipStyleSheet() + toolbarStyle);

    _setMinMenu->setStyleSheet(theme->getMenuStyleSheet() + theme->getLineEditStyleSheet("QSpinBox"));
    _setMaxMenu->setStyleSheet(theme->getMenuStyleSheet() + theme->getLineEditStyleSheet("QSpinBox"));
    _showSetMinMenu->setIcon(theme->getIcon("Icons", "arrowHeadLeft"));
    _showSetMaxMenu->setIcon(theme->getIcon("Icons", "arrowHeadRight"));
    _setMinButton->setIcon(theme->getIcon("Icons", "tick"));
    _setMaxButton->setIcon(theme->getIcon("Icons", "tick"));
}


/**
 * @brief TimelineChart::entityChartHovered
 * @param chart
 * @param hovered
 */
void TimelineChart::entityChartHovered(EntityChart* chart, bool hovered)
{
    if (chart) {
        hoveredChartRect = hovered ? chart->rect() : QRectF();
        hoveredChartRect.moveTo(chart->pos());
        chart->setHovered(hovered);
        update();
    }
}


/**
 * @brief TimelineChart::entityChartRangeChanged
 * @param min
 * @param max
 */
void TimelineChart::entityChartRangeChanged(double min, double max)
{
    bool update = false;
    if (min < _dataMin) {
        _dataMin = min;
        update = true;
    }
    if (max > _dataMax) {
        _dataMax = max;
        update = true;
    }
    // send a signal to update the axis' range to match the chart's range
    if (update) {
        emit rangeChanged(_dataMin, _dataMax);
    }
}


/**
 * @brief TimelineChart::setRangeTriggered
 * This is called when the toolbar was used to change the min/max dislayed range.
 */
void TimelineChart::setRangeTriggered()
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());
    QDateTime dt;
    _toolbar->hide();

    if (button == _setMinButton) {
        _setMinMenu->close();
        dt.setMSecsSinceEpoch(_displayMin);
        dt.setTime(QTime(h1->value(), m1->value(), s1->value(), ms1->value()));
        double mSecs = dt.toMSecsSinceEpoch();
        if (mSecs < _dataMin || mSecs > _displayMax) {
            return;
        }
        _displayMin = mSecs;
    } else if (button == _setMaxButton) {
        _setMaxMenu->close();
        dt.setMSecsSinceEpoch(_displayMax);
        dt.setTime(QTime(h2->value(), m2->value(), s2->value(), ms2->value()));
        double mSecs = dt.toMSecsSinceEpoch();
        if (mSecs < _displayMin || mSecs > _dataMax) {
            return;
        }
        _displayMax = mSecs;
    }

    for (EntityChart* chart : _entityCharts) {
        chart->setRange(_displayMin, _displayMax);
    }
    emit changeDisplayedRange(_displayMin, _displayMax);
}


/**
 * @brief TimelineChart::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool TimelineChart::eventFilter(QObject* watched, QEvent *event)
{
    EntityChart* chart = qobject_cast<EntityChart*>(watched);
    if (event->type() == QEvent::Enter) {
        entityChartHovered(chart, true);
    } else if (event->type() == QEvent::Leave) {
        entityChartHovered(chart, false);
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
        dragMode = PAN;
        panOrigin = event->pos();
        setCursor(Qt::ClosedHandCursor);
        // if the CTRL key is down, go into rubberband mode
        if (event->modifiers() & Qt::ControlModifier) {
            dragMode = RUBBERBAND;
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
    if (dragMode == RUBBERBAND && !rubberBandRect.isNull()) {
        double min = mapToRange(rubberBandRect.left());
        double max = mapToRange(rubberBandRect.right());
        // make sure that min < max
        if (min > max) {
            double temp = max;
            max = min;
            min = temp;
        }
        // keep min/max within the bounds
        min = qMax(min, mapToRange(rect().left()));
        max = qMin(max, mapToRange(rect().right()));
        for (EntityChart* chart : _entityCharts) {
            chart->setRange(min, max);
        }
        _displayMin = min;
        _displayMax = max;
        emit changeDisplayedRange(min, max);
    }

    emit hoverLineUpdated(hoverRect.isValid(), QPointF(0, 0));

    clearDragMode();
    QWidget::mouseReleaseEvent(event);

    if (event->button() == Qt::RightButton) {
        _toolbar->move(event->screenPos().toPoint());
        _toolbar->show();
    }
}


/**
 * @brief TimelineChart::mouseMoveEvent
 * @param event
 */
void TimelineChart::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    cursorPoint = mapFromGlobal(cursor().pos());
    hoverRect.moveCenter(QPointF(cursorPoint.x(), 0));
    hoverRectUpdated();

    switch (dragMode) {
    case PAN: {
        QPointF delta = cursorPoint - panOrigin;
        panOrigin = cursorPoint;
        emit panned(-delta.x(), 0);
        break;
    }
    case RUBBERBAND:
        rubberBandRect = QRectF(panOrigin.x(), 0, event->pos().x() - panOrigin.x(), height());
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
    if (dragMode == RUBBERBAND) {
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
    hovered = true;

    hoverRect = visibleRegion().boundingRect();
    hoverRect.setWidth(HOVER_LINE_WIDTH);
    hoverRectUpdated();
}


/**
 * @brief TimelineChart::leaveEvent
 * @param event
 */
void TimelineChart::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    hovered = false;

    hoverRect = QRectF();
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
    painter.fillRect(visibleRect, backgroundColor);

    visibleRect = visibleRect.adjusted(axisWidth / 2.0, topLinePen.widthF() / 2.0, 0, 0);

    QLineF axisY(visibleRect.topLeft(), visibleRect.bottomLeft());
    painter.setPen(axisLinePen);
    painter.drawLine(axisY);

    switch (dragMode) {
    case RUBBERBAND: {
        painter.setOpacity(0.25);
        painter.setPen(QPen(highlightColor.darker(), 2));
        painter.setBrush(highlightColor);
        painter.drawRect(rubberBandRect);
        break;
    }
    default: {
        //painter.fillRect(hoveredChartRect, backgroundHighlightColor);
        if (hovered) {
            painter.setPen(hoverLinePen);
            painter.drawLine(cursorPoint.x(), rect().top(), cursorPoint.x(), rect().bottom());
            painter.setPen(cursorPen);
            painter.drawPoint(cursorPoint);
        }
        break;
    }
    }
}


/**
 * @brief TimelineChart::hovereRectUpdated
 */
void TimelineChart::hoverRectUpdated()
{
    if (hoverRect.isNull()) {
        for (EntityChart* chart : _entityCharts) {
            chart->setHoveredRect(hoverRect);
        }
    } else {
        for (EntityChart* chart : _entityCharts) {
            if (!chart->isVisible())
                continue;
            QRect childRect(chart->x(), chart->y(), chart->width(), chart->height());
            if (visibleRegion().contains(childRect)) {
                chart->setHoveredRect(hoverRect);
            }
        }
    }

    emit hoverLineUpdated(hoverRect.isValid(), hoverRect.center());
    update();
}


/**
 * @brief TimelineChart::clearDragMode
 */
void TimelineChart::clearDragMode()
{
    dragMode = NONE;
    rubberBandRect = QRectF();
    setCursor(Qt::BlankCursor);
    update();
}


/**
 * @brief TimelineChart::mapToRange
 * @param value
 * @return
 */
double TimelineChart::mapToRange(double value)
{
    double ratio = value / width();
    return ratio * (_displayMax - _displayMin) + _displayMin;
}


const QList<EntityChart*>& TimelineChart::getEntityCharts()
{
    return _entityCharts;
}
