#include "medeatooldockwidget.h"

MedeaToolDockWidget::MedeaToolDockWidget(QString title) : MedeaDockWidget(MedeaDockWidget::MDW_TOOL)
{
    setTitle(title);
    setFocusEnabled(false);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
}

