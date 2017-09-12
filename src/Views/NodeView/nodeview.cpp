#include "nodeview.h"

#include <QDebug>
#include <QtMath>
#include <QTimer>
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QDateTime>
#include <QOpenGLWidget>
#include "../ContextMenu/contextmenu.h"

#include "SceneItems/Node/defaultnodeitem.h"
#include "SceneItems/Node/stacknodeitem.h"
#include "SceneItems/Node/managementcomponentnodeitem.h"
#include "SceneItems/Node/hardwarenodeitem.h"

#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "SceneItems/Edge/edgeitem.h"
#include "SceneItems/Edge/edgeitem.h"
#include "../../theme.h"

#include "../../Controllers/NotificationManager/notificationobject.h"


#define ZOOM_INCREMENTOR 1.05

NodeView::NodeView(QWidget* parent):QGraphicsView(parent)
{
    setupStateMachine();
    
    QRectF sceneRect;
    sceneRect.setSize(QSize(10000,10000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);

    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setAcceptDrops(true);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);


    //Turn off the Scroll bars.
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //set the background font
    background_font.setPixelSize(70);
    setFont(background_font);

    rubberband = new QRubberBand(QRubberBand::Rectangle, this);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));

    themeChanged();

    connect(WindowManager::manager(), &WindowManager::activeViewDockWidgetChanged, this, &NodeView::activeViewDockChanged);
    connect(NotificationManager::manager(), &NotificationManager::notificationAdded, this, &NodeView::notification_Added);
    connect(NotificationManager::manager(), &NotificationManager::notificationDeleted, this, &NodeView::notification_Destructed);
}


NodeView::~NodeView()
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

void NodeView::setViewController(ViewController *viewController)
{
    this->viewController = viewController;
    if(viewController){

        connect(viewController, &ViewController::vc_viewItemConstructed, this, &NodeView::viewItem_Constructed);
        connect(viewController, &ViewController::vc_viewItemDestructing, this, &NodeView::viewItem_Destructed);

        selectionHandler = viewController->getSelectionController()->constructSelectionHandler(this);
        connect(selectionHandler, &SelectionHandler::itemSelectionChanged, this, &NodeView::selectionHandler_ItemSelectionChanged);
        connect(selectionHandler, &SelectionHandler::itemActiveSelectionChanged, this, &NodeView::selectionHandler_ItemActiveSelectionChanged);

        connect(this, &NodeView::toolbarRequested, viewController, &ViewController::vc_showToolbar);
        connect(this, &NodeView::triggerAction, viewController, &ViewController::vc_triggerAction);
        connect(this, &NodeView::setData, viewController, &ViewController::vc_setData);
        connect(this, &NodeView::removeData, viewController, &ViewController::vc_removeData);
        connect(this, &NodeView::editData, viewController, &ViewController::vc_editTableCell);




        connect(viewController, &ViewController::vc_centerItem, this, &NodeView::centerItem);
        connect(viewController, &ViewController::vc_fitToScreen, this, &NodeView::fitToScreen);
        connect(viewController, &ViewController::vc_selectAndCenterConnectedEntities, this, &NodeView::centerConnections);


        connect(viewController, &ViewController::vc_highlightItem, this, &NodeView::highlightItem);
    }
}

void NodeView::translate(QPointF point)
{
    QGraphicsView::translate(point.x(), point.y());
}

void NodeView::scale(qreal sx, qreal sy)
{
    if(sx != 1 || sy != 1){
        auto t = transform();
        auto zoom = t.m11() * sx;

        //Limit to zoom 25% between 400%
        zoom = qMax(0.25, zoom);
        zoom = qMin(zoom, 4.0);

        //m11 and m22 are x/y scaling respectively
        t.setMatrix(zoom, t.m12(), t.m13(), t.m21(), zoom, t.m23(), t.m31(), t.m32(), t.m33());
        setTransform(t);
    }
}

void NodeView::setContainedViewAspect(VIEW_ASPECT aspect)
{
    containedAspect = aspect;
    isAspectView = true;
    themeChanged();
}

