#include "viewdockwidget.h"
#include "../../theme.h"
#include <QDebug>

ViewDockWidget::ViewDockWidget(QString title, Qt::DockWidgetArea area, VIEWDOCKWIDGET_TYPE type):BaseDockWidget(BaseDockWidget::MDW_VIEW)
{
    setTitle(title);
    setDockWidgetArea(area);

    this->type = type;
    setCloseVisible(true);
    setFocusEnabled(true);
    setMaximizeVisible(true);
    setPopOutVisible(true);
    setHideVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    themeChanged();
}

ViewDockWidget::~ViewDockWidget()
{
}

bool ViewDockWidget::isNodeViewDock()
{
    return type == MVDW_NODEVIEW;
}

void ViewDockWidget::themeChanged()
{
    setStyleSheet(Theme::theme()->getViewDockWidgetStyleSheet(isActive()));
    BaseDockWidget::themeChanged();
}

void ViewDockWidget::setWidget(QWidget *widget)
{
    if (widget) {
        widget->setMinimumSize(400, 300);
        BaseDockWidget::setWidget(widget);
    }
}
