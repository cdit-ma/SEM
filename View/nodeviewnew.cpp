#include "nodeviewnew.h"

#include "SceneItems/entityitemnew.h"
#include "SceneItems/nodeitemnew.h"
#include "SceneItems/edgeitemnew.h"

#include "SceneItems/aspectitemnew.h"
#include "SceneItems/modelitemnew.h"
#include "SceneItems/defaultnodeitem.h"
#include "SceneItems/Hardware/hardwarenodeitem.h"
#include "SceneItems/Assemblies/managementcomponentnodeitem.h"
#include "SceneItems/eventportnodeitem.h"
#include "SceneItems/attributenodeitem.h"
#include "SceneItems/nodeitemcontainer.h"
#include "SceneItems/Assemblies/nodeitemstackcontainer.h"
#include "SceneItems/Assemblies/nodeitemcolumncontainer.h"
#include "SceneItems/Assemblies/nodeitemcolumnitem.h"
#include "SceneItems/Assemblies/assemblyeventportnodeitem.h"
#include "theme.h"
#include <QDebug>
#include <QtMath>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QDateTime>

#define ZOOM_INCREMENTOR 1.05

NodeViewNew::NodeViewNew(QWidget* parent):QGraphicsView(parent)
{
    QRectF sceneRect;
    sceneRect.setSize(QSize(10000,10000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);

    //OPENGL
    //#include <QOpenGLWidget>
    //setViewport(new QOpenGLWidget());
    //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);


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
    isAspectView = false;

    backgroundFont.setPixelSize(70);
    //backgroundFont.setBold(true);


    rubberband = new QRubberBand(QRubberBand::Rectangle, this);


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    viewState = VS_NONE;
    transition();

    themeChanged();
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

        connect(viewController, &ViewController::vc_viewItemConstructed, this, &NodeViewNew::viewItem_Constructed);
        connect(viewController, &ViewController::vc_viewItemDestructing, this, &NodeViewNew::viewItem_Destructed);

        selectionHandler = viewController->getSelectionController()->constructSelectionHandler(this);
        connect(selectionHandler, SIGNAL(itemSelectionChanged(ViewItem*,bool)), this, SLOT(selectionHandler_ItemSelectionChanged(ViewItem*,bool)));
        connect(selectionHandler, SIGNAL(itemActiveSelectionChanged(ViewItem*,bool)), this, SLOT(selectionHandler_ItemActiveSelectionChanged(ViewItem*,bool)));

        connect(this, &NodeViewNew::toolbarRequested, viewController, &ViewController::vc_showToolbar);
        connect(this, &NodeViewNew::triggerAction, viewController, &ViewController::vc_triggerAction);
        connect(this, &NodeViewNew::setData, viewController, &ViewController::vc_setData);
        connect(this, &NodeViewNew::removeData, viewController, &ViewController::vc_removeData);
        connect(this, &NodeViewNew::editData, viewController, &ViewController::vc_editTableCell);


        connect(viewController, &ViewController::vc_centerItem, this, &NodeViewNew::centerItem);
        connect(viewController, &ViewController::vc_fitToScreen, this, &NodeViewNew::fitToScreen);
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
    isAspectView = true;
    themeChanged();
}

void NodeViewNew::setContainedNodeViewItem(NodeViewItem *item)
{
    if(containedNodeViewItem){
        //Unregister
        disconnect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeViewNew::viewItem_LabelChanged);
        containedNodeViewItem->unregisterObject(this);
        if(!isAspectView){
            deleteLater();
        }
    }

    containedNodeViewItem = item;

    if(containedNodeViewItem){
        containedNodeViewItem->registerObject(this);

        connect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeViewNew::viewItem_LabelChanged);

        viewItem_LabelChanged(item->getData("label").toString());

        containedAspect = containedNodeViewItem->getViewAspect();

        if(!isAspectView){
            viewItem_Constructed(item);
            foreach(ViewItem* item, item->getChildren()){
                viewItem_Constructed(item);
            }
        }
    }
    clearSelection();
}

QColor NodeViewNew::getBackgroundColor()
{
    return backgroundColor;
}


