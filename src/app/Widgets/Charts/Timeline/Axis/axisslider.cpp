#include "axisslider.h"
#include "../../../../theme.h"

#include <QMouseEvent>

#define AXIS_PEN_WIDTH 2.0
#define SLIDER_PEN_WIDTH 1.5

#define AXIS_ROUNDNESS 2.0
#define SLIDER_ROUNDNESS 4.0

#define AXIS_THICKNESS 25.0
#define SLIDER_THICKNESS 18.0

/**
 * @brief AxisSlider::AxisSlider
 * @param parent
 */
AxisSlider::AxisSlider(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent)
    : QWidget(parent)
{
    _orientation = orientation;
    _alignment = alignment;

    if (orientation == Qt::Horizontal) {
        _minSlider = QRectF(QPointF(0, 0), QSizeF(SLIDER_THICKNESS, AXIS_THICKNESS));
        _maxSlider = QRectF(QPointF(0, 0), QSizeF(SLIDER_THICKNESS, AXIS_THICKNESS));
        setFixedHeight(AXIS_THICKNESS);
    } else {
        setFixedWidth(AXIS_THICKNESS);
        _minSlider = QRectF(QPointF(0, 0), QSizeF(AXIS_THICKNESS, SLIDER_THICKNESS));
        _maxSlider = QRectF(QPointF(0, 0), QSizeF(AXIS_THICKNESS, SLIDER_THICKNESS));
    }

    _minRatio = 0;
    _maxRatio = 1;

    setMouseTracking(true);
    updateSlidersOnSizeChange();

    connect(Theme::theme(), &Theme::theme_Changed, this, &AxisSlider::themeChanged);
    themeChanged();

    /*
    qDebug() << "----------------------------------------------";
    qDebug() << "slider min: " << _sliderMin;
    qDebug() << "slider max: " << _sliderMax;
    qDebug() << "_actualMin: " << _actualMin;
    qDebug() << "_actualMax: " << _actualMax;
    qDebug() << "----------------------------------------------";
    */
}


/**
 * @brief AxisSlider::getOrientation
 * @return
 */
Qt::Orientation AxisSlider::getOrientation()
{
    return _orientation;
}


/**
 * @brief AxisSlider::getAlignment
 * @return
 */
Qt::Alignment AxisSlider::getAlignment()
{
    return _alignment;
}


/**
 * @brief AxisSlider::getAxisPenWidth
 * @return
 */
double AxisSlider::getAxisPenWidth()
{
    return AXIS_PEN_WIDTH;
}


/**
 * @brief AxisSlider::getMinRatio
 * @return
 */
double AxisSlider::getMinRatio()
{
    return _minRatio;
}


/**
 * @brief AxisSlider::getMaxRatio
 * @return
 */
double AxisSlider::getMaxRatio()
{
    return _maxRatio;
}


/**
 * @brief AxisSlider::setZoomFactor
 * @param factor
 */
void AxisSlider::setZoomFactor(double factor)
{
    zoomFactor = factor;
}


/**
 * @brief AxisSlider::setPanning
 * @param panning
 */
void AxisSlider::setPanning(bool panning)
{
    isPanning = panning;
    update();
}


/**
 * @brief AxisSlider::updateMinRatio
 * This slot is called when the displayed min of the timeline chart is changed.
 * It updates the min ratio and the relative position of the min slider.
 * @param ratio
 */
void AxisSlider::updateMinRatio(double ratio)
{
    if (ratio <= _maxRatio) {
        double min = _sliderRange * ratio;
        moveSliderRects(min, _sliderMax);
        _actualMin = min;
        _minRatio = ratio;
    }
}


/**
 * @brief AxisSlider::updateMaxRatio
 * This slot is called when the displayed max of the timeline chart is changed.
 * It updates the max ratio and the relative position of the max slider.
 * @param ratio
 */
void AxisSlider::updateMaxRatio(double ratio)
{
    if (ratio >= _minRatio) {
        double max = _sliderRange * ratio;
        moveSliderRects(_sliderMin, max);
        _actualMax = max;
        _maxRatio = ratio;
    }
}


/**
 * @brief AxisSlider::zoom
 * @param factor
 */
