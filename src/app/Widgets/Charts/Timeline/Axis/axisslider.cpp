#include "axisslider.h"
#include "../../../../theme.h"

#include <QMouseEvent>

#define AXIS_PEN_WIDTH 2.0
#define AXIS_ROUNDNESS 2.0
#define AXIS_THICKNESS 25.0

#define SLIDER_ROUNDNESS 4.0
#define SLIDER_THICKNESS 18.0

/**
 * @brief AxisSlider::AxisSlider
 * @param orientation
 * @param alignment
 * @param parent
 */
AxisSlider::AxisSlider(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent)
	: QWidget(parent)
{
	orientation_ = orientation;
	alignment_ = alignment;

	if (orientation == Qt::Horizontal) {
		minSlider_ = QRectF(QPointF(0, 0), QSizeF(SLIDER_THICKNESS, AXIS_THICKNESS));
		maxSlider_ = QRectF(QPointF(0, 0), QSizeF(SLIDER_THICKNESS, AXIS_THICKNESS));
		setFixedHeight(AXIS_THICKNESS);
	} else {
		setFixedWidth(AXIS_THICKNESS);
		minSlider_ = QRectF(QPointF(0, 0), QSizeF(AXIS_THICKNESS, SLIDER_THICKNESS));
		maxSlider_ = QRectF(QPointF(0, 0), QSizeF(AXIS_THICKNESS, SLIDER_THICKNESS));
	}

	minRatio_ = 0;
	maxRatio_ = 1;

	setMouseTracking(true);

	// This function initialises the following variables: sliderMin_, sliderMax_, sliderRange_, actualMin_ & actualMax_
	updateSlidersOnSizeChange();

	connect(Theme::theme(), &Theme::theme_Changed, this, &AxisSlider::themeChanged);
	themeChanged();
}

/**
 * @brief AxisSlider::getOrientation
 * @return
 */
Qt::Orientation AxisSlider::getOrientation() const
{
	return orientation_;
}

/**
 * @brief AxisSlider::getAlignment
 * @return
 */
Qt::Alignment AxisSlider::getAlignment() const
{
	return alignment_;
}

/**
 * @brief AxisSlider::getAxisPenWidth
 * @return
 */
double AxisSlider::getAxisPenWidth() const
{
	return AXIS_PEN_WIDTH;
}

/**
 * @brief AxisSlider::setZoomFactor
 * @param factor
 */
void AxisSlider::setZoomFactor(double factor)
{
	zoomFactor_ = factor;
}

/**
 * @brief AxisSlider::setPanning
 * @param panning
 */
void AxisSlider::setPanning(bool panning)
{
	isPanning_ = panning;
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
	if (ratio <= maxRatio_) {
		double min = sliderRange_ * ratio;
		moveSliderRects(min, sliderMax_);
		actualMin_ = min;
		minRatio_ = ratio;
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
	if (ratio >= minRatio_) {
		double max = sliderRange_ * ratio;
		moveSliderRects(sliderMin_, max);
		actualMax_ = max;
		maxRatio_ = ratio;
	}
}

/**
 * @brief AxisSlider::zoom
 * @param factor
 */
void AxisSlider::zoom(double factor)
{
	// TODO - Previously there was some minor precision bugs when this was removed - check if those are fixed
	// stop zooming out when one of the sliders hit the edge
	/*if (factor > 1) {
		if (_sliderMin == 0 || _sliderMax == _sliderRange)
			// TODO - If the zoom isn't going to move the slider, allow it
			return;
	}*/

	double delta = (actualMax_ - actualMin_) * (1 - factor);
	double scaledMin = qMax(0.0, actualMin_ + delta);
	double scaledMax = qMin(sliderRange_, actualMax_ - delta);
	moveSliders(scaledMin, scaledMax);
}

/**
 * @brief AxisSlider::pan
 * @param dx
 * @param dy
 */
void AxisSlider::pan(double dx, double dy)
{
	double delta = orientation_ == Qt::Horizontal ? dx : dy;
	if (delta < 0) {
		delta = qMax(delta, -actualMin_);
	} else {
		delta = qMin(delta, sliderRange_ - actualMax_);
	}
	moveSliders(actualMin_ + delta, actualMax_ + delta);
}

/**
 * @brief AxisSlider::themeChanged
 */
void AxisSlider::themeChanged()
{
    Theme* theme = Theme::theme();

    axisPen_ = QPen(theme->getDisabledBackgroundColor(), AXIS_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    axisColor_ = theme->getAltBackgroundColor();
    axisColor_.setAlphaF(0.3);
    sliderColor_ = theme->getTextColor();

    QColor midBarColor = theme->getAltBackgroundColor();
    midBarColor.setAlphaF(0.7);

    middleBrush_ = QBrush(midBarColor);
    highlightBrush_ = QBrush(theme->getHighlightColor());

    update();
}

/**
 * @brief AxisSlider::mousePressEvent
 * @param event
 */
void AxisSlider::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		sliderOrigin_ = event->pos();
		sliderHitRect_ = getCursorHitRect(sliderOrigin_);
		dragMode_ = PAN_READY_MODE;
	}
	QWidget::mousePressEvent(event);
}

