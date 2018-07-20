#include "nodeitem.h"

#include <math.h>
#include <QStyleOptionGraphicsItem>

#include "../entityitem.h"
#include "../../../../../modelcontroller/entityfactory.h"
#include "../../../../../modelcontroller/Entities/Keys/rowkey.h"

#include <algorithm>


#define RESIZE_RECT_SIZE 4



NodeItem::NodeItem(NodeViewItem *viewItem, NodeItem *parentItem):EntityItem(viewItem, parentItem, EntityItem::NODE)
{
    node_view_item = viewItem;

    setMoveEnabled(true);
    setExpandEnabled(true);

    const auto& grid_size = getGridSize();
    auto margins = QMarginsF(grid_size, grid_size, grid_size, grid_size);
    
    setBodyPadding(margins);
    setMargin(margins);

    //DEFAULTS
    const int height = 20;
    const int width = 55;

    setMinimumHeight(height);
    setMinimumWidth(width);
    setExpandedHeight(height);
    setExpandedWidth(width);

    if(node_view_item){
        connect(node_view_item, &NodeViewItem::edgeAdded, this, &NodeItem::edgeAdded);
        connect(node_view_item, &NodeViewItem::edgeRemoved, this, &NodeItem::edgeRemoved);
        connect(node_view_item, &NodeViewItem::visualEdgeKindsChanged, this, &NodeItem::updateVisualEdgeKinds);
        connect(node_view_item, &NodeViewItem::nestedVisualEdgeKindsChanged2, this, &NodeItem::updateVisualEdgeKinds);
        connect(node_view_item, &NodeViewItem::notificationsChanged, this, &NodeItem::updateNotifications);
        connect(node_view_item, &NodeViewItem::nestedNotificationsChanged, this, &NodeItem::updateNestedNotifications);
    }

    if(parentItem){
        //Lock child in same aspect as parent
        setAspect(parentItem->getAspect());

        
        parentItem->addChildNode(this);
        setPos(QPointF(-1, -1));
    }

    connect(this, &NodeItem::childSizeChanged, this, &NodeItem::childPosChanged);
    connect(this, &NodeItem::childPositionChanged, this, &NodeItem::childPosChanged);


    updateVisualEdgeKinds();
    updateNotifications();

    addHoverFunction(EntityRect::MOVE, std::bind(&NodeItem::moveHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::EXPAND_CONTRACT, std::bind(&NodeItem::expandContractHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::PRIMARY_TEXT, std::bind(&NodeItem::primaryTextHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::SECONDARY_TEXT, std::bind(&NodeItem::secondaryTextHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::TERTIARY_TEXT, std::bind(&NodeItem::tertiaryTextHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::NOTIFICATION_RECT, std::bind(&NodeItem::notificationHover, this, std::placeholders::_1, std::placeholders::_2));
    
    addHoverFunction(EntityRect::CONNECT_SOURCE, std::bind(&NodeItem::edgeKnobHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::CONNECT_TARGET, std::bind(&NodeItem::edgeKnobHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::LOCKED_STATE_ICON, std::bind(&NodeItem::lockHover, this, std::placeholders::_1, std::placeholders::_2));
}

const QList<EDGE_KIND> sorted_edge_kinds = {EDGE_KIND::DEPLOYMENT, EDGE_KIND::QOS, EDGE_KIND::ASSEMBLY, EDGE_KIND::DATA};

void NodeItem::updateVisualEdgeKinds(){
    /*
        TODO: 37.97% function time of Import spends in this call
        Of which 96.33 % is spent in getNestedVisualEdgeKindDirections()
        This is a 10 second improvement on a 30 second model and needs to be patched
    */
    
    //my_visual_edge_kinds.clear();
    //all_visual_edge_kinds.clear();

    QHash<EDGE_DIRECTION, QSet<EDGE_KIND> > new_my_visual_edge_kinds;
    QHash<EDGE_DIRECTION, QSet<EDGE_KIND> > new_all_visual_edge_kinds;



    auto node_view_item = getNodeViewItem();

    if(node_view_item){
        for(const auto&  edge_kind : sorted_edge_kinds){
            for(const auto&  edge_direction : node_view_item->getVisualEdgeKindDirections(edge_kind)){
                new_my_visual_edge_kinds[edge_direction] += edge_kind;
            }
            for(const auto& edge_direction : node_view_item->getNestedVisualEdgeKindDirections(edge_kind)){
                
                new_all_visual_edge_kinds[edge_direction] += edge_kind;
            }
        }
    }

    bool req_update = false;
    if(new_my_visual_edge_kinds != my_visual_edge_kinds){
        my_visual_edge_kinds = new_my_visual_edge_kinds;
        req_update = true;
    }

    if(new_all_visual_edge_kinds != all_visual_edge_kinds){
        all_visual_edge_kinds = new_all_visual_edge_kinds;
        req_update = true;
    }

    if(req_update){
        update();
    }
}

void NodeItem::updateNestedNotifications(){
    if(!isExpanded()){
        updateNotifications();
    }
}

void NodeItem::updateNotifications(){
    notification_counts_.clear();
    auto notifications = isExpanded() ? getViewItem()->getNotifications() : getViewItem()->getNestedNotifications();
    for(auto notification : notifications){
        notification_counts_[notification->getSeverity()] ++;
    }
    update();
}



NodeItem::~NodeItem()
{
    //Unset
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildNode(this);
    }

    //remove children nodes.
    while(child_nodes.size()){
        auto node = *child_nodes.begin();
        removeChildNode(node);
        delete node;
    }

    while(child_edges.size()){
        auto edge = *child_edges.begin();
        removeChildEdge(edge);
        delete edge;
    }
}




QRectF NodeItem::viewRect() const
{
    return EntityItem::viewRect();
}


NodeViewItem *NodeItem::getNodeViewItem() const
{
    return node_view_item;
}

NODE_KIND NodeItem::getNodeKind() const
{
    return node_view_item->getNodeKind();
}

void NodeItem::setRightJustified(bool isRight)
{
    right_justified = isRight;
}

bool NodeItem::isRightJustified() const
{
    return right_justified;
}

void NodeItem::addChildNode(NodeItem *nodeItem)
{
    //If we have added a child, and there is only one. emit a signal
    if(!child_nodes.contains(nodeItem)){
        nodeItem->setParentItem(this);
        connect(nodeItem, &EntityItem::sizeChanged, [=](){childSizeChanged(nodeItem);});
        connect(nodeItem, &EntityItem::positionChanged, [=](){childPositionChanged(nodeItem);});
        connect(nodeItem, &NodeItem::indexChanged, [=](){childIndexChanged(nodeItem);});
        
        child_nodes += nodeItem;
        nodeItem->setBaseBodyColor(getBaseBodyColor());

        nodeItem->setVisible(isExpanded());
        childPosChanged(nodeItem);
        emit childCountChanged();
    }
}



void NodeItem::removeChildNode(NodeItem* nodeItem)
{
    //If we have removed a child, and there is no children left. emit a signal
    if(child_nodes.remove(nodeItem) > 0){
        //Unset child moving.
        nodeItem->unsetParent();

        childPosChanged(nodeItem);
        emit childCountChanged();
    }
}



int NodeItem::getSortOrder() const
{
    const QString key_index("index");
    
    bool okay = false;
    auto index = getData(key_index).toInt(&okay);

    if(!okay){
        index = -1;
    }
    return index;
}

int NodeItem::getSortOrderRow() const{
    const QString key_row("row");

    bool okay = false;
    
    auto row = getData(key_row).toInt(&okay);

    if(!okay){
        row = 0;
    }
    return row;
}

int NodeItem::getSortOrderRowSubgroup() const{
    const QString key_column("column");
    bool okay = false;
    auto column = getData(key_column).toInt(&okay);

    if(!okay){
        column = 0;
    }
    return column;
}

bool NodeItem::hasChildNodes() const
{
    return child_nodes.size();
}

QList<NodeItem *> NodeItem::getChildNodes() const
{
    return child_nodes.toList();
}

QList<NodeItem*> NodeItem::getSortedChildNodes() const{
    auto nodes = getChildNodes();
    std::sort(nodes.begin(), nodes.end(), [](const NodeItem* n1, const NodeItem* n2){
        return n1->getSortOrder() < n2->getSortOrder();
    });
    return nodes;
}


QList<EntityItem *> NodeItem::getChildEntities() const
{
    QList<EntityItem*> children;
    for(auto node : child_nodes){
        children += node;
    }
    for(auto edge : child_edges){
        children += edge;
    }
    return children;
}


void NodeItem::addChildEdge(EdgeItem *edgeItem)
{
    if(!child_edges.contains(edgeItem)){
        edgeItem->setParentItem(this);
        child_edges += edgeItem;

        edgeItem->setBaseBodyColor(getBaseBodyColor().darker(120));

        connect(edgeItem, &EntityItem::positionChanged, this, [=](){childPosChanged(0);});
        edgeItem->setVisible(isExpanded());
    }
}

void NodeItem::removeChildEdge(EdgeItem *edgeItem)
{
    if(child_edges.contains(edgeItem)){
        edgeItem->unsetParent();
        child_edges.remove(edgeItem);
    }
}

QList<EdgeItem *> NodeItem::getChildEdges() const
{
    return child_edges.toList();
}



bool NodeItem::setMoveFinished()
{
    setPos(getNearestGridPoint());
    return EntityItem::setMoveFinished();
}


QRectF NodeItem::boundingRect() const
{
    QRectF rect;
    rect = getElementRect(EntityRect::BODY) | getElementRect(EntityRect::HEADER);
    rect += margin;
    return rect;
}

QRectF NodeItem::currentRect() const
{
    return getElementRect(EntityRect::BODY) | getElementRect(EntityRect::HEADER);
}

QRectF NodeItem::gridRect() const
{
    return getElementRect(EntityRect::BODY).marginsRemoved(getBodyPadding());
}


QRectF NodeItem::bodyRect() const
{
    QRectF rect;
    rect.setTopLeft(getMarginOffset());
    rect.setWidth(getWidth());
    rect.setHeight(getHeight());

    rect.setTop(rect.top() + getMinimumHeight());
    return rect;
}

QRectF NodeItem::headerRect() const
{
    QRectF rect;
    rect.setTopLeft(getMarginOffset());
    rect.setWidth(getWidth());
    rect.setHeight(getMinimumHeight());
    return rect;
}


QRectF NodeItem::childrenRect() const
{
    return _childRect;

}

QSizeF NodeItem::getSize() const
{
    QSizeF size;
    size.setWidth(getWidth());
    size.setHeight(getHeight());
    return size;
}

void NodeItem::adjustExpandedSize(QSizeF delta)
{
    setExpandedSize(getExpandedSize() + delta);
}

void NodeItem::setMinimumWidth(qreal width)
{
    if(min_width != width){
        min_width = width;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
        }
    }
}

void NodeItem::setMinimumHeight(qreal height)
{
    if(min_height != height){
        min_height = height;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
        }
    }
}

void NodeItem::setExpandedWidth(qreal width)
{
    //Limit by the size of all contained children.
    qreal minWidth = childrenRect().right() - getMargin().left();
    //Can't shrink smaller than minimum
    minWidth = qMax(minWidth, model_width);
    minWidth = qMax(minWidth, min_width);
    width = qMax(width, minWidth);




    if(expanded_width != width){
        if(width > model_width){
            model_width = -1;
        }

        expanded_width = width;

        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
        }
    }
}

void NodeItem::setExpandedHeight(qreal height)
{
    //Limit by the size of all contained children.
    qreal minHeight = childrenRect().bottom() - getMargin().top();
    //Can't shrink smaller than minimum
    minHeight = qMax(minHeight, model_height);
    minHeight = qMax(minHeight, min_height);
    height = qMax(height, minHeight);



    if(expanded_height != height){
        if(height > model_height){
            model_height = -1;
        }
        expanded_height = height;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
        }
    }
}

void NodeItem::setExpandedSize(QSizeF size)
{

    setExpandedWidth(size.width());
    setExpandedHeight(size.height());
}


qreal NodeItem::getExpandedWidth() const
{
    return expanded_width;
}

qreal NodeItem::getExpandedHeight() const
{
    return expanded_height;
}

QSizeF NodeItem::getExpandedSize() const
{
    return QSizeF(expanded_width, expanded_height);
}

qreal NodeItem::getMinimumWidth() const
{
    return min_width;
}

qreal NodeItem::getMinimumHeight() const
{
    return min_height;
}

void NodeItem::setMargin(QMarginsF margin)
{
    if(this->margin != margin){
        prepareGeometryChange();
        this->margin = margin;
        update();
    }
}

void NodeItem::setBodyPadding(QMarginsF padding)
{
    if(this->bodyPadding != padding){
        this->bodyPadding = padding;
    }
}

QPointF NodeItem::getMarginOffset() const
{
    return QPointF(margin.left(), margin.top());
}

QPointF NodeItem::getBottomRightMarginOffset() const
{
    return QPointF(margin.right(), margin.bottom());
}

QPointF NodeItem::getTopLeftSceneCoordinate() const
{
    return sceneBoundingRect().topLeft();
}

qreal NodeItem::getWidth() const
{
    if(isExpanded()){
        return getExpandedWidth();
    }else{
        return getMinimumWidth();
    }
}

qreal NodeItem::getHeight() const
{
    if(isExpanded()){
        return getExpandedHeight();
    }else{
        return getMinimumHeight();
    }
}

const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& NodeItem::getAllVisualEdgeKinds() const{
    return all_visual_edge_kinds;
}

const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& NodeItem::getVisualEdgeKinds() const{
    return my_visual_edge_kinds;
}

const QHash<EDGE_DIRECTION, QSet<EDGE_KIND> >& NodeItem::getCurrentVisualEdgeKinds() const{
    return isExpanded() ? getVisualEdgeKinds() : getAllVisualEdgeKinds();
}


void NodeItem::setAspect(VIEW_ASPECT aspect)
{
    this->aspect = aspect;
}

VIEW_ASPECT NodeItem::getAspect()
{
    return aspect;
}

QMarginsF NodeItem::getMargin() const
{
    return margin;
}

QMarginsF NodeItem::getBodyPadding() const
{
    return bodyPadding;
}

void NodeItem::setExpanded(bool expand)
{
    if(isExpanded() != expand){
        //Call the base class
        EntityItem::setExpanded(expand);

        prepareGeometryChange();

        for(auto child : getChildEntities()){
            child->setVisible(isExpanded());
        }

        update();
        updateNotifications();
        updateVisualEdgeKinds();
        emit sizeChanged();
    }
}



void NodeItem::setPrimaryTextKey(QString key)
{
    if(primaryTextKey != key){
        primaryTextKey = key;
        addRequiredData(key);
    }
}

void NodeItem::setSecondaryTextKey(QString key)
{
    if(secondaryTextKey != key){
        secondaryTextKey = key;
        addRequiredData(key);
    }
}

void NodeItem::setTertiaryTextKey(QString key)
{
    if(tertiaryTextKey != key){
        tertiaryTextKey = key;
        addRequiredData(key);
    }
}



QString NodeItem::getPrimaryTextKey() const
{
    return primaryTextKey;
}

QString NodeItem::getTertiaryTextKey() const
{
    return tertiaryTextKey;
}


QString NodeItem::getSecondaryTextKey() const
{
    return secondaryTextKey;
}

bool NodeItem::gotPrimaryTextKey() const
{
    return !primaryTextKey.isEmpty();
}

bool NodeItem::gotSecondaryTextKey() const
{
    return !secondaryTextKey.isEmpty();
}

bool NodeItem::gotTertiaryTextKey() const
{
    return !tertiaryTextKey.isEmpty();
}

QString NodeItem::getPrimaryText() const
{
    if(!primaryTextKey.isEmpty()){
        return getData(primaryTextKey).toString();
    }
    return QString();
}

QString NodeItem::getSecondaryText() const
{
    if(!secondaryTextKey.isEmpty()){
        return getData(secondaryTextKey).toString();
    }
    return QString();
}

QString NodeItem::getTertiaryText() const
{
    if(!tertiaryTextKey.isEmpty()){
        return getData(tertiaryTextKey).toString();
    }
    return QString();
}

void NodeItem::dataChanged(const QString& key_name, const QVariant& data){
    if(isDataRequired(key_name)){
        if(key_name == "width" || key_name == "height"){
            qreal realData = data.toReal();

            if(key_name == "width"){
                model_width = -1;
                setExpandedWidth(realData);
                model_width = getExpandedWidth();
            }else if(key_name == "height"){
                model_height = -1;
                setExpandedHeight(realData);
                model_height = getExpandedHeight();
            }
            setExpandedSize(getGridAlignedSize());
        }else if(key_name == "isExpanded"){
            bool boolData = data.toBool();
            setExpanded(boolData);
        }else if(key_name == "readOnly"){
            update();
        }else if(key_name =="index"){
            emit indexChanged();
        }else if(key_name =="row"){
            emit indexChanged();
        }else if(key_name =="column"){
            emit indexChanged();
        }

        if(key_name == primaryTextKey || key_name == secondaryTextKey || key_name == tertiaryTextKey){
            update();
        }
    }
    EntityItem::dataChanged(key_name, data);
}

void NodeItem::childPosChanged(EntityItem*)
{
    //Update the child rect.    
    QRectF rect;

    for(auto child : getChildEntities()){
        if(child->isNodeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }else if(child->isEdgeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }
    }
    _childRect = rect;
    resizeToChildren();
}

void NodeItem::edgeAdded(EDGE_DIRECTION direction, EDGE_KIND edge_kind, int ID)
{
    attached_edges.insert({direction, edge_kind}, ID);
    switch(edge_kind){
        case EDGE_KIND::DEPLOYMENT:
        case EDGE_KIND::QOS:
            update();
            break;
    default:
        return;
    }
}

void NodeItem::edgeRemoved(EDGE_DIRECTION direction, EDGE_KIND edge_kind, int ID)
{
    attached_edges.remove({direction, edge_kind}, ID);

    switch(edge_kind){
        case EDGE_KIND::DEPLOYMENT:
        case EDGE_KIND::QOS:
            update();
            break;
default:
        return;
    }
}

QSizeF NodeItem::getGridAlignedSize(QSizeF size) const
{
    if(size.isEmpty()){
        size = getExpandedSize();
    }

    qreal gridSize = getGridSize();
    //Get the nearest next grid line.
    qreal modHeight = fmod(size.height(), gridSize);
    qreal modWidth = fmod(size.width(), gridSize);

    if(modHeight != 0){
        size.rheight() += (gridSize - modHeight);
    }
    if(modWidth != 0){
        size.rwidth() += (gridSize - modWidth);
    }

    return size;
}

void NodeItem::resizeToChildren()
{
    setExpandedWidth(-1);
    setExpandedHeight(-1);
    update();
}




void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    //Clip yo!
    painter->setClipRect(option->exposedRect);

    RENDER_STATE state = getRenderState(lod);


    EntityItem::paint(painter, option, widget);
    {
        if(gotPrimaryTextKey()){
            auto is_protected = isDataProtected(getPrimaryTextKey());

            if(state >= RENDER_STATE::REDUCED){
                if(isHovered(EntityRect::PRIMARY_TEXT) && !is_protected){
                    painter->save();
                    painter->setBrush(Qt::NoBrush);
                    painter->setPen(QPen(getTextColor(), 0, Qt::DotLine));
                    painter->drawRect(getElementRect(EntityRect::PRIMARY_TEXT));
                    painter->restore();
                }
            } 

            painter->save();
            
            painter->setPen(getTextColor());
            renderText(painter, lod, EntityRect::PRIMARY_TEXT, getPrimaryText());
            painter->restore();
        }

        if(gotSecondaryTextKey()){
            auto is_protected = isDataProtected(getSecondaryTextKey());
            if(state >= RENDER_STATE::REDUCED){
                if(!is_protected){
                    painter->save();
                    painter->setBrush(getBodyColor());
                    painter->setPen(Qt::NoPen);

                    if(isHovered(EntityRect::SECONDARY_TEXT)){
                        painter->setPen(QPen(getTextColor(), 0, Qt::DotLine));
                    }
                    painter->drawRect(getElementRect(EntityRect::SECONDARY_TEXT));
                    painter->restore();
                }
            }

            painter->save();
            painter->setPen(getTextColor());
            renderText(painter, lod, EntityRect::SECONDARY_TEXT, getSecondaryText());
            painter->restore();
        }

        if(gotTertiaryTextKey()){
            auto is_protected = isDataProtected(getTertiaryTextKey());
            if(state >= RENDER_STATE::REDUCED){
                if(!is_protected){
                    painter->save();
                    painter->setBrush(getBodyColor());
                    painter->setPen(Qt::NoPen);

                    if(isHovered(EntityRect::TERTIARY_TEXT)){
                        painter->setPen(QPen(getTextColor(), 0, Qt::DotLine));
                    }
                    painter->drawRect(getElementRect(EntityRect::TERTIARY_TEXT));
                    painter->restore();
                }
            }

            painter->save();
            painter->setPen(getTextColor());
            renderText(painter, lod, EntityRect::TERTIARY_TEXT, getTertiaryText());
            painter->restore();
        }
    }

    if(state >= RENDER_STATE::REDUCED){
        painter->save();
        const auto& my_edges = getVisualEdgeKinds();

        const auto& edges = getCurrentVisualEdgeKinds();
        
        for(auto edge_direction : edges.keys()){
            for(auto edge_kind : edges.value(edge_direction)){
                auto icon_rect = getEdgeConnectIconRect(edge_direction, edge_kind);
                auto inner_rect = icon_rect.adjusted(.75,.75,-.75,-.75);
                bool is_hovered = IsEdgeKnobHovered({edge_direction, edge_kind});
                bool got_edge = attached_edges.contains({edge_direction, edge_kind});
                bool my_edge = my_edges[edge_direction].contains(edge_kind);
                
                QColor brush_color;
                if(is_hovered){
                    brush_color = getPen().color();
                }else{
                    brush_color = getHeaderColor();
                }

                painter->setBrush(brush_color);
                painter->drawEllipse(icon_rect);

                painter->setBrush(getBodyColor());
                painter->drawEllipse(inner_rect);

                if(got_edge || is_hovered){
                    painter->setOpacity(1);
                }else{
                    painter->setOpacity(.60);   
                }

                
                if(!my_edge){
                    paintPixmap(painter, lod, icon_rect, "EntityIcons", EntityFactory::getEdgeKindString(edge_kind) + "_Gray");

                }else{
                    paintPixmap(painter, lod, icon_rect, "EntityIcons", EntityFactory::getEdgeKindString(edge_kind));
                }
                painter->setOpacity(1);
            }
        }

        
        painter->restore();
    }


    if(state >= RENDER_STATE::REDUCED){
        painter->save();

        for(auto severity : Notification::getSortedSeverities()){
            if(notification_counts_.contains(severity)){
                auto severity_str = Notification::getSeverityString(severity);
                auto count = notification_counts_[severity];
                auto rect = getNotificationRect(severity);
                auto inner_rect = rect.adjusted(.5,.5,-.5,-.5);
                bool is_hovered = hovered_notifications_.contains(severity);

                painter->setPen(Qt::NoPen);
                painter->setBrush(is_hovered ? getHighlightColor() : getHeaderColor());
                painter->drawEllipse(rect);

                const auto icon_color = is_hovered ? getHighlightTextColor() : Theme::theme()->getSeverityColor(severity);

                if(is_hovered){
                    QString icon_path = "number9+";
                    if(count < 10){
                        icon_path = "number" + QString::number(count);
                    }
                    paintPixmap(painter, lod, inner_rect, "Icons", icon_path, icon_color);
                }else{
                    paintPixmap(painter, lod, inner_rect, "Notification", severity_str, icon_color);
                }
            }
        }
        {
            if(isExpandEnabled()){
                const auto icon_path = isExpanded() ? "circleArrowDownDark" : "circleArrowRightDark";
                const auto brush_color = getHeaderColor();
                const auto icon_color = isHovered(EntityRect::MAIN_ICON) ? getHighlightColor() : getAltTextColor();
                auto rect = getExpandStateRect();
                painter->setPen(Qt::NoPen);
                painter->setBrush(brush_color);
                painter->drawEllipse(rect);
                paintPixmap(painter, lod, rect, "Icons", icon_path, icon_color);
            }
        }

        painter->restore();
    }
}



QPainterPath NodeItem::getElementPath(EntityRect rect) const
{
    switch(rect){
        case EntityRect::SHAPE:{
            //Selection Area is the Center Circle and Arrow Heads
            QPainterPath path = EntityItem::getElementPath(EntityRect::SHAPE);
            path.setFillRule(Qt::WindingFill);

            const auto& edge_knobs = getCurrentVisualEdgeKinds();

            //Add Edge Knobs
            for(auto edge_direction : edge_knobs.keys()){
                for(auto edge_kind : edge_knobs.value(edge_direction)){
                    if(edge_kind != EDGE_KIND::NONE){
                        path.addEllipse(getEdgeConnectIconRect(edge_direction, edge_kind));
                    }
                }
            }

            //Add Severity Knobs
            for(auto severity : notification_counts_.uniqueKeys()){
                path.addEllipse(getNotificationRect(severity));
            }
            return path;//return path.simplified();
        }
        default:
            break;
    }
    return EntityItem::getElementPath(rect);
}


QRectF NodeItem::getEdgeConnectIconRect(EDGE_DIRECTION direction, EDGE_KIND kind) const{
    auto rect = getEdgeConnectRect(direction, kind);
    auto center = rect.center();
    //Squarify
    if(rect.height() > rect.width()){
        rect.setHeight(rect.width());
    }else if(rect.width() > rect.height()){
        rect.setWidth(rect.height());
    }
    rect.moveCenter(center);
    return rect;
}



QRectF NodeItem::getEdgeConnectRect(EDGE_DIRECTION direction, EDGE_KIND kind) const{
    //Get the total rect
    auto rect = getEdgeDirectionRect(direction);

    if(kind != EDGE_KIND::NONE){
        const auto& edge_knobs = getCurrentVisualEdgeKinds();
        const auto& edge_set = edge_knobs.value(direction);
        double pos = 0;
        {
            int current_index = 0;
            //COUNT THEM
            for(const auto& e_k : sorted_edge_kinds){
                if(e_k == kind){
                    break;
                }else if(edge_set.contains(e_k)){
                    current_index++;
                }
            }
            pos = current_index;
        }
        
        //Get our position 
        double count = edge_set.size();
        if(count == 0){
            count = 1;
        }
        auto top_left = rect.topLeft();

        //Adjust the height
        auto item_height = rect.height() / count;
        rect.setHeight(item_height);
        //Offset the position
        top_left.ry() += (pos * item_height);
        rect.moveTopLeft(top_left);
        
        
    }
    return rect;
}

QPointF NodeItem::getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND kind) const{
    auto rect = getEdgeConnectRect(direction, kind);
    qreal y = rect.center().y();
    qreal x = direction == EDGE_DIRECTION::SOURCE ? rect.right() : rect.left();
    return mapToScene(x,y);
}

