#include "axiswidget.h"

#include <QWheelEvent>

#define MARGIN 0
#define INNER_MARGIN 0

/**
 * @brief AxisWidget::AxisWidget
 * @param orientation
 * @param alignment
 * @param parent
 * @param type
 */
AxisWidget::AxisWidget(Qt::Orientation orientation, Qt::Alignment alignment, VALUE_TYPE type, QWidget* parent)
	: QWidget(parent),
	  orientation_(orientation),
	  alignment_(alignment),
	  slider_(orientation, alignment),
	  display_(QPointer<AxisSlider>(&slider_), type)
{
	QBoxLayout* mainLayout;
	if (orientation_ == Qt::Horizontal) {
		mainLayout = new QVBoxLayout(this);
		if (alignment_ == Qt::AlignTop) {
			setContentsMargins(0, MARGIN, 0, 0);
			mainLayout->addWidget(&slider_);
			mainLayout->addWidget(&display_);
		} else {
			setContentsMargins(0, 0, 0, MARGIN);
			mainLayout->addWidget(&display_);
			mainLayout->addWidget(&slider_);        }
	} else {
		mainLayout = new QHBoxLayout(this);
		if (alignment_ == Qt::AlignLeft) {
			setContentsMargins(MARGIN, 0, 0, 0);
			mainLayout->addWidget(&slider_);
			mainLayout->addWidget(&display_);
		} else {
			setContentsMargins(0, 0, MARGIN, 0);
			mainLayout->addWidget(&display_);
			mainLayout->addWidget(&slider_);
		}
	}
	
	mainLayout->setMargin(10);
	mainLayout->setSpacing(INNER_MARGIN);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	
	connect(&slider_, &AxisSlider::minRatioChanged, this, &AxisWidget::minRatioChanged);
	connect(&slider_, &AxisSlider::maxRatioChanged, this, &AxisWidget::maxRatioChanged);
	
	// this filter stops the slider's own wheel event from being used
	slider_.installEventFilter(this);
	
	// initialise range and tick count
	// this initialises the following variables: min_, max_ and range_
	setRange(0, 100);
	setDisplayRange(0, 100);
	setTickCount(5);
}

/**
 * @brief AxisWidget::setZoomFactor
 * @param factor
 */
void AxisWidget::setZoomFactor(double factor)
{
	zoomFactor_ = factor;
	slider_.setZoomFactor(factor);
}

/**
 * @brief AxisWidget::setAxisMargin
 * @param margin
 */
void AxisWidget::setAxisMargin(int margin)
{
	display_.setAxisMargin(margin);
}

/**
 * @brief AxisWidget::setTickCount
 * @param ticks
 */
void AxisWidget::setTickCount(int ticks)
{
	display_.setTickCount(ticks);
}

/**
 * @brief AxisWidget::setAxisLineVisible
 * @param visible
 */
void AxisWidget::setAxisLineVisible(bool visible)
{
	display_.setAxisLineVisible(visible);
}

/**
 * @brief AxisWidget::setTickVisible
 * @param visible
 */
void AxisWidget::setTickVisible(bool visible)
{
	display_.setTickVisible(visible);
}

/**
 * @brief AxisWidget::setPanning
 * @param panning
 */
void AxisWidget::setPanning(bool panning)
{
	slider_.setPanning(panning);
}

/**
 * @brief AxisWidget::setRange
 * @param range
 * @param updateDisplay
 */
void AxisWidget::setRange(QPair<double, double> range, bool updateDisplay)
{
	setRange(range.first, range.second, updateDisplay);
}

/**
 * @brief AxisWidget::setRange
 * @param min
 * @param max
 * @param updateDisplay
 */
void AxisWidget::setRange(double min, double max, bool updateDisplay)
{
	// add 1% on either side to include border values
	auto range = max - min;
	min = min - (range * 0.01);
	max = max + (range * 0.01);
	
	min_ = min;
	max_ = max;
	range_ = max - min;
	
	// set the range
	display_.setMin(min);
	display_.setMax(max);
	
	if (updateDisplay) {
		setDisplayRange(min, max);
	} else {
		// update the slider rects' positions
		updateMinSliderRatio(getDisplayedRange().first);
		updateMaxSliderRatio(getDisplayedRange().second);
	}
}

/**
 * @brief AxisWidget::setDisplayedRange
 * @param min
 * @param max
 */
void AxisWidget::setDisplayRange(double min, double max)
{
	// set the display range
	display_.displayedMinChanged(min);
	display_.displayedMaxChanged(max);
	
	// update the slider rects' positions
	updateMinSliderRatio(min);
	updateMaxSliderRatio(max);
}

/**
 * @brief AxisWidget::getRange
 * @return
 */
QPair<double, double> AxisWidget::getRange()
{
	return display_.getRange();
}

/**
 * @brief AxisWidget::getDisplayedRange
 * @return
 */
QPair<double, double> AxisWidget::getDisplayedRange()
{
	return display_.getDisplayedRange();
}

/**
 * @brief AxisWidget::zoom
 * @param factor
 */
void AxisWidget::zoom(double factor)
{
	slider_.zoom(factor);
}

/**
 * @brief AxisWidget::pan
 * @param dx
 * @param dy
 */
void AxisWidget::pan(double dx, double dy)
{
	slider_.pan(dx, dy);
}

/**
 * @brief AxisWidget::setDisplayFormat
 * @param format
 */
void AxisWidget::setDisplayFormat(TIME_DISPLAY_FORMAT format)
{
	display_.setDisplayFormat(format);
}

/**
 * @brief AxisWidget::eventFilter
 * @param watched
 * @param event
 * @return
 */
bool AxisWidget::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Wheel) {
		return true;
	}
	return QWidget::eventFilter(watched, event);
}

/**
 * @brief AxisWidget::hoverLineUpdated
 * @param visible
 * @param pos
 */
void AxisWidget::hoverLineUpdated(bool visible, QPointF pos)
{
	display_.hoverLineUpdated(visible, pos);
}

/**
 * @brief AxisWidget::wheelEvent
 * @param event
 */
void AxisWidget::wheelEvent(QWheelEvent* event)
{
	double factor = zoomFactor_;
	if (event->delta() > 0) {
		factor = 1 / zoomFactor_;
	}
	zoom(factor);
	QWidget::wheelEvent(event);
}

/**
 * @brief AxisWidget::updateMinSliderRatio
 * This updates the slider min rect's position based on min
 * @param min
 */
void AxisWidget::updateMinSliderRatio(double min)
{
	double ratio = range_ > 0 ? (min - min_) / range_ : 0.0;
	slider_.updateMinRatio(ratio);
}

/**
 * @brief AxisWidget::updateMaxSliderRatio
 * This updates the slider min rect's position based on max
 * @param max
 */
void AxisWidget::updateMaxSliderRatio(double max)
{
	double ratio = range_ > 0 ? (max - min_) / range_ : 0.0;
	slider_.updateMaxRatio(ratio);
}