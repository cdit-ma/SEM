#include "nodeviewnew.h"

#include "SceneItems/entityitemnew.h"
#include "SceneItems/nodeitemnew.h"
#include "SceneItems/edgeitemnew.h"

#include "SceneItems/aspectitemnew.h"
#include "SceneItems/modelitemnew.h"
#include "SceneItems/defaultnodeitem.h"
#include "SceneItems/Hardware/hardwarenodeitem.h"
#include "theme.h"

#include <QDebug>
#include <QtMath>
#include <QGraphicsItem>
#include <QKeyEvent>

#define ZOOM_INCREMENTOR 1.05

NodeViewNew::NodeViewNew():QGraphicsView()
{
    QRectF sceneRect;
    sceneRect.setSize(QSize(100000,100000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);



    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setAcceptDrops(true);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    //Turn off the Scroll bars.
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);


    viewController = 0;
    selectionHandler = 0;
    containedAspect = VA_NONE;
    containedNodeViewItem = 0;

    rubberband = new QRubberBand(QRubberBand::Rectangle, this);


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    viewState = VS_NONE;
    transition();
}

NodeViewNew::~NodeViewNew()
{
    if(containedNodeViewItem){
        QList<ViewItem*> items = containedNodeViewItem->getChildren();
        items.insert(0, containedNodeViewItem);

        QListIterator<ViewItem*> it(items);

        it.toBack();
        while(it.hasPrevious()){
            ViewItem* item = it.previous();
            viewItem_Destructed(item->getID(), item);
        }
    }
}

void NodeViewNew::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
    if(viewController){
        selectionHandler = viewController->getSelectionController()->constructSelectionHandler(this);
        connect(selectionHandler, SIGNAL(itemSelectionChanged(ViewItem*,bool)), this, SLOT(selectionHandler_ItemSelectionChanged(ViewItem*,bool)));
        connect(selectionHandler, SIGNAL(itemActiveSelectionChanged(ViewItem*,bool)), this, SLOT(selectionHandler_ItemActiveSelectionChanged(ViewItem*,bool)));
    }
}

void NodeViewNew::translate(QPointF point)
{
    QGraphicsView::translate(point.x(), point.y());
    viewportChanged();
}

void NodeViewNew::scale(qreal sx, qreal sy)
{
    if(sx != 1 || sy != 1){
        QGraphicsView::scale(sx, sy);
        viewportChanged();
    }
}

void NodeViewNew::setContainedViewAspect(VIEW_ASPECT aspect)
{
    containedAspect = aspect;
    setupAspect();
}

void NodeViewNew::setContainedNodeViewItem(NodeViewItem *item)
{
    containedNodeViewItem = item;
    if(item){
        item->registerObject(this);

        if(containedAspect == VA_NONE){
            containedAspect = item->getViewAspect();
            //Request items from ViewController.

            viewItem_Constructed(item);
            foreach(ViewItem* item, item->getChildren()){
                viewItem_Constructed(item);
            }
        }
    }
}


QRectF NodeViewNew::getViewportRect()
{
    return viewportRect();
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
    if(item){
        topLevelGUIItemIDs.removeAll(ID);
        guiItems.remove(ID);
        delete item;
    }
}

void NodeViewNew::selectionHandler_ItemSelectionChanged(ViewItem *item, bool selected)
{
    if(item){
        EntityItemNew* e = getEntityItem(item->getID());
        if(e){
            e->setSelected(selected);
        }
    }
}

void NodeViewNew::selectionHandler_ItemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    if(item){
        EntityItemNew* e = getEntityItem(item->getID());
        if(e){
            e->setActiveSelected(isActive);
        }
    }
}

void NodeViewNew::themeChanged()
{
    aspectColor = Theme::theme()->getAspectBackgroundColor(containedAspect);
    aspectFontColor = aspectColor.darker(110);
}

void NodeViewNew::fitToScreen()
{
    //NodeItemNew* model = getModelItem();

    //if(model){
        centerRect(scene()->itemsBoundingRect());
        //centerOnItems(model->getChildEntities());
        //}
}

void NodeViewNew::item_Selected(ViewItem *item, bool append)
{
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(item, append);
    }
}

void NodeViewNew::item_ActiveSelected(ViewItem *item)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
    }
}

void NodeViewNew::item_SetExpanded(EntityItemNew *item, bool expand)
{
    item->setExpanded(expand);
}

void NodeViewNew::item_SetCentered(EntityItemNew *item)
{
    centerRect(item->sceneViewRect());
}