QRectF NodeItem::getEdgeDirectionRect(EDGE_DIRECTION direction) const{
    switch(direction){
        case EDGE_DIRECTION::SOURCE:{
            return getElementRect(EntityRect::CONNECT_SOURCE);
        }
        case EDGE_DIRECTION::TARGET:{
            return getElementRect(EntityRect::CONNECT_TARGET);
        }
    }
    return QRectF();
}

QRectF NodeItem::getNotificationRect() const{
    return getElementRect(EntityRect::NOTIFICATION_RECT);
}

QRectF NodeItem::getExpandStateRect() const{
    QRectF rect;
    const auto& icon = getElementRect(EntityRect::MAIN_ICON);
    auto icon_size = icon.size() / 4.0;
    const auto& minimum_size = smallIconSize() / 3;

    if(icon_size.width() < minimum_size.width()){
        icon_size = minimum_size;
    }

    
    rect.setSize(icon_size);
    rect.moveBottomLeft(icon.bottomLeft());
    return rect;
}

QRectF NodeItem::getNotificationRect(Notification::Severity severity) const{
    int offset = 2;
    int last_used = 0;
    for(auto sev : Notification::getSortedSeverities()){
        if(notification_counts_.contains(sev)){
            if(severity == sev){
                auto rect = getNotificationRect();
                auto top_left = rect.topLeft();

                QRectF icon_rect;
                auto item_width = rect.height();
                icon_rect.setWidth(item_width);
                icon_rect.setHeight(item_width);

                top_left.rx() += offset + (last_used * (item_width + 2));
                icon_rect.moveTopLeft(top_left);
                return icon_rect;
            }
            last_used++;
        }
    }
    return QRectF();
}