void NodeView::setContainedNodeViewItem(NodeViewItem *item)
{
    if(containedNodeViewItem){
        //Unregister
        disconnect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);
        containedNodeViewItem->unregisterObject(this);
        if(!isAspectView){
            deleteLater();
        }
    }

    containedNodeViewItem = item;

    if(containedNodeViewItem){
        containedNodeViewItem->registerObject(this);

        connect(containedNodeViewItem, &NodeViewItem::labelChanged, this, &NodeView::viewItem_LabelChanged);

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

ViewItem *NodeView::getContainedViewItem()
{
    return containedNodeViewItem;
}

QColor NodeView::getBackgroundColor()
{
    return background_color;
}


QRectF NodeView::getViewportRect()
{
    return viewportRect();
}


void NodeView::viewItem_Constructed(ViewItem *item)
{
    if(item){
        if(item->isNode()){
            nodeViewItem_Constructed((NodeViewItem*)item);
        }else if(item->isEdge()){
            edgeViewItem_Constructed((EdgeViewItem*)item);
        }
    }
}

void NodeView::viewItem_Destructed(int ID, ViewItem *viewItem)
{


    EntityItem* item = getEntityItem(ID);
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

void NodeView::selectionHandler_ItemSelectionChanged(ViewItem *item, bool selected)
{
    if(item){
        EntityItem* e = getEntityItem(item->getID());
        if(e){
            e->setSelected(selected);
        }
    }
}

void NodeView::selectionHandler_ItemActiveSelectionChanged(ViewItem *item, bool isActive)
{
    if(item){
        EntityItem* e = getEntityItem(item->getID());
        if(e){
            e->setActiveSelected(isActive);
        }
    }
}


void NodeView::selectAll()
{
    _selectAll();
}


void NodeView::alignHorizontal()
{
    emit triggerAction("Aligning Selection Horizontally");

    QList<EntityItem*> selection = getSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItem* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItem* parent = item->getParent();
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

void NodeView::alignVertical()
{
    emit triggerAction("Aligning Selection Vertically");

    QList<EntityItem*> selection = getSelectedItems();
    QRectF sceneRect = getSceneBoundingRectOfItems(selection);

    foreach(EntityItem* item, selection){
        item->setMoveStarted();
        QPointF pos = item->getPos();

        EntityItem* parent = item->getParent();
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

void NodeView::clearSelection()
{
    _clearSelection();
}

void NodeView::themeChanged()
{

    if(isAspectView){
        background_color = Theme::theme()->getAspectBackgroundColor(containedAspect);
    }else{
        background_color = Theme::theme()->getAltBackgroundColor();
    }
    background_text_color = background_color.darker(110);
    setBackgroundBrush(background_color);
}

void NodeView::node_ConnectMode(NodeItem *item)
{
    if(selectionHandler && selectionHandler->getSelectionCount() == 1){
        if(item->getViewItem() == selectionHandler->getActiveSelectedItem()){
            emit trans_InActive2Connecting();
        }
    }
}

void NodeView::node_ConnectEdgeMode(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction){
    auto global_pos = mapToGlobal(mapFromScene(scene_pos));
    qCritical() << global_pos;
    viewController->getContextMenu()->popup_edge_menu(global_pos, kind, direction);
}

void NodeView::node_PopOutRelatedNode(NodeViewItem *item, NODE_KIND kind)
{
    //Get the edge
    for(auto edge: item->getEdges()){
        auto src = edge->getSource();
        auto dst = edge->getDestination();
        if(src->getNodeKind() == kind){
            viewController->popupItem(src->getID());
            return;
        }else if(dst->getNodeKind() == kind){
            viewController->popupItem(dst->getID());
            return;
        }
    }
}

void NodeView::item_EditData(ViewItem *item, QString keyName)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
        emit editData(item->getID(), keyName);
    }
}

void NodeView::item_RemoveData(ViewItem *item, QString keyName)
{
    if(item){
        emit removeData(item->getID(), keyName);
    }
}

void NodeView::fitToScreen()
{
    centerOnItems(getTopLevelEntityItems());
}


void NodeView::centerSelection()
{

    centerOnItems(getSelectedItems());
}

void NodeView::centerConnections(ViewItem* item)
{
    if(item){
        QList<EdgeViewItem*> edges;
        if(item->isNode()){
            edges = ((NodeViewItem*)item)->getEdges();
        }else if(item->isEdge()){
            edges.append((EdgeViewItem*)item);
        }

        QList<ViewItem*> toSelect;
        QList<EntityItem*> toCenter;

        foreach(EdgeViewItem* e, edges){
            ViewItem* s = e->getSource();
            ViewItem* d = e->getDestination();

            EntityItem* src = getEntityItem(s);
            EntityItem* dst = getEntityItem(d);
            EntityItem* edge = getEntityItem(e);

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

QList<int> NodeView::getIDsInView()
{
    return guiItems.keys();
}


void NodeView::item_Selected(ViewItem *item, bool append)
{
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(item, append);
    }
}

void NodeView::item_ActiveSelected(ViewItem *item)
{
    if(selectionHandler){
        selectionHandler->setActiveSelectedItem(item);
    }
}

void NodeView::item_SetExpanded(EntityItem *item, bool expand)
{
    if(item){
        int ID = item->getID();
        emit triggerAction("Expanding Selection");
        emit setData(ID, "isExpanded", expand);
    }
}

void NodeView::item_SetCentered(EntityItem *item)
{
    centerRect(item->sceneViewRect());
}

void NodeView::item_MoveSelection(QPointF delta)
{
    //Only when we are in the moving state.
    if(state_Active_Moving->active()){
        //Moves the selection.
        if(selectionHandler){

            //Validate the move for the entire selection.
            foreach(ViewItem* viewItem, selectionHandler->getSelection()){
                EntityItem* item = getEntityItem(viewItem);
                if(item){
                    delta = item->validateMove(delta);
                    //If delta is 0,0 we should ignore.
                    if(delta.isNull()){
                        break;
                    }
                }
            }

            if(!delta.isNull()){
                foreach(ViewItem* viewItem, selectionHandler->getSelection()){
                    EntityItem* item = getEntityItem(viewItem);
                    if(item){
                        //Move!
                        item->adjustPos(delta);
                    }
                }
            }
        }
    }
}

void NodeView::item_Resize(NodeItem *item, QSizeF delta, RECT_VERTEX vertex)
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


void NodeView::minimap_Pan(QPointF delta)
{
    translate(delta);
}

void NodeView::minimap_Zoom(int delta)
{
    zoom(delta);
}

void NodeView::centerItem(int ID)
{
    EntityItem* item = getEntityItem(ID);
    if(item){
        QList<EntityItem*> items;
        items.append(item);
        centerOnItems(items);
        if (parentWidget()) {
            parentWidget()->show();
        }
    }
}

void NodeView::highlightItem(int ID, bool highlighted)
{
    EntityItem* item = getEntityItem(ID);
    if(item){
        item->setHighlighted(highlighted);
    }
}

void NodeView::setupConnections(EntityItem *item)
{
    connect(item, &EntityItem::req_activeSelected, this, &NodeView::item_ActiveSelected);
    connect(item, &EntityItem::req_selected, this, &NodeView::item_Selected);
    connect(item, &EntityItem::req_expanded, this, &NodeView::item_SetExpanded);
    connect(item, &EntityItem::req_centerItem, this, &NodeView::item_SetCentered);

    connect(item, &EntityItem::req_StartMove, this, &NodeView::trans_InActive2Moving);
    connect(item, &EntityItem::req_Move, this, &NodeView::item_MoveSelection);
    connect(item, &EntityItem::req_FinishMove, this, &NodeView::trans_Moving2InActive);


    connect(item, &EntityItem::req_triggerAction, this, &NodeView::triggerAction);
    connect(item, &EntityItem::req_removeData, this, &NodeView::item_RemoveData);
    connect(item, &EntityItem::req_editData, this, &NodeView::item_EditData);



    if(item->isNodeItem()){
        NodeItem* node = (NodeItem*) item;

        connect(node, &NodeItem::req_StartResize, this, &NodeView::trans_InActive2Resizing);
        connect(node, &NodeItem::req_Resize, this, &NodeView::item_Resize);
        connect(node, &NodeItem::req_FinishResize, this, &NodeView::trans_Resizing2InActive);

        connect(node, &NodeItem::req_connectMode, this, &NodeView::node_ConnectMode);
        connect(node, &NodeItem::req_connectEdgeMode, this, &NodeView::node_ConnectEdgeMode);
        
        connect(node, &NodeItem::req_popOutRelatedNode, this, &NodeView::node_PopOutRelatedNode);
        
        
        
        
    }

    
}
void NodeView::showItem(EntityItem* item){
    auto parent = item->getParent();
    while(parent){
        if(parent->isNodeItem()){
            auto node_item = (NodeItem*)parent;
            if(!node_item->isExpanded()){
                node_item->setExpanded(true);
                int ID = parent->getID();
                emit setData(ID, "isExpanded", true);
            }
        }
        parent = parent->getParent();
    }
}

void NodeView::centerOnItems(QList<EntityItem *> items)
{
    emit triggerAction("Expanding Selection");
    for(auto item: items){
        if(!item->isVisibleTo(0)){
            showItem(item);
            //Show item
        }
    }
    centerRect(getSceneBoundingRectOfItems(items));
}

QRectF NodeView::getSceneBoundingRectOfItems(QList<EntityItem *> items)
{
    QRectF itemsRect;
    foreach(EntityItem* item, items){
        if(item->isVisible()){
            itemsRect = itemsRect.united(item->sceneViewRect());
        }
    }
    return itemsRect;
}

void NodeView::centerRect(QRectF rectScene)
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
    }else{
        resetMatrix();
    }
}

void NodeView::centerView(QPointF scenePos)
{
    QPointF delta = viewportRect().center() - scenePos;
    translate(delta);
    viewportCenter_Scene = viewportRect().center();
}


SelectionHandler *NodeView::getSelectionHandler()
{
    return selectionHandler;
}


void NodeView::update_minimap(){
    emit viewport_changed(viewportRect(), transform().m11());
}

void NodeView::paintEvent(QPaintEvent *event){
    QGraphicsView::paintEvent(event);

    auto new_transform = transform();
    if(old_transform != new_transform){
        old_transform = new_transform;
        update_minimap();
    }
}

void NodeView::viewItem_LabelChanged(QString label)
{
    auto text = label.toUpper();
    background_text.setText(text);


    auto fm = QFontMetrics(background_font);
    //Calculate the rectangle which contains the background test
    background_text_rect = fm.boundingRect(text);
}

void NodeView::activeViewDockChanged(ViewDockWidget* dw){

    bool active = dw && dw->widget() == this;
    if(active != is_active){
        is_active = active;
        update();
    }
}

QPointF NodeView::getTopLeftOfSelection(){
    auto vi = selectionHandler->getActiveSelectedItem();
    auto item = getEntityItem(vi);

    QPointF top_left = viewportRect().topLeft();
    if(item){
        top_left = item->mapFromScene(top_left);
        if(top_left.x() < 20){
            top_left.setX(0);
        }
        if(top_left.y() < 20){
            top_left.setY(0);
        }
    }
    return top_left;
}

QRectF NodeView::viewportRect()
{
    return mapToScene(viewport()->rect()).boundingRect();
}

void NodeView::nodeViewItem_Constructed(NodeViewItem *item)
{
    if(!item || item->getViewAspect() != containedAspect){
        return;
    }

    NodeItem* parentNode = getParentNodeItem(item);

    if(!containedNodeViewItem && item->getViewAspect() == containedAspect){
        setContainedNodeViewItem(item);
        //Don't construct an aspect.
        return;
    }



    if(containedNodeViewItem){
        if(containedNodeViewItem->isAncestorOf(item)){
            int ID = item->getID();
            NodeItem* nodeItem =  0;
            NODE_KIND nodeKind = item->getNodeKind();
            QString nodeKindStr = item->getData("kind").toString();

            //Ignore
            if(nodeKindStr.contains("DDS")){
                return;
            }

            bool ignorePosition = containedNodeViewItem == item;

            QPair<QString, QString> secondary_icon;
            secondary_icon.first = "Icons";
            switch(nodeKind){
            case NODE_KIND::HARDWARE_NODE:
                nodeItem = new HardwareNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("ip_address");
                secondary_icon.second = "arrowTransfer";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::MANAGEMENT_COMPONENT:
                nodeItem = new ManagementComponentNodeItem(item, parentNode);
                break;
            case NODE_KIND::LOGGINGSERVER:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("database");
                secondary_icon.second = "servers";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::LOGGINGPROFILE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(EDGE_KIND::DEPLOYMENT);
                nodeItem->setSecondaryTextKey("mode");
                secondary_icon.second = "gear";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::IDL:
                nodeItem = new DefaultNodeItem(item, parentNode);
                break;
            case NODE_KIND::SHARED_DATATYPES:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("version");
                secondary_icon.second = "tag";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::COMPONENT:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setVisualNodeKind(NODE_KIND::COMPONENT_IMPL);
                break;
            case NODE_KIND::COMPONENT_INSTANCE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                nodeItem->setSecondaryTextKey("type");
                break;
            case NODE_KIND::COMPONENT_IMPL:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setVisualNodeKind(NODE_KIND::COMPONENT);
                break;
            case NODE_KIND::COMPONENT_ASSEMBLY:
                nodeItem = new DefaultNodeItem(item, parentNode);
                secondary_icon.second = "copyX";
                nodeItem->setSecondaryIconPath(secondary_icon);
                nodeItem->setSecondaryTextKey("replicate_count");
                break;
            case NODE_KIND::BLACKBOX:
            case NODE_KIND::BLACKBOX_INSTANCE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                break;
            case NODE_KIND::TERMINATION:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                break;
            case NODE_KIND::HARDWARE_CLUSTER:
                nodeItem = new StackNodeItem(item, parentNode);
                break;
            case NODE_KIND::INEVENTPORT_INSTANCE:
            case NODE_KIND::OUTEVENTPORT_INSTANCE:
            case NODE_KIND::INEVENTPORT_DELEGATE:
            case NODE_KIND::OUTEVENTPORT_DELEGATE:
                nodeItem = new DefaultNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::ASSEMBLY);
                secondary_icon.second = "tiles";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::FOR_CONDITION:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                break;
            case NODE_KIND::CONDITION:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("value");
                secondary_icon.second = "circleQuestion";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::ATTRIBUTE_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("value");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                secondary_icon.second = "pencil";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::AGGREGATE:
                nodeItem = new StackNodeItem(item, parentNode);
                //Don't show icon
                //secondary_icon.second = "tiles";
                //nodeItem->setSecondaryIconPath(secondary_icon);
                //nodeItem->setSecondaryTextKey("namespace");
                break;
            case NODE_KIND::SETTER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("operator");
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                secondary_icon.second = "gear";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::WORKER_PROCESS:
            case NODE_KIND::PROCESS:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setIconOverlay("Functions", item->getData("operation").toString());
                nodeItem->setIconOverlayVisible(true);
                nodeItem->setSecondaryTextKey("worker");
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                secondary_icon.second = "spanner";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::MEMBER_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::VARIABLE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::ATTRIBUTE_IMPL:
            case NODE_KIND::AGGREGATE_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                secondary_icon.second = "tiles";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::MEMBER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setIconOverlay("Icons", "key");
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::INEVENTPORT_IMPL:
            case NODE_KIND::OUTEVENTPORT_IMPL:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                secondary_icon.second = "tiles";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::ATTRIBUTE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                nodeItem->setExpandEnabled(false);
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::VARIABLE_PARAMETER:
            case NODE_KIND::INPUT_PARAMETER:
            case NODE_KIND::VARIADIC_PARAMETER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setTertiaryIcon("Items", nodeKindStr);
                nodeItem->setTertiaryIconVisible(true);
                nodeItem->setSecondaryTextKey("value");
                secondary_icon.second = "pencil";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;

            case NODE_KIND::RETURN_PARAMETER:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                nodeItem->setTertiaryIcon("Items", nodeKindStr);
                nodeItem->setTertiaryIconVisible(true);
                nodeItem->setSecondaryTextKey("type");
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::INEVENTPORT:
            case NODE_KIND::OUTEVENTPORT:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                secondary_icon.second = "tiles";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::PERIODICEVENT:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("frequency");
                nodeItem->setExpandEnabled(false);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                secondary_icon.second = "clockCycle";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            case NODE_KIND::BRANCH_STATE:
            case NODE_KIND::WHILELOOP:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                break;
            case NODE_KIND::CODE:
            case NODE_KIND::HEADER:
            case NODE_KIND::WORKLOAD:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setVisualEdgeKind(EDGE_KIND::WORKFLOW);
                break;

            case NODE_KIND::VECTOR:
            case NODE_KIND::VECTOR_INSTANCE:
                nodeItem = new StackNodeItem(item, parentNode);
                nodeItem->setSecondaryTextKey("type");
                if(item->getViewAspect() == VIEW_ASPECT::BEHAVIOUR){
                    nodeItem->setVisualEdgeKind(EDGE_KIND::DATA);
                }
                secondary_icon.second = "bracketsAngled";
                nodeItem->setSecondaryIconPath(secondary_icon);
                break;
            default:
                nodeItem = new StackNodeItem(item, parentNode);
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
                }


                //We should care about position and expansion for any entity we visualise
                //emit viewController->vc_setData(ID, "x", 0.0);
                //emit viewController->vc_setData(ID, "y", 0.0);
                //emit viewController->vc_setData(ID, "isExpanded", true);
            

            }
        }
    }
}

void NodeView::edgeViewItem_Constructed(EdgeViewItem *item)
{

    switch(item->getEdgeKind()){
        case EDGE_KIND::ASSEMBLY:
        case EDGE_KIND::DATA:
        case EDGE_KIND::WORKFLOW:
        case EDGE_KIND::DEPLOYMENT:
            break;
        default:
            return;
    }

    if(!containedNodeViewItem || !containedNodeViewItem->isAncestorOf(item->getParentItem())){
        return;
    }



    NodeItem* parent = getParentNodeItem(item->getParentItem());
    NodeItem* source = getParentNodeItem(item->getSource());
    NodeItem* destination = getParentNodeItem(item->getDestination());

    if(source && destination){
        EdgeItem* edgeItem = new EdgeItem(item, parent,source,destination);


        if(edgeItem){
            guiItems[item->getID()] = edgeItem;

            setupConnections(edgeItem);

            if(!scene()->items().contains(edgeItem)){
                scene()->addItem(edgeItem);
            }

            //emit viewController->vc_setData(ID, "x", 0.0);
            //emit viewController->vc_setData(ID, "y", 0.0);
        }
    }
}

QList<ViewItem *> NodeView::getTopLevelViewItems() const
{
    QList<ViewItem *> items;
    foreach(EntityItem* item, getTopLevelEntityItems()){
        items.append(item->getViewItem());
    }
    return items;
}

QList<EntityItem *> NodeView::getTopLevelEntityItems() const
{
    QList<EntityItem*> items;
    foreach(int ID, topLevelGUIItemIDs){
        EntityItem* item = guiItems.value(ID, 0);
        if(item){
            items.append(item);
        }
    }
    return items;
}

QList<EntityItem *> NodeView::getSelectedItems() const
{
    QList<EntityItem*> items;
    foreach(ViewItem* item, selectionHandler->getSelection()){
        EntityItem* eItem = getEntityItem(item);
        if(eItem){
            items.append(eItem);
        }
    }
    return items;
}

NodeItem *NodeView::getParentNodeItem(NodeViewItem *item)
{
     while(item){
        int ID = item->getID();
        if(guiItems.contains(ID)){
            return (NodeItem*)guiItems[ID];
        }else{
            item = item->getParentNodeViewItem();
        }
     }
     return 0;
}

EntityItem *NodeView::getEntityItem(int ID) const
{
    EntityItem* item = 0;
    if(guiItems.contains(ID)){
        item = guiItems[ID];
    }
    return item;
}

EntityItem *NodeView::getEntityItem(ViewItem *item) const
{
    EntityItem* e = 0;
    if(item){
        e = getEntityItem(item->getID());
    }
    return e;
}

NodeItem *NodeView::getNodeItem(ViewItem *item) const
{
    EntityItem* e = getEntityItem(item->getID());
    if(e && e->isNodeItem()){
        return (NodeItem*) e;
    }
    return 0;
}

void NodeView::zoom(int delta, QPoint anchorScreenPos)
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

QPointF NodeView::getScenePosOfPoint(QPoint pos)
{
    if(pos.isNull()){
        //If we haven't been given a point, use the center of the viewport rect.
        pos = viewport()->rect().center();
    }
    return mapToScene(pos);
}

void NodeView::selectItemsInRubberband()
{
    QPolygonF rubberbandRect = mapToScene(rubberband->geometry());

    QList<ViewItem*> itemsToSelect;

    //Check for aspect selection.
    if(selectionHandler->getSelection().contains(containedNodeViewItem)){
        itemsToSelect.append(containedNodeViewItem);
    }

    foreach(QGraphicsItem* i, scene()->items(rubberbandRect,Qt::IntersectsItemShape)){
        EntityItem* entityItem = dynamic_cast<EntityItem*>(i);
        if(entityItem){
            itemsToSelect.append(entityItem->getViewItem());
        }
    }
    if(selectionHandler){
        selectionHandler->toggleItemsSelection(itemsToSelect, true);
    }
}


void NodeView::_selectAll()
{
    if(selectionHandler){
        EntityItem* guiItem = getEntityItem(selectionHandler->getFirstSelectedItem());

        QList<ViewItem*> itemsToSelect;

        if(guiItem){
            if(selectionHandler->getSelectionCount() == 1 && guiItem->isNodeItem()){
                NodeItem* nodeItem = (NodeItem*) guiItem;

                foreach(NodeItem* child, nodeItem->getChildNodes()){
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

void NodeView::_clearSelection()
{
    if(selectionHandler){
        //Depending on the type of NodeView we are.
        if(containedNodeViewItem){
            if(!(selectionHandler->getSelectionCount() == 1 && selectionHandler->getActiveSelectedItem() == containedNodeViewItem)){
                //If we are the aspect select the aspect.
                selectionHandler->toggleItemsSelection(containedNodeViewItem);
            }
        }else{
            //If we aren't an aspect clear the selection.
            selectionHandler->clearSelection();
        }
    }
}


qreal NodeView::distance(QPoint p1, QPoint p2)
{
    return qSqrt(qPow(p2.x() - p1.x(), 2) + qPow(p2.y() - p1.y(), 2));
}

void NodeView::setupStateMachine()
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
    state_InActive->addTransition(this, &NodeView::trans_InActive2Moving, state_Active_Moving);
    state_Active_Moving->addTransition(this, &NodeView::trans_Moving2InActive, state_InActive);

    state_InActive->addTransition(this, &NodeView::trans_InActive2RubberbandMode, state_Active_RubberbandMode);
    state_Active_RubberbandMode->addTransition(this, &NodeView::trans_RubberbandMode2InActive, state_InActive);

    state_Active_RubberbandMode->addTransition(this, &NodeView::trans_RubberbandMode2RubberbandMode_Selecting, state_Active_RubberbandMode_Selecting);
    state_Active_RubberbandMode_Selecting->addTransition(this, &NodeView::trans_RubberbandMode2InActive, state_Active_RubberbandMode);

    state_InActive->addTransition(this, &NodeView::trans_InActive2Resizing, state_Active_Resizing);
    state_Active_Resizing->addTransition(this, &NodeView::trans_Resizing2InActive, state_InActive);


    state_InActive->addTransition(this, &NodeView::trans_InActive2Connecting, state_Active_Connecting);
    state_Active_Connecting->addTransition(this, &NodeView::trans_Connecting2InActive, state_InActive);

    //Connect to states.

    connect(state_InActive, &QState::entered, this, &NodeView::state_Default_Entered);


    connect(state_Active_Moving, &QState::entered, this, &NodeView::state_Moving_Entered);
    connect(state_Active_Moving, &QState::exited, this, &NodeView::state_Moving_Exited);

    connect(state_Active_Resizing, &QState::entered, this, &NodeView::state_Resizing_Entered);
    connect(state_Active_Resizing, &QState::exited, this, &NodeView::state_Resizing_Exited);

    connect(state_Active_RubberbandMode, &QState::entered, this, &NodeView::state_RubberbandMode_Entered);
    connect(state_Active_RubberbandMode, &QState::exited, this, &NodeView::state_RubberbandMode_Exited);

    connect(state_Active_RubberbandMode_Selecting, &QState::entered, this, &NodeView::state_RubberbandMode_Selecting_Entered);
    connect(state_Active_RubberbandMode_Selecting, &QState::exited, this, &NodeView::state_RubberbandMode_Selecting_Exited);

    connect(state_Active_Connecting, &QState::entered, this, &NodeView::state_Connecting_Entered);
    connect(state_Active_Connecting, &QState::exited, this, &NodeView::state_Connecting_Exited);

    viewStateMachine->start();
}

EntityItem *NodeView::getEntityAtPos(QPointF scenePos)
{
    foreach(QGraphicsItem* item, scene()->items(scenePos)){
        EntityItem* entityItem =  dynamic_cast<EntityItem*>(item);
        if(entityItem){
            return entityItem;
        }
    }
    return 0;
}

void NodeView::state_Moving_Entered()
{
    setCursor(Qt::SizeAllCursor);
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getSelection()){
            EntityItem* item = getEntityItem(viewItem);
            if(item){
                item->setMoveStarted();
            }
        }
    }
}

void NodeView::state_Moving_Exited()
{
    if(selectionHandler){
        bool anyMoved = false;

        QVector<ViewItem*> selection = selectionHandler->getSelection();

        foreach(ViewItem* viewItem, selection){
            EntityItem* item = getEntityItem(viewItem);
            if(item){
                if(item->setMoveFinished()){
                    anyMoved = true;
                }
            }
        }

        if(anyMoved){
            emit triggerAction("Moving Selection");
            foreach(ViewItem* viewItem, selection){
                EntityItem* item = getEntityItem(viewItem);
                if(item && !item->isIgnoringPosition()){
                    QPointF pos = item->getNearestGridPoint();
                    emit setData(item->getID(), "x", pos.x());
                    emit setData(item->getID(), "y", pos.y());
                }
            }
        }
    }
}

void NodeView::state_Resizing_Entered()
{
    if(selectionHandler){
        if(selectionHandler->getSelectionCount() != 1){
            emit trans_Resizing2InActive();
            return;
        }

        foreach(ViewItem* viewItem, selectionHandler->getSelection()){
            NodeItem* item = getNodeItem(viewItem);
            if(item){
                item->setResizeStarted();
            }
        }
        setCursor(Qt::SizeAllCursor);
    }
}

void NodeView::state_Resizing_Exited()
{
    if(selectionHandler){
        foreach(ViewItem* viewItem, selectionHandler->getSelection()){
            NodeItem* item = getNodeItem(viewItem);

            if(item && item->setResizeFinished()){
                emit triggerAction("Resizing Item");
                QSizeF size = item->getGridAlignedSize();
                emit setData(item->getID(), "width", size.width());
                emit setData(item->getID(), "height", size.height());
            }
        }
    }
}

void NodeView::state_RubberbandMode_Entered()
{
    setCursor(Qt::CrossCursor);
}

void NodeView::state_RubberbandMode_Exited()
{
}

void NodeView::state_RubberbandMode_Selecting_Entered()
{
    rubberband->setVisible(true);
}

void NodeView::state_RubberbandMode_Selecting_Exited()
{
    rubberband->setVisible(false);
    emit trans_RubberbandMode2InActive();
}

void NodeView::state_Connecting_Entered()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItem* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) item;

        //Highlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(nodeItem->getVisualEdgeKind());
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), true);
        }

        QPointF lineStart = nodeItem->scenePos();
        lineStart += nodeItem->getElementRect(EntityItem::ER_EDGE_KIND_ICON).center();

        if(!connectLineItem){
            connectLineItem = scene()->addLine(connectLine);
            connectLineItem->setPen(Qt::DashLine);
            connectLineItem->setZValue(100);
        }
        connectLine.setP1(lineStart);
        connectLine.setP2(lineStart);
        connectLineItem->setLine(connectLine);
        connectLineItem->setVisible(true);
    }
}

