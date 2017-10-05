#include "popupwidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include "../../theme.h"

/**
 * @brief PopupWidget::PopupWidget
 * @param parent
 */
PopupWidget::PopupWidget(PopupWidget::TYPE type, QWidget* parent) : QDialog(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setModal(false);

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
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_ShowWithoutActivating);
        //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowSystemMenuHint);
        //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
        
        break;
    case TYPE::POPUP:
        setWindowFlags(windowFlags() | Qt::Popup);
        break;
    }
    

    connect(Theme::theme(), &Theme::theme_Changed, this, &PopupWidget::themeChanged);
    themeChanged();
}

void PopupWidget::adjustSize(){
    QWidget::adjustSize();
}


void PopupWidget::themeChanged(){
    auto theme = Theme::theme();
    background_color = theme->getBackgroundColor();
    background_color.setAlphaF(0.85);
    border_color = theme->getTextColor();
    border_color.setAlphaF(0.85);
    update();
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
}


void PopupWidget::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(border_color);
    painter.setBrush(background_color);
    painter.drawRoundedRect(rect(), 4, 4);
}
