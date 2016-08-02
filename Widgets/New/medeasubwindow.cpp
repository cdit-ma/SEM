#include "medeasubwindow.h"
#include "medeaviewwindow.h"
MedeaSubWindow::MedeaSubWindow():MedeaViewWindow()
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
}

