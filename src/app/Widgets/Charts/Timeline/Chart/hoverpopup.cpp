#include "hoverpopup.h"
#include "../../../../theme.h"

#include <QLayout>
#include <QPainter>

/**
 * @brief HoverPopup::HoverPopup
 * @param parent
 */
HoverPopup::HoverPopup(QWidget* parent)
    : PopupWidget(PopupWidget::TYPE::SPLASH, parent)
{
    useDefaultBorder = true;
    connect(Theme::theme(), &Theme::theme_Changed, this, &HoverPopup::themeChanged);
    themeChanged();
}


/**
 * @brief HoverPopup::setBorderColor
 * @param color
 */
void HoverPopup::setBorderColor(QColor color)
{
    _borderColor = color;
    useDefaultBorder = !color.isValid();
    update();
}


/**
 * @brief HoverPopup::themeChanged
 */
void HoverPopup::themeChanged()
{
    Theme* theme = Theme::theme();
    if (useDefaultBorder) {
        _borderColor = theme->getTextColor();
    }
    _backgroundColor = theme->getActiveWidgetBorderColor();
    update();
}


/**
 * @brief HoverPopup::paintEvent
 * @param event
 */
void HoverPopup::paintEvent(QPaintEvent* event)
{
    qreal penWidth = 1.5;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(_borderColor, penWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(_backgroundColor);
    painter.drawRoundedRect(rect().adjusted(penWidth, penWidth, -penWidth, -penWidth), 6, 6);
}


/**
 * @brief HoverPopup::adjustChildrenSize
 * @param widgetName
 * @param options
 */
void HoverPopup::adjustChildrenSize(QString widgetName, Qt::FindChildOptions options)
{
    for (QWidget* w : findChildren<QWidget*>(widgetName, options)) {
        w->adjustSize();
    }
    QWidget::adjustSize();
}