void NodeItem::clearEdgeKnobPressedState(){
    //Clear the Knob Edge data
    edge_knob_pressed = false;
    edge_knob_dragged = false;
    pressed_edge_knob_rect = QRectF();
    pressed_edge_knob_direction = EDGE_DIRECTION::SOURCE;
    pressed_edge_knob_kind = EDGE_KIND::NONE;
}

void NodeItem::clearNotificationKnobPressedState(){
    //Clear the Knob Edge data
    notification_pressed = false;
    pressed_notification_rect = QRectF();
    pressed_notification = Notification::Severity::NONE;
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    EntityItem::mousePressEvent(event);
    bool caughtEdge = false;

    clearEdgeKnobPressedState();
    clearNotificationKnobPressedState();

    for(const auto& direction : my_visual_edge_kinds.keys()){
        const auto& edge_kinds = my_visual_edge_kinds.value(direction);
        for(const auto& edge_kind : edge_kinds){
            auto rect = getEdgeConnectRect(direction, edge_kind);
            if(rect.contains(event->pos())){
                edge_knob_pressed = true;
                pressed_edge_knob_rect = rect;
                pressed_edge_knob_direction = direction;
                pressed_edge_knob_kind = edge_kind;
                caughtEdge = true;
                break;
            }
        }
        if(caughtEdge){
            break;
        }
    }

    for(auto severity : notification_counts_.uniqueKeys()){
        auto rect = getNotificationRect(severity);
        if(rect.contains(event->pos())){
            notification_pressed = true;
            pressed_notification_rect = rect;
            pressed_notification = severity;
            break;
        }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   if(edge_knob_pressed && !edge_knob_dragged){
       emit req_connectEdgeMenu(event->scenePos(), pressed_edge_knob_kind, pressed_edge_knob_direction);
       clearEdgeKnobPressedState();
   }

   if(notification_pressed){
       if(pressed_notification_rect.contains(event->pos() )){
           emit getViewItem()->showNotifications(pressed_notification);
        }
        clearNotificationKnobPressedState();
   }

    EntityItem::mouseReleaseEvent(event);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(edge_knob_pressed){
        //If we have moved out of the rect we pressed, start edge drawing
        if(!edge_knob_dragged && !pressed_edge_knob_rect.contains(event->pos())){
            auto pos = mapToScene(pressed_edge_knob_rect.center());
            emit req_connectEdgeMode(pos, pressed_edge_knob_kind, pressed_edge_knob_direction);
            clearEdgeKnobPressedState();
        }
    }else{
        EntityItem::mouseMoveEvent(event);
    }
}



void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    auto primary_key = getPrimaryTextKey();
    auto secondary_key = getSecondaryTextKey();
    auto tertiary_key = getTertiaryTextKey();

    if(!primary_key.isEmpty() && !isDataProtected(primary_key)){
        if(event->button() == Qt::LeftButton && getElementPath(EntityRect::PRIMARY_TEXT).contains(event->pos())){
            emit req_editData(getViewItem(), primary_key);
        }
    }
    if(!secondary_key.isEmpty() && !isDataProtected(secondary_key)){
        if(event->button() == Qt::LeftButton && getElementPath(EntityRect::SECONDARY_TEXT).contains(event->pos())){
            emit req_editData(getViewItem(), secondary_key);
        }
    }

    if(!tertiary_key.isEmpty() && !isDataProtected(tertiary_key)){
        if(event->button() == Qt::LeftButton && getElementPath(EntityRect::TERTIARY_TEXT).contains(event->pos())){
            emit req_editData(getViewItem(), tertiary_key);
        }
    }


    EntityItem::mouseDoubleClickEvent(event);
}



