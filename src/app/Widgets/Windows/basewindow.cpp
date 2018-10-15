#include "basewindow.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include <QStringBuilder>

#include "../DockWidgets/docktitlebar.h"

#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../theme.h"

int BaseWindow::_WindowID = 0;

BaseWindow::BaseWindow(QWidget *parent, BaseWindow::WindowType type):QMainWindow(parent)
{
    terminating = false;
    ID = ++_WindowID;
    windowType = type;

    // Why was this set to false?
    //setDockNestingEnabled(false);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setFocusPolicy(Qt::ClickFocus);

    //setMinimumSize(550,350);

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(showContextMenu(const QPoint &)));

    reset_action = new QAction("Reset Docked Widgets", this);
    connect(reset_action, &QAction::triggered, this, &BaseWindow::resetDockWidgets);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    themeChanged();
}

void BaseWindow::resetDockWidgets(){
    for(auto dock_widget : getDockWidgets()){
        dock_widget->setVisible(true);
    }
    emit dockWidgetVisibilityChanged();
}

BaseWindow::~BaseWindow()
{
    terminating = true;
    removeAllDockWidgets();
}

QList<BaseDockWidget *> BaseWindow::getDockWidgets()
{
    return currentDockWidgets.values();
}

int BaseWindow::getID()
{
    return ID;
}

BaseWindow::WindowType BaseWindow::getType()
{
    return windowType;
}

void BaseWindow::setDockWidgetsVisible(bool visible)
{
    setDockWidgetMaximized(-1, !visible);
}

void BaseWindow::addDockWidget(BaseDockWidget *widget)
{
    if(widget){
        addDockWidget(widget->getDockWidgetArea(), widget, Qt::Horizontal);
    }
}

void BaseWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget)
{
    addDockWidget(area, widget, Qt::Horizontal);
}

void BaseWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget *widget, Qt::Orientation orientation)
{
    BaseDockWidget* dockWidget = qobject_cast<BaseDockWidget*>(widget);
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


            connect(dockWidget->toggleViewAction(), &QAction::triggered, this, &BaseWindow::dockWidgetVisibilityChanged);
            connect(dockWidget, &QDockWidget::visibilityChanged, this, &BaseWindow::dockWidgetVisibilityChanged);
            connect(dockWidget, &QDockWidget::topLevelChanged, this, &BaseWindow::dockWidgetVisibilityChanged);
            connect(dockWidget, &BaseDockWidget::req_Maximize, this, &BaseWindow::setDockWidgetMaximized);
            connect(dockWidget, &BaseDockWidget::req_Visible, this, &BaseWindow::_setDockWidgetVisibility);
        }
        emit dockWidgetAdded(dockWidget);
    }
    QMainWindow::addDockWidget(area, widget, orientation);
}

void BaseWindow::removeDockWidget(QDockWidget *widget)
{
    BaseDockWidget* dockWidget = qobject_cast<BaseDockWidget*>(widget);
    if(dockWidget){
        int ID = dockWidget->getID();
        if(currentDockWidgets.contains(ID)){
            currentDockWidgets.remove(ID);
        }
        previouslyVisibleDockIDs.removeAll(ID);
        updateActions();


        disconnect(dockWidget->toggleViewAction(), &QAction::triggered, this, &BaseWindow::dockWidgetVisibilityChanged);
        disconnect(dockWidget, &QDockWidget::visibilityChanged, this, &BaseWindow::dockWidgetVisibilityChanged);
        disconnect(dockWidget, &QDockWidget::visibilityChanged, this, &BaseWindow::dockWidgetVisibilityChanged);
        disconnect(dockWidget, &BaseDockWidget::req_Maximize, this, &BaseWindow::setDockWidgetMaximized);
        disconnect(dockWidget, &BaseDockWidget::req_Visible, this, &BaseWindow::_setDockWidgetVisibility);
    }

    QMainWindow::removeDockWidget(widget);

    if(!terminating && currentDockWidgets.isEmpty()){
        WindowManager::manager()->destructWindow(this);
    }
}

void BaseWindow::setDockWidgetVisibility(BaseDockWidget *widget, bool visible)
{
    if(widget){
        _setDockWidgetVisibility(widget->getID(), visible);
    }
}



void BaseWindow::closeEvent(QCloseEvent *)
{
    tryClose();
}

bool BaseWindow::focusNextPrevChild(bool)
{
    //Disable Tabs
    return false;
}

void BaseWindow::showContextMenu(const QPoint & point)
{
    createPopupMenu()->exec(mapToGlobal(point));
}

void BaseWindow::setDockWidgetMaximized(int ID, bool maximized)
{
    if(maximized){
        //Clear the old list of visible docks
        previouslyVisibleDockIDs.clear();

        foreach(BaseDockWidget* dw, currentDockWidgets.values()){
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
            BaseDockWidget* dw = currentDockWidgets.value(ID, 0);
            if(dw){
                dw->setVisible(true);
                dw->setMaximizeToggled(false);
            }
        }
        previouslyVisibleDockIDs.clear();
    }
}

void BaseWindow::_setDockWidgetVisibility(int ID, bool visible)
{
    //If we are hiding an item, remove it from the list of previously visible items.
    if(!visible){
        previouslyVisibleDockIDs.removeAll(ID);
    }

    BaseDockWidget* dw = currentDockWidgets.value(ID, 0);
    if(dw){
        dw->setVisible(visible);
    }
    updateActions();
}

void BaseWindow::tryClose()
{
    removeAllDockWidgets();
    WindowManager::manager()->destructWindow(this);
}

void BaseWindow::removeAllDockWidgets()
{
    //Unset Original flag on all owned DockWidgets.
    while(!ownedDockWidgets.isEmpty()){
        BaseDockWidget* dockWidget = ownedDockWidgets.takeFirst();
        dockWidget->setSourceWindow(0);
    }

    //Clean up current dock widget.
    foreach(int ID, currentDockWidgets.uniqueKeys()){
        BaseDockWidget* dockWidget = currentDockWidgets.take(ID);
        if(dockWidget){
            //This should cause MedeaWindowManager to shutdown the widget.
            dockWidget->close();
        }
    }

}

void BaseWindow::updateActions()
{

    int visibleCount = 0;
    BaseDockWidget* maximizedDW = 0;
    foreach(BaseDockWidget* dw, currentDockWidgets.values()){
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

QMenu *BaseWindow::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();
    menu->addSeparator();
    menu->addAction(reset_action);
    return menu;
}

void BaseWindow::themeChanged()
{
    Theme* theme = Theme::theme();
    reset_action->setIcon(theme->getIcon("Icons", "refresh"));
    bool show_image = windowType == VIEW_WINDOW;
    setStyleSheet(theme->getWindowStyleSheet(show_image) %
                  theme->getMenuBarStyleSheet() %
                  theme->getMenuStyleSheet() %
                  theme->getToolBarStyleSheet() %
                  theme->getPushButtonStyleSheet() %
                  theme->getTabbedWidgetStyleSheet() %
                  theme->getScrollBarStyleSheet() %
                  "QToolButton{ padding: 4px; }"
                  "QMessageBox {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QMessageBox QLabel {"
                  "background:" + theme->getBackgroundColorHex() + ";"
                  "color:" + theme->getTextColorHex() + ";"
                  "}"
                  "QMessageBox QPushButton {"
                  "padding: 5px;"
                  "}"
                  % theme->getToolTipStyleSheet());
                  //"QToolTip{ background: white; }");
}


