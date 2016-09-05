#include "nodeitemnew.h"
#include "entityitemnew.h"

#include <QDebug>
#include <QStyleOptionGraphicsItem>
#include <math.h>
#include "../theme.h"
#define RESIZE_RECT_SIZE 5



NodeItemNew::NodeItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, NodeItemNew::KIND kind):EntityItemNew(viewItem, parentItem, EntityItemNew::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    manuallyAdjustedWidth = 0;
    manuallyAdjustedHeight = 0;
    gridVisible = false;
    gridEnabled = false;
    resizeEnabled = false;
    ignorePosition = false;
    _rightJustified = false;
    aspect = VA_NONE;
    selectedResizeVertex = RV_NONE;
    hoveredResizeVertex = RV_NONE;
    readState = NodeItemNew::NORMAL;


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




    if(parentItem){
        //Lock child in same aspect as parent
        setAspect(parentItem->getAspect());
        setParentItem(parentItem);
        parentItem->addChildNode(this);
        setCenter(getNearestGridPoint());
    }
}

NodeItemNew::~NodeItemNew()
{
    //Unset
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildNode(this);
    }

    //remove children nodes.
    while(!childNodes.isEmpty()){
        int key = childNodes.keys().takeFirst();
        NodeItemNew* child = childNodes[key];
        removeChildNode(child);
    }

    //remove children nodes.
    while(!childEdges.isEmpty()){
        int key = childEdges.keys().takeFirst();
        removeChildEdge(key);
    }
}




QRectF NodeItemNew::viewRect() const
{
    return EntityItemNew::viewRect();
}

NodeItemNew *NodeItemNew::getParentNodeItem() const
{
    EntityItemNew* parent = getParent();
    if(parent && parent->isNodeItem()){
        return (NodeItemNew*) parent;
    }
    return 0;
}

NodeItemNew::KIND NodeItemNew::getNodeItemKind()
{
    return nodeItemKind;
}

Node::NODE_KIND NodeItemNew::getNodeKind() const
{
    return nodeViewItem->getNodeKind();
}

NodeItemNew::NODE_READ_STATE NodeItemNew::getReadState() const
{
    return readState;
}

void NodeItemNew::setRightJustified(bool isRight)
{
    _rightJustified = isRight;
}

bool NodeItemNew::isRightJustified() const
{
    return _rightJustified;
}