QPointF NodeItem::getTopLeftOffset() const
{
    return getMarginOffset();
}

QRectF NodeItem::getElementRect(EntityItem::EntityRect rect) const
{
    switch(rect){
    case EntityRect::HEADER:
        return headerRect();
    case EntityRect::BODY:
        return bodyRect();
    default:
        break;
    }
    return EntityItem::getElementRect(rect);
}


void NodeItem::paintBackground(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    painter->setClipRect(option->exposedRect);
    painter->setPen(Qt::NoPen);

    auto IS_BLOCK = state == RENDER_STATE::BLOCK;

    if(state >= RENDER_STATE::BLOCK){
        
        if(isExpanded()){
            //Paint the Body
            painter->setBrush(getBodyColor());
            painter->drawRect(getElementRect(EntityRect::BODY));
        }

        //Paint the Header
        painter->setBrush(getHeaderColor());
        painter->drawRect(getElementRect(EntityRect::HEADER));
    }
}

bool NodeItem::isExpandEnabled(){
    return hasChildNodes() && EntityItem::isExpandEnabled();
}

void NodeItem::expandContractHover(bool hovered, const QPointF&){
    if(isExpandEnabled()){
        if(hovered){
            AddTooltip(isExpanded() ? "Double-Click to Contract" : "Double Click to Expand");
        }
    }
}

