#include "tooldockwidget.h"
#include "../../theme.h"

ToolDockWidget::ToolDockWidget(QString title, QWidget* parent) : BaseDockWidget(BaseDockType::TOOL, parent)
{
    setTitle(title, Qt::AlignCenter);
    setFocusEnabled(false);
    setHideVisible(true);
    setIconVisible(false);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    themeChanged();
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
