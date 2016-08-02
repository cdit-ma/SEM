#include "medeacentralwindow.h"

MedeaCentralWindow::MedeaCentralWindow():MedeaViewWindow()
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);
}


