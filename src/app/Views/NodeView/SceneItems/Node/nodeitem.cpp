#include "nodeitem.h"

#include <math.h>
#include <QStyleOptionGraphicsItem>

#include "../entityitem.h"
#include "../../../../../modelcontroller/entityfactory.h"
#include "../../../../../modelcontroller/Entities/Keys/rowkey.h"

#include <algorithm>


#define RESIZE_RECT_SIZE 4



NodeItem::NodeItem(NodeViewItem *viewItem, NodeItem *parentItem, NodeItem::KIND kind):EntityItem(viewItem, parentItem, EntityItem::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    modelHeight = 0;
    modelWidth = 0;
    
    gridVisible = false;
    gridEnabled = false;
    hoveredConnect = false;
    resizeEnabled = false;
    ignorePosition = false;
    _rightJustified = false;
    aspect = VIEW_ASPECT::NONE;
    selectedResizeVertex = NodeItem::RectVertex::NONE;
    hoveredResizeVertex = NodeItem::RectVertex::NONE;
    readState = NodeItem::NORMAL;

    visualEdgeKind = EDGE_KIND::NONE;


    nodeViewItem = viewItem;
    nodeItemKind = kind;

    setMoveEnabled(true);
    setGridEnabled(true);
    setSelectionEnabled(true);
    setResizeEnabled(true);
    setExpandEnabled(true);

    //setDefaultPen(Qt::NoPen);

    setUpColors();

    addRequiredData("isExpanded");

    addRequiredData("readOnlyDefinition");
    addRequiredData("snippetID");

    if(nodeViewItem){
        connect(nodeViewItem, &NodeViewItem::edgeAdded, this, &NodeItem::edgeAdded);
        connect(nodeViewItem, &NodeViewItem::edgeRemoved, this, &NodeItem::edgeRemoved);
    }

    if(parentItem){
        //Lock child in same aspect as parent
        setAspect(parentItem->getAspect());

        parentItem->addChildNode(this);
        setPos(getNearestGridPoint());
    }

    connect(this, &NodeItem::childSizeChanged, this, &NodeItem::childPosChanged);
    connect(this, &NodeItem::childPositionChanged, this, &NodeItem::childPosChanged);


    gridLinePen.setColor(getBaseBodyColor().darker(150));
    gridLinePen.setStyle(Qt::DotLine);
    gridLinePen.setWidthF(.5);
}

NodeItem::~NodeItem()
{
    //Unset
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildNode(this);
    }

    //remove children nodes.
    while(!childNodes.isEmpty()){
        int key = childNodes.keys().takeFirst();
        NodeItem* child = childNodes[key];
        removeChildNode(child);
    }

    //remove children nodes.
    while(!childEdges.isEmpty()){
        int key = childEdges.keys().takeFirst();
        removeChildEdge(key);
    }
}




QRectF NodeItem::viewRect() const
{
    return EntityItem::viewRect();
}

NodeItem::KIND NodeItem::getNodeItemKind()
{
    return nodeItemKind;
}

NodeViewItem *NodeItem::getNodeViewItem() const
{
    return nodeViewItem;
}

NODE_KIND NodeItem::getNodeKind() const
{
    return nodeViewItem->getNodeKind();
}

NodeItem::NODE_READ_STATE NodeItem::getReadState() const
{
    return readState;
}

void NodeItem::setRightJustified(bool isRight)
{
    _rightJustified = isRight;
}

bool NodeItem::isRightJustified() const
{
    return _rightJustified;
}

void NodeItem::addChildNode(NodeItem *nodeItem)
{
    int ID = nodeItem->getID();
    //If we have added a child, and there is only one. emit a signal
    if(!childNodes.contains(ID)){
        nodeItem->setParentItem(this);
        connect(nodeItem, &EntityItem::sizeChanged, [=](){childSizeChanged(nodeItem);});
        connect(nodeItem, &EntityItem::positionChanged, [=](){childPositionChanged(nodeItem);});
        connect(nodeItem, &NodeItem::indexChanged, [=](){childIndexChanged(nodeItem);});
        

        
        childNodes[ID] = nodeItem;
        if(childNodes.count() == 1){
            emit gotChildNodes(true);
        }
        nodeItem->setBaseBodyColor(getBaseBodyColor());

        nodeItem->setVisible(isExpanded());
        childPosChanged(nodeItem);
        emit childCountChanged();
    }
}



