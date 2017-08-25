#include "tooldockwidget.h"
#include "../../theme.h"

ToolDockWidget::ToolDockWidget(QString title) : BaseDockWidget(BaseDockType::TOOL)
{
    setTitle(title, Qt::AlignCenter);
    setFocusEnabled(false);
    setHideVisible(true);

    setTitleBarIconSize(12);
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    themeChanged();
}

ToolDockWidget::~ToolDockWidget()
{
}

void ToolDockWidget::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getDockWidgetStyleSheet());

    if (getTitleBar()) {
        getTitleBar()->setStyleSheet(theme->getToolDockWidgetTitleBarStyleSheet());
    }

    BaseDockWidget::themeChanged();
}