void AxisSlider::zoom(double factor)
{
    // stop zooming out when one of the sliders hit the edge
    if (factor > 1) {
        if (_sliderMin == 0 || _sliderMax == _sliderRange)
            // TODO - If the zoom isn't going to move the slider, allow it
            return;
    }

    double delta = (_actualMax - _actualMin) * (1 - factor);
    double scaledMin = qMax(0.0, _actualMin + delta);
    double scaledMax = qMin(_sliderRange, _actualMax - delta);
    moveSliders(scaledMin, scaledMax);
}


/**
 * @brief AxisSlider::pan
 * @param dx
 * @param dy
 */
void AxisSlider::pan(double dx, double dy)
{
    double delta = _orientation == Qt::Horizontal ? dx : dy;
    if (delta < 0) {
        delta = qMax(delta, -_actualMin);
    } else {
        delta = qMin(delta, _sliderRange - _actualMax);
    }
    moveSliders(_actualMin + delta, _actualMax + delta);
}


/**
 * @brief AxisSlider::themeChanged
 */
void AxisSlider::themeChanged()
{
    Theme* theme = Theme::theme();

    _axisPen = QPen(theme->getDisabledBackgroundColor(), AXIS_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    _axisColor = theme->getAltBackgroundColor();
    _axisColor.setAlphaF(0.2);
    _sliderColor = theme->getTextColor();

    QColor midBarColor = theme->getAltBackgroundColor();
    midBarColor.setAlphaF(0.8);

    _middleBrush = QBrush(midBarColor);
    _highlightBrush = QBrush(theme->getHighlightColor());

    update();
}


/**
 * @brief AxisSlider::mousePressEvent
 * @param event
 */
void AxisSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        sliderOrigin = event->pos();
        sliderHitRect = getCursorHitRect(sliderOrigin);
        dragMode = PAN_READY_MODE;
    }
    QWidget::mousePressEvent(event);
}


/**
 * @brief AxisSlider::mouseReleaseEvent
 * @param event
 */
void AxisSlider::mouseReleaseEvent(QMouseEvent* event)
{
    if (dragMode != PAN_MODE) {
        sliderHitRect = getCursorHitRect(event->pos());
        switch (sliderHitRect) {
        case HIT_RECT::NO_SLIDER: {
            // NOTE - The cursor pos can't be outside of the slider's rect in this function
            QPointF cursorPos = event->pos();
            QPointF deltaMin = _minSlider.topLeft() - cursorPos;
            QPointF deltaMax = cursorPos - _maxSlider.bottomRight();
            double min = _sliderMin;
            double max = _sliderMax;
            if (qMax(deltaMin.x(), deltaMin.y()) > 0) {
                min = _orientation == Qt::Horizontal ? cursorPos.x() : cursorPos.y();
            } else if (qMax(deltaMax.x(), deltaMax.y()) > 0) {
                max = _orientation == Qt::Horizontal ? cursorPos.x() : cursorPos.y();
            }
            moveSliders(min, max);
            break;
        }
        default:
            break;
        }
        prevClickedRect = sliderHitRect;
    }

    dragMode = NO_DRAG_MODE;
    sliderHitRect = HIT_RECT::NO_SLIDER;
    QWidget::mouseReleaseEvent(event);
}


/**
 * @brief AxisSlider::mouseDoubleClickEvent
 * Double-clicking a slider resets it to its original position.
 * @param event
 */
void AxisSlider::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {        
        sliderHitRect = getCursorHitRect(event->pos());
        if (sliderHitRect != prevClickedRect)
            return;
        switch (sliderHitRect) {
        case HIT_RECT::MIN_SLIDER:
            moveSliders(0.0, _sliderMax);
            break;
        case HIT_RECT::MAX_SLIDER:
            moveSliders(_sliderMin, _sliderRange);
            break;
        default:
            break;
        }
    }
}


/**
 * @brief AxisSlider::mouseMoveEvent
 * @param event
 */
