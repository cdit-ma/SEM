#include "selectioncontroller.h"
#include "../../Controller/viewcontroller.h"
#include "medeaviewdockwidget.h"
#include "medeawindowmanager.h"
#include "medeanodeviewdockwidget.h"

#include <QDebug>
SelectionController::SelectionController(ViewController *vc):QObject(vc)
{
    currentHandler = 0;
    currentViewDockWidget = 0;
    viewController = vc;

    connect(MedeaWindowManager::manager(), SIGNAL(activeViewDockWidgetChanged(MedeaViewDockWidget*, MedeaViewDockWidget*)), this, SLOT(activeViewDockWidgetChanged(MedeaViewDockWidget*)));
}

SelectionHandler *SelectionController::constructSelectionHandler(QObject *object)
{
    if(selectionHandlerIDLookup.contains(object)){
        int sID = selectionHandlerIDLookup[object];
        qCritical() << "SelectionController::constructSelectionHandler() - Already got Selection Handler for QObject: " << object;
        return selectionHandlers[sID];
    }else{
        SelectionHandler* handler = new SelectionHandler(this);
        connect(handler, SIGNAL(lastRegisteredObjectRemoved()), this, SLOT(removeSelectionHandler()));
        connect(viewController, SIGNAL(viewItemDestructing(int,ViewItem*)), handler, SLOT(itemDeleted(int,ViewItem*)));
        selectionHandlers[handler->getID()] = handler;
        registerSelectionHandler(object, handler);
        return handler;
    }
}

void SelectionController::registerSelectionHandler(QObject *object, SelectionHandler *handler)
{
    if(!selectionHandlerIDLookup.contains(object)){
        selectionHandlerIDLookup[object] = handler->getID();
        handler->registerObject(object);
    }
}

void SelectionController::unregisterSelectionHandler(QObject *object, SelectionHandler *handler)
{
    if(selectionHandlerIDLookup.contains(object)){
        int sID = selectionHandlerIDLookup[object];
        if(sID == handler->getID()){
            handler->unregisterObject(object);
            selectionHandlerIDLookup.remove(object);
        }
    }
}

QVector<ViewItem *> SelectionController::getSelection()
{
    if(currentHandler){
        return currentHandler->getSelection();
    }
    return QVector<ViewItem*>();
}

QList<int> SelectionController::getSelectionIDs()
{
    QList<int> selection;

    foreach(ViewItem* item, getSelection()){
        selection.append(item->getID());
    }
    return selection;
}

int SelectionController::getSelectionCount()
{
    int count = -1;
    if(currentHandler){
        count = currentHandler->getSelectionCount();
    }
    return count;
}

ViewItem *SelectionController::getFirstSelectedItem()
{
    ViewItem* item = 0;
    if(currentHandler){
        item = currentHandler->getFirstSelectedItem();
    }
    return item;
}

int SelectionController::getFirstSelectedItemID()
{
    int ID = -1;
    if(currentHandler){
        ViewItem* item = currentHandler->getFirstSelectedItem();
        if(item){
            ID = item->getID();
        }
    }
    return ID;
}

void SelectionController::activeViewDockWidgetChanged(MedeaViewDockWidget *dockWidget)
{

    setCurrentViewDockWidget(dockWidget);
}

void SelectionController::cycleActiveSelectionBackward()
{
    cycleActiveSelectedItem(false);
}

void SelectionController::cycleActiveSelectionForward()
{
    cycleActiveSelectedItem(true);
}

void SelectionController::cycleActiveSelectedItem(bool forward)
{
    if(currentHandler){
        currentHandler->cycleActiveSelectedItem(forward);
    }
}

void SelectionController::setCurrentViewDockWidget(MedeaViewDockWidget *d)
{
    MedeaNodeViewDockWidget* newDock = 0;

    if(d && d->isNodeViewDock()){
        newDock = (MedeaNodeViewDockWidget*)d;
    }

    if(newDock != currentViewDockWidget){
        if(currentViewDockWidget){
            NodeViewNew* nodeView = currentViewDockWidget->getNodeView();
            disconnect(this, SIGNAL(clearSelection()), nodeView, SLOT(clearSelection()));
            disconnect(this, SIGNAL(selectAll()), nodeView, SLOT(selectAll()));
        }
        currentViewDockWidget = newDock;

        SelectionHandler* selectionHandler = 0;
        if(currentViewDockWidget){
            selectionHandler = currentViewDockWidget->getSelectionHandler();
            NodeViewNew* nodeView = currentViewDockWidget->getNodeView();
            connect(this, SIGNAL(clearSelection()), nodeView, SLOT(clearSelection()));
            connect(this, SIGNAL(selectAll()), nodeView, SLOT(selectAll()));
        }

        setCurrentSelectionHandler(selectionHandler);
    }
}

void SelectionController::removeSelectionHandler()
{
    SelectionHandler* handler = qobject_cast<SelectionHandler*>(sender());
    if(handler){
        if(!handler->hasRegisteredObjects()){
            selectionHandlers.remove(handler->getID());


            if(currentHandler == handler){
                //Unset the current Handler.
                setCurrentSelectionHandler(0);
            }
            handler->deleteLater();
        }
    }
}

QVector<ViewItem *> SelectionController::getOrderedSelection(QList<int> selection)
{
    QVector<ViewItem*> items;
    if(viewController){
        items = viewController->getOrderedSelection(selection);
    }
    return items;
}

void SelectionController::setCurrentSelectionHandler(SelectionHandler *handler)
{
    if(currentHandler != handler){
        if(currentHandler){
            disconnect(currentHandler, SIGNAL(selectionChanged(int)), this, SIGNAL(selectionChanged(int)));
            disconnect(currentHandler, SIGNAL(itemActiveSelectionChanged(ViewItem*, bool)), this, SIGNAL(itemActiveSelectionChanged(ViewItem*, bool)));
        }
        currentHandler = handler;
        int selectionCount = 0;
        if(handler){
            connect(currentHandler, SIGNAL(selectionChanged(int)), this, SIGNAL(selectionChanged(int)));
            connect(currentHandler, SIGNAL(itemActiveSelectionChanged(ViewItem*, bool)), this, SIGNAL(itemActiveSelectionChanged(ViewItem*, bool)));

            selectionCount = currentHandler->getSelectionCount();
            emit itemActiveSelectionChanged(currentHandler->getActiveSelectedItem(), true);
        }
        emit selectionChanged(selectionCount);
    }
}

