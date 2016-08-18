#include "medeatooldockwidget.h"

MedeaToolDockWidget::MedeaToolDockWidget(QString title) : MedeaDockWidget(MedeaDockWidget::MDW_TOOL)
{
    setTitle(title, Qt::AlignCenter);
    setFocusEnabled(false);
    setHideVisible(true);

    setTitleBarIconSize(12);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    setStyleSheet("DockTitleBarWidget QToolButton {padding:0px;border-radius:2px;}");
}