void AxisSlider::mouseMoveEvent(QMouseEvent* event)
{
    cursorPoint = event->pos();

    QPointF deltaPoint = cursorPoint - sliderOrigin;
    double delta = _orientation == Qt::Horizontal ? deltaPoint.x() : deltaPoint.y();

    switch (sliderHitRect) {
    case HIT_RECT::MIN_SLIDER: {
        double min = qMin(_sliderMin + delta, _sliderMax - SLIDER_THICKNESS);
        moveSliders(min, _sliderMax);
        break;
    }
    case HIT_RECT::MAX_SLIDER: {
        double max = qMax(_sliderMin + SLIDER_THICKNESS, _sliderMax + delta);
        moveSliders(_sliderMin, max);
        break;
    }
    case HIT_RECT::MID_SLIDER:
        pan(delta, delta);
        break;
    default:
        update();
        break;
    }

    sliderOrigin = cursorPoint;

    if (dragMode == PAN_READY_MODE) {
        dragMode = PAN_MODE;
    }
}


/**
 * @brief AxisSlider::wheelEvent
 * Positive delta means zoom in, and vice versa.
 * @param event
 */
void AxisSlider::wheelEvent(QWheelEvent* event)
{
    if (event->delta() > 0) {
        zoom(1 / zoomFactor);
    } else {
        zoom(zoomFactor);
    }
}


/**
 * @brief AxisSlider::leaveEvent
 * @param event
 */
void AxisSlider::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    cursorPoint = QPointF(-1, -1);
    update();
}


/**
 * @brief AxisSlider::resizeEvent
 * This updates the min/max slider's position based on the new range and their ratios.
 * @param event
 */
void AxisSlider::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateSlidersOnSizeChange();
}


/**
 * @brief AxisSlider::paintEvent
 * @param event
 */
void AxisSlider::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    double roundness = SLIDER_ROUNDNESS;
    double offset = AXIS_PEN_WIDTH;
    double midSliderThickness = _midSlider.width();
    QRectF minRect = _minSlider.adjusted(offset, offset, -offset, -offset);
    QRectF maxRect = _maxSlider.adjusted(offset, offset, -offset, -offset);
    QRectF midRect = QRectF(0, 0, SLIDER_THICKNESS, _midSlider.height());

    if (_orientation == Qt::Vertical) {
        midSliderThickness = _midSlider.height();
        midRect = QRectF(0, 0, _midSlider.width(), SLIDER_THICKNESS);
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(_sliderColor);

    if (midSliderThickness >= -SLIDER_THICKNESS + offset) {
        painter.fillRect(_midSlider.adjusted(-offset, -offset, offset, offset), _middleBrush);
        painter.drawRoundedRect(minRect, roundness, roundness);
        painter.drawRoundedRect(maxRect, roundness, roundness);
    } else {
        roundness = AXIS_ROUNDNESS;
        midRect.moveCenter(_midSlider.center());
        midRect.adjust(offset, offset, -offset, -offset);
        painter.drawRoundedRect(midRect, roundness, roundness);
    }

    // this keeps the selected slider highlighted when it's being moved even if the cursor is elsewhere
    HIT_RECT hitRect = sliderHitRect == HIT_RECT::NO_SLIDER ? getCursorHitRect(cursorPoint) : sliderHitRect;
    QRectF highlightRect;
    bool highlightMidRect = isPanning;

    switch (hitRect) {
    case HIT_RECT::MIN_SLIDER:
        highlightRect = minRect;
        break;
    case HIT_RECT::MAX_SLIDER:
        highlightRect = maxRect;
        break;
    case HIT_RECT::MID_SLIDER:
        highlightMidRect = true;
        break;
    default:
        break;
    }

    if (highlightMidRect) {
        if (_minSlider == _maxSlider) {
            highlightRect = midRect;
        } else {
            highlightRect = QRectF(minRect.topLeft(), maxRect.bottomRight());
        }
    }

    if (!highlightRect.isNull()) {
        painter.setBrush(_highlightBrush);
        painter.drawRoundedRect(highlightRect, roundness, roundness);
    }

    double axisOffset = AXIS_PEN_WIDTH / 2.0;
    QRectF mainRect = rect().adjusted(axisOffset, axisOffset, -axisOffset, -axisOffset);
    painter.setPen(_axisPen);
    painter.setBrush(_axisColor);
    painter.drawRoundedRect(mainRect, AXIS_ROUNDNESS, AXIS_ROUNDNESS);
}


/**
 * @brief AxisSlider::getCursorHitRect
 * @param cursorPos
 * @return
 */