void NodeView::state_Connecting_Exited()
{
    ViewItem* vi = selectionHandler->getActiveSelectedItem();
    EntityItem* item = getEntityItem(vi);
    if(item && item->isNodeItem()){
        NodeItem* nodeItem = (NodeItem*) item;

        EDGE_KIND edgeKind = nodeItem->getVisualEdgeKind();

        //Unhighlight things we can connect to
        QList<ViewItem*> items = viewController->getValidEdges(edgeKind);
        foreach(ViewItem* item, items){
            highlightItem(item->getID(), false);
        }

        if(connectLineItem){
            QPointF scenePos = connectLine.p2();
            EntityItem* otherItem = getEntityAtPos(scenePos);
            if(otherItem){
                emit triggerAction("Constructing Edge");
                emit viewController->vc_constructEdge(selectionHandler->getSelectionIDs().toList(), otherItem->getID(), edgeKind);
            }
            connectLineItem->setVisible(false);
        }
    }
}

void NodeView::state_Default_Entered()
{
    unsetCursor();
}

void NodeView::keyPressEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(CONTROL && SHIFT){
        emit trans_InActive2RubberbandMode();
    }
}

void NodeView::keyReleaseEvent(QKeyEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    if(!(CONTROL && SHIFT)){
        emit trans_RubberbandMode2InActive();
    }
}

