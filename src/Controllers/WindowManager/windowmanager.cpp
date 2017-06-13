#include "windowmanager.h"
#include <QDebug>
#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QObject>
#include <QApplication>


#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/Windows/centralwindow.h"
#include "../../Widgets/Windows/mainwindow.h"
#include "../../Widgets/Windows/viewwindow.h"
#include "../../Widgets/Windows/subwindow.h"

#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Widgets/DockWidgets/nodeviewdockwidget.h"
#include "../../Widgets/DockWidgets/tooldockwidget.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "../../Widgets/ViewManager/viewmanagerwidget.h"

#include "../../theme.h"


#define WINDOW_ID_DATA "WINDOW_ID"
WindowManager* WindowManager::managerSingleton = 0;

WindowManager* WindowManager::manager()
{
    if(!managerSingleton){
        managerSingleton = new WindowManager();
    }
    return managerSingleton;
}

BaseWindow *WindowManager::constructMainWindow(ViewController* vc)
{
    return manager()->_constructMainWindow(vc);
}

BaseWindow *WindowManager::constructSubWindow(QString title)
{
    return manager()->_constructSubWindow(title);
}

BaseWindow *WindowManager::constructCentralWindow(QString title)
{
    return manager()->_constructCentralWindow(title);
}

NodeViewDockWidget *WindowManager::constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    return manager()->_constructNodeViewDockWidget(title, area);
}

ViewDockWidget *WindowManager::constructViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    return manager()->_constructViewDockWidget(title, area);
}

ToolDockWidget *WindowManager::constructToolDockWidget(QString title)
{
    return manager()->_constructToolDockWidget(title);
}


void WindowManager::destructWindow(BaseWindow *window)
{
    return manager()->_destructWindow(window);
}

void WindowManager::destructDockWidget(BaseDockWidget *widget)
{
    return manager()->_destructDockWidget(widget);
}

void WindowManager::_destructWindow(BaseWindow *window)
{
    if(window){
        int wID = window->getID();

        //Teardown
        if(window->getType() == BaseWindow::MAIN_WINDOW && windows.contains(wID)){
            delete this;
        }else{
            if(centralWindow != window){
                removeWindow(window);
                window->deleteLater();
            }
        }
    }
}

void WindowManager::_destructDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget){
        removeDockWidget(dockWidget);
        dockWidget->deleteLater();
    }
}

void WindowManager::teardown()
{
    if(managerSingleton){
       delete managerSingleton;
    }
    managerSingleton = 0;
}

BaseWindow *WindowManager::getMainWindow()
{
    if(managerSingleton){
        return managerSingleton->mainWindow;
    }
    return 0;
}

WindowManager::WindowManager():QObject(0)
{
    activeViewDockWidget = 0;
    mainWindow = 0;
    centralWindow = 0;
    viewManagerWidget = new ViewManagerWidget(this);

    QApplication* a = (QApplication*) QApplication::instance();

    connect(a, &QApplication::focusChanged, this, &WindowManager::focusChanged);
}

WindowManager::~WindowManager()
{
    if(centralWindow){
        int ID = centralWindow->getID();
        windows.remove(ID);
        windowIDs.removeAll(ID);
    }
    if(mainWindow){
        int ID = mainWindow->getID();
        windows.remove(ID);
        windowIDs.removeAll(ID);
    }

    //Delete all subwindows.
    while(!windows.isEmpty()){
        int ID = windows.keys().first();
        BaseWindow* window = windows[ID];
        if(window){
            _destructWindow(window);
        }
    }
	if(centralWindow){
		_destructWindow(centralWindow);
	}
	if(mainWindow){
		_destructWindow(mainWindow);
    }
}

BaseWindow *WindowManager::getCentralWindow()
{
    return centralWindow;
}

ViewManagerWidget *WindowManager::getViewManagerGUI()
{
    return viewManagerWidget;
}

BaseWindow *WindowManager::getActiveWindow()
{
    if(activeViewDockWidget){
        return (BaseWindow*) activeViewDockWidget->parentWidget();
    }
    return centralWindow;
}

BaseWindow *WindowManager::_constructMainWindow(ViewController* vc)
{
    MainWindow* window = 0;
    if(!mainWindow){
        window = new MainWindow(vc);
        mainWindow = window;
        addWindow(window);
    }
    return window;
}

BaseWindow *WindowManager::_constructSubWindow(QString title)
{
    BaseWindow* window = new SubWindow();
    window->setWindowTitle(title);
    addWindow(window);
    return window;
}

