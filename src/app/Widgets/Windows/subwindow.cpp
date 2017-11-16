#include "subwindow.h"
#include "viewwindow.h"

SubWindow::SubWindow(BaseWindow* parent_window):ViewWindow(parent_window)
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
    //setMinimumSize(450,300);

    addActions(parent_window->actions());
}

SubWindow::~SubWindow() {}