QRectF NodeViewNew::getViewportRect()
{
    return viewportRect();
}

void NodeViewNew::resetMinimap()
{
    emit viewportChanged(viewportRect(), transform().m11());
    emit sceneRectChanged(currentSceneRect);
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
        if(item->scene()){
            scene()->removeItem(item);
        }

        delete item;
    }

    if(viewItem && containedNodeViewItem == viewItem){
        setContainedNodeViewItem(0);
    }
}

void NodeViewNew::selectionHandler_ItemSelectionChanged(ViewItem *item, bool selected)
{
    if(item){
        EntityItemNew* e = getEntityItem(item->getID());
        if(e){
            e->setSelected(selected);
        }
        if(item == containedNodeViewItem){
            isBackgroundSelected = selected;
            update();
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


void NodeViewNew::selectAll()
{
    _selectAll();
}

void NodeViewNew::itemsMoved()
{
    QRectF newSceneRect = getSceneBoundingRectOfItems(getTopLevelEntityItems());
    if(newSceneRect != currentSceneRect){
        currentSceneRect = newSceneRect;
        emit sceneRectChanged(currentSceneRect);
    }
}

void NodeViewNew::clearSelection()
{
    _clearSelection();
}

void NodeViewNew::themeChanged()
{

    if(isAspectView){
        backgroundColor = Theme::theme()->getAspectBackgroundColor(containedAspect);
    }else{
        backgroundColor = Theme::theme()->getAltBackgroundColor();
    }
    backgroundFontColor = backgroundColor.darker(110);


    QColor selectedColor = Qt::blue;

    //Merge to blue
    qreal ratio = 1;
    selectedBackgroundFontColor.setRed((ratio * backgroundFontColor.red() + ((1 - ratio) * selectedColor.red())));
    selectedBackgroundFontColor.setGreen((ratio *backgroundFontColor.green() + ((1 - ratio) * selectedColor.green()) ));
    selectedBackgroundFontColor.setBlue((ratio *backgroundFontColor.blue() + ((1 - ratio) * selectedColor.blue()) ));

    update();
}

void NodeViewNew::item_EditData(ViewItem *item, QString keyName)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
        emit editData(item->getID(), keyName);
    }
}

void NodeViewNew::item_RemoveData(ViewItem *item, QString keyName)
{
    if(item){
        emit removeData(item->getID(), keyName);
    }
}

void NodeViewNew::fitToScreen()
{
    centerOnItems(getTopLevelEntityItems());
}

void NodeViewNew::centerSelection()
{

    centerOnItems(getSelectedItems());
}

QList<int> NodeViewNew::getIDsInView()
{
    return guiItems.keys();
}

void NodeViewNew::test()
{

    //scene->setSceneRect(QRectF());
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
    if(item){
        int ID = item->getID();
        emit triggerAction("Expanding Selection");
        emit setData(ID, "isExpanded", expand);
    }
}

void NodeViewNew::item_SetCentered(EntityItemNew *item)
{
    centerRect(item->sceneViewRect());
}

void NodeViewNew::item_AdjustingPos(bool adjusting)
{
    if(selectionHandler){

        if(adjusting){
            emit triggerAction("Moving Selection");
        }

        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            EntityItemNew* item = getEntityItem(viewItem);
            if(!item){
                continue;
            }
            bool itemWasMoved = item->hasBeenMoved();
            item->setMoving(adjusting);
            if(!adjusting){
                int id = item->getID();
                QPointF pos = item->getCenter();

                bool sendRequest = itemWasMoved && !item->isIgnoringPosition();

                if(sendRequest){
                    emit setData(id, "x", pos.x());
                    emit setData(id, "y", pos.y());
                }
            }
        }
    }
}

void NodeViewNew::item_AdjustPos(QPointF delta)
{
    //Moves the selection.
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            EntityItemNew* item = getEntityItem(viewItem);
            if(item){
                delta = item->validateAdjustPos(delta);
                //If delta is 0,0 we should ignore.
                if(delta.isNull()){
                    break;
                }
            }
        }

        if(!delta.isNull()){
            foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
                EntityItemNew* item = getEntityItem(viewItem);
                if(item){
                    //Move!
                    item->adjustPos(delta);
                }
            }
        }
    }
}