void NodeView::wheelEvent(QWheelEvent *event)
{
    //Call Zoom
    if(viewController->isControllerReady()){
        zoom(event->delta(), event->pos());
    }
}

void NodeView::mousePressEvent(QMouseEvent *event)
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
            EntityItem* item = getEntityAtPos(scenePos);
            if(!item){
                clearSelection();
                handledEvent = true;
            }
        }
    }

    if(event->button() == Qt::MiddleButton){
        EntityItem* item = getEntityAtPos(scenePos);
        if(!item){
            handledEvent = true;
            fitToScreen();
        }
    }

    if(!handledEvent){
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeView::mouseMoveEvent(QMouseEvent *event)
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

void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;
    //bool SHIFT = event->modifiers() & Qt::ShiftModifier;

    bool handledEvent = false;

    //Exit pan mode yo
    if(isPanning && event->button() == Qt::RightButton){
        isPanning = false;

        //Popup Toolbar if there is an item.
        if(pan_distance < 10){
            QPointF itemPos = mapToScene(event->pos());
            EntityItem* item = getEntityAtPos(itemPos);
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
        //handledEvent = true;
    }


    //if(!handledEvent){
        QGraphicsView::mouseReleaseEvent(event);
    //}
}

void NodeView::drawForeground(QPainter *painter, const QRectF &r){
    QGraphicsView::drawForeground(painter, r);

    if(!is_active){
        //painter->setBrush(QColor(255, 255, 255, 50));
        painter->setBrush(QColor(0, 0, 0, 60));
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);
    }
}