void NodeItemNew::addChildNode(NodeItemNew *nodeItem)
{
    int ID = nodeItem->getID();
    //If we have added a child, and there is only one. emit a signal
    if(!childNodes.contains(ID)){
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



void NodeItemNew::removeChildNode(NodeItemNew* nodeItem)
{
    //If we have removed a child, and there is no children left. emit a signal
    if(childNodes.remove(nodeItem->getID()) > 0){
        if(childNodes.count() == 0){
            emit gotChildNodes(false);
        }
        //Unset child moving.
        setChildNodeMoving(nodeItem, false);
        nodeItem->unsetParent();
        childPosChanged();
    }
}

int NodeItemNew::getSortOrder() const
{
    if(hasData("sortOrder")){
        return getData("sortOrder").toInt();
    }
    return -1;
}

bool NodeItemNew::hasChildNodes() const
{
    return !childNodes.isEmpty();
}

QList<NodeItemNew *> NodeItemNew::getChildNodes() const
{
    return childNodes.values();
}

QList<NodeItemNew *> NodeItemNew::getOrderedChildNodes() const
{
    QMap<int, NodeItemNew*> items;

    foreach(NodeItemNew* child, getChildNodes()){
        int position = items.size();
        if(child->hasData("sortOrder")){
            position = child->getData("sortOrder").toInt();
        }
        items.insertMulti(position, child);
    }
    return items.values();
}

QList<EntityItemNew *> NodeItemNew::getChildEntities() const
{
    QList<EntityItemNew*> children;
    foreach(NodeItemNew* child, getChildNodes()){
        children.append(child);
    }
    foreach(EdgeItemNew* child, getChildEdges()){
        children.append(child);
    }
    return children;
}

QPointF NodeItemNew::validateAdjustPos(QPointF delta)
{
    if(!isTopLevelItem() && getParentNodeItem()){
        QPointF adjustedPos = pos() + delta;
        QPointF minPos = getParentNodeItem()->gridRect().topLeft();

        //Minimize on the minimum position this item can go.
        if(adjustedPos.x() < minPos.x()){
            adjustedPos.rx() = minPos.x();
        }
        if(adjustedPos.y() < minPos.y()){
            adjustedPos.ry() = minPos.y();
        }

        //Offset by the pos() to get the restricted delta.
        delta = adjustedPos - pos();
    }

    return EntityItemNew::validateAdjustPos(delta);
}

void NodeItemNew::addChildEdge(EdgeItemNew *edgeItem)
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

void NodeItemNew::removeChildEdge(int ID)
{
    if(childEdges.contains(ID)){
        EdgeItemNew* item = childEdges[ID];
        item->unsetParent();
        childEdges.remove(ID);
    }
}

QList<EdgeItemNew *> NodeItemNew::getChildEdges() const
{
    return childEdges.values();
}



void NodeItemNew::setGridEnabled(bool enabled)
{
    if(gridEnabled != enabled){
        gridEnabled = enabled;
        if(gridEnabled){
            updateGridLines();
        }
    }
}

bool NodeItemNew::isGridEnabled() const
{
    return gridEnabled;
}

void NodeItemNew::setGridVisible(bool visible)
{
    if(gridVisible != visible){
        gridVisible = visible;
        update();
    }
}

bool NodeItemNew::isGridVisible() const
{
    return isGridEnabled() && gridVisible;
}

void NodeItemNew::setResizeEnabled(bool enabled)
{
    if(resizeEnabled != enabled){
        resizeEnabled = enabled;
    }
}

bool NodeItemNew::isResizeEnabled()
{
    return resizeEnabled;
}

void NodeItemNew::setChildNodeMoving(NodeItemNew *child, bool moving)
{
    if(childNodes.contains(child->getID())){
        if(moving && !movingChildren.contains(child)){
            movingChildren.append(child);
            if(movingChildren.size() == 1){
                setGridVisible(true);
            }
        }else if(!moving && movingChildren.contains(child)){
            movingChildren.removeAll(child);
            if(movingChildren.isEmpty()){
                setGridVisible(false);
            }
        }
    }
}

void NodeItemNew::setMoving(bool moving)
{
    EntityItemNew::setMoving(moving);

    NodeItemNew* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildNodeMoving(this, moving);
    }
    if(!moving){
        setCenter(getNearestGridPoint());
    }
}

QRectF NodeItemNew::sceneBoundingRect() const
{
    return EntityItemNew::sceneBoundingRect();
}

QRectF NodeItemNew::boundingRect() const
{
    QRectF rect;
    rect.setWidth(margin.left() + margin.right() + getWidth());
    rect.setHeight(margin.top() + margin.bottom() + getHeight());
    return rect;
}


QRectF NodeItemNew::contractedRect() const
{
    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getMinimumWidth(), getMinimumHeight()));
}

QRectF NodeItemNew::expandedRect() const
{
    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getExpandedWidth(), getExpandedHeight()));
}

QRectF NodeItemNew::currentRect() const
{

    return QRectF(boundingRect().topLeft() + getMarginOffset(), QSizeF(getWidth(), getHeight()));
}


QRectF NodeItemNew::gridRect() const
{
    return bodyRect().marginsRemoved(getBodyPadding());
}

QRectF NodeItemNew::expandedGridRect() const
{
    return expandedRect().marginsRemoved(getBodyPadding());
}

QRectF NodeItemNew::bodyRect() const
{
    return currentRect();
}

QRectF NodeItemNew::headerRect() const
{
    return currentRect();
}

QRectF NodeItemNew::childrenRect() const
{
    return _childRect;

}

QSizeF NodeItemNew::getSize() const
{
    QSizeF size;
    size.setWidth(getWidth());
    size.setHeight(getHeight());
    return size;
}

