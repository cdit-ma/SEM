#include "medeawindownew.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include "../../View/docktitlebarwidget.h"
#include "../../Widgets/New/medeawindowmanager.h"
#include "../../View/theme.h"
MedeaWindowNew::MedeaWindowNew(QWidget *parent):QMainWindow(parent)
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);
    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));
}

void MedeaWindowNew::addMedeaDockWidget(MedeaDockWidget *widget, Qt::DockWidgetArea area)
{
    qCritical() << "addMedeaDockWidget!" << this->windowTitle();
    connect(widget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Closed()));
    connect(widget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximized(bool)));
    connect(widget, SIGNAL(popOutWidget(bool)), this, SLOT(dockWidget_PopOut(bool)));
    widget->setCurrentWindow(this);
    widget->setParent(this);
    QMainWindow::addDockWidget(area, widget, Qt::Horizontal);
}

void MedeaWindowNew::removeDockWidget(MedeaDockWidget *widget)
{
    qCritical() << "removeDockWidget!" << this->windowTitle();
    disconnect(widget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Closed()));
    disconnect(widget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximized(bool)));
    disconnect(widget, SIGNAL(popOutWidget(bool)), this, SLOT(dockWidget_PopOut(bool)));
    widget->setCurrentWindow(0);
    //QMainWindow::removeDockWidget(widget);
}


void MedeaWindowNew::dockWidget_Closed()
{
    MedeaDockWidget* dockPressed = qobject_cast<MedeaDockWidget*>(sender());
    dockPressed->setCurrentWindow(0);
}

void MedeaWindowNew::dockWidget_Maximized(bool maximized)
{
    qCritical() << maximized;


    MedeaDockWidget* dockPressed = qobject_cast<MedeaDockWidget*>(sender());
    foreach(MedeaDockWidget* dockWidget, findChildren<MedeaDockWidget*>()){
        bool setVisible = !maximized;
        dockWidget->setVisible(setVisible);
    }
    if(maximized){
        dockPressed->setVisible(maximized);
    }

}

void MedeaWindowNew::dockWidget_PopOut(bool popOut)
{
    MedeaDockWidget* dockPressed = qobject_cast<MedeaDockWidget*>(sender());
    if(popOut){
        MedeaWindowNew* newWindow = MedeaWindowManager::manager()->getNewSubWindow();
        newWindow->show();
        removeDockWidget(dockPressed);
        newWindow->addMedeaDockWidget(dockPressed);
    }else{
        MedeaWindowNew* newWindow = dockPressed->getSourceWindow();
        removeDockWidget(dockPressed);
        newWindow->addMedeaDockWidget(dockPressed);
        newWindow->show();
    }
}

QMenu *MedeaWindowNew::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}

