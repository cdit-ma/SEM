#include "medeawindownew.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include <QStringBuilder>
#include "../../View/docktitlebarwidget.h"
#include "../../Widgets/New/medeawindowmanager.h"
#include "../../View/theme.h"

int MedeaWindowNew::_WindowID = 0;

MedeaWindowNew::MedeaWindowNew(QWidget *parent, MedeaWindowNew::WindowType type):QMainWindow(parent)
{
    terminating = false;
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

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

MedeaWindowNew::~MedeaWindowNew()
{
    terminating = true;
    removeAllDockWidgets();
}

bool MedeaWindowNew::hasDockWidgets()
{
    return !currentDockWidgets.isEmpty();
}

QList<MedeaDockWidget *> MedeaWindowNew::getDockWidgets()
{
    return currentDockWidgets.values();
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
        int ID = dockWidget->getID();
        if(!currentDockWidgets.contains(ID)){
            if(!dockWidget->getSourceWindow()){
                //Set the Dock's source window to this.
                dockWidget->setSourceWindow(this);
                ownedDockWidgets.append(dockWidget);
            }

            dockWidget->setCurrentWindow(this);
            currentDockWidgets.insert(ID, dockWidget);
            updateActions();

            connect(dockWidget, &MedeaDockWidget::req_Maximize, this, &MedeaWindowNew::setDockWidgetMaximized);
            connect(dockWidget, &MedeaDockWidget::req_Visible, this, &MedeaWindowNew::setDockWidgetVisibility);
        }
    }
    QMainWindow::addDockWidget(area, widget, orientation);
    if(dockWidget){
        emit dockWidgetAdded(dockWidget);
    }
}

void MedeaWindowNew::removeDockWidget(QDockWidget *widget)
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(widget);
    if(dockWidget){
        int ID = dockWidget->getID();
        if(currentDockWidgets.contains(ID)){
            currentDockWidgets.remove(ID);
        }
        previouslyVisibleDockIDs.removeAll(ID);
        updateActions();

        disconnect(dockWidget, &MedeaDockWidget::req_Maximize, this, &MedeaWindowNew::setDockWidgetMaximized);
        disconnect(dockWidget, &MedeaDockWidget::req_Visible, this, &MedeaWindowNew::setDockWidgetVisibility);
    }

    QMainWindow::removeDockWidget(widget);

    if(!terminating && currentDockWidgets.isEmpty()){
        MedeaWindowManager::destructWindow(this);
    }
}



void MedeaWindowNew::closeEvent(QCloseEvent * event)
{
    removeAllDockWidgets();
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

void MedeaWindowNew::setDockWidgetMaximized(int ID, bool maximized)
{
    if(maximized){
        //Clear the old list of visible docks
        previouslyVisibleDockIDs.clear();

        foreach(MedeaDockWidget* dw, currentDockWidgets.values()){
            int dID = dw->getID();
            if(dw->isVisible()){
                //if the dock was visible before maximizing, store it's ID
                previouslyVisibleDockIDs.append(dID);
            }

            //The only item which should be visible should be the ID passed.
            dw->setVisible(dID == ID);
            //The only item which is maximized is the ID passed.
            dw->setMaximizeToggled(dID == ID);
        }
    }else{
        //Minimize

        if(!previouslyVisibleDockIDs.contains(ID)){
            //We should minimize this item
            previouslyVisibleDockIDs.append(ID);
        }


        foreach(int ID, previouslyVisibleDockIDs){
            MedeaDockWidget* dw = currentDockWidgets.value(ID, 0);
            if(dw){
                dw->setVisible(true);
                dw->setMaximizeToggled(false);
            }
        }
        previouslyVisibleDockIDs.clear();
    }
}

void MedeaWindowNew::setDockWidgetVisibility(int ID, bool visible)
{
    //If we are hiding an item, remove it from the list of previously visible items.
    if(!visible){
        previouslyVisibleDockIDs.removeAll(ID);
    }

    MedeaDockWidget* dw = currentDockWidgets.value(ID, 0);
    if(dw){
        dw->setVisible(visible);
    }
    updateActions();
}

void MedeaWindowNew::removeAllDockWidgets()
{
    //Unset Original flag on all owned DockWidgets.
    while(!ownedDockWidgets.isEmpty()){
        MedeaDockWidget* dockWidget = ownedDockWidgets.takeFirst();
        dockWidget->setSourceWindow(0);
    }

    //Clean up current dock widget.
    foreach(int ID, currentDockWidgets.uniqueKeys()){
        MedeaDockWidget* dockWidget = currentDockWidgets.take(ID);
        if(dockWidget){
            //This should cause MedeaWindowManager to shutdown the widget.
            dockWidget->close();
        }
    }

}

void MedeaWindowNew::updateActions()
{

    int visibleCount = 0;
    MedeaDockWidget* maximizedDW = 0;
    foreach(MedeaDockWidget* dw, currentDockWidgets.values()){
        if(dw->isVisible()){
            if(visibleCount == 0){
                maximizedDW = dw;
            }
            visibleCount ++;
        }
        dw->setMaximizeToggled(false);
    }
    if(visibleCount == 1 && maximizedDW){
        maximizedDW->setMaximizeToggled(true);
    }
}

QMenu *MedeaWindowNew::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();

    return menu;
}

void MedeaWindowNew::themeChanged()
{
    Theme* theme = Theme::theme();
    setStyleSheet(theme->getWindowStyleSheet() %
                  theme->getMenuBarStyleSheet() %
                  theme->getMenuStyleSheet() %
                  theme->getToolBarStyleSheet() %
                  theme->getPushButtonStyleSheet() %
                  theme->getTabbedWidgetStyleSheet() %
                  theme->getScrollBarStyleSheet() %
                  "QToolButton{ padding: 4px; }");
}