void NodeItem::removeChildNode(NodeItem* nodeItem)
{
    //If we have removed a child, and there is no children left. emit a signal
    if(childNodes.remove(nodeItem->getID()) > 0){
        if(childNodes.count() == 0){
            emit gotChildNodes(false);
        }
        //Unset child moving.
        nodeItem->unsetParent();

        childPosChanged(nodeItem);
        emit childCountChanged();
    }
}

int NodeItem::getSortOrder() const
{
    if(hasData("index")){
        return getData("index").toInt();
    }else if(hasData("sortOrder")){
        return getData("sortOrder").toInt();
    }
    return -1;
}

int NodeItem::getSortOrderRow() const{
    if(hasData("row")){
        return getData("row").toInt();
    }
    return 0;
}

int NodeItem::getSortOrderRowSubgroup() const{
    if(hasData("column")){
        return getData("column").toInt();
    }
    return 0;
}

bool NodeItem::hasChildNodes() const
{
    return !childNodes.isEmpty();
}

QList<NodeItem *> NodeItem::getChildNodes() const
{
    return childNodes.values();
}

QList<NodeItem*> NodeItem::getSortedChildNodes() const{
    auto children = getChildNodes();
    std::sort(children.begin(), children.end(), [](const NodeItem* n1, const NodeItem* n2){
        return n1->getSortOrder() < n2->getSortOrder();
    });
    return children;
}


QList<EntityItem *> NodeItem::getChildEntities() const
{
    QList<EntityItem*> children;
    foreach(NodeItem* node, getChildNodes()){
        children.append(node);
    }
    foreach(EdgeItem* edge, getChildEdges()){
        children.append(edge);
    }
    return children;
}


void NodeItem::addChildEdge(EdgeItem *edgeItem)
{
    int ID = edgeItem->getID();
    if(!childEdges.contains(ID)){
        edgeItem->setParentItem(this);
        childEdges[ID] = edgeItem;

        edgeItem->setBaseBodyColor(getBaseBodyColor().darker(120));

        connect(edgeItem, &EntityItem::positionChanged, this, [=](){childPosChanged(0);});
        edgeItem->setVisible(isExpanded());
    }
}

void NodeItem::removeChildEdge(int ID)
{
    if(childEdges.contains(ID)){
        EdgeItem* item = childEdges[ID];
        item->unsetParent();
        childEdges.remove(ID);
    }
}

QList<EdgeItem *> NodeItem::getChildEdges() const
{
    return childEdges.values();
}



void NodeItem::setGridEnabled(bool enabled)
{
    if(gridEnabled != enabled){
        gridEnabled = enabled;
        if(gridEnabled){
            updateGridLines();
        }
    }
}

bool NodeItem::isGridEnabled() const
{
    return gridEnabled;
}

void NodeItem::setGridVisible(bool visible)
{
    if(gridVisible != visible){
        gridVisible = visible;
        update();
    }
}

bool NodeItem::isGridVisible() const
{
    return isGridEnabled() && gridVisible;
}

void NodeItem::setResizeEnabled(bool enabled)
{
    if(resizeEnabled != enabled){
        resizeEnabled = enabled;
    }
}

bool NodeItem::isResizeEnabled()
{
    return resizeEnabled;
}

void NodeItem::setChildMoving(EntityItem *child, bool moving)
{
    if(child){
        setGridVisible(moving);
    }
}

void NodeItem::setMoveStarted()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, true);
    }
    EntityItem::setMoveStarted();
}

bool NodeItem::setMoveFinished()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, false);
    }
    setPos(getNearestGridPoint());
    return EntityItem::setMoveFinished();
}

void NodeItem::setResizeStarted()
{
    sizePreResize = getExpandedSize();
    _isResizing = true;

    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, true);
    }
}

bool NodeItem::setResizeFinished()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, false);
    }
    _isResizing = false;
    return getExpandedSize() != sizePreResize;
}



QRectF NodeItem::boundingRect() const
{
    QRectF rect;
    rect.setWidth(margin.left() + margin.right() + getWidth());
    rect.setHeight(margin.top() + margin.bottom() + getHeight());
    return rect;
}


QRectF NodeItem::contractedRect() const
{
    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getMinimumWidth(), getMinimumHeight()));
}

QRectF NodeItem::expandedRect() const
{
    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getExpandedWidth(), getExpandedHeight()));
}

QRectF NodeItem::currentRect() const
{

    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getWidth(), getHeight()));
}


QRectF NodeItem::gridRect() const
{
    return bodyRect().marginsRemoved(getBodyPadding());
}