HIT_RECT AxisSlider::getCursorHitRect(QPointF cursorPos)
{
    if (_minSlider.contains(cursorPos)) {
        // this is the case where the 2 sliders are over each other
        if (_minSlider == _maxSlider) {
            return HIT_RECT::MID_SLIDER;
        }
        return HIT_RECT::MIN_SLIDER;
    } else if (_maxSlider.contains(cursorPos)) {
        return HIT_RECT::MAX_SLIDER;
    } else if (_midSlider.contains(cursorPos)) {
        return HIT_RECT::MID_SLIDER;
    } else {
        return HIT_RECT::NO_SLIDER;
    }
}


/**
 * @brief AxisSlider::moveSliders
 * This function moves the sliders, updates the stored min/max/ratios and sends signals to update the display.
 * @param min
 * @param max
 */
void AxisSlider::moveSliders(double min, double max)
{
    if (min <= max) {
        moveSliderRects(min, max);
        minSliderMoved(min);
        maxSliderMoved(max);
    }
}


/**
 * @brief AxisSlider::minSliderMoved
 * This function is called when the left/top slider is moved by the user.
 * It sends a signal to update the displayed values.
 * @param min
 */
void AxisSlider::minSliderMoved(double min)
{
    if (min == _actualMin)
        return;

    _actualMin = min;
    _minRatio = min / _sliderRange;

    if (_orientation == Qt::Horizontal) {
        emit minRatioChanged(_minRatio);
    } else {
        double invertedRatio = 1 - _minRatio;
        emit maxRatioChanged(invertedRatio);
    }
}


/**
 * @brief AxisSlider::maxSliderMoved
 * This function is called when the right/bottom slider is moved by the user.
 * It sends a signal to update the displayed values.
 * @param max
 */
void AxisSlider::maxSliderMoved(double max)
{
    if (max == _actualMax)
        return;

    _actualMax = max;
    _maxRatio = max / _sliderRange;

    if (_orientation == Qt::Horizontal) {
        emit maxRatioChanged(_maxRatio);
    } else {
        double invertedRatio = 1 - _maxRatio;
        emit minRatioChanged(invertedRatio);
    }
}


/**
 * @brief AxisSlider::moveSliderRects
 * This moves the position of the sliders to the given values without updating the stored min/max or sending a signal.
 * @param min
 * @param max
 */
void AxisSlider::moveSliderRects(double min, double max)
{
    // cap min/max so that they stay within the bounds
    min = qMax(0.0, min);
    max = qMin(_sliderRange, max);

    // adjust the min/max so that the sliders don't surpass each other
    auto delta = max - min;
    if (delta < SLIDER_THICKNESS) {
        max = max + (SLIDER_THICKNESS - delta) / 2.0;
        if (max < SLIDER_THICKNESS) {
            max = SLIDER_THICKNESS;
        } else if (max > _sliderRange) {
            max = _sliderRange;
        }
        min = max - SLIDER_THICKNESS;
    }

    if (_orientation == Qt::Horizontal) {
        _minSlider.moveLeft(min);
        _maxSlider.moveRight(max);
        // need to cap the mid slider's width when the min and max sliders are on top of each other
        auto cappedWidth = qMax(max - min - 2 * SLIDER_THICKNESS, -SLIDER_THICKNESS);
        _midSlider = QRectF(min + SLIDER_THICKNESS, 0, cappedWidth, AXIS_THICKNESS);
    } else {
        _minSlider.moveTop(min);
        _maxSlider.moveBottom(max);
        _midSlider = QRectF(_minSlider.bottomLeft(), _maxSlider.topRight());
    }

    _sliderMin = min;
    _sliderMax = max;
    update();
}


/**
 * @brief AxisSlider::updateSlidersOnSizeChange
 * This function updates the sliders' positions based on the new widget size (range).
 * It also updates the current min/max/range without sending a min/max ratio changed signal.
 */
void AxisSlider::updateSlidersOnSizeChange()
{
    auto displayRangeRatio = (_actualMax - _actualMin) / (_sliderMax - _sliderMin);
    _sliderRange = _orientation == Qt::Horizontal ? width() : height();
    moveSliderRects(_minRatio * _sliderRange, _maxRatio * _sliderRange);

    // adjust the actual min/max based on the new sliders' positions
    auto range = _sliderMax - _sliderMin;
    auto displayRange = range * displayRangeRatio;
    _actualMin = _sliderMin + (range - displayRange) / 2.0;
    _actualMax = _sliderMax - (range - displayRange) / 2.0;
}