void NodeItem::lockHover(bool hovered, const QPointF&){
    if(isReadOnly()){
        if(hovered){
            AddTooltip("Entity is read-only");
        }
    }
}




void NodeItem::primaryTextHover(bool hovered, const QPointF&){
    if(gotPrimaryTextKey()){
        const auto& data_key = getPrimaryTextKey();
        const auto& data = getPrimaryText();
        if(hovered){
            QString tooltip = "\"" + data_key + "\" = " + data;
            if(!isDataProtected(data_key)){
                tooltip += "\nDouble-Click to edit";
            }
            AddTooltip(tooltip);
        }
    }
}

void NodeItem::secondaryTextHover(bool hovered, const QPointF&){
    if(gotSecondaryTextKey()){
        const auto& data_key = getSecondaryTextKey();
        const auto& data = getSecondaryText();
        if(hovered){
            QString tooltip = "[" + data_key + "] = " + data;
            if(!isDataProtected(data_key)){
                tooltip += "\nDouble-Click to edit";
            }
            AddTooltip(tooltip);
        }
    }
}

void NodeItem::tertiaryTextHover(bool hovered, const QPointF&){
    if(gotTertiaryTextKey()){
        const auto& data_key = getTertiaryTextKey();
        const auto& data = getTertiaryText();
        if(hovered){
            QString tooltip = "[" + data_key + "] = " + data;
            if(!isDataProtected(data_key)){
                tooltip += "\nDouble-Click to edit";
            }
            AddTooltip(tooltip);
        }
    }
}

