#include "medeaviewdockwidget.h"
#include "../../View/theme.h"
#include <QDebug>

MedeaViewDockWidget::MedeaViewDockWidget(QString title, Qt::DockWidgetArea area, VIEWDOCKWIDGET_TYPE type):MedeaDockWidget(MedeaDockWidget::MDW_VIEW)
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

bool MedeaViewDockWidget::isNodeViewDock()
{
    return type == MVDW_NODEVIEW;
}

void MedeaViewDockWidget::themeChanged()
{
    setStyleSheet(Theme::theme()->getViewDockWidgetStyleSheet(isActive()));
}
