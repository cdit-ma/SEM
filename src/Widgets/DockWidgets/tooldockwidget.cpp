#include "tooldockwidget.h"
#include "../../theme.h"

ToolDockWidget::ToolDockWidget(QString title) : BaseDockWidget(BaseDockType::TOOL)
{
    setTitle(title, Qt::AlignCenter);
    setFocusEnabled(false);
    setHideVisible(true);

    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    themeChanged();
    auto title_bar = getTitleBar();
    title_bar->getAction(DockTitleBar::DA_ICON)->setVisible(false);
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
