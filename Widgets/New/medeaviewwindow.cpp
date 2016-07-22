#include "medeaviewwindow.h"
#include "medeaviewdockwidget.h"
#include "../../View/theme.h"
#include <QMenu>
MedeaViewWindow::MedeaViewWindow():MedeaWindowNew(0, MedeaWindowNew::VIEW_WINDOW)
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    //Setup Tab positions
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

    //Setup Reset action
    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);
    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));
    connect(resetDockedWidgetsAction, SIGNAL(triggered(bool)), this, SLOT(resetDockWidgets()));

}
void MedeaViewWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    MedeaViewDockWidget* dockWidget = qobject_cast<MedeaViewDockWidget*>(widget);
    if(dockWidget){
        //Only allow View Dock Widgets.
        MedeaWindowNew::addDockWidget(area, widget, orientation);
    }
}

void MedeaViewWindow::resetDockWidgets()
{
    foreach(MedeaDockWidget* child, getDockWidgets()){
        child->setVisible(true);
    }
}

void MedeaViewWindow::showContextMenu(const QPoint & point)
{
    createPopupMenu()->exec(mapToGlobal(point));
}
QMenu *MedeaViewWindow::createPopupMenu()
{
    QMenu* menu = MedeaWindowNew::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}

