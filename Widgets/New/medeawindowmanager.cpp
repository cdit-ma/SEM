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
    removeWindow(window);
    delete window;
}

void MedeaWindowManager::_destructDockWidget(MedeaDockWidget *widget)
{
    removeDockWidget(widget);
    delete widget;
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

void MedeaWindowManager::addDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(!dockWidgets.contains(ID)){
            dockWidgets[ID] = dockWidget;
            connect(dockWidget, SIGNAL(popOutWidget(bool)), this, SLOT(dockWidgetPopOut(bool)));
        }
    }
}

void MedeaWindowManager::removeDockWidget(MedeaDockWidget *dockWidget)
{
    if(dockWidget){
        int ID = dockWidget->getID();
        if(dockWidgets.contains(ID)){
            //Clear Active if we are.
            if(activeDockWidget == dockWidget){
                setActiveDockWidget();
            }
            dockWidgets.remove(ID);
            disconnect(dockWidget, SIGNAL(popOutWidget(bool)), this, SLOT(dockWidgetPopOut(bool)));
        }
    }
}

void MedeaWindowManager::dockWidgetPopOut(bool popOut)
{
    MedeaDockWidget* dockWidget = qobject_cast<MedeaDockWidget*>(sender());
    if(dockWidget){
        showPopOutDialog(dockWidget);
    }
}

void MedeaWindowManager::widgetButtonPressed()
{
    bool isInt;
    int ID = sender()->property(WINDOW_ID_DATA).toInt(&isInt);
    if(isInt){
        MedeaWindowNew* window = 0;
        if(ID >= 0 && windows.contains(ID)){
            window = windows[ID];
        }else{
            window = _constructSubWindow("New Sub Window");
        }
        reparentDockWidget(activeDockWidget, window);
    }
}



void MedeaWindowManager::addWindow(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        if(!windows.contains(ID)){
            windows[ID] = window;

        }
    }
}

void MedeaWindowManager::removeWindow(MedeaWindowNew *window)
{
    if(window){
        int ID = window->getID();
        if(windows.contains(ID)){
            windows.remove(ID);
        }
        QList<MedeaDockWidget*> children = window->getDockWidgets();
        while(!children.isEmpty()){
            MedeaDockWidget* child = children.takeFirst();

            if(child->isProtected() && !window->isMainWindow()){
                MedeaWindowNew* sourceWindow = child->getSourceWindow();
                if(sourceWindow){
                    child->setCurrentWindow(sourceWindow);
                }
            }else{
                _destructDockWidget(child);
            }
        }
    }
}

void MedeaWindowManager::reparentDockWidget(MedeaDockWidget *dockWidget, MedeaWindowNew *window)
{
    if(dockWidget && window){
        dockWidget->setCurrentWindow(window);
        window->show();
        window->activateWindow();
    }
}

void MedeaWindowManager::showPopOutDialog(MedeaDockWidget *dockWidget)
{
    if(dockWidget != activeDockWidget){
        setActiveDockWidget(dockWidget);
    }


    QDialog* dialog = new QDialog(dockWidget);
    dialog->setWindowTitle("Select Parent Window");
    dialog->setParent(0);
    //dialog->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    //dialog->setAttribute(Qt::WA_NoSystemBackground, true);
    //dialog->setAttribute(Qt::WA_TranslucentBackground, true);
    dialog->setModal(true);

    QHBoxLayout* layout = new QHBoxLayout(dialog);
    dialog->setLayout(layout);


    MedeaWindowNew* currentWindow = dockWidget->getCurrentWindow();
    foreach(MedeaWindowNew* w, windows.values()){
        if(w != currentWindow){
            QToolButton* button = constructPopOutWindowButton(w);
            connect(button, SIGNAL(clicked(bool)), this, SLOT(widgetButtonPressed()));
            connect(button, SIGNAL(clicked(bool)), dialog, SLOT(accept()));
            layout->addWidget(button);
        }
    }
    QToolButton* button = constructPopOutWindowButton(0);
    connect(button, SIGNAL(clicked(bool)), this, SLOT(widgetButtonPressed()));
    connect(button, SIGNAL(clicked(bool)), dialog, SLOT(accept()));
    layout->addWidget(button);

    dialog->exec();
}

QToolButton *MedeaWindowManager::constructPopOutWindowButton(MedeaWindowNew *window)
{
    QToolButton* button = new QToolButton();
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    button->setIconSize(QSize(200,100));
    if(window){
        button->setIcon(QIcon(window->grab()));
        QString text = window->windowTitle();
        if(text == ""){
            text = "MEDEA - Main Window";
        }
        button->setText(text);
        button->setProperty(WINDOW_ID_DATA, window->getID());
    }else{
        button->setIcon(Theme::theme()->getImage("Actions", "Plus"));
        button->setText("New Window");
        button->setProperty(WINDOW_ID_DATA, -1);
    }
    return button;
}

