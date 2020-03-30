#include "subwindow.h"

SubWindow::SubWindow(BaseWindow* parent_window)
    : ViewWindow(parent_window)
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

    addActions(parent_window->actions());
}