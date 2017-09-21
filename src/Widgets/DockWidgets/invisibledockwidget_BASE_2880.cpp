#include "invisibledockwidget.h"
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

void InvisibleDockWidget::themeChanged()
{
}