void NodeViewNew::item_Resizing(bool resizing)
{
}

void NodeViewNew::item_ResizeFinished(NodeItemNew *item, RECT_VERTEX vertex)
{
    int id = item->getID();
    QSizeF size = item->getExpandedSize();
    emit triggerAction("Resizing Item");
    emit setData(id, "width", size.width());
    emit setData(id, "height", size.height());
}

void NodeViewNew::item_Resize(NodeItemNew *item, QSizeF delta, RECT_VERTEX vertex)
{
    QPointF offset(delta.width(), delta.height());

    if(vertex == RV_TOP || vertex == RV_BOTTOM){
        delta.setWidth(0);
        offset.setX(0);
    }else if(vertex == RV_LEFT || vertex == RV_RIGHT){
        delta.setHeight(0);
        offset.setY(0);
    }

    if(vertex == RV_TOP || vertex == RV_TOPLEFT || vertex == RV_TOPRIGHT){
        //Invert the H
        delta.rheight() *= -1;
    }
    if(vertex == RV_TOPLEFT || vertex == RV_LEFT || vertex == RV_BOTTOMLEFT){
        //Invert the W
        delta.rwidth() *= -1;
    }

    if(vertex == RV_BOTTOM || vertex == RV_BOTTOMLEFT || vertex == RV_BOTTOMRIGHT){
        //Ignore the delta Y
        offset.setY(0);
    }
    if(vertex == RV_RIGHT || vertex == RV_BOTTOMRIGHT || vertex == RV_TOPRIGHT){
        //Ignore the delta X
        offset.setX(0);
    }

    if(delta.width() == 0){
        offset.setX(0);
    }
    if(delta.height() == 0){
        offset.setY(0);
    }

    item->adjustPos(offset);
    item->adjustExpandedSize(delta);

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

void NodeViewNew::centerItem(int ID)
{
    EntityItemNew* item = getEntityItem(ID);
    if(item){
        QList<EntityItemNew*> items;
        items.append(item);
        centerOnItems(items);
    }
}

void NodeViewNew::setupConnections(EntityItemNew *item)
{
    connect(item, &EntityItemNew::req_activeSelected, this, &NodeViewNew::item_ActiveSelected);
    connect(item, &EntityItemNew::req_selected, this, &NodeViewNew::item_Selected);
    connect(item, &EntityItemNew::req_expanded, this, &NodeViewNew::item_SetExpanded);
    connect(item, &EntityItemNew::req_centerItem, this, &NodeViewNew::item_SetCentered);
    connect(item, &EntityItemNew::req_adjustPos, this, &NodeViewNew::item_AdjustPos);
    connect(item, &EntityItemNew::req_adjustingPos, this, &NodeViewNew::item_AdjustingPos);

    connect(item, &EntityItemNew::req_triggerAction, this, &NodeViewNew::triggerAction);
    connect(item, &EntityItemNew::req_removeData, this, &NodeViewNew::item_RemoveData);
    connect(item, &EntityItemNew::req_editData, this, &NodeViewNew::item_EditData);



    if(item->isNodeItem()){
        NodeItemNew* node = (NodeItemNew*) item;
        connect(node, &NodeItemNew::req_adjustSize, this, &NodeViewNew::item_Resize);
        connect(node, &NodeItemNew::req_adjustSizeFinished, this, &NodeViewNew::item_ResizeFinished);
    }
}


void NodeViewNew::centerOnItems(QList<EntityItemNew *> items)
{
    centerRect(getSceneBoundingRectOfItems(items));
}

QRectF NodeViewNew::getSceneBoundingRectOfItems(QList<EntityItemNew *> items)
{
    QRectF itemsRect;
    foreach(EntityItemNew* item, items){
        if(item->isVisible()){
            itemsRect = itemsRect.united(item->sceneViewRect());
        }
    }
    return itemsRect;
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

void NodeViewNew::viewItem_LabelChanged(QString label)
{
    backgroundText = label.toUpper();
    update();
}

QRectF NodeViewNew::viewportRect()
{
//    return mapToScene(rect()).boundingRect();
    return mapToScene(viewport()->rect()).boundingRect();
}

void NodeViewNew::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(!item || item->getViewAspect() != containedAspect){
        return;
    }

    NodeItemNew* parentNode = getParentNodeItem(item);

    if(!containedNodeViewItem && item->getViewAspect() == containedAspect){
        setContainedNodeViewItem(item);
        //Don't construct an aspect.
        return;
    }



    if(containedNodeViewItem){
        if(containedNodeViewItem->isAncestorOf(item)){
            int ID = item->getID();
            NodeItemNew* nodeItem =  0;
            Node::NODE_KIND nodeKind = item->getNodeKind();
            QString nodeKindStr = item->getData("kind").toString();

            //Ignore
            if(nodeKindStr.contains("DDS")){
                return;
            }

            bool ignorePosition = containedNodeViewItem == item;


            switch(nodeKind){
            case Node::NK_HARDWARE_NODE:
                nodeItem = new HardwareNodeItem(item, parentNode);
                break;
            case Node::NK_MANAGEMENT_COMPONENT:
                nodeItem = new ManagementComponentNodeItem(item, parentNode);
                break;
            case Node::NK_IDL:
            case Node::NK_COMPONENT:
            case Node::NK_COMPONENT_ASSEMBLY:
            case Node::NK_COMPONENT_INSTANCE:
            case Node::NK_COMPONENT_IMPL:
            case Node::NK_TERMINATION:
                nodeItem = new ContainerNodeItem(item, parentNode);
                break;
            case Node::NK_HARDWARE_CLUSTER:
                nodeItem = new ContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("ip_address");
                break;
            case Node::NK_INEVENTPORT_INSTANCE:
            case Node::NK_OUTEVENTPORT_INSTANCE:
            case Node::NK_INEVENTPORT_DELEGATE:
            case Node::NK_OUTEVENTPORT_DELEGATE:
                nodeItem = new ContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_CONDITION:
            case Node::NK_ATTRIBUTE_INSTANCE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("value");
                break;
            case Node::NK_AGGREGATE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
               // nodeItem->setSecondaryTextKey("value");
                break;
            case Node::NK_PROCESS:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("operation");
                break;
            case Node::NK_MEMBER_INSTANCE:
            case Node::NK_ATTRIBUTE:
            case Node::NK_VARIABLE:
            case Node::NK_MEMBER:
            case Node::NK_AGGREGATE_INSTANCE:
            case Node::NK_INEVENTPORT_IMPL:
            case Node::NK_OUTEVENTPORT_IMPL:
            case Node::NK_INEVENTPORT:
            case Node::NK_ATTRIBUTE_IMPL:
            case Node::NK_INPUTPARAMETER:
            case Node::NK_RETURNPARAMETER:
            case Node::NK_OUTEVENTPORT:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_PERIODICEVENT:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("frequency");
                break;
            case Node::NK_BRANCH_STATE:
            case Node::NK_WHILELOOP:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                break;
            case Node::NK_WORKLOAD:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                break;
            default:
                nodeItem = new DefaultNodeItem(item, parentNode);
                break;
            }

            if(nodeItem){

                if(ignorePosition){
                    nodeItem->setIgnorePosition(true);
                }

                guiItems[ID] = nodeItem;

                setupConnections(nodeItem);


                if(!scene()->items().contains(nodeItem)){
                    scene()->addItem(nodeItem);
                    topLevelGUIItemIDs.append(ID);

                    connect(nodeItem, SIGNAL(positionChanged()), this, SLOT(itemsMoved()));
                    connect(nodeItem, SIGNAL(sizeChanged()), this, SLOT(itemsMoved()));

                }
            }
        }
    }
}

void NodeViewNew::edgeViewItem_Constructed(EdgeViewItem *item)
{
    if(!item || !containedNodeViewItem || !containedNodeViewItem->isAncestorOf(item->getParentItem())){
        return;
    }

    NodeItemNew* parent = getParentNodeItem(item->getParentItem());
    NodeItemNew* source = getParentNodeItem(item->getSource());
    NodeItemNew* destination = getParentNodeItem(item->getDestination());

    if(source && destination){
        EdgeItemNew* edgeItem = new EdgeItemNew(item, parent,source,destination);


        if(edgeItem){
            guiItems[item->getID()] = edgeItem;

            setupConnections(edgeItem);

            if(!scene()->items().contains(edgeItem)){
                scene()->addItem(edgeItem);
            }
        }
    }
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
        EntityItemNew* item = guiItems.value(ID, 0);
        if(item){
            items.append(item);
        }
    }
    return items;
}