void NodeItem::notificationHover(bool hovered, const QPointF& pos){
    bool should_update = false;
    
    for(auto severity : notification_counts_.uniqueKeys()){
        auto in_rect = hovered && getNotificationRect(severity).contains(pos);
        
        if(SetNotificationHovered(severity, in_rect)){
            should_update = true;
        }
        if(in_rect){
            AddTooltip("Double-Click to show attached <" + Notification::getSeverityString(severity)+ "> Notifications");
        }
    }

    if(should_update){
        update();
    }
}

void NodeItem::edgeKnobHover(bool hovered, const QPointF& pos){
    bool should_update = false;

    const auto& my_edges = getVisualEdgeKinds();

    for(const auto& direction : my_visual_edge_kinds.keys()){
        auto edge_kinds = my_visual_edge_kinds.value(direction);
        edge_kinds.insert(EDGE_KIND::NONE);
        for(auto edge_kind : edge_kinds){
            QPair<EDGE_DIRECTION, EDGE_KIND> edge_knob = {direction, edge_kind};

            auto in_rect = hovered && getEdgeConnectRect(direction, edge_kind).contains(pos);

            if(SetEdgeKnobHovered(edge_knob, in_rect)){
                should_update = true;
            }

            if(in_rect && my_edges[direction].contains(edge_kind)){
                AddCursor(Qt::ArrowCursor);
                AddTooltip("Click to show <" + EntityFactory::getPrettyEdgeKindString(edge_kind)+ "> Connect Menu.\nOr Click and drag to enter connect mode.");
            }
        }
    }


    if(should_update){
        update();
    }
}

