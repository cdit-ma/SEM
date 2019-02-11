#include "axiswidget.h"

#include <QWheelEvent>
#include <QDebug>

#define MARGIN 0
#define INNER_MARGIN 3

/**
 * @brief AxisWidget::AxisWidget
 * @param orientation
 * @param alignment
 * @param parent
 * @param type
 */
AxisWidget::AxisWidget(Qt::Orientation orientation, Qt::Alignment alignment, QWidget* parent, VALUE_TYPE type)
    : QWidget(parent),
      _orientation(orientation),
      _alignment(alignment)
{
    /*
     * TODO: Instead of resizing the filler widgets and the slider widget, resize the chart view instead
     * This should stop the jumping around of sliders and displayed values
     */

    _slider = new AxisSlider(_orientation, _alignment, this);
    _display = new AxisDisplay(_slider, this, type);

    QBoxLayout* mainLayout;
    if (_orientation == Qt::Horizontal) {
        mainLayout = new QVBoxLayout(this);
        if (_alignment == Qt::AlignTop) {
            setContentsMargins(0, MARGIN, 0, 0);
            mainLayout->addWidget(_slider);
            mainLayout->addWidget(_display);
        } else {
            setContentsMargins(0, 0, 0, MARGIN);
            mainLayout->addWidget(_display);
            mainLayout->addWidget(_slider);        }
    } else {
        mainLayout = new QHBoxLayout(this);
        if (_alignment == Qt::AlignLeft) {
            setContentsMargins(MARGIN, 0, 0, 0);
            mainLayout->addWidget(_slider);
            mainLayout->addWidget(_display);
        } else {
            setContentsMargins(0, 0, MARGIN, 0);
            mainLayout->addWidget(_display);
            mainLayout->addWidget(_slider);
        }
    }

    mainLayout->setMargin(0);
    mainLayout->setSpacing(INNER_MARGIN);
    mainLayout->setContentsMargins(0,0,0,0);

    connect(_slider, &AxisSlider::minRatioChanged, this, &AxisWidget::minRatioChanged);
    connect(_slider, &AxisSlider::maxRatioChanged, this, &AxisWidget::maxRatioChanged);

    // this filter stops the slider's own wheel event from being used
    _slider->installEventFilter(this);

    // initialise range and tick count
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
    zoomFactor = factor;
    _slider->setZoomFactor(factor);
}


/**
 * @brief AxisWidget::setAxisMargin
 * @param margin
 */
void AxisWidget::setAxisMargin(int margin)
{
    _display->setAxisMargin(margin);
}


/**
 * @brief AxisWidget::setTickCount
 * @param ticks
 */
void AxisWidget::setTickCount(int ticks)
{
    _display->setTickCount(ticks);
}


/**
 * @brief AxisWidget::setAxisLineVisible
 * @param visible
 */
void AxisWidget::setAxisLineVisible(bool visible)
{
    _display->setAxisLineVisible(visible);
}


/**
 * @brief AxisWidget::setTickVisible
 * @param visible
 */
void AxisWidget::setTickVisible(bool visible)
{
    _display->setTickVisible(visible);
}


/**
 * @brief AxisWidget::setPanning
 * @param panning
 */
void AxisWidget::setPanning(bool panning)
{
    _slider->setPanning(panning);
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
    // added 1ms on either side to include border values
    min--;
    max++;

    _min = min;
    _max = max;
    _range = max - min;

    // set the range
    _display->setMax(max);
    _display->setMin(min);

    if (updateDisplay) {
        setDisplayRange(min, max);
    } else {
        // update the slider rects' positions
        updateMaxSliderRatio(getDisplayedRange().second);
        updateMinSliderRatio(getDisplayedRange().first);
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
    _display->displayedMaxChanged(max);
    _display->displayedMinChanged(min);

    // update the slider rects' positions
    updateMaxSliderRatio(max);
    updateMinSliderRatio(min);
}


/**
 * @brief AxisWidget::getRange
 * @return
 */
QPair<double, double> AxisWidget::getRange()
{
    return _display->getRange();
}


/**
 * @brief AxisWidget::getDisplayedRange
 * @return
 */
QPair<double, double> AxisWidget::getDisplayedRange()
{
    return _display->getDisplayedRange();
}


/**
 * @brief AxisWidget::zoom
 * @param factor
 */
void AxisWidget::zoom(double factor)
{
    _slider->zoom(factor);
}


/**
 * @brief AxisWidget::pan
 * @param dx
 * @param dy
 */
void AxisWidget::pan(double dx, double dy)
{
    _slider->pan(dx, dy);
}


/**
 * @brief AxisWidget::setDisplayFormat
 * @param format
 */
void AxisWidget::setDisplayFormat(TIME_DISPLAY_FORMAT format)
{
    _display->setDisplayFormat(format);
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
    _display->hoverLineUpdated(visible, pos);
}


/**
 * @brief AxisWidget::wheelEvent
 * @param event
 */
void AxisWidget::wheelEvent(QWheelEvent* event)
{
    double factor = zoomFactor;
    if (event->delta() > 0) {
        factor = 1 / zoomFactor;
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
    double ratio = _range > 0 ? (min - _min) / _range : 0.0;
    _slider->updateMinRatio(ratio);
}


/**
 * @brief AxisWidget::updateMaxSliderRatio
 * This updates the slider min rect's position based on max
 * @param max
 */
void AxisWidget::updateMaxSliderRatio(double max)
{
    double ratio = _range > 0 ? (max - _min) / _range : 0.0;
    _slider->updateMaxRatio(ratio);
}