QList<EntityItemNew *> NodeViewNew::getSelectedItems()
{
    QList<EntityItemNew*> items;
    foreach(ViewItem* item, selectionHandler->getSelection()){
        EntityItemNew* eItem = getEntityItem(item);
        if(eItem){
            items.append(eItem);
        }
    }
    return items;
}

NodeItemNew *NodeViewNew::getParentNodeItem(NodeViewItem *item)
{
     while(item){
        int ID = item->getID();
        if(guiItems.contains(ID)){
            return (NodeItemNew*)guiItems[ID];
        }else{
            item = item->getParentNodeViewItem();
        }
     }
     return 0;
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
    QPointF anchorScenePos;

    if(!topLevelGUIItemIDs.isEmpty()){
        anchorScenePos = getScenePosOfPoint(anchorScreenPos);
    }

    //Calculate the zoom change.
    qreal scaleFactor = pow(ZOOM_INCREMENTOR, (delta / abs(delta)));
    if(scaleFactor != 1){
        scale(scaleFactor, scaleFactor);

        if(!topLevelGUIItemIDs.isEmpty()){
            QPointF delta = getScenePosOfPoint(anchorScreenPos) - anchorScenePos;
            translate(delta);
        }
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

    //Check for aspect selection.
    if(selectionHandler->getSelection().contains(containedNodeViewItem)){
        itemsToSelect.append(containedNodeViewItem);
    }

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


void NodeViewNew::_selectAll()
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

void NodeViewNew::_clearSelection()
{
    if(selectionHandler){
        //Depending on the type of NodeView we are.
        if(isAspectView && containedNodeViewItem){
            //If we are the aspect select the aspect.
            selectionHandler->toggleItemsSelection(containedNodeViewItem);
        }else{
            //If we aren't an aspect clear the selection.
            selectionHandler->clearSelection();
        }
    }
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

EntityItemNew *NodeViewNew::getEntityAtPos(QPointF scenePos)
{
    EntityItemNew* entityItem = 0;
    QGraphicsItem* item = scene()->itemAt(scenePos, transform());
    if(item){
        entityItem = qgraphicsitem_cast<EntityItemNew*>(item);
    }
    return entityItem;
}

void NodeViewNew::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        setState(VS_RUBBERBAND);
    }
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
    if(viewController->isModelReady()){
        zoom(event->delta(), event->pos());
    }
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
            EntityItemNew* item = getEntityAtPos(scenePos);
            if(!item){
                clearSelection();
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
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    switch(viewState){
    case VS_PAN:{
        //Do Nothing
    }
    case VS_PANNING:{
        setState(VS_NONE);
        if(pan_distance < 10){
            QPointF itemPos = mapToScene(event->pos());
            EntityItemNew* item = getEntityAtPos(itemPos);
            if(item){
                itemPos = item->mapFromScene(itemPos);
                if(!item->isSelected()){
                    selectionHandler->toggleItemsSelection(item->getViewItem(), CONTROL);
                }
            }
            //Check for item under mouse.
            emit toolbarRequested(event->globalPos(), itemPos);
            return;
        }
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
    painter->setBrush(backgroundColor);
    painter->drawRect(rect());

    if(backgroundText != ""){
        painter->setFont(backgroundFont);
        if(isBackgroundSelected){
            painter->setPen(selectedBackgroundFontColor);
        }else{
            painter->setPen(backgroundFontColor);
        }
        painter->drawText(rect(), Qt::AlignHCenter | Qt::AlignBottom, backgroundText);
    }
}

void NodeViewNew::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    viewportChanged();
}

