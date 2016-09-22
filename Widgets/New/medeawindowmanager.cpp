#include "medeawindowmanager.h"
#include <QDebug>
#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QObject>

#include "medeatooldockwidget.h"
#include "medeaviewdockwidget.h"

#include "viewmanagerwidget.h"
#include "medeasubwindow.h"
#include "medeamainwindow.h"
#include "medeaviewwindow.h"
#include "medeacentralwindow.h"
#include "medeanodeviewdockwidget.h"

#include "../../View/theme.h"
#define WINDOW_ID_DATA "WINDOW_ID"
MedeaWindowManager* MedeaWindowManager::managerSingleton = 0;

MedeaWindowManager* MedeaWindowManager::manager()
{
    if(!managerSingleton){
        managerSingleton = new MedeaWindowManager();
    }
    return managerSingleton;
}

MedeaWindowNew *MedeaWindowManager::constructMainWindow(ViewController* vc)
{
    return manager()->_constructMainWindow(vc);
}

MedeaWindowNew *MedeaWindowManager::constructSubWindow(QString title)
{
    return manager()->_constructSubWindow(title);
}

MedeaWindowNew *MedeaWindowManager::constructCentralWindow(QString title)
{
    return manager()->_constructCentralWindow(title);
}

MedeaNodeViewDockWidget *MedeaWindowManager::constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    return manager()->_constructNodeViewDockWidget(title, area);
}

MedeaViewDockWidget *MedeaWindowManager::constructViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    return manager()->_constructViewDockWidget(title, area);
}

MedeaToolDockWidget *MedeaWindowManager::constructToolDockWidget(QString title)
{
    return manager()->_constructToolDockWidget(title);
}


void MedeaWindowManager::destructWindow(MedeaWindowNew *window)
{
    return manager()->_destructWindow(window);
}

void MedeaWindowManager::destructDockWidget(MedeaDockWidget *widget)
{
    return manager()->_destructDockWidget(widget);
}

void MedeaWindowManager::_destructWindow(MedeaWindowNew *window)
{
    if(window){
        int wID = window->getID();

        //Teardown
        if(window->getType() == MedeaWindowNew::MAIN_WINDOW && windows.contains(wID)){
            qCritical() << "DELETING MYSELF!";
            delete this;
        }else{
            removeWindow(window);
            window->deleteLater();
        }
    }
}

void MedeaWindowManager::_destructDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        removeDockWidget(dockWidget);
        dockWidget->deleteLater();
    }
}

void MedeaWindowManager::teardown()
{
    if(managerSingleton){
       delete managerSingleton;
    }
    managerSingleton = 0;
}

MedeaWindowNew *MedeaWindowManager::getMainWindow()
{
    if(managerSingleton){
        return managerSingleton->mainWindow;
    }
}

MedeaWindowManager::MedeaWindowManager():QObject(0)
{
    activeViewDockWidget = 0;
    mainWindow = 0;
    centralWindow = 0;
    viewManagerWidget = new ViewManagerWidget(this);
}

