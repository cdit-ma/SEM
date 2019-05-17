#include "axisdisplay.h"
#include "../../../../theme.h"

#include <QDateTime>

#define SPACING 3

/**
 * @brief AxisDisplay::AxisDisplay
 * @param slider
 * @param parent
 * @param type
 */
AxisDisplay::AxisDisplay(const AxisSlider& slider, VALUE_TYPE type, QWidget* parent)
    : QWidget(parent),
      slider_(slider)
{
    orientation_ = slider.getOrientation();
    penWidth_ = slider.getAxisPenWidth();
    textHeight_ = fontMetrics().height();
    tickLength_ = textHeight_ / 4;

    valueType_ = type;
    axisFormat_ = TIME_DISPLAY_FORMAT::VALUE;

    if (orientation_ == Qt::Horizontal) {
        spacing_ = SPACING;
        setMinimumHeight(textHeight_ + tickLength_ + spacing_ * 2);
        if (valueType_ == VALUE_TYPE::DATE_TIME) {
            setMinimumHeight((textHeight_ + tickLength_) * 2 + spacing_ * 2);
            setMinimumWidth(fontMetrics().width(QDateTime::fromMSecsSinceEpoch(0).toString(TIME_FORMAT)));
            axisFormat_ = TIME_DISPLAY_FORMAT::DATE_TIME;
        }
    } else {
        spacing_ = SPACING * 2;
        setMinimumWidth(slider.width());
    }

    switch (slider.getAlignment()) {
    case Qt::AlignTop:
        textAlignment_ = Qt::AlignBottom | Qt::AlignHCenter;
        break;
    case Qt::AlignBottom:
        textAlignment_ = Qt::AlignTop | Qt::AlignHCenter;
        break;
    case Qt::AlignLeft:
        textAlignment_ = Qt::AlignRight | Qt::AlignVCenter;
        break;
    case Qt::AlignRight:
        textAlignment_ = Qt::AlignLeft | Qt::AlignVCenter;
        break;
    default:
        textAlignment_ = Qt::AlignTop | Qt::AlignHCenter;
        break;
    }

    // when the slider is destroyed, delete this object as well
    connect(&slider , &AxisSlider::destroyed, this, &AxisDisplay::deleteLater);

    connect(&slider, &AxisSlider::minRatioChanged, this, &AxisDisplay::updateDisplayedMin);
    connect(&slider, &AxisSlider::maxRatioChanged, this, &AxisDisplay::updateDisplayedMax);
    connect(Theme::theme(), &Theme::theme_Changed, this, &AxisDisplay::themeChanged);
    themeChanged();
}


/**
 * @brief AxisDisplay::setAxisMargin
 * @param margin
 */
void AxisDisplay::setAxisMargin(int margin)
{
    spacing_ = margin;
    if (orientation_ == Qt::Horizontal) {
        setMinimumHeight(textHeight_ + tickLength_ + spacing_ * 2);
    }
    update();
}


/**
 * @brief AxisDisplay::setAxisLineVisible
 * @param visible
 */
void AxisDisplay::setAxisLineVisible(bool visible)
{
    axisLineVisible_ = visible;
    update();
}


/**
 * @brief AxisDisplay::setTicksVisible
 * @param visible
 */
void AxisDisplay::setTickVisible(bool visible)
{
    tickVisible_ = visible;
    update();
}


/**
 * @brief AxisDisplay::setTickCount
 */
void AxisDisplay::setTickCount(int ticks)
{
    tickCount_ = qMax(1, ticks);
    update();
}


/**
 * @brief AxisDisplay::setMax
 * @param max
 */
void AxisDisplay::setMax(double max)
{
    max_ = max;
    range_ = max - min_;
    update();
}


/**
 * @brief AxisDisplay::setMin
 * @param min
 */
void AxisDisplay::setMin(double min)
{
    min_ = min;
    range_ = max_ - min;
    update();
}


/**
 * @brief AxisDisplay::setRange
 * @param min
 * @param max
 */
void AxisDisplay::setRange(double min, double max)
{
    min_ = min;
    max_ = max;
    range_ = max - min;
    update();
}


