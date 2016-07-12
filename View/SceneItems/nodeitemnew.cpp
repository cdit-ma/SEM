#include "nodeitemnew.h"
#include "entityitemnew.h"

#include <QDebug>
#include <QStyleOptionGraphicsItem>

#define RESIZE_RECT_SIZE 5
NodeItemNew::NodeItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, NodeItemNew::KIND kind):EntityItemNew(viewItem, parentItem, EntityItemNew::NODE)
{
    minimumHeight = 0;
    minimumWidth = 0;
    expandedHeight = 0;
    expandedWidth = 0;
    gridVisible = false;
    gridEnabled = false;
    resizeEnabled = false;
    aspect = VA_NONE;
    selectedResizeVertex = RV_NONE;
    hoveredResizeVertex = RV_NONE;

    nodeViewItem = viewItem;
    nodeItemKind = kind;

    setMoveEnabled(true);
    setGridEnabled(true);
    setSelectionEnabled(true);
    setResizeEnabled(true);
    setExpandEnabled(true);


    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
    addRequiredData("isExpanded");

    reloadRequiredData();

    if(parentItem){
        //Lock child in same aspect as parent
        setAspect(parentItem->getAspect());

        parentItem->addChildNode(this);
        setParentItem(parentItem);
    }
}

NodeItemNew::~NodeItemNew()
{

}


QColor NodeItemNew::getBodyColor() const
{
    return bodyColor;
}

void NodeItemNew::setBodyColor(QColor color)
{
    if(bodyColor != color){
        bodyColor = color;
        update();
    }
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

void NodeItemNew::addChildNode(NodeItemNew *nodeItem)
{
    int ID = nodeItem->getID();
    //If we have added a child, and there is only one. emit a signal
    if(!childNodes.contains(ID)){
        childNodes[ID] = nodeItem;
        if(childNodes.count() == 1){
            emit gotChildNodes(true);
        }
    }
}

void NodeItemNew::removeChildNode(int ID)
{
    //If we have removed a child, and there is no children left. emit a signal
    if(childNodes.remove(ID) > 0){
        if(childNodes.count() == 0){
            emit gotChildNodes(false);
        }
    }
}

bool NodeItemNew::hasChildNodes() const
{
    return !childNodes.isEmpty();
}

QList<NodeItemNew *> NodeItemNew::getChildNodes() const
{
    return childNodes.values();
}

QList<EntityItemNew *> NodeItemNew::getChildEntities() const
{
    QList<EntityItemNew*> children;
    foreach(NodeItemNew* child, childNodes.values()){
        children.append(child);
    }
    foreach(EdgeItemNew* child, childEdges.values()){
        children.append(child);
    }
    return children;
}

void NodeItemNew::addChildEdge(EdgeItemNew *edgeItem)
{
    int ID = edgeItem->getID();
    if(!childEdges.contains(ID)){
        childEdges[ID] = edgeItem;
    }
}

void NodeItemNew::removeChildEdge(int ID)
{
    childEdges.remove(ID);
}

QList<EdgeItemNew *> NodeItemNew::getChildEdges()
{
    return childEdges.values();
}

void NodeItemNew::addProxyEdge(EdgeItemNew *edgeItem)
{
    int ID = edgeItem->getID();
    if(!proxyChildEdges.contains(ID)){
        proxyChildEdges[ID] = edgeItem;

        if(proxyChildEdges.count() == 1){
            emit gotChildProxyEdges(true);
        }
    }
}

void NodeItemNew::removeProxyEdge(int ID)
{
    if(proxyChildEdges.remove(ID) > 0){
        if(proxyChildEdges.count() == 0){
            emit gotChildProxyEdges(false);
        }
    }
}

QList<EdgeItemNew *> NodeItemNew::getProxyEdges()
{
    return proxyChildEdges.values();
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
    return QRectF(getMarginOffset(), QSizeF(getMinimumWidth(), getMinimumHeight()));
}

QRectF NodeItemNew::expandedRect() const
{
    return QRectF(getMarginOffset(), QSizeF(getExpandedWidth(), getExpandedHeight()));
}

QRectF NodeItemNew::currentRect() const
{
    return QRectF(getMarginOffset(), QSizeF(getWidth(), getHeight()));
}

QRectF NodeItemNew::gridRect() const
{
    return bodyRect().marginsRemoved(getBodyPadding());
}

QRectF NodeItemNew::bodyRect() const
{
    return currentRect();
}

QRectF NodeItemNew::moveRect() const
{
    return EntityItemNew::moveRect();
}


QRectF NodeItemNew::childrenRect() const
{
    QRectF rect;
    foreach(EntityItemNew* child, getChildEntities()){
        rect = rect.united(child->translatedBoundingRect());
    }
    return rect;
}

QSizeF NodeItemNew::getSize() const
{
    QSizeF size;
    size.setWidth(getWidth());
    size.setHeight(getHeight());
    return size;
}

void NodeItemNew::setMinimumWidth(qreal width)
{
    if(minimumWidth != width){
        minimumWidth = width;
        if(!isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
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
            emit sizeChanged(getSize());
        }
    }
}

void NodeItemNew::setExpandedWidth(qreal width)
{
    //Limit by the size of all contained children.
    qreal minWidth = childrenBoundingRect().right();
    //Can't shrink smaller than minimum
    minWidth = qMax(minWidth, minimumWidth);
    width = qMax(width, minWidth);

    if(expandedWidth != width){
        expandedWidth = width;

        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
            updateGridLines();
        }
    }
}