void NodeItemNew::adjustExpandedSize(QSizeF delta)
{
    setExpandedSize(getExpandedSize() + delta);
}

void NodeItemNew::setMinimumWidth(qreal width)
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

void NodeItemNew::setMinimumHeight(qreal height)
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

void NodeItemNew::setExpandedWidth(qreal width)
{
    //Limit by the size of all contained children.
    qreal minWidth = childrenRect().right() - getMargin().left();
    //Can't shrink smaller than minimum
    minWidth = qMax(minWidth, minimumWidth);
    minWidth = qMax(minWidth, manuallyAdjustedWidth);
    width = qMax(width, minWidth);

    if(expandedWidth != width){
        expandedWidth = width;

        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
            updateGridLines();
        }
    }
}

void NodeItemNew::setExpandedHeight(qreal height)
{
    //Limit by the size of all contained children.
    qreal minHeight = childrenRect().bottom() - getMargin().top();
    //Can't shrink smaller than minimum
    minHeight = qMax(minHeight, minimumHeight);
    minHeight = qMax(minHeight, manuallyAdjustedHeight);
    height = qMax(height, minHeight);

    if(expandedHeight != height){
        expandedHeight = height;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged();
            updateGridLines();
        }
    }
}

void NodeItemNew::setExpandedSize(QSizeF size)
{

    setExpandedWidth(size.width());
    setExpandedHeight(size.height());
}


qreal NodeItemNew::getExpandedWidth() const
{
    return expandedWidth;
}

qreal NodeItemNew::getExpandedHeight() const
{
    return expandedHeight;
}

QSizeF NodeItemNew::getExpandedSize() const
{
    return QSizeF(expandedWidth, expandedHeight);
}

qreal NodeItemNew::getMinimumWidth() const
{
    return minimumWidth;
}

qreal NodeItemNew::getMinimumHeight() const
{
    return minimumHeight;
}

void NodeItemNew::setMargin(QMarginsF margin)
{
    if(this->margin != margin){
        prepareGeometryChange();
        this->margin = margin;
        update();
    }
}

void NodeItemNew::setBodyPadding(QMarginsF padding)
{
    if(this->bodyPadding != padding){
        this->bodyPadding = padding;
        updateGridLines();
    }
}

QPointF NodeItemNew::getMarginOffset() const
{
    return QPointF(margin.left(), margin.top());
}

QPointF NodeItemNew::getBottomRightMarginOffset() const
{
    return QPointF(margin.right(), margin.bottom());
}

QPointF NodeItemNew::getTopLeftSceneCoordinate() const
{
    return sceneBoundingRect().topLeft();
}

qreal NodeItemNew::getWidth() const
{
    if(isExpanded()){
        return getExpandedWidth();
    }else{
        return getMinimumWidth();
    }
}

qreal NodeItemNew::getHeight() const
{
    if(isExpanded()){
        return getExpandedHeight();
    }else{
        return getMinimumHeight();
    }
}



QPointF NodeItemNew::getCenterOffset() const
{
    return contractedRect().center();
}

QPointF NodeItemNew::getSceneEdgeTermination(bool left) const
{
    qreal y = contractedRect().center().y();
    qreal x = left ? currentRect().left() : currentRect().right();
    return mapToScene(x,y);
}


void NodeItemNew::setPos(const QPointF &pos)
{
    QPointF deltaPos = pos - this->pos();
    deltaPos = validateAdjustPos(deltaPos);
    if(!deltaPos.isNull()){
        EntityItemNew::setPos(this->pos() + deltaPos);

        if(getParentNodeItem()){
            //setRightJustified(x() > (getParentNodeItem()->getWidth() / 2));
        }
        updateGridLines();
    }
}

void NodeItemNew::setAspect(VIEW_ASPECT aspect)
{
    this->aspect = aspect;
}

VIEW_ASPECT NodeItemNew::getAspect()
{
    return aspect;
}

void NodeItemNew::setManuallyAdjusted(RECT_VERTEX corner)
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
        manuallyAdjustedWidth = -1;
    }
    if(adjustingHeight){
        manuallyAdjustedHeight = -1;
    }
}