void NodeViewNew::item_AdjustingPos(bool adjusting)
{
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getSelection()){
            EntityItemNew* item = getEntityItem(viewItem);
            if(item){
                item->setMoving(adjusting);
            }
        }
    }
}

void NodeViewNew::item_AdjustPos(QPointF delta)
{
    //Moves the selection.
    if(selectionHandler){
        bool isMoveOkay = true;
        foreach(ViewItem* viewItem, selectionHandler->getSelection()){
            EntityItemNew* item = getEntityItem(viewItem);
            if(item){
                //Try see if move is okay.
                if(!item->isAdjustValid(delta)){
                    isMoveOkay = false;
                    break;
                }
            }
        }
        if(isMoveOkay){
            foreach(ViewItem* viewItem, selectionHandler->getSelection()){
                EntityItemNew* item = getEntityItem(viewItem);
                if(item){
                    //Move!
                    item->adjustPos(delta);
                }
            }
        }
    }
}

void NodeViewNew::minimap_Panning(bool panning)
{

}

void NodeViewNew::minimap_Pan(QPointF delta)
{
    translate(delta);
}

void NodeViewNew::minimap_Zoom(int delta)
{
    zoom(delta);
}

void NodeViewNew::centerOnItems(QList<EntityItemNew *> items)
{
    QRectF itemsRect;
    foreach(EntityItemNew* item, items){
        itemsRect = itemsRect.united(item->sceneViewRect());
    }
    centerRect(itemsRect);
}

NodeItemNew *NodeViewNew::getModelItem()
{
    NodeItemNew* item = 0;
    if(viewController){
        ViewItem* viewItem = viewController->getModel();
        EntityItemNew* model = 0;
        if(viewItem){
            model = getEntityItem(viewItem->getID());
        }
        if(model && model->isNodeItem()){
            item = (NodeItemNew*) model;
        }
    }
    return item;
}

void NodeViewNew::centerRect(QRectF rectScene)
{
    //Inflate by 110%
    QRectF visibleRect = viewportRect();
    qreal widthRatio = visibleRect.width() / (rectScene.width() * 1.1);
    qreal heightRatio = visibleRect.height() / (rectScene.height() * 1.1);

    qreal scaleRatio = qMin(widthRatio, heightRatio);

    //Change the scale.
    scale(scaleRatio, scaleRatio);
    centerView(rectScene.center());
}

void NodeViewNew::centerView(QPointF scenePos)
{
    QPointF delta = viewportRect().center() - scenePos;
    translate(delta);
    viewportCenter_Scene = viewportRect().center();
}

void NodeViewNew::viewportChanged()
{
    emit viewportChanged(viewportRect(), transform().m11());
}

SelectionHandler *NodeViewNew::getSelectionHandler()
{
    return selectionHandler;
}

QRectF NodeViewNew::viewportRect()
{
    return mapToScene(rect()).boundingRect();
}

void NodeViewNew::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(!item || item->getViewAspect() != containedAspect){
        return;
    }

    NodeItemNew* parentNode = getParentNodeItem(item);



    if(!containedNodeViewItem){
        if(item->getViewAspect() == containedAspect){
            setContainedNodeViewItem(item);
            //Don't construct an aspect.
            return;
        }
    }
    if(containedNodeViewItem){
        if(containedNodeViewItem->isAncestorOf(item)){
            int ID = item->getID();
            NodeItemNew* nodeItem =  0;
            QString nodeKind = item->getData("kind").toString();

            if(nodeKind == "HardwareNode"){
                nodeItem = new HardwareNodeItem(item, parentNode);
            }else{
                nodeItem = new DefaultNodeItem(item, parentNode);
            }

            if(nodeItem){
                guiItems[ID] = nodeItem;
                connect(nodeItem, SIGNAL(req_activeSelected(ViewItem*)), this, SLOT(item_ActiveSelected(ViewItem*)));
                connect(nodeItem, SIGNAL(req_selected(ViewItem*,bool)), this, SLOT(item_Selected(ViewItem*,bool)));

                connect(nodeItem, SIGNAL(req_expanded(EntityItemNew*,bool)), this, SLOT(item_SetExpanded(EntityItemNew*,bool)));
                connect(nodeItem, SIGNAL(req_centerItem(EntityItemNew*)), this, SLOT(item_SetCentered(EntityItemNew*)));
                connect(nodeItem, SIGNAL(req_adjustPos(QPointF)), this, SLOT(item_AdjustPos(QPointF)));
                connect(nodeItem, SIGNAL(req_adjustingPos(bool)), this, SLOT(item_AdjustingPos(bool)));

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
                    topLevelGUIItemIDs.append(ID);
                }
            }
        }
    }
}