void NodeItemNew::setExpandedHeight(qreal height)
{
    //Limit by the size of all contained children.
    qreal minHeight = childrenBoundingRect().bottom();
    //Can't shrink smaller than minimum
    minHeight = qMax(minHeight, minimumHeight);
    height = qMax(height, minHeight);

    if(expandedHeight != height){
        expandedHeight = height;
        if(isExpanded()){
            prepareGeometryChange();
            update();
            emit sizeChanged(getSize());
            updateGridLines();
        }
    }
}


qreal NodeItemNew::getExpandedWidth() const
{
    return expandedWidth;
}

qreal NodeItemNew::getExpandedHeight() const
{
    return expandedHeight;
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

QPointF NodeItemNew::getCenter() const
{
    QPointF center = pos() + contractedRect().center();

    NodeItemNew* parent = getParentNodeItem();

    if(parent){
        center -= parent->getMarginOffset();
    }
    return center;
}

void NodeItemNew::setCenter(QPointF center)
{
    center -= contractedRect().center();

    NodeItemNew* parent = getParentNodeItem();

    if(parent){
        center += parent->getMarginOffset();
    }

    setPos(center);
}

void NodeItemNew::setPos(const QPointF &pos)
{
    if(this->pos() != pos){
        QGraphicsObject::setPos(pos);
        updateGridLines();
        emit positionChanged();
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
        foreach(NodeItemNew* child, getChildNodes()){
            child->setVisible(isExpanded());
        }

        update();
        emit sizeChanged(getSize());
    }
}



void NodeItemNew::dataChanged(QString keyName, QVariant data)
{
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
            setExpandedWidth(realData);
        }else if(keyName == "height"){
            setExpandedHeight(realData);
        }
    }else if(keyName == "isExpanded"){
        bool boolData = data.toBool();
        setExpanded(boolData);
    }
}

int NodeItemNew::getResizeArrowRotation(RECT_VERTEX vert) const
{
    //Image starts Facing Bottom.
    return -45 + (-45 * int(vert));
}

int NodeItemNew::getGridSize() const
{
    return 10;
}

int NodeItemNew::getMajorGridCount() const
{
    return 5;
}

QPainterPath NodeItemNew::getChildNodePath()
{
    QPainterPath path;
    foreach(NodeItemNew* child, childNodes.values()){
        path.addRect(child->translatedBoundingRect());
    }
    return path;
}

QPointF NodeItemNew::getNextChildPos(QRectF childRect)
{
    if(childRect.isNull()){
        childRect = contractedRect();
    }


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
        linePen.setColor(getBodyColor().darker(150));
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
        painter->drawRect(currentRect());
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

            painter->rotate(-getResizeArrowRotation(hoveredResizeVertex));

            painter->translate(-(arrowRect.width() / 2), - (arrowRect.height() / 2));

            paintPixmap(painter, lod, arrowRect.translated(-arrowRect.topLeft()), "Actions", "Resize", Qt::black);
            painter->restore();
        }


        painter->restore();
    }


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
        emit req_adjustSize(nodeViewItem, QSizeF(deltaPos.x(), deltaPos.y()), selectedResizeVertex);
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
