#include "invisibledockwidget.h"
#include "../ViewManager/viewmanagerwidget.h"
#include "../ViewManager/windowitem.h"
#include "../../theme.h"

InvisibleDockWidget::InvisibleDockWidget(QString title, QWidget* parent) : BaseDockWidget(BaseDockType::INVISIBLE, parent)
{
    removeTitleBar();
    setTitleBarWidget(new QWidget(this));
    setTitle(title);
}