void NodeViewNew::edgeViewItem_Constructed(EdgeViewItem *item)
{
    //Do nothing.
}

QList<ViewItem *> NodeViewNew::getTopLevelViewItems()
{
    QList<ViewItem *> items;
    foreach(EntityItemNew* item, getTopLevelEntityItems()){
        items.append(item->getViewItem());
    }
    return items;
}

QList<EntityItemNew *> NodeViewNew::getTopLevelEntityItems()
{
    QList<EntityItemNew*> items;
    foreach(int ID, topLevelGUIItemIDs){
        if(guiItems.contains(ID)){
            items.append(guiItems[ID]);
        }
    }
    return items;
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
    EntityItemNew* e = 0;
    if(item){
        e = getEntityItem(item->getID());
    }
    return e;
}

void NodeViewNew::zoom(int delta, QPoint anchorScreenPos)
{
    QPointF anchorScenePos = getScenePosOfPoint(anchorScreenPos);

    //Calculate the zoom change.
    qreal scaleFactor = pow(ZOOM_INCREMENTOR, (delta / abs(delta)));
    if(scaleFactor != 1){
        scale(scaleFactor, scaleFactor);

        QPointF delta = getScenePosOfPoint(anchorScreenPos) - anchorScenePos;
        translate(delta);
    }
}

QPointF NodeViewNew::getScenePosOfPoint(QPoint pos)
{
    if(pos.isNull()){
        //If we haven't been given a point, use the center of the viewport rect.
        pos = viewport()->rect().center();
    }
    return mapToScene(pos);
}

void NodeViewNew::selectItemsInRubberband()
{
    QPolygonF rubberbandRect = mapToScene(rubberband->geometry());

    QList<ViewItem*> itemsToSelect;

    foreach(QGraphicsItem* i, scene()->items(rubberbandRect,Qt::IntersectsItemShape)){
        EntityItemNew* entityItem = dynamic_cast<EntityItemNew*>(i);
        if(entityItem){
            itemsToSelect.append(entityItem->getViewItem());
        }
    }
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(itemsToSelect, true);
    }
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

        QList<ViewItem*> itemsToSelect;

        if(guiItem){
            if(selectionHandler->getSelectionCount() == 1 && guiItem->isNodeItem()){
                NodeItemNew* nodeItem = (NodeItemNew*) guiItem;

                foreach(NodeItemNew* child, nodeItem->getChildNodes()){
                    itemsToSelect.append(child->getViewItem());
                }
            }
        }else{
            //Get all top level children.
            itemsToSelect = getTopLevelViewItems();
        }
        if(itemsToSelect.size() > 0){
            selectionHandler->toggleItemsSelection(itemsToSelect, false);
        }
    }
}

void NodeViewNew::setupAspect()
{
    aspectName = GET_ASPECT_NAME(containedAspect).toUpper();
    aspectColor = GET_ASPECT_COLOR(containedAspect);
    aspectFontColor = aspectColor.darker(110);
    aspectFont.setPixelSize(70);
    aspectFont.setBold(true);
}

void NodeViewNew::setState(VIEW_STATE state)
{
    VIEW_STATE newState = viewState;

    switch(viewState){
    case VS_NONE:
        //Go onto VS_SELECTED
    case VS_SELECTED:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_MOVING || state == VS_RESIZING || state == VS_PAN || state == VS_RUBBERBAND || state == VS_CONNECT){
            newState = state;
        }
        break;
    case VS_MOVING:
        //Go onto VS_RESIZING state
    case VS_RESIZING:
        if(state == VS_NONE || state == VS_SELECTED){
            newState = state;
        }
        break;
    case VS_RUBBERBAND:
        //Go onto VS_RUBBERBANDING state
    case VS_RUBBERBANDING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_RUBBERBAND || state == VS_RUBBERBANDING){
            newState = state;
        }
        break;
    case VS_PAN:
        //Go Onto VS_PANNING state
    case VS_PANNING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_PANNING){
            newState = state;
        }
        break;
    case VS_CONNECT:
    case VS_CONNECTING:
        if(state == VS_NONE || state == VS_SELECTED || state == VS_CONNECTING){
            newState = state;
        }
        break;
    default:
        break;
    }

    //Transition
    if(newState != viewState){
        viewState = newState;
        transition();
    }


}

