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
#include "SceneItems/Node/compactnodeitem.h"
#include "SceneItems/Node/hardwarenodeitem.h"
#include "SceneItems/Node/membernodeitem.h"
#include "SceneItems/Node/deploymentcontainernodeitem.h"

#include "../../Controllers/WindowManager/windowmanager.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "SceneItems/Edge/edgeitem.h"
#include "SceneItems/Edge/edgeitem.h"
#include "../../theme.h"




#define ZOOM_INCREMENTOR 1.05

NodeView::NodeView(QWidget* parent):QGraphicsView(parent)
{
    
    setMinimumSize(200, 200);
    setupStateMachine();
    
    QRectF sceneRect;
    sceneRect.setSize(QSize(50000,50000));
    sceneRect.moveCenter(QPointF(0,0));
    setSceneRect(sceneRect);

    setFocusPolicy(Qt::StrongFocus);
    setScene(new QGraphicsScene(this));
    scene()->setItemIndexMethod(QGraphicsScene::NoIndex);

    //Set QT Options for this QGraphicsView
    setDragMode(NoDrag);
    setAcceptDrops(true);
    setTransformationAnchor(QGraphicsView::NoAnchor);
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setRenderHint(QPainter::HighQualityAntialiasing, true);

    setCacheMode(QGraphicsView::CacheBackground);



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
        //Add the actions
        addActions(viewController->getActionController()->getNodeViewActions());

        connect(viewController, &ViewController::vc_viewItemConstructed, this, &NodeView::viewItem_Constructed);
        connect(viewController, &ViewController::vc_viewItemDestructing, this, &NodeView::viewItem_Destructed);

        connect(viewController->getActionController()->edit_clearSelection, &QAction::triggered, this, &NodeView::trans_inactive);

        selectionHandler = viewController->getSelectionController()->constructSelectionHandler(this);

        connect(selectionHandler, &SelectionHandler::itemSelectionChanged, this, &NodeView::selectionHandler_ItemSelectionChanged);
        connect(selectionHandler, &SelectionHandler::itemActiveSelectionChanged, this, &NodeView::selectionHandler_ItemActiveSelectionChanged);

        connect(this, &NodeView::toolbarRequested, viewController, &ViewController::vc_showToolbar);
        connect(this, &NodeView::triggerAction, viewController, &ViewController::TriggerAction);
        connect(this, &NodeView::setData, viewController, &ViewController::SetData);
        connect(this, &NodeView::removeData, viewController, &ViewController::RemoveData);
        connect(this, &NodeView::editData, viewController, &ViewController::vc_editTableCell);




        connect(viewController, &ViewController::vc_centerItem, this, &NodeView::centerItem);
        connect(viewController, &ViewController::vc_fitToScreen, this, &NodeView::AllFitToScreen);

        connect(viewController, &ViewController::vc_selectAndCenterConnectedEntities, this, &NodeView::centerConnections);


        connect(viewController, &ViewController::vc_highlightItem, this, &NodeView::highlightItem);
    }
}

void NodeView::AllFitToScreen(bool only_if_active){
    if(is_active || !only_if_active){
        FitToScreen();
    }
}