QRectF NodeItem::expandedGridRect() const
{
    return expandedRect().marginsRemoved(getBodyPadding());
}

QRectF NodeItem::bodyRect() const
{
    return currentRect();
}

QRectF NodeItem::headerRect() const
{
    return currentRect();
}

QRectF NodeItem::translatedHeaderRect() const{
    QRectF rect = headerRect();
    //we should use the bounding rect coordinates!
    rect.translate(pos());
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
    if(minimumWidth != width){
        minimumWidth = width;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
        }
    }
}

void NodeItem::setMinimumHeight(qreal height)
{
    if(minimumHeight != height){
        minimumHeight = height;
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
    qreal minWidth = childrenRect().right() - getMargin().left();// - getBodyPadding().left();// getBodyPadding().right();
    //Can't shrink smaller than minimum
    minWidth = qMax(minWidth, modelWidth);
    minWidth = qMax(minWidth, minimumWidth);
    width = qMax(width, minWidth);




    if(expandedWidth != width){
        if(width > modelWidth){
            modelWidth = -1;
        }

        expandedWidth = width;

        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
            updateGridLines();
        }
    }
}

void NodeItem::setExpandedHeight(qreal height)
{
    //Limit by the size of all contained children.
    qreal minHeight = childrenRect().bottom() - getMargin().top();
    //Can't shrink smaller than minimum
    minHeight = qMax(minHeight, modelHeight);
    minHeight = qMax(minHeight, minimumHeight);
    height = qMax(height, minHeight);



    if(expandedHeight != height){
        if(height > modelHeight){
            modelHeight = -1;
        }
        expandedHeight = height;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
            updateGridLines();
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
    return expandedWidth;
}

qreal NodeItem::getExpandedHeight() const
{
    return expandedHeight;
}

QSizeF NodeItem::getExpandedSize() const
{
    return QSizeF(expandedWidth, expandedHeight);
}

qreal NodeItem::getMinimumWidth() const
{
    return minimumWidth;
}

qreal NodeItem::getMinimumHeight() const
{
    return minimumHeight;
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
        updateGridLines();
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

QMultiMap<EDGE_DIRECTION, EDGE_KIND> NodeItem::getAllVisualEdgeKinds() const{
    QMultiMap<EDGE_DIRECTION, EDGE_KIND> map = getVisualEdgeKinds();

    if(!isExpanded()){
        for(auto child : getChildNodes()){
            auto child_map = child->getAllVisualEdgeKinds();
            for(auto direction : child_map.uniqueKeys()){
                for(auto edge_kind : child_map.values(direction)){
                    if(!map.contains(direction, edge_kind)){
                        map.insert(direction, edge_kind);
                    }
                }
            }
        }
    }
    return map;
}

QMultiMap<EDGE_DIRECTION, EDGE_KIND> NodeItem::getVisualEdgeKinds() const{
    return visual_edge_kinds;
}


QPointF NodeItem::getCenterOffset() const
{
    return contractedRect().center();
}

QPointF NodeItem::getSceneEdgeTermination(bool left) const
{
    qreal y = contractedRect().center().y();
    qreal x = left ? currentRect().left() : currentRect().right();
    return mapToScene(x,y);
}

void NodeItem::setAspect(VIEW_ASPECT aspect)
{
    this->aspect = aspect;
}

VIEW_ASPECT NodeItem::getAspect()
{
    return aspect;
}

void NodeItem::setManuallyAdjusted(NodeItem::RectVertex corner)
{
    bool adjustingWidth = false;
    bool adjustingHeight = false;

    if(corner == NodeItem::RectVertex::TOP_LEFT || corner == NodeItem::RectVertex::TOP_RIGHT || corner == NodeItem::RectVertex::BOTTOM_RIGHT || corner == NodeItem::RectVertex::BOTTOM_LEFT){
        adjustingWidth = true;
        adjustingHeight = true;
    }
    if(corner == NodeItem::RectVertex::RIGHT || corner == NodeItem::RectVertex::LEFT){
        adjustingWidth = true;
    }
    if(corner == NodeItem::RectVertex::TOP || corner == NodeItem::RectVertex::BOTTOM){
        adjustingHeight = true;
    }

    if(adjustingWidth){
        //manuallyAdjustedWidth = -1;
    }
    if(adjustingHeight){
       // manuallyAdjustedHeight = -1;
    }
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
        //Hide/Show Children
        foreach(EntityItem* child, getChildEntities()){
            child->setVisible(isExpanded());
        }

        update();
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



void NodeItem::addVisualEdgeKind(EDGE_DIRECTION direction, EDGE_KIND kind)
{
    if(!visual_edge_kinds.contains(direction, kind)){
        visual_edge_kinds.insert(direction, kind);
        update();
    }
}

void NodeItem::setVisualNodeKind(NODE_KIND kind)
{
    visualNodeKind = kind;
    visualEntityIcon = "ComponentImpl";
    update();
}

EDGE_KIND NodeItem::getVisualEdgeKind() const
{
    return visualEdgeKind;
}

NODE_KIND NodeItem::getVisualNodeKind() const
{
    return visualNodeKind;
}

bool NodeItem::gotVisualNodeKind() const
{
    return visualNodeKind != NODE_KIND::NONE;
}

bool NodeItem::gotVisualEdgeKind() const
{
    return visualEdgeKind != EDGE_KIND::NONE;
}

bool NodeItem::gotVisualButton() const
{
    return gotVisualNodeKind() || gotVisualEdgeKind();
}


QString NodeItem::getPrimaryTextKey() const
{
    return primaryTextKey;
}

QPair<QString, QString> NodeItem::getSecondaryIconPath() const{
    return secondary_icon;
}

void NodeItem::setSecondaryIconPath(QPair<QString, QString> pair){
    secondary_icon = pair;
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


void NodeItem::dataChanged(QString keyName, QVariant data)
{
    if(getRequiredDataKeys().contains(keyName)){
        if(keyName == "width" || keyName == "height"){
            qreal realData = data.toReal();

            if(keyName == "width"){
                modelWidth = -1;
                setExpandedWidth(realData);
                modelWidth = getExpandedWidth();
            }else if(keyName == "height"){
                modelHeight = -1;
                setExpandedHeight(realData);
                modelHeight = getExpandedHeight();
            }
            setExpandedSize(getGridAlignedSize());
        }else if(keyName == "isExpanded"){
            bool boolData = data.toBool();
            setExpanded(boolData);
        }else if(keyName == "readOnlyDefinition" || keyName == "snippetID"){
            updateReadState();
        }else if(keyName == "readOnly"){
            update();
        }else if(keyName == "key" && getNodeKind() == NODE_KIND::MEMBER){
            bool boolData = data.toBool();
            setIconOverlayVisible(boolData);
        }else if(keyName =="index"){
            emit indexChanged();
        }else if(keyName =="row"){
            emit indexChanged();
        }else if(keyName =="column"){
            emit indexChanged();
        }

        if(keyName == primaryTextKey || keyName == secondaryTextKey){
            update();
        }
    }
    EntityItem::dataChanged(keyName, data);
}

void NodeItem::propertyChanged(QString, QVariant)
{
}

void NodeItem::dataRemoved(QString keyName)
{
    if(keyName == "readOnlyDefinition" || keyName == "snippetID"){
        updateReadState();
    }else if(keyName == "readOnly"){
        update();
    }
}

void NodeItem::childPosChanged(EntityItem*)
{
    //Update the child rect.    
    QRectF rect;
    for(auto child : getChildEntities()){
        if(child->isNodeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }else if(child->isEdgeItem()){
            rect = rect.united(child->currentRect());
        }
    }
    _childRect = rect;
    resizeToChildren();
}

void NodeItem::edgeAdded(EDGE_KIND kind)
{
    switch(kind){
        case EDGE_KIND::DEPLOYMENT:
        case EDGE_KIND::QOS:
            update();
            break;
    default:
        return;
    }
}

void NodeItem::edgeRemoved(EDGE_KIND kind)
{
    switch(kind){
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

void NodeItem::updateReadState()
{
    bool readOnlyDef = getData("readOnlyDefinition").toBool();

    NODE_READ_STATE newState = NORMAL;
    if(readOnlyDef){
        newState = READ_ONLY_DEFINITION;
    }else{
        bool readOnlyInstance = getData("snippetID").toBool();
        if(readOnlyInstance){
            newState = READ_ONLY_INSTANCE;
        }
    }
    if(readState != newState){
        readState = newState;
        update();
    }
}

void NodeItem::setUpColors()
{
    qreal blendFactor = 0.6;
    QColor blue = QColor(100,149,237);
    QColor brown = QColor(222,184,135);
    QColor originalColor = EntityItem::getBodyColor();


    readOnlyDefinitionColor = Theme::blendColors(originalColor, brown, blendFactor);
    readOnlyInstanceColor = Theme::blendColors(originalColor, blue, blendFactor);

    resizeColor = originalColor.darker(140);
    resizeColor.setAlpha(120);
}

void NodeItem::resizeToChildren()
{
    setExpandedWidth(-1);
    setExpandedHeight(-1);
}

int NodeItem::getVertexAngle(NodeItem::RectVertex vert) const
{
    switch(vert){
        case NodeItem::RectVertex::TOP:
            return 0;
        case NodeItem::RectVertex::TOP_RIGHT:
            return 45;
        case NodeItem::RectVertex::RIGHT:
            return 90;
        case NodeItem::RectVertex::BOTTOM_RIGHT:
            return 135;
        case NodeItem::RectVertex::BOTTOM:
            return 180;
        case NodeItem::RectVertex::BOTTOM_LEFT:
            return 225;
        case NodeItem::RectVertex::LEFT:
            return 270;
        case NodeItem::RectVertex::TOP_LEFT:
            return 315;
        default:
            return 0;
    }
}

int NodeItem::getResizeArrowRotation(NodeItem::RectVertex vert) const
{
    //Image starts Facing Bottom. which is 180
    int imageOffset = 180;
    return (imageOffset + getVertexAngle(vert)) % 360;
}



QPainterPath NodeItem::getChildNodePath()
{
    QPainterPath path;
    foreach(NodeItem* child, childNodes.values()){
        path.addRect(child->translatedBoundingRect());
    }
    return path;
}


void NodeItem::updateGridLines()
{
    if(isGridEnabled()){
        QRectF grid = gridRect();
        QRectF drawnGrid = gridLines.boundingRect();

        if(grid.width() > drawnGrid.width() || grid.height() > drawnGrid.height()){
            //Go to double the width of the grid.
            grid.setWidth(grid.width() * 2);
            grid.setHeight(grid.height() * 2);

            QPainterPath path;


            int gridSize = getGridSize();

            QPointF gridOffset =  grid.topLeft();

            qreal modX = fmod(gridOffset.x(), gridSize);
            qreal modY = fmod(gridOffset.y(), gridSize);

            if(modX != 0){
                gridOffset.rx() += (gridSize - modX);
            }

            if(modY != 0){
                gridOffset.ry() += (gridSize - modY);
            }

            for(qreal x = gridOffset.x(); x <= grid.right(); x += gridSize){
                path.moveTo(x, grid.top());
                path.lineTo(x, grid.bottom());
            }

            for(qreal y = gridOffset.y(); y <= grid.bottom(); y += gridSize){
                path.moveTo(grid.left(), y);
                path.lineTo(grid.right(), y);
            }

            if(path != gridLines){
                gridLines = path;
                update();
            }
        }
    }
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    //Clip yo!
    painter->setClipRect(option->exposedRect);

    RENDER_STATE state = getRenderState(lod);

    //Paint the grid lines.
    if(isGridVisible()){
        painter->save();
        painter->setClipRect(gridRect());
        painter->strokePath(gridLines, gridLinePen);
        painter->restore();
    }

    if(state > RENDER_STATE::MINIMAL){
        if(gotVisualButton()){
            if(isSelected() && gotVisualEdgeKind()){
                paintPixmap(painter, lod, ER_CONNECT_ICON, "Icons", "connect");
            }else if(isSelected() && gotVisualNodeKind()){
                paintPixmap(painter, lod, ER_CONNECT_ICON, "Icons", "popOut");
            }
        }

        if(getNodeViewItem()->gotEdge(EDGE_KIND::DEPLOYMENT)){
            paintPixmap(painter, lod, ER_DEPLOYED, "Icons", "screen");
        }

        if(getNodeViewItem()->gotEdge(EDGE_KIND::QOS)){
            paintPixmap(painter, lod, ER_QOS, "Icons", "speedGauge");
        }

        if(isExpandEnabled() && hasChildNodes()){
            paintPixmap(painter, lod, ER_EXPANDED_STATE, "Icons", isExpanded() ? "triangleNorthWest" : "triangleSouthEast");
        }

        if(gotSecondaryTextKey()){
            paintPixmap(painter, lod, ER_SECONDARY_ICON, secondary_icon.first, secondary_icon.second);
        }
    }



    if(state > RENDER_STATE::BLOCK){
        painter->save();

        //Paint the selection path.
        painter->setPen(getPen());
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(getElementPath(ER_SELECTION));

        //Paint the selected section.
        if(selectedResizeVertex != NodeItem::RectVertex::NONE){
            painter->setPen(Qt::NoPen);
            painter->setBrush(resizeColor);
            painter->drawRect(getResizeRect(hoveredResizeVertex));
        }

        //Paint the hovered section.
        if(hoveredResizeVertex != NodeItem::RectVertex::NONE){
            painter->setPen(Qt::NoPen);
            painter->setBrush(resizeColor);
            painter->drawRect(getResizeRect(hoveredResizeVertex));

            if(hoveredResizeVertex != NodeItem::RectVertex::NONE){
                //Rotate
                painter->save();
                QRectF arrowRect = getElementRect(ER_RESIZE_ARROW);
                //Move to Center.
                painter->translate(arrowRect.center());
                //Rotate the Image
                painter->rotate(getResizeArrowRotation(hoveredResizeVertex));
                //Move back to top left.
                painter->translate(-arrowRect.center());

                //Paint the resize section
                paintPixmap(painter, lod, ER_RESIZE_ARROW, "Icons", "triangleDown");
                painter->restore();
            }
        }
        painter->restore();
    }

    EntityItem::paint(painter, option, widget);
    {
        painter->save();
        painter->setPen(getDefaultPen());

        if(gotPrimaryTextKey()){
            renderText(painter, lod, ER_PRIMARY_TEXT, getPrimaryText());
        }

        if(gotSecondaryTextKey()){
            renderText(painter, lod, ER_SECONDARY_TEXT, getSecondaryText());
        }
        painter->restore();
    }

    if(state > RENDER_STATE::BLOCK){
        painter->save();

        QColor resizeColor(255, 255, 255, 130);

        painter->setPen(Qt::NoPen);
        painter->setBrush(resizeColor);
        
        
        for(auto pair : hovered_edge_kinds){
            auto edge_direction = pair.first;
            auto edge_kind = pair.second;
            auto rect = getEdgeConnectRect(edge_direction, edge_kind);
            painter->drawRect(rect);
        }

        painter->setBrush(QColor(255, 165, 70, 150));

        auto map = isExpanded() ? getVisualEdgeKinds() : getAllVisualEdgeKinds();

        for(auto edge_direction : map.uniqueKeys()){
            bool is_direction_hovered = hovered_edge_kinds.contains({edge_direction, EDGE_KIND::NONE});
            if(is_direction_hovered){
                for(auto edge_kind : map.values(edge_direction)){
                    if(edge_kind != EDGE_KIND::NONE){
                        auto rect = getEdgeConnectRect(edge_direction, edge_kind);
                        auto icon_rect = getEdgeConnectIconRect(edge_direction, edge_kind);

                        if(hovered_edge_kinds.contains({edge_direction, edge_kind})){
                            painter->drawRect(rect);
                        }
                        paintPixmap(painter, lod, icon_rect, "EntityIcons", EntityFactory::getEdgeKindString(edge_kind));
                    }
                }
            }
        }
        
        //painter->drawRect(getElementRect(ER_CONNECT));

        /*if(gotVisualButton()){
            paintPixmap(painter, lod, ER_EDGE_KIND_ICON, "EntityIcons", visualEntityIcon);
        }*/
        painter->restore();
    }
}

QRectF NodeItem::getElementRect(EntityItem::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_MOVE:{
            return headerRect();
        }
        case ER_RESIZE_ARROW:{
            return getResizeArrowRect();
        }
        default:
            break;
    }
    //Just call base class.
    return EntityItem::getElementRect(rect);
}

QPainterPath NodeItem::getElementPath(EntityItem::ELEMENT_RECT rect) const
{
    return EntityItem::getElementPath(rect);
}

QRectF NodeItem::getResizeRect(NodeItem::RectVertex vert) const
{
    QRectF rect;

    if(vert != NodeItem::RectVertex::NONE){
        int resizeRectRadius = RESIZE_RECT_SIZE;
        int resizeRectSize = resizeRectRadius * 2;
        rect.setWidth(resizeRectSize);
        rect.setHeight(resizeRectSize);

        if(vert == NodeItem::RectVertex::TOP || vert == NodeItem::RectVertex::BOTTOM){
            //Horizontal Rectangle
            rect.setWidth(getWidth() - resizeRectSize);

            QPointF topLeft;

            if(vert == NodeItem::RectVertex::TOP){
                topLeft = expandedRect().topLeft() + QPointF(resizeRectRadius, - resizeRectRadius);
            }else{
                topLeft = expandedRect().bottomLeft() + QPointF(resizeRectRadius, - resizeRectRadius);
            }

            rect.moveTopLeft(topLeft);
        }else if(vert == NodeItem::RectVertex::LEFT || vert == NodeItem::RectVertex::RIGHT){
            //Vertical Rectangle
            rect.setHeight(getHeight() - resizeRectSize);

            QPointF topLeft;

            if(vert == NodeItem::RectVertex::LEFT){
                topLeft = expandedRect().topLeft() + QPointF(-resizeRectRadius, resizeRectRadius);
            }else{
                topLeft = expandedRect().topRight() + QPointF(-resizeRectRadius, resizeRectRadius);
            }

            rect.moveTopLeft(topLeft);
        }else{
            switch(vert){
                case NodeItem::RectVertex::TOP_LEFT:
                    rect.moveCenter(expandedRect().topLeft());
                    break;
                case NodeItem::RectVertex::TOP_RIGHT:
                    rect.moveCenter(expandedRect().topRight());
                    break;
                case NodeItem::RectVertex::BOTTOM_RIGHT:
                    rect.moveCenter(expandedRect().bottomRight());
                    break;
                case NodeItem::RectVertex::BOTTOM_LEFT:
                    rect.moveCenter(expandedRect().bottomLeft());
                    break;
                default:
                break;
            }
        }
    }
    return rect;
}

QRectF NodeItem::getResizeArrowRect() const
{
    QRectF rect;
    if(hoveredResizeVertex != NodeItem::RectVertex::NONE){
        rect.setSize(QSize(2 * RESIZE_RECT_SIZE, 2 * RESIZE_RECT_SIZE));
        rect.moveCenter(getResizeRect(hoveredResizeVertex).center());
    }
    return rect;
}

int NodeItem::getEdgeConnectPos(EDGE_DIRECTION direction, EDGE_KIND kind) const{
    //Values in list are backwards
    auto list = getAllVisualEdgeKinds().values(direction);
    auto index = list.size() - (list.indexOf(kind) + 1);
    return index;
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
        //Get our position 
        double pos = getEdgeConnectPos(direction, kind);
        double count = getAllVisualEdgeKinds().count(direction);
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
    qreal x = direction == EDGE_DIRECTION::SOURCE ? rect.left() : rect.right();
    return mapToScene(x,y);
}

QRectF NodeItem::getEdgeDirectionRect(EDGE_DIRECTION direction) const{
    switch(direction){
        case EDGE_DIRECTION::SOURCE:{
            return getElementRect(ER_CONNECT_SOURCE);
        }
        case EDGE_DIRECTION::TARGET:{
            return getElementRect(ER_CONNECT_TARGET);
        }
    }
    return QRectF();
}

QSet<QPair<EDGE_DIRECTION, EDGE_KIND> > NodeItem::getEdgeConnectRectAtPos(QPointF pos) const{
    QSet<QPair<EDGE_DIRECTION, EDGE_KIND> > kinds;
    auto map = getAllVisualEdgeKinds();
    for(auto direction : map.uniqueKeys()){
        for(auto kind : map.values(direction)){
            if(getEdgeConnectRect(direction, kind).contains(pos)){
                kinds.insert({direction, kind});
            }
        }
    }
    return kinds;
}

std::list<NodeItem::RectVertex> NodeItem::getRectVertex(){
    return {NodeItem::RectVertex::LEFT, NodeItem::RectVertex::TOP_LEFT, NodeItem::RectVertex::TOP, NodeItem::RectVertex::TOP_RIGHT, NodeItem::RectVertex::RIGHT, NodeItem::RectVertex::BOTTOM_RIGHT, NodeItem::RectVertex::BOTTOM, NodeItem::RectVertex::BOTTOM_LEFT};
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    EntityItem::mousePressEvent(event);
    bool caughtResize = false;

    for(auto direction : visual_edge_kinds.uniqueKeys()){
        auto edge_kinds = visual_edge_kinds.values(direction);
        for(auto edge_kind : edge_kinds){
            auto rect = getEdgeConnectRect(direction, edge_kind);
            if(rect.contains(event->pos())){
                auto pos = mapToScene(rect.center());
                emit req_connectEdgeMode(pos, edge_kind, direction);
                caughtResize = true;
            }
        }
    }

    if(isSelected() && !caughtResize && isResizeEnabled() && isExpanded()){
        NodeItem::RectVertex vertex = NodeItem::RectVertex::NONE;

        for(auto vert : getRectVertex()){
            if(getResizeRect(vert).contains(event->pos())){
                vertex = vert;
            }
        }
        if(vertex != selectedResizeVertex){
            selectedResizeVertex = vertex;
            if(selectedResizeVertex != NodeItem::RectVertex::NONE){
                previousResizePoint = event->scenePos();
            }
            emit req_StartResize();
            update();
            caughtResize = true;
        }
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   if(selectedResizeVertex != NodeItem::RectVertex::NONE){
       emit req_FinishResize();
       selectedResizeVertex = NodeItem::RectVertex::NONE;
       update();
   }

   for(auto direction : visual_edge_kinds.uniqueKeys()){
        auto edge_kinds = visual_edge_kinds.values(direction);
        for(auto edge_kind : edge_kinds){
            if(getEdgeConnectRect(direction, edge_kind).contains(event->pos())){
                emit req_connectEdgeMenu(event->scenePos(), edge_kind, direction);
            }
        }
    }
    EntityItem::mouseReleaseEvent(event);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(selectedResizeVertex != NodeItem::RectVertex::NONE){
        QPointF deltaPos = event->scenePos() - previousResizePoint;
        previousResizePoint = event->scenePos();
        setManuallyAdjusted(selectedResizeVertex);
        emit req_Resize(this, QSizeF(deltaPos.x(), deltaPos.y()), selectedResizeVertex);
    }else{
        EntityItem::mouseMoveEvent(event);
    }
}

void NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    bool need_update = false;

    for(auto direction : visual_edge_kinds.uniqueKeys()){
        auto edge_kinds = visual_edge_kinds.values(direction);
        edge_kinds.push_front(EDGE_KIND::NONE);
        for(auto edge_kind : edge_kinds){
            auto in_rect = getEdgeConnectRect(direction, edge_kind).contains(event->pos());
            QPair<EDGE_DIRECTION, EDGE_KIND> key = {direction, edge_kind};
            auto contains = hovered_edge_kinds.contains(key);
            if(in_rect != contains){
                if(in_rect){
                    hovered_edge_kinds.insert(key);
                }else{
                    hovered_edge_kinds.remove(key);
                }
                need_update = true;
            }
        }
    }

    if(isSelected() && isResizeEnabled() && isExpanded() && hasChildNodes()){
        NodeItem::RectVertex vertex = NodeItem::RectVertex::NONE;
        for(auto vert : getRectVertex()){
            if(getResizeRect(vert).contains(event->pos())){
                vertex = vert;
            }
        }
        if(vertex != hoveredResizeVertex){
            hoveredResizeVertex = vertex;
            need_update = true;
            
        }
    }

    if(need_update){
        update();
    }
    EntityItem::hoverMoveEvent(event);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    bool needs_update = false;

    if(!hovered_edge_kinds.empty()){
        hovered_edge_kinds.clear();
        needs_update = true;
    }
    if(hoveredResizeVertex != NodeItem::RectVertex::NONE){
        hoveredResizeVertex = NodeItem::RectVertex::NONE;
        needs_update = true;
    }
    if(needs_update){
        update();
    }
    EntityItem::hoverLeaveEvent(event);
}

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(getVisualNodeKind() != NODE_KIND::NONE && event->button() == Qt::LeftButton){
        if(getElementPath(ER_CONNECT).contains(event->pos())){
            emit req_popOutRelatedNode(getNodeViewItem(), visualNodeKind);
        }
    }

    if(!getPrimaryTextKey().isEmpty()){
        if(event->button() == Qt::LeftButton && getElementPath(ER_PRIMARY_TEXT).contains(event->pos())){
            emit req_editData(getViewItem(), getPrimaryTextKey());
        }
    }
    if(!getSecondaryTextKey().isEmpty()){
        if(event->button() == Qt::LeftButton && getElementPath(ER_SECONDARY_TEXT).contains(event->pos())){
            emit req_editData(getViewItem(), getSecondaryTextKey());
        }
    }


    EntityItem::mouseDoubleClickEvent(event);
}

QColor NodeItem::getBodyColor() const
{
    if(isHighlighted()){
        //Do nothing
    }else if(getReadState() == READ_ONLY_DEFINITION){
        return readOnlyDefinitionColor;
    }else if(getReadState() == READ_ONLY_INSTANCE){
        return readOnlyInstanceColor;
    }
    return EntityItem::getBodyColor();
}

QColor NodeItem::getHeaderColor() const
{
    auto color = header_color;
    if(!isHighlighted() && paint_notification){
        color = Theme::blendColors(notification_color, color, .60);
    }
    return color;
}

void NodeItem::setHeaderColor(QColor color){
    header_color = color;
    update();
}

QPointF NodeItem::getTopLeftOffset() const
{
    return getMarginOffset();
}
