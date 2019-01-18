#include "selectioncontroller.h"

#include "../ViewController/viewcontroller.h"
#include "../../Widgets/DockWidgets/defaultdockwidget.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "../WindowManager/windowmanager.h"

#include <QDebug>
SelectionController::SelectionController(ViewController *vc):QObject(vc)
{
    currentHandler = 0;
    currentViewDockWidget = 0;
    viewController = vc;

    connect(WindowManager::manager(), &WindowManager::activeViewDockWidgetChanged, this, &SelectionController::activeViewDockWidgetChanged);
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


        connect(viewController, &ViewController::vc_viewItemDestructing, handler, &SelectionHandler::itemDeleted);

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

QVector<ViewItem *> SelectionController::getSelection()
{
    if(currentHandler){
        return currentHandler->getSelection();
    }
    return QVector<ViewItem*>();
}

QSet<NODE_KIND> SelectionController::getSelectedNodeKinds()
{
    QSet<NODE_KIND> selectedNodeKinds;
    for (auto item : getSelection()) {
        if (item->isNode()) {
            auto nodeItem = (NodeViewItem*) item;
            selectedNodeKinds.insert(nodeItem->getNodeKind());
        }
    }
    return selectedNodeKinds;
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

ViewItem *SelectionController::getActiveSelectedItem()
{
    ViewItem* item = 0;
    if(currentHandler){
        item = currentHandler->getActiveSelectedItem();
    }
    return item;
}

int SelectionController::getActiveSelectedID(){
    int id = -1;
    auto active = getActiveSelectedItem();
    if(active){
        id = active->getID();
    }
    return id;
}

void SelectionController::activeViewDockWidgetChanged(ViewDockWidget *dockWidget)
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

void SelectionController::setCurrentViewDockWidget(ViewDockWidget *new_dock)
{
    if(new_dock != currentViewDockWidget){
        if(currentViewDockWidget){
            NodeView* nodeView = currentViewDockWidget->getNodeView();
            
            disconnect(this, &SelectionController::zoomIn, nodeView, &NodeView::zoomIn);
            disconnect(this, &SelectionController::zoomOut, nodeView, &NodeView::zoomOut);
            disconnect(this, &SelectionController::clearSelection, nodeView, &NodeView::clearSelection);
            disconnect(this, &SelectionController::selectAll, nodeView, &NodeView::selectAll);
        }
        currentViewDockWidget = new_dock;

        SelectionHandler* selectionHandler = 0;
        if(currentViewDockWidget){
            selectionHandler = currentViewDockWidget->getSelectionHandler();
            NodeView* nodeView = currentViewDockWidget->getNodeView();
            connect(this, &SelectionController::zoomIn, nodeView, &NodeView::zoomIn);
            connect(this, &SelectionController::zoomOut, nodeView, &NodeView::zoomOut);
            connect(this, &SelectionController::clearSelection, nodeView, &NodeView::clearSelection);
            connect(this, &SelectionController::selectAll, nodeView, &NodeView::selectAll);
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



void SelectionController::setCurrentSelectionHandler(SelectionHandler *handler)
{
    if(currentHandler != handler){
        if(currentHandler){
            disconnect(currentHandler, &SelectionHandler::selectionChanged, this, &SelectionController::selectionChanged);
            disconnect(currentHandler, &SelectionHandler::itemActiveSelectionChanged, this, &SelectionController::itemActiveSelectionChanged);
        }
        currentHandler = handler;
        int selectionCount = 0;
        if(currentHandler){
            connect(currentHandler, &SelectionHandler::selectionChanged, this, &SelectionController::selectionChanged);
            connect(currentHandler, &SelectionHandler::itemActiveSelectionChanged, this, &SelectionController::itemActiveSelectionChanged);

            selectionCount = currentHandler->getSelectionCount();
            emit itemActiveSelectionChanged(currentHandler->getActiveSelectedItem(), true);
        }else{
            emit itemActiveSelectionChanged(0, true);
        }
        emit selectionChanged(selectionCount);

    }
}


