#include "toolbarwidgetbutton.h"

#include <QDebug>

/**
 * @brief ToolbarWidgetButton::ToolbarWidgetButton
 * @param parent
 */
ToolbarWidgetButton::ToolbarWidgetButton(QWidget *parent) :
    QPushButton(parent)
{
    checked = false;
}

/**
 * @brief ToolbarWidgetButton::~ToolbarWidgetButton
 */
ToolbarWidgetButton::~ToolbarWidgetButton()
{

}

/**
 * @brief ToolbarWidgetButton::setChecked
 * @param checked
 */
void ToolbarWidgetButton::setCheck(bool checked)
{
    this->checked = checked;
}

/**
 * @brief ToolbarWidgetButton::getChecked
 * @return
 */
bool ToolbarWidgetButton::getCheck()
{
    return checked;
}

/**
 * @brief ToolbarWidgetButton::enterEvent
 */
void ToolbarWidgetButton::enterEvent(QEvent *e)
{
    //qDebug() << "MouseEnter";
    emit mouseEntered();
    QWidget::enterEvent(e);
}

/**
 * @brief ToolbarWidgetButton::leaveEvent
 */
void ToolbarWidgetButton::leaveEvent(QEvent *e)
{
    //qDebug() << "mouseExit";
    emit mouseExited();
    QWidget::leaveEvent(e);
}
