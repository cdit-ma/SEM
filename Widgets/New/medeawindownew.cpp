#include "medeawindownew.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include "../../View/docktitlebarwidget.h"
#include "../../Widgets/New/medeawindowmanager.h"
#include "../../View/theme.h"

int MedeaWindowNew::_WindowID = 0;

MedeaWindowNew::MedeaWindowNew(QWidget *parent, bool mainWindow):QMainWindow(parent)
{
    ID = ++_WindowID;

    _isMainWindow = mainWindow;
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);
    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));

    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::TopDockWidgetArea, QTabWidget::West);
    setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::West);

    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
    connect(resetDockedWidgetsAction, SIGNAL(triggered(bool)), this, SLOT(resetDockWidgets()));
}

MedeaWindowNew::~MedeaWindowNew()
{
}

QList<MedeaDockWidget *> MedeaWindowNew::getDockWidgets()
{
    return childrenDockWidgets;
}

int MedeaWindowNew::getID()
{
    return ID;
}

void MedeaWindowNew::addMedeaDockWidget(MedeaDockWidget *widget, Qt::DockWidgetArea area)
{
    if (childrenDockWidgets.contains(widget)) {
        return;
    }
    connect(widget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Closed()));
    connect(widget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximized(bool)));
    connect(widget, SIGNAL(customContextMenuRequested(QPoint)), this, SIGNAL(customContextMenuRequested(QPoint)));
    QMainWindow::addDockWidget(area, widget, Qt::Horizontal);
    childrenDockWidgets.append(widget);
    widget->setCurrentWindow(this);
    widget->show();
}


void MedeaWindowNew::removeMedeaDockWidget(MedeaDockWidget *widget)
{
    disconnect(widget, SIGNAL(closeWidget()), this, SLOT(dockWidget_Closed()));
    disconnect(widget, SIGNAL(maximizeWidget(bool)), this, SLOT(dockWidget_Maximized(bool)));
    disconnect(widget, SIGNAL(customContextMenuRequested(QPoint)), this, SIGNAL(customContextMenuRequested(QPoint)));
    QMainWindow::removeDockWidget(widget);
    childrenDockWidgets.removeAll(widget);

    if (!isMainWindow() && childrenDockWidgets.isEmpty()) {
        close();
    }
}

bool MedeaWindowNew::isMainWindow()
{
    return _isMainWindow;
}

void MedeaWindowNew::dockWidget_Closed()
{
    MedeaDockWidget* dockPressed = qobject_cast<MedeaDockWidget*>(sender());
    removeMedeaDockWidget(dockPressed);

}

void MedeaWindowNew::dockWidget_Maximized(bool maximized)
{
    MedeaDockWidget* dockPressed = qobject_cast<MedeaDockWidget*>(sender());
    foreach(MedeaDockWidget* dockWidget, findChildren<MedeaDockWidget*>()){
        bool setVisible = !maximized;
        dockWidget->setVisible(setVisible);
    }
    if(maximized){
        dockPressed->setVisible(maximized);
    }

}

void MedeaWindowNew::resetDockWidgets()
{
    foreach(MedeaDockWidget* child, childrenDockWidgets){
        child->setVisible(true);
    }
}

void MedeaWindowNew::showContextMenu(const QPoint & point)
{
    createPopupMenu()->exec(mapToGlobal(point));
}

void MedeaWindowNew::closeEvent(QCloseEvent *)
{
    if(!isMainWindow()){
        MedeaWindowManager::destructWindow(this);
    }
}

QMenu *MedeaWindowNew::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}

