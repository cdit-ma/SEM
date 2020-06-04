#include "invisibledockwidget.h"
#include "../ViewManager/viewmanagerwidget.h"
#include "../ViewManager/windowitem.h"

InvisibleDockWidget::InvisibleDockWidget(const QString& title, QWidget* parent)
    : BaseDockWidget(BaseDockType::INVISIBLE, parent)
{
    removeTitleBar();
    setTitleBarWidget(new QWidget(this));
    setTitle(title);
}