/**
 * @brief AxisSlider::mouseReleaseEvent
 * @param event
 */
void AxisSlider::mouseReleaseEvent(QMouseEvent* event)
{
	if (dragMode_ != PAN_MODE) {
		sliderHitRect_ = getCursorHitRect(event->pos());
		if (sliderHitRect_ == HIT_RECT::NO_SLIDER) {
			// NOTE: The cursor pos can't be outside of the slider's rect in this function
			QPointF cursorPos = event->pos();
			QPointF deltaMin = minSlider_.topLeft() - cursorPos;
			QPointF deltaMax = cursorPos - maxSlider_.bottomRight();
			double min = sliderMin_;
			double max = sliderMax_;
			if (qMax(deltaMin.x(), deltaMin.y()) > 0) {
				min = orientation_ == Qt::Horizontal ? cursorPos.x() : cursorPos.y();
			} else if (qMax(deltaMax.x(), deltaMax.y()) > 0) {
				max = orientation_ == Qt::Horizontal ? cursorPos.x() : cursorPos.y();
			}
			moveSliders(min, max);
		}
		prevClickedRect_ = sliderHitRect_;
	}

	dragMode_ = NO_DRAG_MODE;
	sliderHitRect_ = HIT_RECT::NO_SLIDER;
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
		sliderHitRect_ = getCursorHitRect(event->pos());
		if (sliderHitRect_ == prevClickedRect_) {
			if (sliderHitRect_ == HIT_RECT::MIN_SLIDER) {
				moveSliders(0.0, sliderMax_);
			} else if (sliderHitRect_ == HIT_RECT::MAX_SLIDER) {
				moveSliders(sliderMin_, sliderRange_);
			}
		}
	}
}

/**
 * @brief AxisSlider::mouseMoveEvent
 * @param event
 */
void AxisSlider::mouseMoveEvent(QMouseEvent* event)
{
	cursorPoint_ = event->pos();

	QPointF deltaPoint = cursorPoint_ - sliderOrigin_;
	double delta = orientation_ == Qt::Horizontal ? deltaPoint.x() : deltaPoint.y();

	switch (sliderHitRect_) {
		case HIT_RECT::MIN_SLIDER: {
			double min = qMin(sliderMin_ + delta, sliderMax_ - SLIDER_THICKNESS);
			moveSliders(min, sliderMax_);
			break;
		}
		case HIT_RECT::MAX_SLIDER: {
			double max = qMax(sliderMin_ + SLIDER_THICKNESS, sliderMax_ + delta);
			moveSliders(sliderMin_, max);
			break;
		}
		case HIT_RECT::MID_SLIDER:
			pan(delta, delta);
			break;
		default:
			update();
			break;
	}

	sliderOrigin_ = cursorPoint_;

	if (dragMode_ == PAN_READY_MODE) {
		dragMode_ = PAN_MODE;
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
		zoom(1 / zoomFactor_);
	} else {
		zoom(zoomFactor_);
	}
}

/**
 * @brief AxisSlider::leaveEvent
 * @param event
 */