bool NodeItem::IsNotificationHovered(Notification::Severity severity) const{
    return hovered_notifications_.contains(severity);
}

bool NodeItem::SetNotificationHovered(Notification::Severity severity, bool hovered){
    if(hovered && !IsNotificationHovered(severity)){
        hovered_notifications_.insert(severity);
    }else if(!hovered && IsNotificationHovered(severity)){
        hovered_notifications_.remove(severity);
    }else{
        return false;
    }
    return true;
}


bool NodeItem::IsEdgeKnobHovered(const QPair<EDGE_DIRECTION, EDGE_KIND>& edge_knob) const{
    return hovered_edge_kinds.contains(edge_knob);
}

bool NodeItem::SetEdgeKnobHovered(const QPair<EDGE_DIRECTION, EDGE_KIND>& edge_knob, bool hovered){
    if(hovered && !IsEdgeKnobHovered(edge_knob)){
        hovered_edge_kinds.insert(edge_knob);
    }else if(!hovered && IsEdgeKnobHovered(edge_knob)){
        hovered_edge_kinds.remove(edge_knob);
    }else{
        return false;
    }
    return true;
}



void NodeItem::moveHover(bool handle, const QPointF&){
    if(handle && isMoveEnabled()){
        AddTooltip("Click and drag to move entity");
        AddCursor(Qt::OpenHandCursor);
    }
}