void NodeViewNew::transition()
{
    switch(viewState){
    case VS_NONE:
        //Do the VS_SELECTED case.
    case VS_SELECTED:
        //setConnectMode(false);
        //setRubberBandMode(false);
        rubberband->setVisible(false);
        unsetCursor();
        break;
    case VS_MOVING:
        //triggerAction("View: Moving Selection");
        setCursor(Qt::SizeAllCursor);
        break;
    case VS_RESIZING:
        //triggerAction("View: Resizing Selection");
        //Cursor is set by EntityItem
        break;
    case VS_PAN:
        setCursor(Qt::ClosedHandCursor);
        break;
    case VS_PANNING:
        //wasPanning = true;
        break;
    case VS_RUBBERBAND:
        setCursor(Qt::CrossCursor);
        rubberband->setVisible(false);
        break;
    case VS_RUBBERBANDING:
        rubberband->setVisible(true);
        break;
    case VS_CONNECT:
        //setConnectMode(true);
        setCursor(Qt::CrossCursor);
        break;
    case VS_CONNECTING:
        setCursor(Qt::CrossCursor);
        break;
    default:
        break;
    }
}

qreal NodeViewNew::distance(QPoint p1, QPoint p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void NodeViewNew::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        setState(VS_RUBBERBAND);
    }
    if(CONTROL && event->key() == Qt::Key_A){
        selectAll();
    }
    if(event->key() == Qt::Key_Escape){
        clearSelection();
        event->accept();
    }
    /*
    if(event->key() == Qt::Key_Tab){
        //Cycle Selection.
        selectionHandler->cycleActiveSelectedItem(true);
    }
    if(event->key() == Qt::Key_Backtab){
        selectionHandler->cycleActiveSelectedItem(false);
    }*/
}

void NodeViewNew::keyReleaseEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if((viewState == VS_RUBBERBAND || viewState == VS_RUBBERBAND) && !(CONTROL && SHIFT)){
        setState(VS_NONE);
    }
}

void NodeViewNew::wheelEvent(QWheelEvent *event)
{
    //Call Zoom
    zoom(event->delta(), event->pos());
}

void NodeViewNew::mousePressEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    bool handledEvent = false;

    if(event->button() == Qt::RightButton){
        setState(VS_PAN);
        pan_lastPos = event->pos();
        pan_lastScenePos = scenePos;
        pan_distance = 0;
        handledEvent = true;
    }
    if(event->button() == Qt::LeftButton){
        switch(viewState){
            case VS_RUBBERBAND:{
                setState(VS_RUBBERBANDING);
            }
            case VS_RUBBERBANDING:{
                rubberband_lastPos = event->pos();
                if(rubberband){
                    rubberband->setGeometry(QRect(rubberband_lastPos, rubberband_lastPos));
                }
                handledEvent = true;
                break;
            }
        default:
            bool itemUnderMouse = scene()->itemAt(scenePos, transform());
            if(!itemUnderMouse){
                selectionHandler->clearSelection();
                handledEvent = true;
            }
            break;
        }
    }
    if(!handledEvent){
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeViewNew::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    bool handledEvent = false;

    switch(viewState){
    case VS_PAN:{
        //Continue on
        setState(VS_PANNING);
    }
    case VS_PANNING:{
        //Calculate the distance in screen pixels travelled
        pan_distance += distance(event->pos(), pan_lastPos);
        //Pan the Canvas
        translate(scenePos - pan_lastScenePos);
        pan_lastPos = event->pos();
        pan_lastScenePos = mapToScene(event->pos());
        handledEvent = true;
        break;
    }
    case VS_RUBBERBAND:
    case VS_RUBBERBANDING:{
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        handledEvent = true;
        break;
    }
    default:
        break;
    }

    //Only pass down if we haven't handled it.
    if(!handledEvent){
        QGraphicsView::mouseMoveEvent(event);
    }
}

void NodeViewNew::mouseReleaseEvent(QMouseEvent *event)
{
    switch(viewState){
    case VS_PAN:{
        //Do Nothing
    }
    case VS_PANNING:{
        if(pan_distance < 10){
            emit toolbarRequested(event->screenPos());
        }
        setState(VS_NONE);
        break;
    }
    case VS_RUBBERBANDING:
        selectItemsInRubberband();
        setState(VS_RUBBERBAND);
        break;
    default:
        break;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void NodeViewNew::drawBackground(QPainter *painter, const QRectF &r)
{
    painter->resetTransform();

    painter->setPen(Qt::NoPen);
    painter->setBrush(aspectColor);
    painter->drawRect(rect());

    painter->setFont(aspectFont);
    painter->setPen(aspectFontColor);
    painter->drawText(rect(), Qt::AlignHCenter | Qt::AlignBottom, aspectName);
}

void NodeViewNew::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    viewportChanged();
}