void NodeView::FitToScreen()
{
    centerOnItems(getTopLevelEntityItems());
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

        //zoom = qMax(0.25, zoom);
        zoom = qMin(zoom, 5.0);

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



void NodeView::viewItem_Constructed(ViewItem *item)
{
    //return;
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

void NodeView::themeItem(EntityItem* entity){
    if(entity){
        entity->setBaseBodyColor(body_color);
        entity->setAltBodyColor(alt_body_color);
        
        entity->setTextColor(text_color);
        entity->setAltTextColor(alt_text_color);
        entity->setHeaderColor(header_color);
        entity->setHighlightColor(highlight_color);
        entity->setHighlightTextColor(highlight_text_color);
        entity->setDefaultPen(default_pen);
    }
}

void NodeView::themeChanged()
{
    auto theme = Theme::theme();
    if(isAspectView){
        background_color = theme->getAspectBackgroundColor(containedAspect);
    }else{
        background_color = theme->getAltBackgroundColor();
    }
    background_text_color = background_color.darker(110);
    setBackgroundBrush(background_color);


    body_color = theme->getAltBackgroundColor();
    text_color = theme->getTextColor();
    alt_text_color = theme->getAltTextColor();
    header_color = theme->getBackgroundColor();
    highlight_color = theme->getHighlightColor();
    highlight_text_color = theme->getTextColor(ColorRole::SELECTED);
    alt_body_color = theme->getDisabledBackgroundColor();
    default_pen = QPen(theme->getTextColor(ColorRole::DISABLED));
    default_pen.setCosmetic(true);

    for(auto entity : guiItems){
        themeItem(entity);
    }
}


void NodeView::node_ConnectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction){
    auto edge_direction = direction == EDGE_DIRECTION::SOURCE ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;
    auto global_pos = mapToGlobal(mapFromScene(scene_pos));
    //Flip the direction
    viewController->getContextMenu()->popup_edge_menu(global_pos, kind, edge_direction);
}

void NodeView::node_AddMenu(QPointF scene_pos, int index){
    auto global_pos = mapToGlobal(mapFromScene(scene_pos));
    //Flip the direction
    viewController->getContextMenu()->popup_add_menu(global_pos, index);
}



void NodeView::node_ConnectEdgeMode(QPointF scene_pos, EDGE_KIND edge_kind, EDGE_DIRECTION direction){
    emit trans_InActive2Connecting();
    if(state_Active_Connecting->active()){
        auto item_map = viewController->getValidEdges(edge_kind);

        auto edge_direction = direction == EDGE_DIRECTION::SOURCE ? EDGE_DIRECTION::TARGET : EDGE_DIRECTION::SOURCE;

        state_Active_Connecting->setProperty("edge_kind", QVariant::fromValue(edge_kind));
        state_Active_Connecting->setProperty("edge_direction", QVariant::fromValue(edge_direction));
    
        for(auto item : item_map.values(edge_direction)){
            emit viewController->vc_highlightItem(item->getID(), true);
        }
        
        if(!connect_line){
            connect_line = new ArrowLine();
            scene()->addItem(connect_line);
            connect_line->setPen(Qt::DashLine);
            connect_line->setZValue(100);
        }
        connect_line->set_begin_point(scene_pos);
        connect_line->set_end_point(scene_pos);
        connect_line->setVisible(true);
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



void NodeView::centerSelection()
{

    centerOnItems(getSelectedItems());
}

void NodeView::centerConnections(ViewItem* item)
{
    if(item){
        QSet<EdgeViewItem*> edges;
        if(item->isNode()){
            edges = ((NodeViewItem*)item)->getEdges();
        }else if(item->isEdge()){
            edges += ((EdgeViewItem*)item);
        }

        QSet<ViewItem*> to_select;
        QSet<EntityItem*> to_center;
        
        for(auto e : edges){
            auto s = e->getSource();
            auto d = e->getDestination();
            
            auto src = getEntityItem(s);
            auto dst = getEntityItem(d);
            auto edge = getEntityItem(e);

            if(src && s){
                to_select += s;
                to_center += src;
            }

            if(dst && d){
                to_select += d;
                to_center += dst;
            }

            if(edge && e){
                to_select += e;
                to_center += edge;
            }
        }

        if(to_select.size()){
            if(selectionHandler){
                selectionHandler->toggleItemsSelection(to_select.toList());
            }
            centerOnItems(to_center.toList());
        }else{
            clearSelection();
        }
    }
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
    if(item){
        showItem(item);
        centerRect(item->sceneViewRect());
    }
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



void NodeView::minimap_Pan(QPointF delta)
{
    translate(delta);
}

void NodeView::minimap_Zoom(int delta)
{
    zoom(delta);
}


/**
 * @brief NodeView::receiveMouseMove
 * This function is used to pass mouse move events from the overlay splitter to this view.
 * This is needed for the hover on graphics items to work.
 * @param event
 */
void NodeView::receiveMouseMove(QMouseEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseMove:
        mouseMoveEvent(event);
        break;
    case QEvent::MouseButtonDblClick:
        mouseDoubleClickEvent(event);
        break;
    default:
        break;
    }

    //mouseMoveEvent(event);
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
    themeItem(item);
    
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
        
        connect(node, &NodeItem::req_connectEdgeMode, this, &NodeView::node_ConnectEdgeMode);
        connect(node, &NodeItem::req_connectEdgeMenu, this, &NodeView::node_ConnectEdgeMenu);
        connect(node, &NodeItem::req_addNodeMenu, this, &NodeView::node_AddMenu);

        
    }
}
void NodeView::showItem(EntityItem* item){
    auto parent = item->getParent();
    emit triggerAction("Expanding Selection");
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

void NodeView::topLevelItemMoved()
{
    auto new_scene_rect = getSceneBoundingRectOfItems(getTopLevelEntityItems());
    if(new_scene_rect != currentSceneRect){
        currentSceneRect = new_scene_rect;
        emit scenerect_changed(currentSceneRect);
    }
}



void NodeView::update_minimap(){
    emit viewport_changed(viewportRect(), transform().m11());
    emit scenerect_changed(currentSceneRect);
}

void NodeView::paintEvent(QPaintEvent *event){
    QGraphicsView::paintEvent(event);

    auto new_transform = transform();
    if(old_transform != new_transform){
        old_transform = new_transform;
        update_minimap();
    }
}


/**
 * @brief NodeView::event
 * Need to override this function in order to properly pass through events from the Panel.
 * @param event
 * @return
 */
bool NodeView::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        mousePressEvent((QMouseEvent*)event);
    } else if (event->type() == QEvent::MouseMove) {
        mouseMoveEvent((QMouseEvent*)event);
    } else if (event->type() == QEvent::MouseButtonRelease) {
        mouseReleaseEvent((QMouseEvent*)event);
    } else if (event->type() == QEvent::Wheel) {
        wheelEvent((QWheelEvent*)event);
    } else if (event->type() == QEvent::Paint) {
        // Not really sure why I have to call the functions above but not this one...
        // Calling this prints out "Painter is not active."
        //paintEvent((QPaintEvent*)event);
    }
    return QGraphicsView::event(event);
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
        return;
    }

    if(containedNodeViewItem){
        if(containedNodeViewItem->isAncestorOf(item)){
            NodeItem* node_item =  0;


            auto ID = item->getID();
            auto node_kind = item->getNodeKind();
            auto parent_node_kind = item->getParentNodeKind();
            
            //Ignore QOS Elements
            if(item->isNodeOfType(NODE_TYPE::QOS)){
                return;
            }
            
            //Ignore Functions contained within Class Instances
            if(node_kind == NODE_KIND::FUNCTION && parent_node_kind == NODE_KIND::CLASS_INSTANCE){
                return;
            }

            switch(node_kind){
            case NODE_KIND::HARDWARE_NODE:
                node_item = new HardwareNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("ip_address");
                node_item->setExpandEnabled(true);
                break;
            case NODE_KIND::LOGGINGSERVER:
                node_item = new DefaultNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("database");
                node_item->setIconVisible(NodeItem::EntityRect::SECONDARY_ICON, {"Icons", "servers"}, true);
                break;
            case NODE_KIND::DEPLOYMENT_CONTAINER:
                node_item = new DeploymentContainerNodeItem(item, parentNode);
                    break;
            case NODE_KIND::LOGGINGPROFILE:
                node_item = new DefaultNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("mode");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "gear"}, true);
                break;
            case NODE_KIND::IDL:
                node_item = new DefaultNodeItem(item, parentNode);
                break;
            case NODE_KIND::NAMESPACE:{
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey("namespace");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            }
            case NODE_KIND::SHARED_DATATYPES:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey("version");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tag"}, true);
                break;
            case NODE_KIND::COMPONENT:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Vertical);
                    stack_item->SetRenderCellSuffixIcon(3, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                break;
            case NODE_KIND::COMPONENT_IMPL:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    node_item = stack_item;
                }
                break;
            case NODE_KIND::CLASS:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    node_item = stack_item;
                }
                break;
            case NODE_KIND::COMPONENT_INSTANCE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Component"}, true);
                break;
            case NODE_KIND::COMPONENT_ASSEMBLY:
                node_item = new DefaultNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("replicate_value");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "copyX"}, true);
                break;
            case NODE_KIND::HARDWARE_CLUSTER:
                node_item = new StackNodeItem(item, parentNode);
                break;
            case NODE_KIND::PORT_REQUEST_DELEGATE:
                node_item = new DefaultNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "ServerInterface"}, true);
                break;
            case NODE_KIND::PORT_SUBSCRIBER_DELEGATE:
            case NODE_KIND::PORT_PUBLISHER_DELEGATE:
            case NODE_KIND::PORT_PUBSUB_DELEGATE:
                node_item = new DefaultNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);

                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                break;

            case NODE_KIND::PORT_REPLIER_INST:
            case NODE_KIND::PORT_REQUESTER_INST:
            case NODE_KIND::PORT_SUBSCRIBER_INST:
            case NODE_KIND::PORT_PUBLISHER_INST:
                node_item = new CompactNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                node_item->setTertiaryTextKey("middleware");
                node_item->setIconVisible(EntityItem::EntityRect::TERTIARY_ICON, {"Icons", "sliders"}, true);

                if(node_kind == NODE_KIND::PORT_PUBLISHER_INST || node_kind == NODE_KIND::PORT_REQUESTER_INST){
                    node_item->setRightJustified(true);
                }
                break;
            case NODE_KIND::EXTERNAL_SERVER_DELEGATE:{
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "ServerInterface"}, true);
                break;
            }
            case NODE_KIND::EXTERNAL_PUBSUB_DELEGATE:{
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"EntityIcons", "Aggregate"}, true);
                break;
            }
            case NODE_KIND::DEPLOYMENT_ATTRIBUTE:
                node_item = new CompactNodeItem(item, parentNode);
                node_item->setMoveEnabled(true);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                node_item->setTertiaryTextKey("value");
                node_item->setIconVisible(EntityItem::EntityRect::TERTIARY_ICON, {"Icons", "pencil"}, true);
                break;
            case NODE_KIND::ATTRIBUTE_INSTANCE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);

                switch(item->getViewAspect()){
                    case VIEW_ASPECT::BEHAVIOUR:{
                        node_item->setSecondaryTextKey("type");
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                        break;
                    }
                    default:{
                        node_item->setSecondaryTextKey("value");
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                        break;
                    }
                }

                break;
            case NODE_KIND::SERVER_INTERFACE:
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("namespace");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            case NODE_KIND::AGGREGATE:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey("namespace");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "letterA"}, true);
                break;
            case NODE_KIND::AGGREGATE_INSTANCE:
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    node_item = stack_item;
                }
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::FUNCTION_CALL:
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("class");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "spanner"}, true);
                break;
            case NODE_KIND::MEMBER:
            case NODE_KIND::MEMBER_INSTANCE:
                node_item = new MemberNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                
                if(item->hasData("value")){
                    node_item->setSecondaryTextKey("value");
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                }else{
                    node_item->setSecondaryTextKey("type");
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                }
                break;
            case NODE_KIND::VARIABLE:
                node_item = new StackNodeItem(item, parentNode);

                switch(item->getViewAspect()){
                    case VIEW_ASPECT::ASSEMBLIES:{
                        node_item->setSecondaryTextKey("value");
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                        break;
                    }
                    default:{
                        node_item->setSecondaryTextKey("type");
                        node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                        break;
                    }
                }
                
                break;
            case NODE_KIND::ATTRIBUTE_IMPL:
                node_item = new StackNodeItem(item, parentNode, Qt::Vertical);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;

            case NODE_KIND::ENUM:{
                {
                    auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                    stack_item->SetUseColumnCount(0, 0, true);
                    stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                    node_item = stack_item;
                }
                break;
            }
            case NODE_KIND::ENUM_INSTANCE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                
                if(item->hasData("value")){
                    node_item->setSecondaryTextKey("value");
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                }else{
                    node_item->setSecondaryTextKey("type");
                    node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                }
                break;
            
            case NODE_KIND::PORT_PUBLISHER_IMPL:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::ATTRIBUTE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
            case NODE_KIND::VARIABLE_PARAMETER:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "value"}, true);
                break;
            case NODE_KIND::INPUT_PARAMETER:{
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);

                if(item->hasData("editable_key")){
                    node_item->setPrimaryTextKey("");
                    node_item->setSecondaryTextKey(item->getData("editable_key").toString());
                }else{
                    node_item->setPrimaryTextKey("label");
                    node_item->setSecondaryTextKey("value");
                }

                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                break;
            }
            case NODE_KIND::VARIADIC_PARAMETER:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setPrimaryTextKey("");
                node_item->setSecondaryTextKey("value");

                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "pencil"}, true);
                break;

            case NODE_KIND::RETURN_PARAMETER:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
            
            
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:{
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            }
            case NODE_KIND::PORT_REQUESTER_IMPL:{
                auto stack_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item = stack_item;
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                
                stack_item->SetRenderCellArea(0, -1, true, true);
                stack_item->SetCellOrientation(0, -1, Qt::Vertical);
                stack_item->SetRenderCellArea(0, 1, true, true);
                stack_item->SetCellOrientation(0, 1, Qt::Vertical);

                break;
            }
            case NODE_KIND::VOID_TYPE:{
                node_item = new BasicNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                node_item->setContractedHeight(node_item->getContractedHeight() / 2);
                node_item->setContractedWidth(40);
                break;
            }

            case NODE_KIND::CLASS_INSTANCE:{
                node_item = new StackNodeItem(item, parentNode);
                if(item->getData(KeyName::IsWorker).toBool()){
                    node_item->setIconVisible(EntityItem::EntityRect::MAIN_ICON_OVERLAY, {"Icons", "spanner"}, true);
                }
                break;
            }
            case NODE_KIND::INPUT_PARAMETER_GROUP:
            case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:
            case NODE_KIND::RETURN_PARAMETER_GROUP:
            case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:
            case NODE_KIND::PORT_PERIODIC_INST:{
                node_item = new StackNodeItem(item, parentNode);
                node_item->setContractedHeight(node_item->getContractedHeight() / 2);
                node_item->setContractedWidth(40);
                break;
            }

            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_SUBSCRIBER:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::PORT_SUBSCRIBER_IMPL:
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "tiles"}, true);
                break;
            case NODE_KIND::PORT_PERIODIC:
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                break;
            
            case NODE_KIND::CODE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setExpandEnabled(false);
                break;
            case NODE_KIND::VECTOR:
            case NODE_KIND::VECTOR_INSTANCE:
                node_item = new StackNodeItem(item, parentNode);
                node_item->setSecondaryTextKey("type");
                node_item->setIconVisible(EntityItem::EntityRect::SECONDARY_ICON, {"Icons", "category"}, true);
                break;
             case NODE_KIND::FUNCTION:
                node_item = new StackNodeItem(item, parentNode, Qt::Horizontal);
                
                if(!item->isDataProtected("operation")){
                    node_item->setPrimaryTextKey("operation");
                }else{
                    node_item->setPrimaryTextKey("label");
                }
                break;
            default:
                node_item = new StackNodeItem(item, parentNode);
                break;
            }

            if(node_item){
                
                //Ignore the position if we are 
                if(containedNodeViewItem == item){
                    node_item->setIgnorePosition(true);
                }

                auto stack_item = qobject_cast<StackNodeItem*>(node_item);
                
                if(stack_item){
                    stack_item->setDefaultCellSpacing(stack_item->getGridSize());

                    if(item->isNodeOfType(NODE_TYPE::BEHAVIOUR_CONTAINER) || item->isNodeOfType(NODE_TYPE::TOP_BEHAVIOUR_CONTAINER)){
                        stack_item->setAlignment(Qt::Horizontal);

                        if(node_kind == NODE_KIND::COMPONENT_IMPL || node_kind == NODE_KIND::CLASS){
                            stack_item->SetRenderCellText(0, 0, true, "Functions");
                            stack_item->SetCellOrientation(0, 0, Qt::Vertical);
                            stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                        }else{
                            stack_item->SetRenderCellArea(0, -1, true, true);
                            stack_item->SetCellOrientation(0, -1, Qt::Vertical);
                            
                            stack_item->SetRenderCellArea(0, 1, true, true);
                            stack_item->SetCellOrientation(0, 1, Qt::Vertical);

                            stack_item->SetRenderCellArea(0, 0, true);
                            stack_item->SetRenderCellText(0, 0, true, "WORKFLOW");

                            stack_item->SetRenderCellPrefixIcon(0, 0, true, "Icons", "arrowDownRightLong");
                            stack_item->SetRenderCellGapIcons(0, 0, true, "Icons", "arrowRightLong");
                            stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                            stack_item->SetRenderCellHoverIcons(0, 0, true, "Icons", "plus");
                            
                            stack_item->SetCellSpacing(0, 0, 20);
                        }

                        stack_item->SetRenderCellArea(1, 0, true, true);
                        stack_item->SetRenderCellText(1, 0, true, "Attributes");
                        stack_item->SetUseColumnCount(1, 0, true);
                        stack_item->SetCellSpacing(1, 0, 10);

                        stack_item->SetRenderCellArea(1, 1, true, true);
                        stack_item->SetRenderCellText(1, 1, true, "Variables");
                        stack_item->SetUseColumnCount(1, 1, true);
                        stack_item->SetCellSpacing(1, 1, 10);


                        stack_item->SetRenderCellArea(1, -1, true, true);
                        stack_item->SetRenderCellText(1, -1, true, "Headers");
                        stack_item->SetCellOrientation(1, -1, Qt::Vertical);
                        stack_item->SetUseColumnCount(1, -1, true);

                        stack_item->SetRenderCellArea(1, 2, true, true);
                        stack_item->SetRenderCellText(1, 2, true, "Workers");
                        stack_item->SetUseColumnCount(1, 2, true);
                        stack_item->SetCellSpacing(1, 2, 10);
                    }else{
                        if(node_kind == NODE_KIND::AGGREGATE || node_kind == NODE_KIND::INPUT_PARAMETER_GROUP || node_kind == NODE_KIND::RETURN_PARAMETER_GROUP || node_kind == NODE_KIND::AGGREGATE_INSTANCE){
                            stack_item->SetRenderCellSuffixIcon(0, 0, true, "Icons", "plus");
                        }
                    }
                }
                
                //Insert into the GUI hash
                guiItems[ID] = node_item;
                setupConnections(node_item);

                if(!parentNode){
                    scene()->addItem(node_item);
                    
                    topLevelGUIItemIDs.append(ID);
                    connect(node_item, &NodeItem::positionChanged, this, &NodeView::topLevelItemMoved);
                    connect(node_item, &NodeItem::sizeChanged, this, &NodeView::topLevelItemMoved);
                }

                if(stack_item){
                    stack_item->RecalculateCells();
                }
            }
        }
    }
}