MedeaWindowManager::~MedeaWindowManager()
{
    if(centralWindow){
        windows.remove(centralWindow->getID());
    }
    if(mainWindow){
        windows.remove(mainWindow->getID());
    }

    //Delete all subwindows.
    while(!windows.isEmpty()){
        int ID = windows.keys().first();
        MedeaWindowNew* window = windows[ID];
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

MedeaWindowNew *MedeaWindowManager::getCentralWindow()
{
    return centralWindow;
}

ViewManagerWidget *MedeaWindowManager::getViewManagerGUI()
{
    return viewManagerWidget;
}

MedeaWindowNew *MedeaWindowManager::getActiveWindow()
{
    if(activeViewDockWidget){
        return (MedeaWindowNew*) activeViewDockWidget->parentWidget();
    }
    return centralWindow;
}

MedeaWindowNew *MedeaWindowManager::_constructMainWindow(ViewController* vc)
{
    MedeaMainWindow* window = 0;
    if(!mainWindow){
        window = new MedeaMainWindow(vc);
        mainWindow = window;
        addWindow(window);
    }
    return window;
}

MedeaWindowNew *MedeaWindowManager::_constructSubWindow(QString title)
{
    MedeaWindowNew* window = new MedeaSubWindow();
    window->setWindowTitle(title);
    addWindow(window);
    return window;
}

MedeaWindowNew *MedeaWindowManager::_constructCentralWindow(QString title)
{
    MedeaWindowNew* window = 0;
    if(!centralWindow){
        window = new MedeaCentralWindow();
        window->setWindowTitle(title);
        centralWindow = window;
        addWindow(window);
    }
    return window;
}

MedeaToolDockWidget *MedeaWindowManager::_constructToolDockWidget(QString title)
{
    //Construct new DockWidget
    MedeaToolDockWidget* dockWidget = new MedeaToolDockWidget(title);
    addDockWidget(dockWidget);
    return dockWidget;

}

MedeaViewDockWidget *MedeaWindowManager::_constructViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    //Construct new DockWidget
    MedeaViewDockWidget* dockWidget = new MedeaViewDockWidget(title, area);
    addDockWidget(dockWidget);
    return dockWidget;
}

MedeaNodeViewDockWidget *MedeaWindowManager::_constructNodeViewDockWidget(QString title, Qt::DockWidgetArea area)
{
    //Construct new DockWidget
    MedeaNodeViewDockWidget* dockWidget = new MedeaNodeViewDockWidget(title, area);
    addDockWidget(dockWidget);
    return dockWidget;
}


MedeaViewDockWidget *MedeaWindowManager::getActiveViewDockWidget()
{
    return activeViewDockWidget;
}

void MedeaWindowManager::setActiveDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget != activeViewDockWidget){
        MedeaViewDockWidget* prevDock = activeViewDockWidget;

        //Unset the old
        if(activeViewDockWidget){
            activeViewDockWidget->setActive(false);
            activeViewDockWidget = 0;
        }

        //Set the New.
        if(dockWidget && dockWidget->getDockType() == MedeaDockWidget::MDW_VIEW){
            dockWidget->setActive(true);
        }

        activeViewDockWidget = (MedeaViewDockWidget*)dockWidget;
        emit activeViewDockWidgetChanged(activeViewDockWidget, prevDock);
    }
}

QList<MedeaViewDockWidget *> MedeaWindowManager::getViewDockWidgets()
{
    QList<MedeaViewDockWidget*> views;
    foreach(int ID, viewDockIDs){
        MedeaDockWidget* dockWidget = dockWidgets.value(ID, 0);
        if(dockWidget && dockWidget->getDockType() == MedeaDockWidget::MDW_VIEW){
            views.append((MedeaViewDockWidget*)dockWidget);
        }
    }
    return views;
}

QList<MedeaNodeViewDockWidget *> MedeaWindowManager::getNodeViewDockWidgets()
{
    QList<MedeaNodeViewDockWidget*> views;
    foreach(MedeaViewDockWidget* dock, getViewDockWidgets()){
        if(dock->isNodeViewDock()){
            views.append((MedeaNodeViewDockWidget*) dock);
        }
    }
    return views;
}


void MedeaWindowManager::addWindow(MedeaWindowNew *window)
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

void MedeaWindowManager::removeWindow(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        emit windowDestructed(ID);
        qCritical() << "MedeaWindowManager::removeWindow() " << ID;
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

void MedeaWindowManager::addDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(!dockWidgets.contains(ID)){
            if(dockWidget->getDockType() == MedeaDockWidget::MDW_VIEW){
                viewDockIDs.append(ID);

                emit viewDockWidgetConstructed(dockWidget);
            }


            dockWidgets[ID] = dockWidget;
            connect(dockWidget, &MedeaDockWidget::req_PopOut, this, &MedeaWindowManager::dockWidget_PopOut);
            connect(dockWidget, &MedeaDockWidget::req_Close, this, &MedeaWindowManager::dockWidget_Close);
        }else{
            qCritical() << "MedeaWindowManager::addDockWidget() - Got duplicated MedeaDockWidget ID.";
        }
    }
}

void MedeaWindowManager::removeDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(dockWidgets.contains(ID)){
            //Clear the active flag.
            if(activeViewDockWidget == dockWidget){
                setActiveDockWidget();
            }

            emit viewDockWidgetDestructed(ID);

            if(dockWidget->getDockType() == MedeaDockWidget::MDW_VIEW){
                viewDockIDs.removeAll(ID);
            }

            disconnect(dockWidget, &MedeaDockWidget::req_PopOut, this, &MedeaWindowManager::dockWidget_PopOut);
            disconnect(dockWidget, &MedeaDockWidget::req_Close, this, &MedeaWindowManager::dockWidget_Close);
            dockWidgets.remove(ID);

            MedeaWindowNew* window = dockWidget->getCurrentWindow();
            if(window){
                window->removeDockWidget(dockWidget);
            }
        }else{
            qCritical() << "MedeaWindowManager::removeDockWidget() - Trying to remove non-hashed MedeaDockWidget.";
        }
    }
}

