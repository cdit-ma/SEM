#include "centralwindow.h"

CentralWindow::CentralWindow():ViewWindow()
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
}

CentralWindow::~CentralWindow()
{
}