/**
 * @brief AxisDisplay::getRange
 * @return
 */
QPair<double, double> AxisDisplay::getRange()
{
    return {min_, max_};
}


/**
 * @brief AxisDisplay::getDisplayedRange
 * @return
 */
QPair<double, double> AxisDisplay::getDisplayedRange()
{
    return {displayedMin_, displayedMax_};
}


/**
 * @brief AxisDisplay::setDisplayFormat
 * @param format
 */
void AxisDisplay::setDisplayFormat(TIME_DISPLAY_FORMAT format)
{
    switch (format) {
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        setMinimumHeight(minimumHeight() * 2.0);
        break;
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        setMinimumHeight(minimumHeight() / 2.0);
        break;
    default:
        return;
    }
    axisFormat_ = format;
    update();
}


/**
 * @brief AxisDisplay::hoverLineUpdated
 * @param visible
 * @param globalPos
 */
void AxisDisplay::hoverLineUpdated(bool visible, QPointF globalPos)
{
    displayHoverValue_ = visible;
    if (visible) {
        QPointF pos = mapFromGlobal(globalPos.toPoint());
        if (orientation_ == Qt::Horizontal) {
            hoveredPos_ = pos.x();
            hoveredValue_ = (hoveredPos_ / width()) * displayedRange_ + displayedMin_;
        } else {
            hoveredPos_ = pos.y();
            hoveredValue_ = (1.0 - (hoveredPos_ / height())) * displayedRange_ + displayedMin_;
        }
    }
    update();
}


/**
 * @brief AxisDisplay::themeChanged
 */
void AxisDisplay::themeChanged()
{
    Theme* theme = Theme::theme();
    labelColor_ = theme->getTextColor();
    axisColor_ = theme->getAltTextColor();

    hoveredRectColor_ = theme->getActiveWidgetBorderColor();
    hoverTextColor_ = theme->getTextColor();

    update();
}


/**
 * @brief AxisDisplay::updateDisplayedMin
 * This is called when the min slider is moved by the user.
 * @param minRatio
 */
void AxisDisplay::updateDisplayedMin(double minRatio)
{
    displayedMinChanged(minRatio * range_ + min_);
}


/**
 * @brief AxisDisplay::updateDisplayedMax
 * This is called when the max slider is moved by the user.
 * @param maxRatio
 */
void AxisDisplay::updateDisplayedMax(double maxRatio)
{
    displayedMaxChanged(maxRatio * range_ + min_);
}


/**
 * @brief AxisDisplay::resizeEvent
 * @param event
 */
void AxisDisplay::resizeEvent(QResizeEvent* event)
{
    if ((orientation_ == Qt::Horizontal) && (minimumWidth() > 0)) {
        if (axisFormat_ == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
            setTickCount(width() / minimumWidth() / 2.0);
        } else {
            setTickCount(width() / minimumWidth() / 3.0);
        }
    }
    // update tick label locations
    QWidget::resizeEvent(event);
    update();
}


/**
 * @brief AxisDisplay::paintEvent
 * @param event
 */
