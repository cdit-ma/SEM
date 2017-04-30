#include "nodeitem.h"

#include <math.h>
#include <QStyleOptionGraphicsItem>

#include "../entityitem.h"


#define RESIZE_RECT_SIZE 4



NodeItem::NodeItem(NodeViewItem *viewItem, NodeItem *parentItem, NodeItem::KIND kind):EntityItem(viewItem, parentItem, EntityItem::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    modelHeight = 0;
    modelWidth = 0;
    //manuallyAdjustedWidth = 0;
    //manuallyAdjustedHeight = 0;
    gridVisible = false;
    gridEnabled = false;
    hoveredConnect = false;
    resizeEnabled = false;
    ignorePosition = false;
    _rightJustified = false;
    aspect = VA_NONE;
    selectedResizeVertex = RV_NONE;
    hoveredResizeVertex = RV_NONE;
    readState = NodeItem::NORMAL;

    visualEdgeKind = Edge::EC_NONE;


    nodeViewItem = viewItem;
    nodeItemKind = kind;

    setMoveEnabled(true);
    setGridEnabled(true);
    setSelectionEnabled(true);
    setResizeEnabled(true);
    setExpandEnabled(true);

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

        connect(nodeItem, SIGNAL(sizeChanged()), this, SLOT(childPosChanged()));
        connect(nodeItem, SIGNAL(positionChanged()), this, SLOT(childPosChanged()));
        childNodes[ID] = nodeItem;
        if(childNodes.count() == 1){
            emit gotChildNodes(true);
        }
        nodeItem->setBaseBodyColor(getBaseBodyColor().darker(110));

        nodeItem->setVisible(isExpanded());
        childPosChanged();
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
        childPosChanged();
    }
}

int NodeItem::getSortOrder() const
{
    if(hasData("sortOrder")){
        return getData("sortOrder").toInt();
    }
    return -1;
}

bool NodeItem::hasChildNodes() const
{
    return !childNodes.isEmpty();
}

QList<NodeItem *> NodeItem::getChildNodes() const
{
    return childNodes.values();
}

