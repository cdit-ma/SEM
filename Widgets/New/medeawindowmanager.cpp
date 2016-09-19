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
        }else{
            qCritical() << "NON HASHED";
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

            if(wID == -1){
                window = _constructSubWindow("Sub Window");
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
        window->show();
        //Make it visible
        emit dockWidget->req_Visible(dockWidget->getID(), true);

        window->activateWindow();

        //destructWindowIfEmpty(previousWindow);
    }
}

void MedeaWindowManager::showPopOutDialog(MedeaDockWidget *dockWidget)
{
    if(dockWidget != activeViewDockWidget){
        setActiveDockWidget(dockWidget);
    }

    /*
    QDialog* dialog = new QDialog();
    dialog->setWindowTitle("Select Destination Window");
    //dialog->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    //dialog->setAttribute(Qt::WA_NoSystemBackground, true);
    //dialog->setAttribute(Qt::WA_TranslucentBackground, true);
    dialog->setStyleSheet("QDialog{ background:" + Theme::theme()->getBackgroundColorHex() + ";}");
    //dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);

    dialog->setModal(true);

    QHBoxLayout* layout = new QHBoxLayout(dialog);
    dialog->setLayout(layout);
    */

    /*
     * TODO - Figure out why this doesn't work!
     */
    QWidget* widget = new QWidget(mainWindow);
    QLabel* titleLabel = new QLabel("Select Destination Window", mainWindow);
    titleLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* vlayout = new QVBoxLayout(widget);
    vlayout->addWidget(titleLabel);

    QHBoxLayout* layout = new QHBoxLayout();
    vlayout->addLayout(layout);

    Theme* theme = Theme::theme();
    PopupWidget* dialog = new PopupWidget(PopupWidget::DIALOG, mainWindow);
    dialog->setWidget(widget);
    dialog->setStyleSheet(theme->getPopupWidgetStyleSheet());
    widget->setStyleSheet("QWidget{ background: rgba(0,0,0,0); border: 0px; color:" + theme->getTextColorHex() + ";}"
                          "QToolButton{ color: black; }");

    MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();
    foreach(MedeaWindowNew* w, windows.values()){
        if(w != currentWindow && w != mainWindow){
            QToolButton* button = constructPopOutWindowButton(dialog, w);
            layout->addWidget(button, 0 , Qt::AlignCenter);
        }
    }
    QToolButton* button = constructPopOutWindowButton(dialog, 0);
    layout->addWidget(button);

    dialog->setSize(layout->sizeHint().width() + 50, layout->sizeHint().height() + 50);


    dialog->exec();

    delete dialog;
}

QToolButton *MedeaWindowManager::constructPopOutWindowButton(QDialog *parent, MedeaWindowNew *window)
{
    QToolButton* button = new QToolButton(parent);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setStyleSheet("margin: 10px; border-radius: 5px; border: 1px solid gray; background: white; padding-top: 10px;");
    button->setFont(mainWindow->font());

    QFont guiFont = QFont("Verdana", 10);
    button->setFont(guiFont);
    button->setFixedSize(200, 170);
    button->setIconSize(QSize(175, 80));

    if(window){
        button->setIcon(QIcon(window->grab()));
        QString text = window->windowTitle();
        if(text == ""){
            text = "MEDEA - Main Window";
        }
        button->setText(text);
        button->setProperty(WINDOW_ID_DATA, window->getID());
    }else{
        button->setIcon(Theme::theme()->getImage("Actions", "Add", QSize(), QColor(50, 128, 50)));
        button->setText("New Window");
        button->setProperty(WINDOW_ID_DATA, -1);
        button->setStyleSheet("margin: 10px; border-radius: 5px; border: 1px solid gray; background: white; padding-top: 20px;");
        button->setFixedWidth(170);
    }

    connect(button, SIGNAL(clicked(bool)), this, SLOT(reparentDockWidget_ButtonPressed()));
    connect(button, SIGNAL(clicked(bool)), parent, SLOT(accept()));
    return button;
}