void AxisDisplay::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(rect(), Qt::transparent);
    painter.setPen(labelColor_);

    QVector<QLineF> tickLines;
    auto paintRect = QRectF(rect()); //getAdjustedRect();

    if (orientation_ == Qt::Horizontal) {
        paintHorizontal(painter, tickLines, paintRect);
    } else {
        paintVertical(painter, tickLines, paintRect);
    }

    // display the hovered value
    if (displayHoverValue_) {
        QString hoveredStr = getCovertedString(hoveredValue_);
        double offset = tickLength_ + spacing_;
        double x = 0, y = 0, w = 0;

        if (orientation_ == Qt::Horizontal) {
            w = qMin(paintRect.width() - 1, fontMetrics().width(hoveredStr) + spacing_ * 2.0);
            y = slider_.getAlignment() == Qt::AlignTop ? paintRect.top() : paintRect.top() + offset;
            x = hoveredPos_ - w / 2.0;
            if (x < 0) {
                x = 0;
            } else if ((x + w) >= width()) {
                x = width() - w - 1;
            }
        } else {
            hoveredStr.truncate(qMin(hoveredStr.length(), widestTextLength_ - 1));
            if (hoveredStr.at(hoveredStr.length() - 1) == ".") {
                hoveredStr.truncate(hoveredStr.length() - 1);
            }
            w = fontMetrics().width(hoveredStr) + spacing_ * 2.0;
            x = slider_.getAlignment() == Qt::AlignLeft ? width()- w - offset : offset;
            y = hoveredPos_ - textHeight_ / 2.0;
            if (y < 0) {
                y = 0;
            } else if ((y + textHeight_) >= height()) {
                y = height() - textHeight_ - 1;
            }
        }

        QRectF hoveredRect(x, y, w, textHeight_);
        painter.setBrush(hoveredRectColor_);
        painter.setPen(hoverTextColor_);
        painter.drawRect(hoveredRect);
        painter.drawText(hoveredRect, hoveredStr, QTextOption(Qt::AlignCenter));
    }

    painter.setPen(QPen(axisColor_, penWidth_));
    if (axisLineVisible_){
        painter.drawLine(axisLine_);
    }
    if (tickVisible_) {
        painter.drawLines(tickLines);
    }
}


/**
 * @brief AxisDisplay::paintHorizontal
 * @param painter
 * @param tickLines
 * @param rect
 */
void AxisDisplay::paintHorizontal(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect)
{
    double rectWidth = rect.width() / tickCount_;
    double lineLength = tickLength_;
    double tickY = rect.top() + penWidth_ / 2.0;
    double offset = tickLength_ + spacing_;
    QSizeF rectSize(rectWidth, rect.height() - offset);
    QPointF startPoint(-rectWidth / 2.0 + rect.left(), rect.top() + offset);
    QPointF dateRectCenter(rect.center().x(), rect.center().y() + rectSize.height() / 2.0);

    if (slider_.getAlignment() == Qt::AlignTop) {
        tickY = rect.bottom() - penWidth_ / 2.0;
        lineLength = -tickLength_;
        startPoint.setY(rect.top());
        dateRectCenter.setY(rect.center().y() - rectSize.height() / 2.0);
    }
    axisLine_.setLine(rect.left(), tickY, rect.right(), tickY);

    if (tickCount_ == 1) {
        startPoint.setX(0);
        dateRectCenter.setX(rect.center().x());
    }

    bool dateTimeFormat = (valueType_ == VALUE_TYPE::DATE_TIME) && (axisFormat_ == TIME_DISPLAY_FORMAT::DATE_TIME);
    QRectF textRect;
    QDate prevDate;

    double minTextWidth = 0.0;
    double maxTextWidth = 0.0;

    for (int i = 0; i <= tickCount_; i++) {

        textRect = QRectF(startPoint, rectSize);

        // tickX has to be an int so that the hovered value matches the displayed value
        int tickX = textRect.center().x();
        double value = (double)tickX / width() * displayedRange_ + displayedMin_;

        if (tickCount_ == 1) {
            tickX = dateRectCenter.x();
            value = displayedMin_ + displayedRange_ / 2.0;
        } else if (i == 0) {
            tickX = penWidth_ / 2.0;
            value = displayedMin_;
            minTextWidth = fontMetrics().width(getCovertedString(value));
            textRect = textRect.adjusted(minTextWidth + penWidth_, 0, 0, 0);
            if (axisFormat_ == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
                startPoint += QPointF(rectWidth, 0);
                continue;
            }
        } else if (i == tickCount_) {
            if (axisFormat_ == TIME_DISPLAY_FORMAT::ELAPSED_TIME) {
                return;
            }
            tickX -= penWidth_ / 2.0;
            value = displayedMax_;
            maxTextWidth = fontMetrics().width(getCovertedString(value));
            textRect = textRect.adjusted(0, 0, -maxTextWidth - penWidth_, 0);
        }

        tickLines.append(QLineF(tickX, tickY, tickX, tickY + lineLength));
        painter.drawText(textRect, getCovertedString(value), QTextOption(textAlignment_));
        startPoint += QPointF(rectWidth, 0);

        // paint the date-time
        if (dateTimeFormat) {
            QDate date = QDateTime::fromMSecsSinceEpoch(value).date();
            if (date == prevDate)
                continue;
            auto dateStr = date.toString(DATE_FORMAT);
            textRect.moveCenter(QPointF(textRect.center().x(), dateRectCenter.y()));
            if (tickCount_ != 1) {
                if (i == 0) {
                    auto strLengthDiff = fontMetrics().width(dateStr) - minTextWidth;
                    if (strLengthDiff > 0) {
                        textRect.moveRight(textRect.right() + strLengthDiff - 5);
                    }
                } else if (i == tickCount_) {
                    auto strLengthDiff = fontMetrics().width(dateStr) - maxTextWidth;
                    if (strLengthDiff > 0) {
                        textRect.moveLeft(textRect.left() - strLengthDiff + 5);
                    }
                }
            }
            painter.drawText(textRect, dateStr, QTextOption(textAlignment_));
            prevDate = date;
        }
    }
}


