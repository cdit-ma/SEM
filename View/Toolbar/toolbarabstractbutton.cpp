#include "toolbarabstractbutton.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QMouseEvent>


/**
 * @brief ToolbarAbstractButton::ToolbarAbstractButton
 * @param parent
 * @param defaultColor
 * @param checkedColor
 * @param borderWidth
 * @param borderRadius
 */
ToolbarAbstractButton::ToolbarAbstractButton(QWidget* parent, QColor defaultColor, QColor checkedColor, float borderWidth, int borderRadius) :
    QAbstractButton(parent)
{
    state = BS_DEFAULT;
    acceptMouseEvents = true;

    defaultBorderWidth = borderWidth;
    this->borderRadius = borderRadius;

    if (defaultColor == QColor()) {
        this->defaultColor = palette().color(QPalette::Button);
    } else {
        this->defaultColor = defaultColor;
    }

    if (checkedColor == QColor()) {
        this->checkedColor = this->defaultColor.darker(120);
    } else {
        this->checkedColor = checkedColor;
    }

    currentColor = this->defaultColor;
    currentBorderWidth = defaultBorderWidth;

    setMouseTracking(true);
    enableHoveredColor(true);
    updateButtonStyle();
}


/**
 * @brief ToolbarAbstractButton::enableHoveredColor
 * @param enable
 * @param darker
 */
void ToolbarAbstractButton::enableHoveredColor(bool enable, bool darker)
{
    if (enable) {
        if (darker) {
            hoveredColor = defaultColor.darker(110);
        } else {
            hoveredColor = defaultColor.lighter(110);
        }
    } else {
        hoveredColor = defaultColor;
    }
}


/**
 * @brief ToolbarAbstractButton::setAcceptMouseEvents
 * @param accept
 */
void ToolbarAbstractButton::setAcceptMouseEvents(bool accept)
{
    acceptMouseEvents = accept;
}


/**
 * @brief ToolbarAbstractButton::setButtonHovered
 * @param hover
 */
void ToolbarAbstractButton::setButtonHovered(bool hover)
{
    if (hover) {
        switch (state) {
        case BS_DEFAULT:
            state = BS_HOVERED;
            break;
        default:
            return;
        }
    } else {
        switch (state) {
        case BS_HOVERED:
            state = BS_DEFAULT;
            break;
        default:
            return;
        }
    }
    updateButtonStyle();
}


/**
 * @brief ToolbarAbstractButton::setButtonChecked
 */
void ToolbarAbstractButton::setButtonChecked()
{
    switch (state) {
    case BS_DEFAULT:
        return;
    case BS_HOVERED:
        state = BS_CHECKED;
        break;
    case BS_CHECKED:
        state = BS_HOVERED;
        break;
    default:
        return;
    }
    updateButtonStyle();
}


/**
 * @brief ToolbarAbstractButton::isButtonChecked
 * @return
 */
bool ToolbarAbstractButton::isButtonChecked()
{
    return state == BS_CHECKED;
}


/**
 * @brief ToolbarAbstractButton::paintEvent
 * @param event
 */
void ToolbarAbstractButton::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    QRectF trimmedRect(rect());

    QPainter painter(this);
    QBrush brush(currentColor);

    if (currentBorderWidth > 0) {
        QPen pen(Qt::gray);
        pen.setWidthF(currentBorderWidth);
        painter.setPen(pen);
        trimmedRect.adjust(currentBorderWidth/2, currentBorderWidth/2, -currentBorderWidth/2, -currentBorderWidth/2);
    } else {
        painter.setPen(Qt::NoPen);
    }

    path.addRoundedRect(trimmedRect, borderRadius, borderRadius);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPath(path);
    painter.fillPath(path, brush);
    QWidget::paintEvent(event);
}


/**
 * @brief ToolbarAbstractButton::enterEvent
 * @param event
 */
void ToolbarAbstractButton::enterEvent(QEvent* event)
{
    if (acceptMouseEvents) {
        setButtonHovered(true);
        QAbstractButton::enterEvent(event);
    }
}


/**
 * @brief ToolbarAbstractButton::leaveEvent
 * @param event
 */
void ToolbarAbstractButton::leaveEvent(QEvent* event)
{
    if (acceptMouseEvents) {
        setButtonHovered(false);
        QAbstractButton::leaveEvent(event);
    }
}


/**
 * @brief ToolbarAbstractButton::mousePressEvent
 * @param event
 */
void ToolbarAbstractButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QAbstractButton::mousePressEvent(event);
    }
}


/**
 * @brief ToolbarAbstractButton::mouseDoubleClickEvent
 * @param event
 */
void ToolbarAbstractButton::mouseDoubleClickEvent(QMouseEvent*)
{
    // Ignore double mouse clicks
}


/**
 * @brief ToolbarAbstractButton::stealMouse
 */
void ToolbarAbstractButton::stealMouse()
{
    grabMouse();
    releaseMouse();
}


/**
 * @brief ToolbarAbstractButton::updateButtonStyle
 */
void ToolbarAbstractButton::updateButtonStyle()
{
    switch(state) {
    case BS_DEFAULT:
        currentColor = defaultColor;
        currentBorderWidth = defaultBorderWidth;
        break;
    case BS_HOVERED:
        currentColor = hoveredColor;
        currentBorderWidth = defaultBorderWidth + 1;
        break;
    case BS_CHECKED:
        currentColor = checkedColor;
        currentBorderWidth = defaultBorderWidth;
        break;
    }
    repaint();
}