BaseWindow *WindowManager::_constructCentralWindow(QString title)
{
    BaseWindow* window = 0;
    if(!centralWindow){
        window = new CentralWindow();
        window->setWindowTitle(title);
        centralWindow = window;
        addWindow(window);
    }
    return window;
}

ToolDockWidget *WindowManager::_constructToolDockWidget(QString title)
{
    //Construct new DockWidget
    ToolDockWidget* dockWidget = new ToolDockWidget(title);
    addDockWidget(dockWidget);
    return dockWidget;

}

ViewDockWidget *WindowManager::_constructViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    //Construct new DockWidget
    ViewDockWidget* dockWidget = new ViewDockWidget(title, area);
    addDockWidget(dockWidget);
    return dockWidget;
}

NodeViewDockWidget *WindowManager::_constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    //Construct new DockWidget
    NodeViewDockWidget* dockWidget = new NodeViewDockWidget(title, area);
    addDockWidget(dockWidget);
    return dockWidget;
}


ViewDockWidget *WindowManager::getActiveViewDockWidget()
{
    return activeViewDockWidget;
}

void WindowManager::setActiveDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget != activeViewDockWidget){
        ViewDockWidget* prevDock = activeViewDockWidget;
        activeViewDockWidget = 0;

        //Unset the old
        if(prevDock){
            prevDock->setActive(false);
            disconnect(prevDock, &BaseDockWidget::visibilityChanged, this, &WindowManager::activeDockWidgetVisibilityChanged);
        }

        if(dockWidget){
            activeViewDockWidget = (ViewDockWidget*) dockWidget;
        }


        //Set the New.
        if(dockWidget && dockWidget->getDockType() == BaseDockWidget::MDW_VIEW){
            dockWidget->setActive(true);

            connect(activeViewDockWidget, &BaseDockWidget::visibilityChanged, this, &WindowManager::activeDockWidgetVisibilityChanged);
        }


        emit activeViewDockWidgetChanged(activeViewDockWidget, prevDock);
    }
}

void WindowManager::setActiveDockWidget(int ID)
{
    BaseDockWidget* dockWidget = dockWidgets.value(ID, 0);
    if(dockWidget && dockWidget != activeViewDockWidget && dockWidget->isFocusEnabled()){
        setActiveDockWidget(dockWidget);
    }
}

QList<ViewDockWidget *> WindowManager::getViewDockWidgets()
{
    QList<ViewDockWidget*> views;
    foreach(int ID, viewDockIDs){
        BaseDockWidget* dockWidget = dockWidgets.value(ID, 0);
        if(dockWidget && dockWidget->getDockType() == BaseDockWidget::MDW_VIEW){
            views.append((ViewDockWidget*)dockWidget);
        }
    }
    return views;
}

QList<NodeViewDockWidget *> WindowManager::getNodeViewDockWidgets()
{
    QList<NodeViewDockWidget*> views;
    foreach(ViewDockWidget* dock, getViewDockWidgets()){
        if(dock->isNodeViewDock()){
            views.append((NodeViewDockWidget*) dock);
        }
    }
    return views;
}

NodeViewDockWidget *WindowManager::getNodeViewDockWidget(ViewItem *item)
{
    foreach(ViewDockWidget* dock, getViewDockWidgets()){
        if(dock->isNodeViewDock()){
            auto viewDock = (NodeViewDockWidget*) dock;

            if(viewDock->getNodeView()->getContainedViewItem() == item){
                return viewDock;
            }
        }
    }
    return 0;
}

void WindowManager::reparentDockWidget(BaseDockWidget *dockWidget)
{
    showPopOutDialog(dockWidget);
}

void WindowManager::focusChanged(QWidget*, QWidget* now)
{
    if(now){
        bool okay;
        int ID = now->property("ID").toInt(&okay);
        if(okay){
            setActiveDockWidget(ID);
            return;
        }
        QWidget* parent = now->parentWidget();
        if(parent){
            ID = parent->property("ID").toInt(&okay);
            if(okay){
                setActiveDockWidget(ID);
            }
        }
    }
}


void WindowManager::addWindow(BaseWindow *window)
{
    if(window){
        int ID = window->getID();
        if(!windows.contains(ID)){
            windows[ID] = window;

            emit windowConstructed(window);
        }else{
            qCritical() << "MedeaWindowManager::addWindow() " << ID << " - Got duplicated MedeaWindow ID.";
        }
    }
}

void WindowManager::removeWindow(BaseWindow *window)
{
    if(window){
        int ID = window->getID();
        emit windowDestructed(ID);
        if(windows.contains(ID)){

            if(mainWindow == window){
                mainWindow = 0;
            }
            if(centralWindow == window){
                centralWindow = 0 ;
            }
            windows.remove(ID);
        }
    }
}