/**
 * @brief AxisDisplay::paintVertical
 * @param painter
 * @param tickLines
 * @param rect
 */
void AxisDisplay::paintVertical(QPainter &painter, QVector<QLineF> &tickLines, QRectF &rect)
{
    double rectHeight = rect.height() / tickCount_;
    double lineLength = tickLength_;
    double tickX = rect.left();
    double offset = tickLength_ + spacing_;
    QPointF startPoint(rect.left() + offset, rect.top() + (penWidth_ - rectHeight) / 2.0);

    if (slider_.getAlignment() == Qt::AlignLeft) {
        tickX = rect.right();
        lineLength = -tickLength_;
        startPoint.setX(rect.left());
    }
    axisLine_.setLine(tickX, rect.top(), tickX, rect.bottom());

    QRectF textRect;

    for (int i = 0; i <= tickCount_; i++) {

        textRect = QRectF(startPoint, QSize(rect.width() - offset, rectHeight));

        int tickY = textRect.center().y();
        double value = (1.0 - (double)tickY / height()) * displayedRange_ + displayedMin_;
        if (i == 0) {
            tickY = penWidth_ / 2.0;
            value = displayedMax_;
            textRect = textRect.adjusted(0, textHeight_, 0, 0);
        } else if (i == tickCount_){
            tickY -= penWidth_ / 2.0;
            value = displayedMin_;
            textRect = textRect.adjusted(0, 0, 0, -textHeight_);
        }

        tickLines.append(QLineF(tickX, tickY, tickX + lineLength, tickY));
        painter.drawText(textRect, getCovertedString(value), QTextOption(textAlignment_));
        startPoint += QPointF(0, rectHeight);
    }
}


/**
 * @brief AxisDisplay::displayedMinChanged
 * @param min
 */
void AxisDisplay::displayedMinChanged(double min)
{
    displayedMin_ = min;
    displayedRange_ = displayedMax_ - min;
    rangeChanged();
}


/**
 * @brief AxisDisplay::displayedMaxChanged
 * @param max
 */
void AxisDisplay::displayedMaxChanged(double max)
{
    displayedMax_ = max;
    displayedRange_ = max - displayedMin_;
    rangeChanged();
}


/**
 * @brief AxisDisplay::rangeChanged
 */
void AxisDisplay::rangeChanged()
{
    // update the vertical axes' width based on the new displayed range
    if (orientation_ == Qt::Vertical) {
        double rectHeight = getAdjustedRect().height() / tickCount_;
        int tickY = getAdjustedRect().top();
        int maxLength = 0;
        for (int i = 0; i <= tickCount_; i++) {
            double value = (1.0 - (double)tickY / height()) * displayedRange_ + displayedMin_;
            if (i == 0) {
                value = displayedMax_;
            } else if (i == tickCount_){
                value = displayedMin_;
            }
            maxLength = qMax(maxLength, getCovertedString(value).length());
            tickY += rectHeight;
        }

        if (maxLength != widestTextLength_) {
            setFixedWidth(maxLength * fontMetrics().width("0") + tickLength_ + spacing_ * 2);
            widestTextLength_ = maxLength;
        }
    } 
    update();
}


