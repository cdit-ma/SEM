#include "medeawindowmanager.h"
#include "medeawindownew.h"
#include <QDebug>
#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QObject>
#include "medeamainwindow.h"

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

MedeaWindowNew *MedeaWindowManager::constructMainWindow(QString title)
{
    return manager()->_constructMainWindow(title);
}

MedeaWindowNew *MedeaWindowManager::constructSubWindow(QString title)
{
    return manager()->_constructSubWindow(title);
}

MedeaDockWidget *MedeaWindowManager::constructDockWidget(QString title, Qt::DockWidgetArea initialArea)
{
    return manager()->_constructDockWidget(title, initialArea);
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
        if(window->isMainWindow() && windows.contains(wID)){
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

MedeaWindowManager::MedeaWindowManager():QObject(0)
{
    activeDockWidget = 0;
    mainWindow = 0;
}

MedeaWindowManager::~MedeaWindowManager()
{
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
    _destructWindow(mainWindow);
}

MedeaWindowNew *MedeaWindowManager::_constructMainWindow(QString title)
{
    MedeaMainWindow* window = 0;
    if(!mainWindow){
        window = new MedeaMainWindow();
        window->setWindowTitle(title);
        mainWindow = window;
        addWindow(window);
    }
    return window;
}

MedeaWindowNew *MedeaWindowManager::_constructSubWindow(QString title)
{
    MedeaWindowNew* window = new MedeaWindowNew();
    window->setWindowTitle(title);
    addWindow(window);
    return window;
}

MedeaDockWidget *MedeaWindowManager::_constructDockWidget(QString title, Qt::DockWidgetArea initialArea)
{
    //Construct new DockWidget
    MedeaDockWidget* dockWidget = new MedeaDockWidget(title, initialArea);
    addDockWidget(dockWidget);
    return dockWidget;
}

MedeaDockWidget *MedeaWindowManager::getActiveDockWidget()
{
    return activeDockWidget;
}

void MedeaWindowManager::setActiveDockWidget(MedeaDockWidget *dockWidget)
{
    //Unset the old
    if(activeDockWidget){
        activeDockWidget->setActive(false);
        activeDockWidget = 0;
    }

    //Set the New.
    if(dockWidget){
        dockWidget->setActive(true);
        activeDockWidget = dockWidget;
    }
}


void MedeaWindowManager::addWindow(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        if(!windows.contains(ID)){
            windows[ID] = window;
        }else{
            qCritical() << "MedeaWindowManager::addWindow() - Got duplicated MedeaWindow ID.";
        }
    }
}

void MedeaWindowManager::removeWindow(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        if(windows.contains(ID)){
            windows.remove(ID);
        }else{
            qCritical() << "MedeaWindowManager::removeWindow() - Trying to remove non-hashed MedeaWindow.";
        }
    }
}

void MedeaWindowManager::addDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        qCritical() << "MedeaWindowManager::addDockWidget() - " << ID;
        if(!dockWidgets.contains(ID)){
            dockWidgets[ID] = dockWidget;
            connect(dockWidget, SIGNAL(popOutWidget()), this, SLOT(dockWidget_PopOut()));
            connect(dockWidget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximize(bool)));
            connect(dockWidget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Close()));
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
            if(activeDockWidget == dockWidget){
                setActiveDockWidget();
            }

            disconnect(dockWidget, SIGNAL(popOutWidget()), this, SLOT(dockWidget_PopOut()));
            disconnect(dockWidget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximize(bool)));
            disconnect(dockWidget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Close()));
            dockWidgets.remove(ID);
        }else{
            qCritical() << "MedeaWindowManager::removeDockWidget() - Trying to remove non-hashed MedeaDockWidget.";
        }
    }
}

void MedeaWindowManager::dockWidget_Close()
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(sender());

    if(dockWidget){
        MedeaWindowNew* sourceWindow = dockWidget->getSourceWindow();
        MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();

        if(currentWindow){
            //Remove it from the current window
            currentWindow->removeDockWidget(dockWidget);
        }

        if(sourceWindow == currentWindow || sourceWindow == 0){
            //If the source window is the current window we should destruct the dock.
            _destructDockWidget(dockWidget);
        }else{
            //Reparent back into source window.
            _reparentDockWidget(dockWidget, sourceWindow);
        }
        destructWindowIfEmpty(currentWindow);
    }
}

void MedeaWindowManager::dockWidget_PopOut()
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(sender());
    if(dockWidget){
        showPopOutDialog(dockWidget);
    }
}

void MedeaWindowManager::dockWidget_Maximize(bool maximize)
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(sender());
    if(dockWidget){
        MedeaWindowNew* window = dockWidget->getCurrentWindow();
        if(window){
            window->setDockWidgetMaximized(dockWidget, maximize);
        }
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
            if(activeDockWidget && window){
                _reparentDockWidget(activeDockWidget, window);
            }
        }
    }
}

void MedeaWindowManager::destructWindowIfEmpty(MedeaWindowNew *window)
{
    if(window){
        int wID = window->getID();
        if(windows.contains(wID) && !window->hasDockWidgets() && !window->isMainWindow()){
           _destructWindow(window);
        }
    }
}

void MedeaWindowManager::_reparentDockWidget(MedeaDockWidget *dockWidget, MedeaWindowNew *window)
{
    if(dockWidget && window){
        MedeaWindowNew* previousWindow = dockWidget->getCurrentWindow();
        if(previousWindow){
            previousWindow->removeDockWidget(dockWidget);
        }

        window->addDockWidget(dockWidget);

        dockWidget->show();
        window->show();
        window->activateWindow();

        destructWindowIfEmpty(previousWindow);
    }
}

void MedeaWindowManager::showPopOutDialog(MedeaDockWidget *dockWidget)
{
    if(dockWidget != activeDockWidget){
        setActiveDockWidget(dockWidget);
    }


    QDialog* dialog = new QDialog();
    dialog->setWindowTitle("Select Destination Window");
    //dialog->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    //dialog->setAttribute(Qt::WA_NoSystemBackground, true);
    dialog->setAttribute(Qt::WA_TranslucentBackground, true);
    dialog->setStyleSheet("QDialog{ background: rgba(150,150,150,255); }");
    //dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::Popup);

    dialog->setModal(true);

    QHBoxLayout* layout = new QHBoxLayout(dialog);
    dialog->setLayout(layout);


    MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();
    foreach(MedeaWindowNew* w, windows.values()){
        if(w != currentWindow){
            QToolButton* button = constructPopOutWindowButton(dialog, w);
            layout->addWidget(button);

        }
    }
    QToolButton* button = constructPopOutWindowButton(dialog, 0);
    layout->addWidget(button);

    /*
    QToolButton* cancelButton = new QToolButton(dialog);
    cancelButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    cancelButton->setIcon(Theme::theme()->getImage("Actions", "Failure", QSize(), QColor(178, 50, 50)));
    cancelButton->setText("Cancel");
    cancelButton->setProperty(WINDOW_ID_DATA, -1);
    cancelButton->setStyleSheet("margin: 10px; border-radius: 5px; border: 1px solid gray; background: white; padding-top: 20px;");
    cancelButton->setFixedSize(170,170);
    cancelButton->setIconSize(QSize(175, 80));
    cancelButton->setFont(QFont("Verdana", 10));
    connect(cancelButton, SIGNAL(clicked(bool)), dialog, SLOT(reject()));
    layout->addWidget(cancelButton);
    */

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