void MedeaWindowManager::dockWidget_Close(int ID)
{
    MedeaDockWidget* dockWidget = dockWidgets.value(ID, 0);
    if(dockWidget){
        MedeaWindowNew* sourceWindow = dockWidget->getSourceWindow();
        MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();

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

void MedeaWindowManager::dockWidget_PopOut(int ID)
{
    MedeaDockWidget* dockWidget = dockWidgets.value(ID, 0);
    if(dockWidget){
        showPopOutDialog(dockWidget);
    }
}

void MedeaWindowManager::reparentDockWidget_ButtonPressed()
{
    if(sender()){
        bool isInt = false;
        //Get the
        int wID = sender()->property(WINDOW_ID_DATA).toInt(&isInt);
        if(isInt){
            MedeaWindowNew* window = 0;

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
    }
}

void MedeaWindowManager::destructWindowIfEmpty(MedeaWindowNew *window)
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

void MedeaWindowManager::_reparentDockWidget(MedeaDockWidget *dockWidget, MedeaWindowNew *window)
{
    if(dockWidget && window){
        MedeaWindowNew* previousWindow = dockWidget->getCurrentWindow();
        if(previousWindow){
            //Remove it from its previousWindow
            previousWindow->removeDockWidget(dockWidget);
        }



        window->addDockWidget(dockWidget);
        emit dockWidget->req_Visible(dockWidget->getID(), true);
        window->raise();
    }
}

void MedeaWindowManager::showPopOutDialog(MedeaDockWidget *dockWidget)
{
    if(dockWidget != activeViewDockWidget){
        setActiveDockWidget(dockWidget);
    }

    PopupWidget* dialog = new PopupWidget(PopupWidget::DIALOG, mainWindow);
    /*
     * TODO - Figure out why this doesn't work!
     */

    QFont titleFont;
    titleFont.setPixelSize(15);
    QWidget* widget = new QWidget(mainWindow);
    QLabel* titleLabel = new QLabel("Select Destination Window", mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(titleFont);

    QVBoxLayout* vlayout = new QVBoxLayout(widget);
    vlayout->addWidget(titleLabel);

    QToolBar* toolbar = new QToolBar(widget);
    toolbar->setIconSize(QSize(150,100));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    vlayout->addWidget(toolbar);

    Theme* theme = Theme::theme();


    MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();


    foreach(MedeaWindowNew* w, windows.values()){
        if(w != currentWindow && w != mainWindow){
            QAction* action = constructPopOutWindowAction(dialog, w);
            toolbar->addAction(action);
        }
    }

    toolbar->addAction(constructPopOutWindowAction(dialog));

    dialog->setWidget(widget);
    dialog->setStyleSheet(theme->getPopupWidgetStyleSheet());

    titleLabel->setStyleSheet("border:none; background:rgba(0,0,0,0);color:" + theme->getTextColorHex() + ";}");
    //toolbar->setStyleSheet("QToolButton{text-align:right;}");


    dialog->exec();

    delete dialog;
}

QAction *MedeaWindowManager::constructPopOutWindowAction(QDialog *parent, MedeaWindowNew *window)
{
    QAction* action = new QAction(this);
    //button->setStyleSheet("margin: 10px; border-radius: 5px; border: 1px solid gray; background: white; padding-top: 10px;");
    //button->setFont(mainWindow->font());

    if(window){
        action->setIcon(QIcon(window->grab()));
        QString text = window->windowTitle();
        action->setText(text);
        action->setProperty(WINDOW_ID_DATA, window->getID());
    }else{
        action->setIcon(Theme::theme()->getIcon("Actions", "MEDEA"));
        action->setText("New Window");
        action->setProperty(WINDOW_ID_DATA, -1);
        //button->setStyleSheet("margin: 10px; border-radius: 5px; border: 1px solid gray; background: white; padding-top: 20px;");
        //button->setFixedWidth(170);
    }

    connect(action, SIGNAL(triggered(bool)), parent, SLOT(accept()));
    connect(action, SIGNAL(triggered(bool)), this, SLOT(reparentDockWidget_ButtonPressed()));
    return action;
}

