#include "viewwindow.h"

#include "../DockWidgets/viewdockwidget.h"
#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../theme.h"

#include <QStringBuilder>
#include <QMenu>
ViewWindow::ViewWindow():BaseWindow(0, BaseWindow::VIEW_WINDOW)
{

    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    //Setup Tab positions
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

}

ViewWindow::~ViewWindow()
{

}
void ViewWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    auto default_dock_widget = qobject_cast<DefaultDockWidget*>(widget);
    if(default_dock_widget){
        //Only allow View Dock Widgets.
        BaseWindow::addDockWidget(area, widget, orientation);
    }
}




