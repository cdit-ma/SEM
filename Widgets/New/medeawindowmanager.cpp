#include "medeawindowmanager.h"
#include "medeawindownew.h"
#include <QDebug>
#include <QDialog>
#include <QPushButton>
#include <QBoxLayout>
#include <QObject>
#include "../../View/docktitlebarwidget.h"

MedeaWindowManager* MedeaWindowManager::managerSingleton = 0;

MedeaWindowManager* MedeaWindowManager::manager()
{
    if(!managerSingleton){
        managerSingleton = new MedeaWindowManager();
    }
    return managerSingleton;
}

MedeaWindowManager::MedeaWindowManager():QObject(0)
{
    qCritical() << "YO!";
    windowCount = 0;
    dock = 0;
}

void MedeaWindowManager::setMainWindow(MedeaWindowNew *window)
{
    mainWindow = window;

    addWindow(window);
    setParent(window);
}

MedeaWindowNew *MedeaWindowManager::getNewSubWindow()
{
    MedeaWindowNew* newWindow = new MedeaWindowNew();
    newWindow->setWindowTitle("MEDEA - Sub Window");

    addWindow(newWindow);
    return newWindow;
}

void MedeaWindowManager::changeWidgetWindow()
{
    dock = qobject_cast<DockTitleBarWidget*>(sender());
    if(dock){
        QDialog* dialog = new QDialog(dock);
        dialog->setModal(true);
        QHBoxLayout* layout = new QHBoxLayout(dialog);
        dialog->setLayout(layout);
        foreach(MedeaWindowNew* w, windows.keys()){
            int index = windows[w];
            //if(w != dock->getDockWidget()->parentWidget()){
                QPushButton* button = new QPushButton();
                button->setIconSize(QSize(200, 200));
                button->setIcon(QIcon(w->grab()));
                button->setProperty("Widget", index);
                layout->addWidget(button);
                connect(button, SIGNAL(clicked(bool)), this, SLOT(widgetButtonPressed()));
                connect(button, SIGNAL(clicked(bool)), dialog, SLOT(accept()));
            //}
        }
        dialog->exec();
    }
}

void MedeaWindowManager::widgetButtonPressed()
{
    int ID = sender()->property("Widget").toInt();
    qCritical() << ID;
    if(ID >= 0){
        MedeaWindowNew* window = windows.key(ID);
    }
}


void MedeaWindowManager::addWindow(MedeaWindowNew *window)
{
    if(!windows.contains(window)){
        windows[window] = windowCount;

        QAction* action = new QAction(this);
        action->setText(window->windowTitle());
        windowCount ++;
    }
}

void MedeaWindowManager::removeWindow(MedeaWindowNew *window)
{
    if(windows.contains(window)){
        delete window;
    }
}