QList<NodeItem *> NodeItem::getOrderedChildNodes() const
{
    QMap<int, NodeItem*> items;

    foreach(NodeItem* child, getChildNodes()){
        int position = items.size();
        if(child->hasData("sortOrder")){
            position = child->getData("sortOrder").toInt();
        }
        items.insertMulti(position, child);
    }
    return items.values();
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

        connect(edgeItem, SIGNAL(positionChanged()), this, SLOT(childPosChanged()));
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

void NodeItem::setManuallyAdjusted(RECT_VERTEX corner)
{
    bool adjustingWidth = false;
    bool adjustingHeight = false;

    if(corner == RV_TOPLEFT || corner == RV_TOPRIGHT || corner == RV_BOTTOMRIGHT || corner == RV_BOTTOMLEFT){
        adjustingWidth = true;
        adjustingHeight = true;
    }
    if(corner == RV_RIGHT || corner == RV_LEFT){
        adjustingWidth = true;
    }
    if(corner == RV_TOP || corner == RV_BOTTOM){
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



void NodeItem::setVisualEdgeKind(Edge::EDGE_KIND kind)
{
    visualEdgeKind = kind;
    visualEntityIcon = EntityFactory::getEdgeKindString(kind);
    update();
}

void NodeItem::setVisualNodeKind(NODE_KIND kind)
{
    visualNodeKind = kind;
    visualEntityIcon = "ComponentImpl";
    update();
}

Edge::EDGE_KIND NodeItem::getVisualEdgeKind() const
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
    return visualEdgeKind != Edge::EC_NONE;
}

bool NodeItem::gotVisualButton() const
{
    return gotVisualNodeKind() || gotVisualEdgeKind();
}


QString NodeItem::getPrimaryTextKey() const
{
    return primaryTextKey;
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

void NodeItem::childPosChanged()
{
    //Update the child rect.
    QRectF rect;
    foreach(EntityItem* child, getChildEntities()){
        if(child->isNodeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }else if(child->isEdgeItem()){
            rect = rect.united(child->currentRect());
        }
    }
    _childRect = rect;
    resizeToChildren();
}

void NodeItem::edgeAdded(Edge::EDGE_KIND kind)
{
    switch(kind){
        case Edge::EC_DEPLOYMENT:
        case Edge::EC_QOS:
            update();
            break;
    default:
        return;
    }
}

void NodeItem::edgeRemoved(Edge::EDGE_KIND kind)
{
    switch(kind){
        case Edge::EC_DEPLOYMENT:
        case Edge::EC_QOS:
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

int NodeItem::getVertexAngle(RECT_VERTEX vert) const
{
    //Bottom Left is 360 and 0, going clockwise
    //Bottom left is technically 225 degrees from 0
    int interval = 45;
    int initial = 225;
    return (initial + (vert * interval)) % 360;
}

int NodeItem::getResizeArrowRotation(RECT_VERTEX vert) const
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
    //painter->setClipRect(option->exposedRect);

    RENDER_STATE state = getRenderState(lod);

    //Paint the grid lines.
    if(isGridVisible()){
        painter->save();
        painter->setClipRect(gridRect());
        painter->strokePath(gridLines, gridLinePen);
        painter->restore();
    }

    if(state > RS_MINIMAL){
        if(gotVisualButton()){
            if(isSelected() && gotVisualEdgeKind()){
                paintPixmap(painter, lod, ER_CONNECT_ICON, "Icons", "connect");
            }else if(gotVisualNodeKind()){
                paintPixmap(painter, lod, ER_CONNECT_ICON, "Icons", "popOut");
            }
        }

        if(getNodeViewItem()->gotEdge(Edge::EC_DEPLOYMENT)){
            paintPixmap(painter, lod, ER_DEPLOYED, "Icons", "screen");
        }

        if(getNodeViewItem()->gotEdge(Edge::EC_QOS)){
            paintPixmap(painter, lod, ER_QOS, "Icons", "speedGauge");
        }

        if(isExpandEnabled() && hasChildNodes()){
            paintPixmap(painter, lod, ER_EXPANDED_STATE, "Icons", isExpanded() ? "triangleNorthWest" : "triangleSouthEast");
        }

        if(gotSecondaryTextKey()){
            paintPixmap(painter, lod, ER_SECONDARY_ICON, "Icons", getSecondaryTextKey());
        }
    }



    if(state > RS_BLOCK){
        painter->save();

        //Paint the selection path.
        painter->setPen(getPen());
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(getElementPath(ER_SELECTION));

        //Paint the selected section.
        if(selectedResizeVertex != RV_NONE){
            painter->setPen(Qt::NoPen);
            painter->setBrush(resizeColor);
            painter->drawRect(getResizeRect(hoveredResizeVertex));
        }

        //Paint the hovered section.
        if(hoveredResizeVertex != RV_NONE){
            painter->setPen(Qt::NoPen);
            painter->setBrush(resizeColor);
            painter->drawRect(getResizeRect(hoveredResizeVertex));

            if(hoveredResizeVertex != RV_NONE){
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
                paintPixmap(painter, lod, ER_RESIZE_ARROW, "Icons", "triangleSouth");
                painter->restore();
            }
        }
        painter->restore();
    }

    EntityItem::paint(painter, option, widget);
    if(gotPrimaryTextKey()){
        renderText(painter, lod, ER_PRIMARY_TEXT, getPrimaryText());
    }

    if(gotSecondaryTextKey()){
        renderText(painter, lod, ER_SECONDARY_TEXT, getSecondaryText());
    }

    if(state > RS_BLOCK){
        if(hoveredConnect){
            painter->save();

            QColor resizeColor(255, 255, 255, 130);

            painter->setPen(Qt::NoPen);
            painter->setBrush(resizeColor);
            painter->drawRect(getElementRect(ER_CONNECT));

            if(gotVisualButton()){
                paintPixmap(painter, lod, ER_EDGE_KIND_ICON, "EntityIcons", visualEntityIcon);
            }
            painter->restore();
        }
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

QRectF NodeItem::getResizeRect(RECT_VERTEX vert) const
{
    QRectF rect;

    if(vert != RV_NONE){
        int resizeRectRadius = RESIZE_RECT_SIZE;
        int resizeRectSize = resizeRectRadius * 2;
        rect.setWidth(resizeRectSize);
        rect.setHeight(resizeRectSize);

        if(vert == RV_TOP || vert == RV_BOTTOM){
            //Horizontal Rectangle
            rect.setWidth(getWidth() - resizeRectSize);

            QPointF topLeft;

            if(vert == RV_TOP){
                topLeft = expandedRect().topLeft() + QPointF(resizeRectRadius, - resizeRectRadius);
            }else{
                topLeft = expandedRect().bottomLeft() + QPointF(resizeRectRadius, - resizeRectRadius);
            }

            rect.moveTopLeft(topLeft);
        }else if(vert == RV_LEFT || vert == RV_RIGHT){
            //Vertical Rectangle
            rect.setHeight(getHeight() - resizeRectSize);

            QPointF topLeft;

            if(vert == RV_LEFT){
                topLeft = expandedRect().topLeft() + QPointF(-resizeRectRadius, resizeRectRadius);
            }else{
                topLeft = expandedRect().topRight() + QPointF(-resizeRectRadius, resizeRectRadius);
            }

            rect.moveTopLeft(topLeft);
        }else{
            //Small Square
            if(vert == RV_TOPLEFT){
                rect.moveCenter(expandedRect().topLeft());
            }else if(vert == RV_TOPRIGHT){
                rect.moveCenter(expandedRect().topRight());
            }else if(vert == RV_BOTTOMRIGHT){
                rect.moveCenter(expandedRect().bottomRight());
            }else if(vert == RV_BOTTOMLEFT){
                rect.moveCenter(expandedRect().bottomLeft());
            }
        }
    }
    return rect;
}

QRectF NodeItem::getResizeArrowRect() const
{
    QRectF rect;
    if(hoveredResizeVertex != RV_NONE){
        rect.setSize(QSize(2 * RESIZE_RECT_SIZE, 2 * RESIZE_RECT_SIZE));
        rect.moveCenter(getResizeRect(hoveredResizeVertex).center());
    }
    return rect;
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool caughtResize = false;

    if(gotVisualEdgeKind() && isSelected() && event->button() == Qt::LeftButton){
        if(getElementPath(ER_CONNECT).contains(event->pos()) || getElementPath(ER_CONNECT_ICON).contains(event->pos())){
            caughtResize = true;
            emit req_connectMode(this);
        }
    }

    if(isSelected() && !caughtResize && isResizeEnabled() && isExpanded()){
        RECT_VERTEX vertex = RV_NONE;
        for(int i = RV_LEFT;i <= RV_BOTTOMLEFT; i++){
            RECT_VERTEX vert = (RECT_VERTEX)i;
            if(getResizeRect(vert).contains(event->pos())){
                vertex = vert;
            }
        }
        if(vertex != selectedResizeVertex){
            selectedResizeVertex = vertex;
            if(selectedResizeVertex != RV_NONE){
                previousResizePoint = event->scenePos();
            }
            emit req_StartResize();
            update();
            caughtResize = true;
        }
    }



    if(!caughtResize){
        EntityItem::mousePressEvent(event);
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   if(selectedResizeVertex != RV_NONE){
       emit req_FinishResize();
       selectedResizeVertex = RV_NONE;
       update();
   }else{
       EntityItem::mouseReleaseEvent(event);
   }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(selectedResizeVertex != RV_NONE){
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
    if(gotVisualButton()){
        if(gotVisualNodeKind() || (gotVisualEdgeKind() && isSelected())){
            bool showHover = getElementRect(ER_CONNECT).contains(event->pos()) || getElementRect(ER_CONNECT_ICON).contains(event->pos());

            if(showHover != hoveredConnect){
                hoveredConnect = showHover;
                update();
            }
        }
    }

    if(isSelected() && isResizeEnabled() && isExpanded() && hasChildNodes()){
        RECT_VERTEX vertex = RV_NONE;
        for(int i = RV_LEFT;i <= RV_BOTTOMLEFT; i++){
            RECT_VERTEX vert = (RECT_VERTEX)i;
            if(getResizeRect(vert).contains(event->pos())){
                vertex = vert;
            }
        }
        if(vertex != hoveredResizeVertex){
            hoveredResizeVertex = vertex;
            update();
        }
    }
    EntityItem::hoverMoveEvent(event);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(hoveredConnect){
        hoveredConnect = false;
        update();
    }
    if(hoveredResizeVertex != RV_NONE){
        hoveredResizeVertex = RV_NONE;
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
    return getBodyColor().darker(120);
}

QPointF NodeItem::getTopLeftOffset() const
{
    return getMarginOffset();
}
