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
    setupStateMachine();
    QRectF sceneRect;
    sceneRect.setSize(QSize(10000,10000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);

    connectLineItem = 0;
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


    isPanning = false;
    viewController = 0;
    selectionHandler = 0;
    containedAspect = VA_NONE;
    containedNodeViewItem = 0;
    isAspectView = false;

    backgroundFont.setPixelSize(70);
    //backgroundFont.setBold(true);


    rubberband = new QRubberBand(QRubberBand::Rectangle, this);


    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();
}

NodeViewNew::~NodeViewNew()
{
    if(containedNodeViewItem){
        QList<ViewItem*> items = containedNodeViewItem->getNestedChildren();
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
        connect(viewController, &ViewController::vc_centerConnections, this, &NodeViewNew::centerConnections);


        connect(viewController, &ViewController::vc_highlightItem, this, &NodeViewNew::highlightItem);
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
            foreach(ViewItem* item, item->getNestedChildren()){
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

void NodeViewNew::alignHorizontal()
{
    emit triggerAction("Aligning Selection Horizontally");

    QList<EntityItemNew*> selection = getOrderedSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItemNew* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItemNew* parent = item->getParent();
        if(!parent){
            parent = item;
        }

        pos.setY(parent->mapFromScene(sceneRect.topLeft()).y());
        pos.ry() += item->getTopLeftOffset().y();
        item->setPos(pos);

        if(item->setMoveFinished()){
            pos = item->getNearestGridPoint();
            emit setData(item->getID(), "x", pos.x());
            emit setData(item->getID(), "y", pos.y());
        }
    }
}

void NodeViewNew::alignVertical()
{
    emit triggerAction("Aligning Selection Vertically");

    QList<EntityItemNew*> selection = getOrderedSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItemNew* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItemNew* parent = item->getParent();
        if(!parent){
            parent = item;
        }
        pos.setX(parent->mapFromScene(sceneRect.topLeft()).x());
        pos.rx() += item->getTopLeftOffset().x();
        item->setPos(pos);

        if(item->setMoveFinished()){
            pos = item->getNearestGridPoint();
            emit setData(item->getID(), "x", pos.x());
            emit setData(item->getID(), "y", pos.y());
        }
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

void NodeViewNew::node_ConnectMode(NodeItemNew *item)
{
    if(selectionHandler && selectionHandler->getSelectionCount() == 1){
        if(item->getViewItem() == selectionHandler->getActiveSelectedItem()){
            emit trans_InActive2Connecting();
        }
    }
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

void NodeViewNew::centerConnections(ViewItem* item)
{
    if(item){
        QList<EdgeViewItem*> edges;
        if(item->isNode()){
            edges = ((NodeViewItem*)item)->getEdges();
        }else if(item->isEdge()){
            edges.append((EdgeViewItem*)item);
        }

        QList<ViewItem*> toSelect;
        QList<EntityItemNew*> toCenter;

        foreach(EdgeViewItem* e, edges){
            ViewItem* s = e->getSource();
            ViewItem* d = e->getDestination();

            EntityItemNew* src = getEntityItem(s);
            EntityItemNew* dst = getEntityItem(d);
            EntityItemNew* edge = getEntityItem(e);

            if(src && !toSelect.contains(s)){
                toCenter.append(src);
                toSelect.append(s);
            }

            if(dst && !toSelect.contains(d)){
                toCenter.append(dst);
                toSelect.append(d);
            }

            if(edge && !toSelect.contains(e)){
                toCenter.append(edge);
                toSelect.append(e);
            }
        }
        if(!toSelect.isEmpty()){
            if(selectionHandler){
                selectionHandler->toggleItemsSelection(toSelect);
            }
            centerOnItems(toCenter);
        }else{
            clearSelection();
        }
    }
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

void NodeViewNew::item_MoveSelection(QPointF delta)
{
    //Only when we are in the moving state.
    if(state_Active_Moving->active()){
        //Moves the selection.
        if(selectionHandler){

            //Validate the move for the entire selection.
            foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
                EntityItemNew* item = getEntityItem(viewItem);
                if(item){
                    delta = item->validateMove(delta);
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
}

void NodeViewNew::item_Resize(NodeItemNew *item, QSizeF delta, RECT_VERTEX vertex)
{
    if(state_Active_Resizing->active()){

        if(vertex == RV_TOP || vertex == RV_BOTTOM){
            delta.setWidth(0);
        }else if(vertex == RV_LEFT || vertex == RV_RIGHT){
            delta.setHeight(0);
        }

        if(vertex == RV_TOP || vertex == RV_TOPLEFT || vertex == RV_TOPRIGHT){
            //Invert the H
            delta.rheight() *= -1;
        }
        if(vertex == RV_TOPLEFT || vertex == RV_LEFT || vertex == RV_BOTTOMLEFT){
            //Invert the W
            delta.rwidth() *= -1;
        }

        QSizeF preSize = item->getExpandedSize();
        item->adjustExpandedSize(delta);
        QSizeF postSize = item->getExpandedSize();
        if(preSize != postSize){
            QSizeF deltaSize = preSize - postSize;
            QPointF offset(deltaSize.width(), deltaSize.height());

            if(vertex == RV_BOTTOM || vertex == RV_BOTTOMLEFT || vertex == RV_BOTTOMRIGHT){
                //Ignore the delta Y
                offset.setY(0);
            }
            if(vertex == RV_RIGHT || vertex == RV_BOTTOMRIGHT || vertex == RV_TOPRIGHT){
                //Ignore the delta X
                offset.setX(0);
            }
            item->adjustPos(offset);
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

void NodeViewNew::centerItem(int ID)
{
    EntityItemNew* item = getEntityItem(ID);
    if(item){
        QList<EntityItemNew*> items;
        items.append(item);
        centerOnItems(items);
    }
}

void NodeViewNew::highlightItem(int ID, bool highlighted)
{
    EntityItemNew* item = getEntityItem(ID);
    if(item){
        item->setHighlighted(highlighted);
    }
}

void NodeViewNew::setupConnections(EntityItemNew *item)
{
    connect(item, &EntityItemNew::req_activeSelected, this, &NodeViewNew::item_ActiveSelected);
    connect(item, &EntityItemNew::req_selected, this, &NodeViewNew::item_Selected);
    connect(item, &EntityItemNew::req_expanded, this, &NodeViewNew::item_SetExpanded);
    connect(item, &EntityItemNew::req_centerItem, this, &NodeViewNew::item_SetCentered);

    connect(item, &EntityItemNew::req_StartMove, this, &NodeViewNew::trans_InActive2Moving);
    connect(item, &EntityItemNew::req_Move, this, &NodeViewNew::item_MoveSelection);
    connect(item, &EntityItemNew::req_FinishMove, this, &NodeViewNew::trans_Moving2InActive);


    connect(item, &EntityItemNew::req_triggerAction, this, &NodeViewNew::triggerAction);
    connect(item, &EntityItemNew::req_removeData, this, &NodeViewNew::item_RemoveData);
    connect(item, &EntityItemNew::req_editData, this, &NodeViewNew::item_EditData);



    if(item->isNodeItem()){
        NodeItemNew* node = (NodeItemNew*) item;

        connect(node, &NodeItemNew::req_StartResize, this, &NodeViewNew::trans_InActive2Resizing);
        connect(node, &NodeItemNew::req_Resize, this, &NodeViewNew::item_Resize);
        connect(node, &NodeItemNew::req_FinishResize, this, &NodeViewNew::trans_Resizing2InActive);

        //connect(node, &NodeItemNew::req_adjustSize, this, &NodeViewNew::item_Resize);
        //connect(node, &NodeItemNew::req_adjustSizeFinished, this, &NodeViewNew::item_ResizeFinished);
        connect(node, &NodeItemNew::req_connectMode, this, &NodeViewNew::node_ConnectMode);


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
    if(rectScene.isValid()){
        //Inflate by 110%
        QRectF visibleRect = viewportRect();
        qreal widthRatio = visibleRect.width() / (rectScene.width() * 1.1);
        qreal heightRatio = visibleRect.height() / (rectScene.height() * 1.1);

        qreal scaleRatio = qMin(widthRatio, heightRatio);

        //Change the scale.
        scale(scaleRatio, scaleRatio);
        centerView(rectScene.center());
    }
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
            //qCritical() << "CONSTRUCTING: " << item;
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
                nodeItem = new ContainerNodeItem(item, parentNode);
                break;
            case Node::NK_TERMINATION:
                nodeItem = new ContainerNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
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
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_ASSEMBLY);
                break;
            case Node::NK_CONDITION:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_ATTRIBUTE_INSTANCE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_AGGREGATE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                break;
            case Node::NK_PROCESS:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("worker");
                break;
            case Node::NK_MEMBER_INSTANCE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_VARIABLE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_ATTRIBUTE_IMPL:
            case Node::NK_AGGREGATE_INSTANCE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                break;
            case Node::NK_MEMBER:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_INEVENTPORT_IMPL:
            case Node::NK_OUTEVENTPORT_IMPL:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_ATTRIBUTE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                break;
            case Node::NK_INPUTPARAMETER:
            case Node::NK_RETURNPARAMETER:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_INEVENTPORT:
            case Node::NK_OUTEVENTPORT:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                break;
            case Node::NK_PERIODICEVENT:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("frequency");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_BRANCH_STATE:
            case Node::NK_WHILELOOP:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_WORKLOAD:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_WORKFLOW);
                break;
            case Node::NK_VECTOR:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("max_size");
                break;
            case Node::NK_VECTOR_INSTANCE:
                nodeItem = new StackContainerNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(Edge::EC_DATA);
                nodeItem->setSecondaryTextKey("type");
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

    switch(item->getEdgeKind()){
        case Edge::EC_ASSEMBLY:
        case Edge::EC_DATA:
        case Edge::EC_WORKFLOW:
            break;
        default:
            return;
    }

    if(!containedNodeViewItem || !containedNodeViewItem->isAncestorOf(item->getParentItem())){
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

QList<ViewItem *> NodeViewNew::getTopLevelViewItems() const
{
    QList<ViewItem *> items;
    foreach(EntityItemNew* item, getTopLevelEntityItems()){
        items.append(item->getViewItem());
    }
    return items;
}

QList<EntityItemNew *> NodeViewNew::getTopLevelEntityItems() const
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

QList<EntityItemNew *> NodeViewNew::getSelectedItems() const
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

QList<EntityItemNew *> NodeViewNew::getOrderedSelectedItems() const
{
    QList<EntityItemNew*> items;
    foreach(ViewItem* item, selectionHandler->getOrderedSelection()){
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

EntityItemNew *NodeViewNew::getEntityItem(int ID) const
{
    EntityItemNew* item = 0;
    if(guiItems.contains(ID)){
        item = guiItems[ID];
    }
    return item;
}

EntityItemNew *NodeViewNew::getEntityItem(ViewItem *item) const
{
    EntityItemNew* e = 0;
    if(item){
        e = getEntityItem(item->getID());
    }
    return e;
}

NodeItemNew *NodeViewNew::getNodeItem(ViewItem *item) const
{
    EntityItemNew* e = getEntityItem(item->getID());
    if(e && e->isNodeItem()){
        return (NodeItemNew*) e;
    }
    return 0;
}

void NodeViewNew::zoom(int delta, QPoint anchorScreenPos)
{
    if(delta != 0){
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


qreal NodeViewNew::distance(QPoint p1, QPoint p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void NodeViewNew::setupStateMachine()
{
    viewStateMachine = new QStateMachine(this);

    state_InActive = new QState();

    state_Active_Moving = new QState();
    state_Active_Resizing = new QState();
    state_Active_RubberbandMode = new QState();
    state_Active_RubberbandMode_Selecting = new QState();
    state_Active_Connecting = new QState();

    //Add States
    viewStateMachine->addState(state_InActive);
    viewStateMachine->addState(state_Active_Moving);
    viewStateMachine->addState(state_Active_Resizing);
    viewStateMachine->addState(state_Active_RubberbandMode);
    viewStateMachine->addState(state_Active_RubberbandMode_Selecting);
    viewStateMachine->addState(state_Active_Connecting);

    viewStateMachine->setInitialState(state_InActive);

    //Setup Transitions
    state_InActive->addTransition(this, &NodeViewNew::trans_InActive2Moving, state_Active_Moving);
    state_Active_Moving->addTransition(this, &NodeViewNew::trans_Moving2InActive, state_InActive);

    state_InActive->addTransition(this, &NodeViewNew::trans_InActive2RubberbandMode, state_Active_RubberbandMode);
    state_Active_RubberbandMode->addTransition(this, &NodeViewNew::trans_RubberbandMode2InActive, state_InActive);

    state_Active_RubberbandMode->addTransition(this, &NodeViewNew::trans_RubberbandMode2RubberbandMode_Selecting, state_Active_RubberbandMode_Selecting);
    state_Active_RubberbandMode_Selecting->addTransition(this, &NodeViewNew::trans_RubberbandMode2InActive, state_Active_RubberbandMode);

    state_InActive->addTransition(this, &NodeViewNew::trans_InActive2Resizing, state_Active_Resizing);
    state_Active_Resizing->addTransition(this, &NodeViewNew::trans_Resizing2InActive, state_InActive);


    state_InActive->addTransition(this, &NodeViewNew::trans_InActive2Connecting, state_Active_Connecting);
    state_Active_Connecting->addTransition(this, &NodeViewNew::trans_Connecting2InActive, state_InActive);

    //Connect to states.

    connect(state_InActive, &QState::entered, this, &NodeViewNew::state_Default_Entered);


    connect(state_Active_Moving, &QState::entered, this, &NodeViewNew::state_Moving_Entered);
    connect(state_Active_Moving, &QState::exited, this, &NodeViewNew::state_Moving_Exited);

    connect(state_Active_Resizing, &QState::entered, this, &NodeViewNew::state_Resizing_Entered);
    connect(state_Active_Resizing, &QState::exited, this, &NodeViewNew::state_Resizing_Exited);

    connect(state_Active_RubberbandMode, &QState::entered, this, &NodeViewNew::state_RubberbandMode_Entered);
    connect(state_Active_RubberbandMode, &QState::exited, this, &NodeViewNew::state_RubberbandMode_Exited);

    connect(state_Active_RubberbandMode_Selecting, &QState::entered, this, &NodeViewNew::state_RubberbandMode_Selecting_Entered);
    connect(state_Active_RubberbandMode_Selecting, &QState::exited, this, &NodeViewNew::state_RubberbandMode_Selecting_Exited);

    connect(state_Active_Connecting, &QState::entered, this, &NodeViewNew::state_Connecting_Entered);
    connect(state_Active_Connecting, &QState::exited, this, &NodeViewNew::state_Connecting_Exited);

    viewStateMachine->start();
}

EntityItemNew *NodeViewNew::getEntityAtPos(QPointF scenePos)
{
    foreach(QGraphicsItem* item, scene()->items(scenePos)){
        EntityItemNew* entityItem =  dynamic_cast<EntityItemNew*>(item);
        if(entityItem){
            return entityItem;
        }
    }
    return 0;
}

void NodeViewNew::state_Moving_Entered()
{
    setCursor(Qt::SizeAllCursor);
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            EntityItemNew* item = getEntityItem(viewItem);
            if(item){
                item->setMoveStarted();
            }
        }
    }
}

void NodeViewNew::state_Moving_Exited()
{
    if(selectionHandler){
        bool anyMoved = false;

        QVector<ViewItem*> selection = selectionHandler->getOrderedSelection();

        foreach(ViewItem* viewItem, selection){
            EntityItemNew* item = getEntityItem(viewItem);
            if(item){
                if(item->setMoveFinished()){
                    anyMoved = true;
                }
            }
        }

        if(anyMoved){
            emit triggerAction("Moving Selection");
            foreach(ViewItem* viewItem, selection){
                EntityItemNew* item = getEntityItem(viewItem);
                if(item){
                    QPointF pos = item->getNearestGridPoint();
                    emit setData(item->getID(), "x", pos.x());
                    emit setData(item->getID(), "y", pos.y());
                }
            }
        }
    }
}

void NodeViewNew::state_Resizing_Entered()
{
    qCritical() << "state_Resizing_Entered";
    if(selectionHandler){
        if(selectionHandler->getSelectionCount() != 1){
            emit trans_Resizing2InActive();
            return;
        }

        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            NodeItemNew* item = getNodeItem(viewItem);
            if(item){
                item->setResizeStarted();
            }
        }
        setCursor(Qt::SizeAllCursor);
    }
}

void NodeViewNew::state_Resizing_Exited()
{
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getOrderedSelection()){
            NodeItemNew* item = getNodeItem(viewItem);

            if(item && item->setResizeFinished()){
                emit triggerAction("Resizing Item");
                QSizeF size = item->getGridAlignedSize();
                emit setData(item->getID(), "width", size.width());
                emit setData(item->getID(), "height", size.height());
            }
        }
    }
}

void NodeViewNew::state_RubberbandMode_Entered()
{
    qCritical() << "state_RubberbandMode_Entered";
    setCursor(Qt::CrossCursor);
}

void NodeViewNew::state_RubberbandMode_Exited()
{
    qCritical() << "state_RubberbandMode_Exited";
}

void NodeViewNew::state_RubberbandMode_Selecting_Entered()
{
    qCritical() << "state_RubberbandMode_Selecting_Entered";
    rubberband->setVisible(true);
}

void NodeViewNew::state_RubberbandMode_Selecting_Exited()
{
     qCritical() << "state_RubberbandMode_Selecting_Exited";
    rubberband->setVisible(false);
    emit trans_RubberbandMode2InActive();
}

void NodeViewNew::state_Connecting_Entered()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItemNew* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItemNew* nodeItem = (NodeItemNew*) item;

        //Highlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(nodeItem->getVisualEdgeKind());
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), true);
        }

        QPointF lineStart = nodeItem->scenePos();
        lineStart += nodeItem->getElementRect(EntityItemNew::ER_EDGE_KIND_ICON).center();

        if(!connectLineItem){
            connectLineItem = scene()->addLine(connectLine);
        }
        connectLine.setP1(lineStart);
        connectLine.setP2(lineStart);
        connectLineItem->setLine(connectLine);
        connectLineItem->setVisible(true);
    }
    qCritical() << "state_Connecting_Entered";
}

void NodeViewNew::state_Connecting_Exited()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItemNew* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItemNew* nodeItem = (NodeItemNew*) item;

        Edge::EDGE_KIND edgeKind = nodeItem->getVisualEdgeKind();

        //Unhighlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(edgeKind);
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), false);
        }

        if(connectLineItem){
            QPointF scenePos = connectLine.p2();
            EntityItemNew* otherItem = getEntityAtPos(scenePos);
            if(otherItem){
                emit triggerAction("Constructing Edge");
                emit viewController->vc_constructEdge(selectionHandler->getSelectionIDs().toList(), otherItem->getID(), edgeKind);
            }
            connectLineItem->setVisible(false);
        }
    }
}

void NodeViewNew::state_Default_Entered()
{
    qCritical() << "state_Default_Entered";
    unsetCursor();
}

void NodeViewNew::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        emit trans_InActive2RubberbandMode();
    }
}