void WindowManager::addDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(!dockWidgets.contains(ID)){
            if(dockWidget->getDockType() == BaseDockWidget::MDW_VIEW){
                viewDockIDs.append(ID);

                emit viewDockWidgetConstructed(dockWidget);
            }


            dockWidgets[ID] = dockWidget;
            connect(dockWidget, &BaseDockWidget::req_PopOut, this, &WindowManager::dockWidget_PopOut);
            connect(dockWidget, &BaseDockWidget::req_Close, this, &WindowManager::dockWidget_Close);
        }else{
            qCritical() << "MedeaWindowManager::addDockWidget() - Got duplicated MedeaDockWidget ID.";
        }
    }
}

void WindowManager::removeDockWidget(BaseDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(dockWidgets.contains(ID)){
            //Clear the active flag.
            if(activeViewDockWidget == dockWidget){
                setActiveDockWidget();
            }

            emit viewDockWidgetDestructed(ID);

            if(dockWidget->getDockType() == BaseDockWidget::MDW_VIEW){
                viewDockIDs.removeAll(ID);
            }

            disconnect(dockWidget, &BaseDockWidget::req_PopOut, this, &WindowManager::dockWidget_PopOut);
            disconnect(dockWidget, &BaseDockWidget::req_Close, this, &WindowManager::dockWidget_Close);
            dockWidgets.remove(ID);

            BaseWindow* window = dockWidget->getCurrentWindow();
            if(window){
                window->removeDockWidget(dockWidget);
            }
        }else{
            qCritical() << "MedeaWindowManager::removeDockWidget() - Trying to remove non-hashed MedeaDockWidget.";
        }
    }
}

void WindowManager::dockWidget_Close(int ID)
{
    BaseDockWidget* dockWidget = dockWidgets.value(ID, 0);
    if(dockWidget){
        BaseWindow* sourceWindow = dockWidget->getSourceWindow();
        BaseWindow* currentWindow = dockWidget->getCurrentWindow();

        if(currentWindow){
            //Remove it from the current window
            currentWindow->removeDockWidget(dockWidget);
            dockWidget->setCurrentWindow(0);
        }

        if(sourceWindow == currentWindow || sourceWindow == 0){
            //If the source window is the current window we should destruct the dock.
            _destructDockWidget(dockWidget);
        }else{
            //Reparent back into source window.
            _reparentDockWidget(dockWidget, sourceWindow);
        }
        //destructWindowIfEmpty(currentWindow);
    }
}

void WindowManager::dockWidget_PopOut(int ID)
{
    BaseDockWidget* dockWidget = dockWidgets.value(ID, 0);
    if(dockWidget){
        showPopOutDialog(dockWidget);
    }
}

void WindowManager::reparentDockWidgetAction(int wID)
{
    BaseWindow* window = 0;
    bool newWindow = wID == -1;
    if(newWindow){
        window = _constructSubWindow();
        window->setWindowTitle("Sub Window #" + QString::number(window->getID() - 2));
        window->show();
        window->activateWindow();
    }else{
        if(windows.contains(wID)){
            window = windows[wID];
        }else{
            qCritical() << "MedeaWindowManager::reparentDockWidget_ButtonPressed() - Can't find MedeaWindow with ID: " << wID;
        }
    }
    if(activeViewDockWidget && window){
        _reparentDockWidget(activeViewDockWidget, window);
    }
}


void WindowManager::activeDockWidgetVisibilityChanged()
{
    if(activeViewDockWidget && !activeViewDockWidget->isVisible()){
        //Look for next widget.
        BaseWindow* parent = activeViewDockWidget->getCurrentWindow();
        if(parent){
            BaseDockWidget* nextVisible = 0;
            foreach(BaseDockWidget* dockWidget, parent->getDockWidgets()){
                if(dockWidget && dockWidget->getDockType() == BaseDockWidget::MDW_VIEW && dockWidget->isVisible()){
                    nextVisible = dockWidget;
                    break;
                }
            }
            setActiveDockWidget(nextVisible);
        }
    }
}

void WindowManager::destructWindowIfEmpty(BaseWindow *window)
{
    if(window){
        int wID = window->getID();
        if(windows.contains(wID) && !window->hasDockWidgets()){
            if(window != mainWindow && window != centralWindow){
                _destructWindow(window);
            }
        }
    }
}

