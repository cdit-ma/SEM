#include "medeatooldockwidget.h"
#include "../../View/theme.h"

MedeaToolDockWidget::MedeaToolDockWidget(QString title) : MedeaDockWidget(MedeaDockWidget::MDW_TOOL)
{
    setTitle(title, Qt::AlignCenter);
    setFocusEnabled(false);
    setHideVisible(true);

    setTitleBarIconSize(12);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    themeChanged();
}

void MedeaToolDockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getDockWidgetStyleSheet());
    if (getTitleBar()) {
        getTitleBar()->setStyleSheet(theme->getToolDockWidgetTitleBarStyleSheet());
    }
}
