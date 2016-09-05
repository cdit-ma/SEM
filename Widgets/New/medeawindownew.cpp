#include "medeawindownew.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include "../../View/docktitlebarwidget.h"
#include "../../Widgets/New/medeawindowmanager.h"
#include "../../View/theme.h"

int MedeaWindowNew::_WindowID = 0;

MedeaWindowNew::MedeaWindowNew(QWidget *parent, MedeaWindowNew::WindowType type):QMainWindow(parent)
{
    ID = ++_WindowID;
    windowType = type;

    setDockNestingEnabled(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::ClickFocus);


    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));


    //Setup Tab positions

}

MedeaWindowNew::~MedeaWindowNew()
{
    //Unset Original flag on all owned DockWidgets.
    while(!ownedDockWidgets.isEmpty()){
        MedeaDockWidget* dockWidget = ownedDockWidgets.takeFirst();
        dockWidget->setSourceWindow(0);
    }
    //Clean up current dock widget.
    while(!currentDockWidgets.isEmpty()){
        MedeaDockWidget* dockWidget = currentDockWidgets.takeFirst();
        if(dockWidget){
            //This should cause MedeaWindowManager to shutdown the widget.
            emit dockWidget->closeWidget();
        }
    }
}

bool MedeaWindowNew::hasDockWidgets()
{
    return !currentDockWidgets.isEmpty();
}

QList<MedeaDockWidget *> MedeaWindowNew::getDockWidgets()
{
    return currentDockWidgets;
}

int MedeaWindowNew::getID()
{
    return ID;
}

MedeaWindowNew::WindowType MedeaWindowNew::getType()
{
    return windowType;
}

void MedeaWindowNew::addDockWidget(MedeaDockWidget *widget)
{
    addDockWidget(widget->getDockWidgetArea(), widget, Qt::Horizontal);
}

void MedeaWindowNew::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget)
{
    addDockWidget(area, widget, Qt::Horizontal);
}

void MedeaWindowNew::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(widget);
    if(dockWidget){
        if(!currentDockWidgets.contains(dockWidget)){
            if(!dockWidget->getSourceWindow()){
                //Set the Dock's source window to this.
                dockWidget->setSourceWindow(this);
                ownedDockWidgets.append(dockWidget);
            }
            dockWidget->setCurrentWindow(this);
            currentDockWidgets.append(dockWidget);
        }
    }
    QMainWindow::addDockWidget(area, widget, orientation);
}

void MedeaWindowNew::removeDockWidget(QDockWidget *widget)
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(widget);
    if(dockWidget){
        if(currentDockWidgets.contains(dockWidget)){
            currentDockWidgets.removeAll(dockWidget);
            //Unset current window
            dockWidget->setCurrentWindow(0);
        }
    }
    QMainWindow::removeDockWidget(widget);

    if(currentDockWidgets.isEmpty()){
        MedeaWindowManager::destructWindow(this);
    }
}

void MedeaWindowNew::setDockWidgetMaximized(MedeaDockWidget *dockwidget, bool maximized)
{
    foreach(MedeaDockWidget* dw, currentDockWidgets){
        bool setVisible = !maximized;
        dw->setVisible(setVisible);
    }

    if(maximized){
        dockwidget->setVisible(maximized);
    }

}




void MedeaWindowNew::closeEvent(QCloseEvent * event)
{
    MedeaWindowManager::destructWindow(this);
}

bool MedeaWindowNew::focusNextPrevChild(bool)
{
    //Disable Tabs
    return false;
}

void MedeaWindowNew::showContextMenu(const QPoint & point)
{
    createPopupMenu()->exec(mapToGlobal(point));
}
QMenu *MedeaWindowNew::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();

    return menu;
}


