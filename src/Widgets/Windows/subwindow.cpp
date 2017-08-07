#include "subwindow.h"
#include "viewwindow.h"

SubWindow::SubWindow():ViewWindow()
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
    //setMinimumSize(450,300);
}

SubWindow::~SubWindow() {}

