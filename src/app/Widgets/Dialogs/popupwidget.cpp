#include "popupwidget.h"
#include "../../theme.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QKeyEvent>

/**
 * @brief PopupWidget::PopupWidget
 * @param type
 * @param parent
 */
PopupWidget::PopupWidget(PopupWidget::TYPE type, QWidget* parent) : QDialog(parent)
{
    setModal(false);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);

    switch (type) {
    case TYPE::DIALOG:
        setWindowFlags(windowFlags() | Qt::Dialog);
        setModal(true);
        break;
    case TYPE::TOOL:
        setWindowFlags(windowFlags() | Qt::Tool);
        break;
    case TYPE::SPLASH:
        setAttribute(Qt::WA_ShowWithoutActivating);
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint); // | Qt::WindowSystemMenuHint);
        break;
    case TYPE::POPUP:
        setWindowFlags(windowFlags() | Qt::Popup);
        break;
    }
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &PopupWidget::themeChanged);
    themeChanged();
}


/**
 * @brief PopupWidget::getWidget
 * @return
 */
QWidget* PopupWidget::getWidget()
{
    return widget_;
}


/**
 * @brief PopupWidget::setWidget
 * @param widget
 */
void PopupWidget::setWidget(QWidget* widget)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(5);
    layout->addWidget(widget);
    widget_ = widget;
}


/**
 * @brief PopupWidget::setBackgroundOpacity
 * @param opactiy
 */
void PopupWidget::setBackgroundOpacity(qreal opactiy)
{
    this->opacity = opactiy;
    background_color.setAlphaF(opactiy);
    border_color.setAlphaF(opactiy);
    update();
}


/**
 * @brief PopupWidget::adjustSize
 */
void PopupWidget::adjustSize()
{
    QWidget::adjustSize();
}


/**
 * @brief PopupWidget::themeChanged
 */
void PopupWidget::themeChanged()
{
    auto theme = Theme::theme();
    background_color = theme->getBackgroundColor();
    background_color.setAlphaF(opacity);
    border_color = theme->getTextColor();
    border_color.setAlphaF(opacity);
    update();
}


/**
 * @brief PopupWidget::paintEvent
 * @param event
 */
void PopupWidget::paintEvent(QPaintEvent* event)
{
    qreal penWidth = 1.0;
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(border_color, penWidth));
    painter.setBrush(background_color);
    painter.drawRoundedRect(rect().adjusted(penWidth, penWidth, -penWidth, -penWidth), 4, 4);
}


/**
 * @brief PopupWidget::keyPressEvent
 * @param event
 */
void PopupWidget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        accept();
        return;
    }else if(event->key() == Qt::Key_Escape){
        reject();
        return;
    }
    QDialog::keyPressEvent(event);
}
