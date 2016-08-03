#include "medeasubwindow.h"
#include "medeaviewwindow.h"
#include "../../View/theme.h"
MedeaSubWindow::MedeaSubWindow():MedeaViewWindow()
{
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

}