void NodeView::drawBackground(QPainter *painter, const QRectF & r)
{
    //Paint the background
    QGraphicsView::drawBackground(painter, r);
    {
        painter->save();
        //Reset the transform to ignore zoom/view
        painter->resetTransform();
        //Set the Pen and font
        painter->setPen(background_text_color);
        painter->setFont(background_font);

        auto brect = rect();
        //Calculate the top_left corner of the text rect.
        QPointF point;
        point.setX((brect.width() - background_text_rect.width()) / 2);
        point.setY(brect.height() - background_text_rect.height());
        painter->drawStaticText(point, background_text);
        painter->restore();
    }
}

void NodeView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    update_minimap();
}

void NodeView::notification_Added(NotificationObject* obj){
    //Check for IDs
    auto entity = getEntityItem(obj->getEntityID());
    if(entity){

        auto tint_color = Theme::theme()->getSeverityColor(obj->getSeverity());
        auto icon = obj->getIcon();
        entity->AddNotification(icon.first, icon.second, tint_color);
        notification_id_lookup[obj->getID()] = entity->getID();
    }
}

void NodeView::notification_Destructed(int id){

    auto e_id = notification_id_lookup.value(id, -1);
    if(e_id != -1){
        notification_id_lookup.remove(id);
        auto entity = getEntityItem(e_id);
        if(entity){
            entity->ClearNotification();
        }
    }
}
