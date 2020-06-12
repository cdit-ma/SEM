#include "tooldockwidget.h"
#include "../../theme.h"

ToolDockWidget::ToolDockWidget(const QString& title, QWidget* parent)
    : BaseDockWidget(BaseDockType::TOOL, parent)
{
    setTitle(title, Qt::AlignCenter);
    setHideVisible(true);
    setIconVisible(false);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
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