QMarginsF NodeItemNew::getMargin() const
{
    return margin;
}

QMarginsF NodeItemNew::getBodyPadding() const
{
    return bodyPadding;
}

void NodeItemNew::setExpanded(bool expand)
{
    if(isExpanded() != expand){
        //Call the base class
        EntityItemNew::setExpanded(expand);

        prepareGeometryChange();
        //Hide/Show Children
        foreach(EntityItemNew* child, getChildEntities()){
            child->setVisible(isExpanded());
        }

        update();
        emit sizeChanged();
    }
}



void NodeItemNew::setPrimaryTextKey(QString key)
{
    if(primaryTextKey != key){
        primaryTextKey = key;
        addRequiredData(key);
    }
}

void NodeItemNew::setSecondaryTextKey(QString key)
{
    if(secondaryTextKey != key){
        secondaryTextKey = key;
        addRequiredData(key);
    }
}

QString NodeItemNew::getPrimaryTextKey() const
{
    return primaryTextKey;
}

QString NodeItemNew::getSecondaryTextKey() const
{
    return secondaryTextKey;
}

bool NodeItemNew::gotPrimaryTextKey() const
{
    return !primaryTextKey.isEmpty();
}

bool NodeItemNew::gotSecondaryTextKey() const
{
    return !secondaryTextKey.isEmpty();
}

QString NodeItemNew::getPrimaryText() const
{
    if(!primaryTextKey.isEmpty()){
        return getData(primaryTextKey).toString();
    }
    return QString();
}

QString NodeItemNew::getSecondaryText() const
{
    if(!secondaryTextKey.isEmpty()){
        return getData(secondaryTextKey).toString();
    }
    return QString();
}


void NodeItemNew::dataChanged(QString keyName, QVariant data)
{
    if(getRequiredDataKeys().contains(keyName)){
        if(keyName == "x" || keyName == "y"){
            qreal realData = data.toReal();
            QPointF oldCenter = getCenter();
            QPointF newCenter = oldCenter;

            if(keyName == "x"){
                newCenter.setX(realData);

            }else if(keyName == "y"){
                newCenter.setY(realData);
            }

            if(newCenter != oldCenter){
                setCenter(newCenter);
            }
        }else if(keyName == "width" || keyName == "height"){
            qreal realData = data.toReal();
            if(keyName == "width"){
                manuallyAdjustedWidth = -1;
                setExpandedWidth(realData);
                manuallyAdjustedWidth = realData;

            }else if(keyName == "height"){
                manuallyAdjustedHeight = -1;
                setExpandedHeight(realData);
                manuallyAdjustedHeight = realData;
            }


        }else if(keyName == "isExpanded"){
            bool boolData = data.toBool();
            setExpanded(boolData);
        }else if(keyName == "readOnlyDefinition" || keyName == "snippetID"){
            updateReadState();
        }
        if(keyName == primaryTextKey || keyName == secondaryTextKey){
            update();
        }
    }
}

void NodeItemNew::propertyChanged(QString propertyName, QVariant data)
{
}

void NodeItemNew::dataRemoved(QString keyName)
{
    if(keyName == "readOnlyDefinition" || keyName == "snippetID"){
        updateReadState();
    }
}

void NodeItemNew::childPosChanged()
{
    //Update the child rect.
    QRectF rect;
    foreach(EntityItemNew* child, getChildEntities()){
        if(child->isNodeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }else if(child->isEdgeItem()){
            rect = rect.united(child->currentRect());
        }
    }
    _childRect = rect;
    resizeToChildren();
}

void NodeItemNew::updateReadState()
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

void NodeItemNew::setUpColors()
{
    qreal blendFactor = 0.6;
    QColor blue = QColor(100,149,237);
    QColor brown = QColor(222,184,135);
    QColor originalColor = EntityItemNew::getBodyColor();


    readOnlyDefinitionColor = Theme::blendColors(originalColor, brown, blendFactor);
    readOnlyInstanceColor = Theme::blendColors(originalColor, blue, blendFactor);
}