void NodeView::edgeViewItem_Constructed(EdgeViewItem *item)
{

    switch(item->getEdgeKind()){
        case EDGE_KIND::ASSEMBLY:
        case EDGE_KIND::DATA:
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
        EdgeItem* edgeItem = new EdgeItem(item, parent, source, destination);

        if(edgeItem){
            auto theme = Theme::theme();
            edgeItem->setBaseBodyColor(theme->getAltBackgroundColor());
            edgeItem->setHeaderColor(theme->getBackgroundColor());
            edgeItem->setTextColor(theme->getTextColor());
            QPen defaultPen(theme->getTextColor(ColorRole::DISABLED));
            defaultPen.setCosmetic(true);
            edgeItem->setDefaultPen(defaultPen);

            guiItems[item->getID()] = edgeItem;

            setupConnections(edgeItem);

            if(!scene()->items().contains(edgeItem)){
                scene()->addItem(edgeItem);
            }
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
    for(auto item : selectionHandler->getSelection()){
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
    state_Active_RubberbandMode = new QState();
    state_Active_RubberbandMode_Selecting = new QState();
    state_Active_Connecting = new QState();

    //Add States
    viewStateMachine->addState(state_InActive);
    viewStateMachine->addState(state_Active_Moving);
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



    state_InActive->addTransition(this, &NodeView::trans_InActive2Connecting, state_Active_Connecting);
    state_Active_Connecting->addTransition(this, &NodeView::trans_Connecting2InActive, state_InActive);


    connect(this, &NodeView::trans_inactive, &NodeView::trans_Moving2InActive);
    connect(this, &NodeView::trans_inactive, &NodeView::trans_Connecting2InActive);
    connect(this, &NodeView::trans_inactive, &NodeView::trans_RubberbandMode2InActive);
    //Connect to states.

    connect(state_InActive, &QState::entered, this, &NodeView::state_Default_Entered);


    connect(state_Active_Moving, &QState::entered, this, &NodeView::state_Moving_Entered);
    connect(state_Active_Moving, &QState::exited, this, &NodeView::state_Moving_Exited);

    connect(state_Active_RubberbandMode, &QState::entered, this, &NodeView::state_RubberbandMode_Entered);
    connect(state_Active_RubberbandMode, &QState::exited, this, &NodeView::state_RubberbandMode_Exited);

    connect(state_Active_RubberbandMode_Selecting, &QState::entered, this, &NodeView::state_RubberbandMode_Selecting_Entered);
    connect(state_Active_RubberbandMode_Selecting, &QState::exited, this, &NodeView::state_RubberbandMode_Selecting_Exited);

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
    //setCursor(Qt::SizeAllCursor);
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


void NodeView::state_RubberbandMode_Entered()
{
    setCursor(Qt::CrossCursor);
}

void NodeView::state_RubberbandMode_Exited()
{
    unsetCursor();
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


void NodeView::state_Connecting_Exited()
{
    auto edge_kind = state_Active_Connecting->property("edge_kind").value<EDGE_KIND>();
    auto edge_direction = state_Active_Connecting->property("edge_direction").value<EDGE_DIRECTION>();
    
    auto item_map = viewController->getValidEdges(edge_kind);
    
    for(auto item : item_map.values(edge_direction)){
        emit viewController->vc_highlightItem(item->getID(), false);
    }

    
    if(connect_line){
        QPointF scene_pos = edge_direction == EDGE_DIRECTION::SOURCE ? connect_line->get_end_point() : connect_line->get_begin_point();
        EntityItem* otherItem = getEntityAtPos(scene_pos);
        if(otherItem){
            viewController->constructEdges(otherItem->getID(), edge_kind, edge_direction);
        }
        connect_line->setVisible(false);
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

    if(event->modifiers().testFlag(Qt::AltModifier)){
        switch(event->key()){
            case Qt::Key_Left:
            case Qt::Key_Right:
            case Qt::Key_Up:
            case Qt::Key_Down:{
                auto selectedItems = getSelectedItems();
                if(selectedItems.count() == 1){
                    auto item = selectedItems.first();
                    if(item->isNodeItem()){
                        ShiftOrderInParent((NodeItem*) item, event->key());
                        event->setAccepted(true);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
}

void NodeView::ShiftOrderInParent(NodeItem* item, int key){
    auto node_item = qobject_cast<BasicNodeItem*>(item);

    if(node_item){
        auto parent_stack_item = node_item->getParentStackContainer();
        if(parent_stack_item){
            auto index = StackNodeItem::GetCellIndex(node_item); 
            //Get the orientation of the index
            auto orientation = parent_stack_item->getCellOrientation(index);

            auto increment = key == Qt::Key_Right || key == Qt::Key_Down;
            
            auto move_index = false;
            switch(key){
                case Qt::Key_Left:
                case Qt::Key_Right:{
                    move_index = orientation == Qt::Horizontal;
                    break;
                }
                case Qt::Key_Up:
                case Qt::Key_Down:{
                    move_index = orientation == Qt::Vertical;
                    break;
                }
                default:
                    return;
            }

            auto action_controller = viewController->getActionController();
            QAction* action = 0;
            if(move_index){
                if(increment){
                    action = action_controller->edit_incrementIndex;
                }else{
                    action = action_controller->edit_decrementIndex;
                }
            }else{
                if(increment){
                    action = action_controller->edit_incrementRow;
                }else{
                    action = action_controller->edit_decrementRow;
                }
            }
            if(action && action->isEnabled()){
                action->trigger();
            }
    }
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
        event->accept();
    }

    if(event->button() == Qt::LeftButton){
        emit trans_RubberbandMode2RubberbandMode_Selecting();

        if(state_Active_RubberbandMode_Selecting->active()){
            rubberband_lastPos = event->pos();
            if(rubberband){
                rubberband->setGeometry(QRect(rubberband_lastPos, rubberband_lastPos));
            }
            event->accept();
        }else{
            EntityItem* item = getEntityAtPos(scenePos);
            if(!item){
                clearSelection();
                event->accept();
            }
        }
    }

    if(event->button() == Qt::MiddleButton){
        EntityItem* item = getEntityAtPos(scenePos);
        if(!item){
            FitToScreen();
            event->accept();
        }
    }

    QGraphicsView::mousePressEvent(event);
}

void NodeView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF scenePos = mapToScene(event->pos());
    

    if(isPanning){
        //Calculate the distance in screen pixels travelled
        pan_distance += distance(event->pos(), pan_lastPos);
        
        auto scene_delta = scenePos - pan_lastScenePos;

        //Pan the Canvas
        translate(scene_delta);
        pan_lastPos = event->pos();
        pan_lastScenePos = mapToScene(event->pos());
        event->accept();
    }

    if(state_Active_RubberbandMode_Selecting->active()){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        event->accept();
    }else if(state_Active_Connecting->active()){
        
        auto edge_direction = state_Active_Connecting->property("edge_direction").value<EDGE_DIRECTION>();

        auto item = getEntityAtPos(scenePos);
        if(item){
            item = item->isHighlighted() ? item : 0;
        }

        if(edge_direction == EDGE_DIRECTION::SOURCE){
            connect_line->set_end_point(scenePos);
        }else{
            connect_line->set_begin_point(scenePos);
        }
        connect_line->setHighlighted(item);
        //Check if what we have is 
        event->accept();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void NodeView::mouseReleaseEvent(QMouseEvent *event)
{
    bool CONTROL = event->modifiers() & Qt::ControlModifier;

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
        event->accept();
    }

    if(state_Active_RubberbandMode_Selecting->active() && event->button() == Qt::LeftButton){
        rubberband->setGeometry(QRect(rubberband_lastPos, event->pos()).normalized());
        selectItemsInRubberband();
        emit trans_RubberbandMode2InActive();
        event->accept();
    }

    if(state_Active_Connecting->active() && event->button() == Qt::LeftButton){
        emit trans_Connecting2InActive();
        event->accept();
    }


    QGraphicsView::mouseReleaseEvent(event);
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
