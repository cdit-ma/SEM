#include "popupwidget.h"
#include "../View/theme.h"
#include <QVBoxLayout>


/**
 * @brief PopupWidget::PopupWidget
 * @param parent
 */
PopupWidget::PopupWidget(bool modal, QWidget* parent) : QDialog(parent)
{
    mainFrame = new QFrame(this);

    setAttribute(Qt::WA_TranslucentBackground);
    setModal(modal);

    if (modal) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Dialog);
    } else {
        setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);
    }
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

    // set the default size to be the same as the widget plus the margin
    setSize(widget->sizeHint().width() + 10, widget->sizeHint().height() + 10);
}


/**
 * @brief PopupWidget::setWidth
 * @param width
 * @param type
 */
void PopupWidget::setWidth(int width, PopupWidget::SIZE_TYPE type)
{
    mainFrame->setFixedWidth(width);
    setFixedWidth(width);
    return;

    switch (type) {
    case MIN:
        //mainFrame->setMinimumWidth(width);
        setMinimumWidth(width);
        break;
    case MAX:
        //mainFrame->setMaximumWidth(width);
        setMaximumWidth(width);
        break;
    default:
        mainFrame->setFixedWidth(width);
        setFixedWidth(width);
        break;
    }
    mainFrame->setMinimumWidth(width);
    //mainFrame->resize(sizeHint());
}


/**
 * @brief PopupWidget::setHeight
 * @param height
 * @param type
 */
void PopupWidget::setHeight(int height, PopupWidget::SIZE_TYPE type)
{
    mainFrame->setFixedHeight(height);
    setFixedHeight(height);
    return;

    switch (type) {
    case MIN:
        //mainFrame->setMinimumHeight(height);
        setMinimumHeight(height);
        break;
    case MAX:
        mainFrame->setMaximumHeight(height);
        setMaximumHeight(height);
        break;
    default:
        //mainFrame->setFixedHeight(height);
        setFixedHeight(height);
        break;
    }
    mainFrame->setMinimumHeight(height);
    //mainFrame->resize(sizeHint());
}


/**
 * @brief PopupWidget::setSize
 * @param width
 * @param height
 */
void PopupWidget::setSize(int width, int height, SIZE_TYPE type)
{
    setWidth(width, type);
    setHeight(height, type);
}