void NodeItemNew::resizeToChildren()
{
    QRectF currentGridRect = expandedGridRect();
    QRectF minRect = childrenRect();

    QSizeF deltaSize;
    deltaSize.rwidth() += minRect.right() - currentGridRect.right();
    deltaSize.rheight() += minRect.bottom() - currentGridRect.bottom();

    adjustExpandedSize(deltaSize);
}

int NodeItemNew::getVertexAngle(RECT_VERTEX vert) const
{
    //Bottom Left is 360 and 0, going clockwise
    //Bottom left is technically 225 degrees from 0
    int interval = 45;
    int initial = 225;
    return (initial + (vert * interval)) % 360;
}

int NodeItemNew::getResizeArrowRotation(RECT_VERTEX vert) const
{
    //Image starts Facing Bottom. which is 180
    int imageOffset = 180;
    return (imageOffset + getVertexAngle(vert)) % 360;
}



QPainterPath NodeItemNew::getChildNodePath()
{
    QPainterPath path;
    foreach(NodeItemNew* child, childNodes.values()){
        path.addRect(child->translatedBoundingRect());
    }
    return path;
}


void NodeItemNew::updateGridLines()
{
    if(isGridEnabled()){
        QRectF grid = gridRect();
        int gridSize = getGridSize();
        int majorGridCount = getMajorGridCount();

        //Clear the old grid lines
        gridLines_Major_Horizontal.clear();
        gridLines_Minor_Horizontal.clear();
        gridLines_Major_Vertical.clear();
        gridLines_Minor_Vertical.clear();


        QPointF itemsOriginScenePos = getTopLeftSceneCoordinate();
        QPointF gridRectScenePos =  itemsOriginScenePos + grid.topLeft();

        //Calculate the next grid count.
        int gridX = ceil(gridRectScenePos.x() / gridSize);
        int gridY = ceil(gridRectScenePos.y() / gridSize);

        //Calculate the offset from the grid rect for the next gridlines (Item Coordinates)
        qreal gridOffsetX = (gridX * gridSize) - itemsOriginScenePos.x();
        qreal gridOffsetY = (gridY * gridSize) - itemsOriginScenePos.y();

        int majorCountX = abs(gridX) % majorGridCount;
        int majorCountY = abs(gridY) % majorGridCount;

        //Positive Grid lines are counting down since the last grid line, so we have to invert it.
        if(gridX > 0 && majorCountX > 0){
            majorCountX = majorGridCount - majorCountX;
        }
        if(gridY > 0 && majorCountY > 0){
            majorCountY = majorGridCount - majorCountY;
        }

        for(qreal x = gridOffsetX; x <= grid.right(); x += gridSize){
            QLineF line(x, grid.top(), x, grid.bottom());

            if(majorCountX == 0){
                gridLines_Major_Horizontal.append(line);
                majorCountX = majorGridCount;
            }else{
                gridLines_Minor_Horizontal.append(line);
            }
            majorCountX --;
        }

        for(qreal y = gridOffsetY; y <= grid.bottom(); y += gridSize){
            QLineF line(grid.left(), y, grid.right(), y);
            if(majorCountY == 0){
                gridLines_Major_Horizontal.append(line);
                majorCountY = majorGridCount;
            }else{
                gridLines_Minor_Horizontal.append(line);
            }
            majorCountY --;
        }
    }
}

void NodeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    RENDER_STATE state = getRenderState(lod);

    //Paint the grid lines.
    if(isGridVisible()){
        painter->save();

        painter->setClipRect(gridRect());

        QPen linePen;
        linePen.setColor(getBaseBodyColor().darker(150));
        linePen.setStyle(Qt::DotLine);
        linePen.setWidthF(.5);

        painter->setBrush(Qt::NoBrush);
        painter->setPen(linePen);
        painter->drawLines(gridLines_Minor_Horizontal);
        painter->drawLines(gridLines_Minor_Vertical);

        linePen.setStyle(Qt::SolidLine);
        painter->setPen(linePen);
        painter->drawLines(gridLines_Major_Horizontal);
        painter->drawLines(gridLines_Major_Vertical);
        painter->restore();
    }

    if(state > RS_BLOCK){
        painter->setPen(getPen());
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(getElementPath(ER_SELECTION));

        painter->setPen(Qt::black);

        if(gotPrimaryTextKey()){
            renderText(painter, lod, getElementRect(ER_PRIMARY_TEXT), getPrimaryText());
        }
        if(gotSecondaryTextKey()){
            painter->setBrush(Qt::red);
            painter->setPen(Qt::NoPen);
            //painter->drawRect(getElementRect(ER_SECONDARY_TEXT));
            renderText(painter, lod, getElementRect(ER_SECONDARY_TEXT), getSecondaryText(), 5);
        }
    }


    if(state > RS_BLOCK){
        painter->save();
        QColor resizeColor(150, 150, 150, 150);

        painter->setPen(Qt::NoPen);
        painter->setBrush(resizeColor);


        if(selectedResizeVertex != RV_NONE){
            painter->drawRect(getResizeRect(selectedResizeVertex));
        }

        if(hoveredResizeVertex != RV_NONE){
            painter->drawRect(getResizeRect(hoveredResizeVertex));

            QRectF arrowRect = getResizeArrowRect(hoveredResizeVertex);
            //Rotate
            painter->save();
            painter->translate(arrowRect.center());

            painter->rotate(getResizeArrowRotation(hoveredResizeVertex));

            painter->translate(-(arrowRect.width() / 2), - (arrowRect.height() / 2));

            paintPixmap(painter, lod, arrowRect.translated(-arrowRect.topLeft()), "Actions", "Resize", Qt::black);
            painter->restore();
        }

        painter->restore();
    }

    EntityItemNew::paint(painter, option, widget);
}

QRectF NodeItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MOVE:{
        return headerRect();
    }
    }
    //Just call base class.
    return EntityItemNew::getElementRect(rect);
}

QPainterPath NodeItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    return EntityItemNew::getElementPath(rect);
}

QRectF NodeItemNew::getResizeRect(RECT_VERTEX vert) const
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

QRectF NodeItemNew::getResizeArrowRect(RECT_VERTEX vert) const
{
    QRectF rect(0, 0, 2 * RESIZE_RECT_SIZE, 2 * RESIZE_RECT_SIZE);
    rect.moveCenter(getResizeRect(vert).center());
    return rect;
}

void NodeItemNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool caughtResize = false;
    if(isResizeEnabled() && isExpanded()){
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
            update();
            caughtResize = true;
        }
    }

    if(!caughtResize){
        EntityItemNew::mousePressEvent(event);
    }
}

void NodeItemNew::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
   if(selectedResizeVertex != RV_NONE){
       emit req_adjustSizeFinished(this, selectedResizeVertex);
       selectedResizeVertex = RV_NONE;
       update();
   }else{
       EntityItemNew::mouseReleaseEvent(event);
   }
}

void NodeItemNew::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(selectedResizeVertex != RV_NONE){
        QPointF deltaPos = event->scenePos() - previousResizePoint;
        previousResizePoint = event->scenePos();
        setManuallyAdjusted(selectedResizeVertex);
        emit req_adjustSize(this, QSizeF(deltaPos.x(), deltaPos.y()), selectedResizeVertex);
    }else{
        EntityItemNew::mouseMoveEvent(event);
    }
}

void NodeItemNew::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(isResizeEnabled() && isExpanded()){
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
    EntityItemNew::hoverMoveEvent(event);
}

void NodeItemNew::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(hoveredResizeVertex != RV_NONE){
        hoveredResizeVertex = RV_NONE;
        update();
    }
    EntityItemNew::hoverLeaveEvent(event);
}

void NodeItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
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
    EntityItemNew::mouseDoubleClickEvent(event);
}

QColor NodeItemNew::getBodyColor() const
{
    if(isHighlighted()){
        //Do nothing
    }else if(getReadState() == READ_ONLY_DEFINITION){
        return readOnlyDefinitionColor;
    }else if(getReadState() == READ_ONLY_INSTANCE){
        return readOnlyInstanceColor;
    }
    return EntityItemNew::getBodyColor();
}
