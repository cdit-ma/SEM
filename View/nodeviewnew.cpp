#include "nodeviewnew.h"

#include "SceneItems/entityitemnew.h"
#include "SceneItems/nodeitemnew.h"
#include "SceneItems/edgeitemnew.h"

#include "SceneItems/aspectitemnew.h"
#include "SceneItems/modelitemnew.h"
#include "SceneItems/defaultnodeitem.h"

#include <QDebug>
#include <QKeyEvent>
NodeViewNew::NodeViewNew():QGraphicsView()
{
    setSceneRect(QRectF(-10000,-10000,20000,20000));
    setScene(new QGraphicsScene(this));
    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    viewController = 0;
    selectionHandler = 0;

}

void NodeViewNew::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
    if(viewController){
        this->selectionHandler = viewController->getSelectionHandler(this);
        connect(selectionHandler, SIGNAL(itemSelected(ViewItem*,bool)), this, SLOT(selectionHandler_ItemSelected(ViewItem*,bool)));
    }
}

void NodeViewNew::viewItem_Constructed(ViewItem *item)
{
    if(item){
        if(item->isNode()){
            nodeViewItem_Constructed((NodeViewItem*)item);
        }else if(item->isEdge()){
            edgeViewItem_Constructed((EdgeViewItem*)item);
        }
    }
}

void NodeViewNew::viewItem_Destructed(int ID, ViewItem *viewItem)
{
    EntityItemNew* item = getEntityItem(ID);
    qCritical() << "ID: " << ID;
    if(item){
        guiItems.remove(ID);
        delete item;
    }
}

void NodeViewNew::selectionHandler_ItemSelected(ViewItem *item, bool selected)
{
    if(item){
        EntityItemNew* e = getEntityItem(item->getID());
        if(e){
            e->setSelected(selected);
        }
    }
}

void NodeViewNew::item_SetSelected(EntityItemNew *item, bool selected, bool append)
{
    if(selectionHandler){
        selectionHandler->setItemSelected(item->getViewItem(), selected, append);
    }
}

void NodeViewNew::item_SetExpanded(EntityItemNew *item, bool expand)
{
    item->setExpanded(expand);
}

void NodeViewNew::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(!item->isInModel()){
        return;
    }

    NodeItemNew* parentNode = getParentNodeItem(item);

    NodeItemNew* nodeItem =  0;
    QString nodeKind = item->getData("kind").toString();
    if(nodeKind == "Model"){
        nodeItem = new ModelItemNew(item);
    }else if(nodeKind.endsWith("Definitions")){
        if(nodeKind == "DeploymentDefinitions"){
            return;
        }
        VIEW_ASPECT aspect = GET_ASPECT_FROM_KIND(nodeKind);
        nodeItem = new AspectItemNew(item, parentNode, aspect);
    }else if(parentNode){
        nodeItem = new DefaultNodeItem(item, parentNode);
    }

    if(nodeItem){
        guiItems[item->getID()] = nodeItem;
        connect(nodeItem, SIGNAL(req_setSelected(EntityItemNew*,bool,bool)), this, SLOT(item_SetSelected(EntityItemNew*,bool,bool)));
        connect(nodeItem, SIGNAL(req_expanded(EntityItemNew*,bool)), this, SLOT(item_SetExpanded(EntityItemNew*,bool)));

        /*
        connect(nodeItem, SIGNAL(req_adjustSize(NodeViewItem*,QSizeF, RECT_VERTEX)), this, SLOT(nodeItemNew_AdjustSize(NodeViewItem*,QSizeF,RECT_VERTEX)));
        connect(nodeItem, SIGNAL(req_setData(ViewItem*,QString,QVariant)), this, SLOT(nodeItemNew_SetData(ViewItem*,QString,QVariant)));
        connect(nodeItem, SIGNAL(req_hovered(EntityItemNew*,bool)), this, SLOT(entityItemNew_Hovered(EntityItemNew*, bool)));
        connect(nodeItem, SIGNAL(req_expanded(EntityItemNew*,bool)), this, SLOT(entityItemNew_Expand(EntityItemNew*,bool)));
        connect(nodeItem, SIGNAL(req_setSelected(ViewItem*,bool)), this, SLOT(entityItemNew_Select(ViewItem*,bool)));
        connect(nodeItem, SIGNAL(req_clearSelection()), this, SLOT(clearSelection()));
        connect(nodeItem, SIGNAL(req_adjustPos(QPointF)), this, SLOT(moveSelection(QPointF)));
        connect(nodeItem, SIGNAL(req_adjustPosFinished()), this, SLOT(moveFinished()));
        */
        if(!scene()->items().contains(nodeItem)){
           scene()->addItem(nodeItem);
        }
    }
}

void NodeViewNew::edgeViewItem_Constructed(EdgeViewItem *item)
{
    //Do nothing.
}

NodeItemNew *NodeViewNew::getParentNodeItem(NodeViewItem *item)
{
     NodeItemNew* guiParentItem = 0;

     int parentID = -1;
     int depth = 1;
     while(true){
        parentID = item->getParentID(depth++);
        if(parentID == -1){
            break;
        }else if(guiItems.contains(parentID)){
            EntityItemNew* p = guiItems[parentID];
            if(p->isNodeItem()){
                guiParentItem = (NodeItemNew*)p;
                break;
            }
        }
     }
     return guiParentItem;
}

EntityItemNew *NodeViewNew::getEntityItem(int ID)
{
    EntityItemNew* item = 0;
    if(guiItems.contains(ID)){
        item = guiItems[ID];
    }
    return item;
}

EntityItemNew *NodeViewNew::getEntityItem(ViewItem *item)
{
    return getEntityItem(item->getID());
}

void NodeViewNew::clearSelection()
{
    if(selectionHandler){
        selectionHandler->clearSelection();
    }
}

void NodeViewNew::selectAll()
{
    if(selectionHandler){
        EntityItemNew* guiItem = getEntityItem(selectionHandler->getFirstSelectedItem());
        if(selectionHandler->getSelectionCount() == 1 && guiItem){
            if(guiItem->isNodeItem()){
                NodeItemNew* nodeItem = (NodeItemNew*) guiItem;

                QList<ViewItem*> itemsToSelect;
                foreach(NodeItemNew* child, nodeItem->getChildNodes()){
                    itemsToSelect.append(child->getViewItem());
                }
                selectionHandler->setItemsSelected(itemsToSelect, true);
            }
        }
    }
}

void NodeViewNew::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && event->key() == Qt::Key_A){
        selectAll();
    }
    if(event->key() == Qt::Key_Escape){
        clearSelection();
    }
}