void WindowManager::_reparentDockWidget(BaseDockWidget *dockWidget, BaseWindow *window)
{
    if(dockWidget && window){
        BaseWindow* previousWindow = dockWidget->getCurrentWindow();
        if(previousWindow){
            //Remove it from its previousWindow
            previousWindow->removeDockWidget(dockWidget);
        }



        window->addDockWidget(dockWidget);
        emit dockWidget->req_Visible(dockWidget->getID(), true);
        window->raise();
    }
}

void WindowManager::showPopOutDialog(BaseDockWidget *dockWidget)
{
    BaseWindow* window = getActiveWindow();

    if(dockWidget != activeViewDockWidget){
        setActiveDockWidget(dockWidget);
    }
    Theme* theme = Theme::theme();
    PopupWidget* popupDialog = new PopupWidget(PopupWidget::DIALOG, window);

    QFont titleFont;
    titleFont.setPixelSize(15);

    QWidget* widget = new QWidget(popupDialog);

    QPushButton* invisAcceptButton = new QPushButton(widget);
    invisAcceptButton->setFixedSize(1,1);
    invisAcceptButton->setDefault(true);

    QVBoxLayout* vlayout = new QVBoxLayout(widget);
    QHBoxLayout* topLayout = new QHBoxLayout();
    vlayout->addLayout(topLayout);

    QLabel* titleLabel = new QLabel("Select Destination Window", mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(titleFont);

    QToolBar* topToolbar = new QToolBar(widget);
    topLayout->addWidget(titleLabel, 1);
    topLayout->addWidget(topToolbar);

    QAction* cancelAction = topToolbar->addAction(theme->getIcon("Icons", "cross"), "Cancel");

    QWidget* toolbarContainer = new QWidget(widget);
    toolbarContainer->setMaximumWidth(1220);
    QHBoxLayout* toolbarLayout = new QHBoxLayout(toolbarContainer);

    QToolBar* windowToolbar = new QToolBar(widget);

    QWidget* leftSpacer = new QWidget(windowToolbar);
    QWidget* rightSpacer = new QWidget(windowToolbar);

    leftSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    rightSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    windowToolbar->setIconSize(QSize(150,100));
    windowToolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    vlayout->addWidget(toolbarContainer);

    toolbarLayout->addWidget(leftSpacer);
    toolbarLayout->addWidget(invisAcceptButton);
    toolbarLayout->addWidget(windowToolbar);

    QSignalMapper* windowActionMapper = new QSignalMapper(popupDialog);

    BaseWindow* currentWindow = dockWidget->getCurrentWindow();
    //Construct buttons for all of the windows.
    foreach(BaseWindow* w, windows.values()){
        if(w != currentWindow && w != mainWindow){
            QAction* wAction = constructPopOutWindowAction(windowActionMapper, w);
            windowToolbar->addAction(wAction);
        }
    }

    //Construct a new Window Action
    QAction* wAction = constructPopOutWindowAction(windowActionMapper);
    connect(invisAcceptButton, &QPushButton::clicked, wAction, &QAction::trigger);
    windowToolbar->addAction(wAction);

    toolbarLayout->addWidget(rightSpacer);

    //Accept the dialog and then reparent
    connect(cancelAction, &QAction::triggered, popupDialog, &QDialog::reject);
    connect(windowActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), popupDialog, &QDialog::accept);
    connect(windowActionMapper, static_cast<void(QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &WindowManager::reparentDockWidgetAction);

    popupDialog->setWidget(widget);

    popupDialog->setStyleSheet(theme->getPopupWidgetStyleSheet());
    topToolbar->setStyleSheet(theme->getToolBarStyleSheet() + " QToolButton::!hover{background:rgba(0,0,0,0);border:none;}");
    titleLabel->setStyleSheet("border:none; background:rgba(0,0,0,0); color:" + theme->getTextColorHex() + ";");

    popupDialog->exec();
    delete popupDialog;
}

QAction *WindowManager::constructPopOutWindowAction(QSignalMapper *mapper, BaseWindow *window)
{
    QAction* action = new QAction(mapper);

    int wID = -1;
    if(window){
        QPixmap image = window->grab();
        action->setIcon(QIcon(image.scaledToHeight(100)));
        QString text = window->windowTitle();
        action->setText(text);
        wID = window->getID();
    }else{
        QPixmap image = Theme::theme()->getImage("Icons", "medeaLogo");
        action->setIcon(QIcon(image.scaledToHeight(100)));
        action->setText("New Window");
    }

    if(mapper){
        connect(action, &QAction::triggered, mapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
        mapper->setMapping(action, wID);
    }

    return action;
}