/**
 * @brief AxisDisplay::getDateTimeString
 * @param value
 * @return
 */
QString AxisDisplay::getDateTimeString(double value)
{
    auto displayFormat = TIME_FORMAT;
    auto maxElapedMS = displayedMax_ - displayedMin_;
    auto maxElapsedDays = maxElapedMS / 8.64e7;
    auto maxElapsedHours = maxElapedMS / 3.6e6;
    auto maxElapsedMins = maxElapedMS / 6e4;

    if (maxElapsedDays >= 1 || maxElapsedHours > 1) {
        // if the max displayed elapsed time is a day or more, only show the hours/mins
        displayFormat = "hh:mm";
    } else if (maxElapsedMins >= 1) {
        // if the max displayed elapsed time is a minute or more, show the hours/mins/secs
        displayFormat = "hh:mm:ss";
    } else {
        // if it's anything smaller, show all
        displayFormat = "hh:mm:ss.zzz";
    }

    return QDateTime::fromMSecsSinceEpoch(value).toString(displayFormat);
}


/**
 * @brief AxisDisplay::getElapsedTimeString
 * @param value
 * @return
 */
QString AxisDisplay::getElapsedTimeString(double value)
{
    auto msecs = value - min_;
    if (msecs <= 0) {
        return "0ms";
    } else if (msecs < 1000) {
        return QString::number((int)msecs) + "ms";
    }

    auto maxElapedMS = displayedMax_ - min_;
    auto maxElapsedDays = maxElapedMS / 8.64e7;
    auto maxElapsedHours = maxElapedMS / 3.6e6;
    auto maxElapsedMins = maxElapedMS / 6e4;

    int d = 0, h = 0, m = 0, s = 0, ms = 0;

    if (maxElapsedDays >= 1) {
        // if the max displayed elapsed time is a day or more, only show the elapsed days/hours
        d = msecs / 8.64e7;
        if (d >= 100)
            return QString::number(d) + "d";
        h = (msecs / 3.6e6) - ((int)d * 24);
    } else if (maxElapsedHours >= 1) {
        // if the max displayed elapsed time is an hour or more, only show the elapsed hours/mins
        h = msecs / 3.6e6;
        if (h >= 10)
            return QString::number(h) + "h";
        m = (msecs / 6e4) - (h * 60);
    } else if (maxElapsedMins >= 1) {
        // if the max displayed elapsed time is a minute or more, only show the elapsed mins/secs
        m = msecs / 6e4;
        s = (msecs / 1e3) - (m * 60);
    } else {
        // if it's anything smaller, show the elapsed secs/msecs
        s = msecs / 1e3;
        ms = msecs - (s * 1000);
    }

    QString elapsedTime = "";
    if (d > 0)
        elapsedTime = QString::number(d) + "d";
    if (h > 0)
        elapsedTime += QString::number(h) + "h";
    if (m > 0)
        elapsedTime += QString::number(m) + "m";
    if (s > 0) {
        if (ms > 0) {
            elapsedTime += QString::number(s) + "." + QString::number(ms) + "s";
        } else {
            elapsedTime += QString::number(s) + "s";
        }
    }

    return elapsedTime;
}


/**
 * @brief AxisDisplay::getCovertedString
 * @param value
 * @return
 */
QString AxisDisplay::getCovertedString(double value)
{
    switch (axisFormat_) {
    case TIME_DISPLAY_FORMAT::ELAPSED_TIME:
        return getElapsedTimeString(value);
    case TIME_DISPLAY_FORMAT::DATE_TIME:
        return getDateTimeString(value);
    default:
        return QString::number(value);
    }
}


/**
 * @brief AxisDisplay::getAdjustedRect
 * @return
 */
QRectF AxisDisplay::getAdjustedRect()
{
    double halfPenWidth = penWidth_ / 2.0;
    return rect().adjusted(halfPenWidth, halfPenWidth, -halfPenWidth, -halfPenWidth);
}
