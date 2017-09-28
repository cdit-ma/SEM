#include "invisibledockwidget.h"
#include "../ViewManager/viewmanagerwidget.h"
#include "../ViewManager/windowitem.h"
#include "../../theme.h"

InvisibleDockWidget::InvisibleDockWidget(QString title) : BaseDockWidget(BaseDockType::INVISIBLE)
{
    removeTitleBar();
    setTitleBarWidget(new QWidget(this));
    setTitle(title);
}

InvisibleDockWidget::~InvisibleDockWidget()
{
}

