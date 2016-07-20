#include "nodeviewnew.h"

#include "SceneItems/entityitemnew.h"
#include "SceneItems/nodeitemnew.h"
#include "SceneItems/edgeitemnew.h"

#include "SceneItems/aspectitemnew.h"
#include "SceneItems/modelitemnew.h"
#include "SceneItems/defaultnodeitem.h"
#include "SceneItems/Hardware/hardwarenodeitem.h"

#include <QDebug>
#include <QKeyEvent>

#define ZOOM_INCREMENTOR 1.05



NodeViewNew::NodeViewNew(VIEW_ASPECT aspect):QGraphicsView()
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

    isPanning = false;

    viewController = 0;
    selectionHandler = 0;
    this->aspect = aspect;
    setupAspect();
}

void NodeViewNew::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
    if(viewController){
        selectionHandler = viewController->getSelectionHandler(this);
        connect(selectionHandler, SIGNAL(itemSelected(ViewItem*,bool)), this, SLOT(selectionHandler_ItemSelected(ViewItem*,bool)));
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

void NodeViewNew::fitToScreen()
{
    //NodeItemNew* model = getModelItem();

    //if(model){
        centerRect(scene()->itemsBoundingRect());
        //centerOnItems(model->getChildEntities());
    //}
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

void NodeViewNew::item_SetCentered(EntityItemNew *item)
{
    centerRect(item->sceneViewRect());
}

void NodeViewNew::item_AdjustingPos(bool adjusting)
{
    foreach(ViewItem* viewItem, selectionHandler->getSelection()){
        EntityItemNew* item = getEntityItem(viewItem);
        if(item){
            item->setMoving(adjusting);
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

QRectF NodeViewNew::viewportRect()
{
    return mapToScene(rect()).boundingRect();
}

void NodeViewNew::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(item->getViewAspect() != aspect){
        return;
    }

    NodeItemNew* parentNode = getParentNodeItem(item);

    NodeItemNew* nodeItem =  0;
    QString nodeKind = item->getData("kind").toString();
    if(nodeKind == "Model"){
        //nodeItem = new ModelItemNew(item);
    }else if(nodeKind.endsWith("Definitions")){
        if(nodeKind == "DeploymentDefinitions"){
            return;
        }
        VIEW_ASPECT aspect = GET_ASPECT_FROM_KIND(nodeKind);
        if(this->aspect == aspect){
            //qCritical() << "ASPECT MATCH!";
            //nodeItem = new AspectItemNew(item, parentNode, aspect);
        }
    }else{


        if(nodeKind == "HardwareNode"){
            nodeItem = new HardwareNodeItem(item, parentNode);
        }else{
            nodeItem = new DefaultNodeItem(item, parentNode);
        }
    }

    if(nodeItem){
        guiItems[item->getID()] = nodeItem;
        connect(nodeItem, SIGNAL(req_setSelected(EntityItemNew*,bool,bool)), this, SLOT(item_SetSelected(EntityItemNew*,bool,bool)));
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

void NodeViewNew::setupAspect()
{
    aspectName = GET_ASPECT_NAME(aspect).toUpper();
    aspectColor = GET_ASPECT_COLOR(aspect);
    aspectFontColor = aspectColor.darker(110);
    aspectFont.setPixelSize(70);
    aspectFont.setBold(true);
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

void NodeViewNew::wheelEvent(QWheelEvent *event)
{
    //Call Zoom
    zoom(event->delta(), event->pos());
}

void NodeViewNew::mousePressEvent(QMouseEvent *event)
{
    event->ignore();

    QPointF scenePos = mapToScene(event->pos());
    //bool itemUnderMouse = scene()->itemAt(scenePos, transform());

    if(event->button() == Qt::RightButton){
        isPanning = true;
        panOrigin_Screen = event->pos();
        panPrev_Scene = mapToScene(panOrigin_Screen);
        event->accept();
    }

    if(!event->isAccepted()){
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeViewNew::mouseMoveEvent(QMouseEvent *event)
{
    if(isPanning){
        QPointF pan_Scene = mapToScene(event->pos());
        translate(pan_Scene - panPrev_Scene);
        panPrev_Scene = mapToScene(event->pos());
    }
    QGraphicsView::mouseMoveEvent(event);
}

void NodeViewNew::mouseReleaseEvent(QMouseEvent *event)
{
    if(isPanning){
        isPanning = false;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

/*
void NodeViewNew::focusInEvent(QFocusEvent *event)
{
    emit viewFocussed(this, true);
    QGraphicsView::focusInEvent(event);
}

void NodeViewNew::focusOutEvent(QFocusEvent *event)
{
    emit viewFocussed(this, false);
    QGraphicsView::focusOutEvent(event);
}*/

void NodeViewNew::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->resetTransform();

    painter->setPen(Qt::NoPen);
    painter->setBrush(aspectColor);
    painter->drawRect(this->rect());

    painter->setFont(aspectFont);
    painter->setPen(aspectFontColor);
    painter->drawText(this->rect(), Qt::AlignHCenter | Qt::AlignBottom, aspectName);
}