void AxisSlider::leaveEvent(QEvent *event)
{
	QWidget::leaveEvent(event);
	cursorPoint_ = QPointF(-1, -1);
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
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	double roundness = SLIDER_ROUNDNESS;
	double offset = AXIS_PEN_WIDTH;
	double midSliderThickness = midSlider_.width();
	QRectF minRect = minSlider_.adjusted(offset, offset, -offset, -offset);
	QRectF maxRect = maxSlider_.adjusted(offset, offset, -offset, -offset);
	QRectF midRect = QRectF(0, 0, SLIDER_THICKNESS, midSlider_.height());

	if (orientation_ == Qt::Vertical) {
		midSliderThickness = midSlider_.height();
		midRect = QRectF(0, 0, midSlider_.width(), SLIDER_THICKNESS);
	}

	painter.setPen(Qt::NoPen);
	painter.setBrush(sliderColor_);

	if (midSliderThickness >= -SLIDER_THICKNESS + offset) {
		painter.fillRect(midSlider_.adjusted(-offset, -offset, offset, offset), middleBrush_);
		painter.drawRoundedRect(minRect, roundness, roundness);
		painter.drawRoundedRect(maxRect, roundness, roundness);
	} else {
		roundness = AXIS_ROUNDNESS;
		midRect.moveCenter(midSlider_.center());
		midRect.adjust(offset, offset, -offset, -offset);
		painter.drawRoundedRect(midRect, roundness, roundness);
	}

	// this keeps the selected slider highlighted when it's being moved even if the cursor is elsewhere
	HIT_RECT hitRect = sliderHitRect_ == HIT_RECT::NO_SLIDER ? getCursorHitRect(cursorPoint_) : sliderHitRect_;
	QRectF highlightRect;
	bool highlightMidRect = isPanning_;

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
		if (minSlider_ == maxSlider_) {
			highlightRect = midRect;
		} else {
			highlightRect = QRectF(minRect.topLeft(), maxRect.bottomRight());
		}
	}

	if (!highlightRect.isNull()) {
		painter.setBrush(highlightBrush_);
		painter.drawRoundedRect(highlightRect, roundness, roundness);
	}

	double axisOffset = AXIS_PEN_WIDTH / 2.0;
	QRectF mainRect = rect().adjusted(axisOffset, axisOffset, -axisOffset, -axisOffset);
	painter.setPen(axisPen_);
	painter.setBrush(axisColor_);
	painter.drawRoundedRect(mainRect, AXIS_ROUNDNESS, AXIS_ROUNDNESS);
}

/**
 * @brief AxisSlider::getCursorHitRect
 * @param cursorPos
 * @return
 */
HIT_RECT AxisSlider::getCursorHitRect(QPointF cursorPos)
{
	if (minSlider_.contains(cursorPos)) {
		// this is the case where the 2 sliders are over each other
		if (minSlider_ == maxSlider_) {
			return HIT_RECT::MID_SLIDER;
		}
		return HIT_RECT::MIN_SLIDER;
	} else if (maxSlider_.contains(cursorPos)) {
		return HIT_RECT::MAX_SLIDER;
	} else if (midSlider_.contains(cursorPos)) {
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
	if (min == actualMin_) {
		return;
	}

	actualMin_ = min;
	minRatio_ = min / sliderRange_;

	if (orientation_ == Qt::Horizontal) {
		emit minRatioChanged(minRatio_);
	} else {
		double invertedRatio = 1 - minRatio_;
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
	if (max == actualMax_) {
		return;
	}

	actualMax_ = max;
	maxRatio_ = max / sliderRange_;

	if (orientation_ == Qt::Horizontal) {
		emit maxRatioChanged(maxRatio_);
	} else {
		double invertedRatio = 1 - maxRatio_;
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
	max = qMin(sliderRange_, max);

	// adjust the min/max so that the sliders don't surpass each other
	auto delta = max - min;
	if (delta < SLIDER_THICKNESS) {
		max = max + (SLIDER_THICKNESS - delta) / 2.0;
		if (max < SLIDER_THICKNESS) {
			max = SLIDER_THICKNESS;
		} else if (max > sliderRange_) {
			max = sliderRange_;
		}
		min = max - SLIDER_THICKNESS;
	}

	if (orientation_ == Qt::Horizontal) {
		minSlider_.moveLeft(min);
		maxSlider_.moveRight(max);
		// need to cap the mid slider's width when the min and max sliders are on top of each other
		auto cappedWidth = qMax(max - min - 2 * SLIDER_THICKNESS, -SLIDER_THICKNESS);
		midSlider_ = QRectF(min + SLIDER_THICKNESS, 0, cappedWidth, AXIS_THICKNESS);
	} else {
		minSlider_.moveTop(min);
		maxSlider_.moveBottom(max);
		midSlider_ = QRectF(minSlider_.bottomLeft(), maxSlider_.topRight());
	}

	sliderMin_ = min;
	sliderMax_ = max;
	update();
}

/**
 * @brief AxisSlider::updateSlidersOnSizeChange
 * This function updates the sliders' positions based on the new widget size (range).
 * It also updates the current min/max/range without sending a min/max ratio changed signal.
 */
void AxisSlider::updateSlidersOnSizeChange()
{
	auto displayRangeRatio = (actualMax_ - actualMin_) / (sliderMax_ - sliderMin_);
	sliderRange_ = orientation_ == Qt::Horizontal ? width() : height();
	moveSliderRects(minRatio_ * sliderRange_, maxRatio_ * sliderRange_);

	// adjust the actual min/max based on the new sliders' positions
	auto range = sliderMax_ - sliderMin_;
	auto displayRange = range * displayRangeRatio;
	actualMin_ = sliderMin_ + (range - displayRange) / 2.0;
	actualMax_ = sliderMax_ - (range - displayRange) / 2.0;
}