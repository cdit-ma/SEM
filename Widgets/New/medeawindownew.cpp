#include "medeawindownew.h"
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QMenu>
#include "../../View/docktitlebarwidget.h"
#include "../../View/theme.h"
MedeaWindowNew::MedeaWindowNew(QWidget *parent):QMainWindow(parent)
{
    setAcceptDrops(true);
    setDockNestingEnabled(true);
    resetDockedWidgetsAction = new QAction("Reset Docked Widgets", this);
    resetDockedWidgetsAction->setIcon(Theme::theme()->getImage("Actions", "Maximize"));
}

void MedeaWindowNew::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat(MIME_DRAGWIDGET)){
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else{
        event->ignore();
    }
}

void MedeaWindowNew::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasFormat(MIME_DRAGWIDGET)){
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }else{
        event->ignore();
    }
}

QMenu *MedeaWindowNew::createPopupMenu()
{
    QMenu* menu = QMainWindow::createPopupMenu();
    menu->addSeparator();
    menu->addAction(resetDockedWidgetsAction);
    return menu;
}