void NodeViewNew::keyReleaseEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(!(CONTROL && SHIFT)){
        emit trans_RubberbandMode2InActive();
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
        isPanning = true;
        pan_lastPos = event->pos();
        pan_lastScenePos = scenePos;
        pan_distance = 0;
        handledEvent = true;
    }

    if(event->button() == Qt::LeftButton){
        emit trans_RubberbandMode2RubberbandMode_Selecting();

        if(state_Active_RubberbandMode_Selecting->active()){
            rubberband_lastPos = event->pos();
            if(rubberband){
                rubberband->setGeometry(QRect(rubberband_lastPos, rubberband_lastPos));
            }
            handledEvent = true;
        }else{
            EntityItemNew* item = getEntityAtPos(scenePos);
            if(!item){
                clearSelection();
                handledEvent = true;
            }
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

    if(isPanning){
        //Calculate the distance in screen pixels travelled
        pan_distance += distance(event->pos(), pan_lastPos);
        //Pan the Canvas
        translate(scenePos - pan_lastScenePos);
        pan_lastPos = event->pos();
        pan_lastScenePos = mapToScene(event->pos());
        handledEvent = true;
    }

    if(state_Active_RubberbandMode_Selecting->active()){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        handledEvent = true;
    }else if(state_Active_Connecting->active()){
        connectLine.setP2(mapToScene(event->pos()));
        connectLineItem->setLine(connectLine);
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

    bool handledEvent = false;

    //Exit pan mode yo
    if(isPanning && event->button() == Qt::RightButton){
        isPanning = false;

        //Popup Toolbar if there is an item.
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
        }
        handledEvent = true;
    }

    if(state_Active_RubberbandMode_Selecting->active() && event->button() == Qt::LeftButton){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        selectItemsInRubberband();
        emit trans_RubberbandMode2InActive();
        handledEvent = true;
    }

    if(state_Active_Connecting->active() && event->button() == Qt::LeftButton){
        emit trans_Connecting2InActive();
        handledEvent = true;
    }


    if(!handledEvent){
        QGraphicsView::mouseReleaseEvent(event);
    }
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

