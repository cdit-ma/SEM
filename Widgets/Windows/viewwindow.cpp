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

    //Setup Reset action
    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);

    connect(resetDockedWidgetsAction, SIGNAL(triggered(bool)), this, SLOT(resetDockWidgets()));




    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

ViewWindow::~ViewWindow()
{

}
void ViewWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    ViewDockWidget* dockWidget = qobject_cast<ViewDockWidget*>(widget);
    if(dockWidget){
        //Only allow View Dock Widgets.
        BaseWindow::addDockWidget(area, widget, orientation);
    }
}

void ViewWindow::themeChanged()
{
    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));
}

void ViewWindow::resetDockWidgets()
{
    foreach(BaseDockWidget* child, getDockWidgets()){
        child->setVisible(true);
    }
}

QMenu *ViewWindow::createPopupMenu()
{
    QMenu* menu = BaseWindow::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}


