#include "nodeitem.h"
#include "entityitem.h"
#include "graphmlitem.h"
#include "../nodeview.h"
#include <QDebug>
#include <math.h>

#define MARGIN_RATIO (1.0 / 18.0) //NORMAL

#define SELECTED_LINE_WIDTH 3
#define ITEM_SIZE 72
#define ASPECT_ROW_COUNT 6
#define ASPECT_COL_COUNT 4

#define ACTUAL_ITEM_SIZE (ITEM_SIZE + ((MARGIN_RATIO * 2) * ITEM_SIZE))
#define GRID_COUNT 2
#define GRID_PADDING_RATIO .25

#define GRID_MARGIN (GRID_PADDING_RATIO * ACTUAL_ITEM_SIZE)
#define GRID_SIZE ((ACTUAL_ITEM_SIZE + GRID_MARGIN) / GRID_COUNT)

#define SNAP_PERCENTAGE .5


#define ALL 0
#define CONNECTED 1
#define UNCONNECTED 2

#define THEME_LIGHT 0
#define THEME_DARK 1

#define THEME_DARK_NEUTRAL 10
#define THEME_DARK_COLOURED 11

NodeItem::NodeItem(Node *node, GraphMLItem *parent, GraphMLItem::GUI_KIND kind) : GraphMLItem(node, parent, kind)
{
    if(parent && parent->isNodeItem()){
        setViewAspect(((NodeItem*)parent)->getViewAspect());
    }

    DRAW_GRID = false;
    if(parent && parent->isNodeItem()){
        //Connect Signals.
        connect(this, SIGNAL(GraphMLItem_PositionChanged()), ((NodeItem*)parent), SLOT(childPositionChanged()));
        connect(this, SIGNAL(GraphMLItem_SizeChanged()), ((NodeItem*)parent), SLOT(childSizeChanged()));
    }

    connect(this, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(updateGrid()));


}

NodeItem::~NodeItem()
{

}

QRectF NodeItem::childrenBoundingRect()
{
    QRectF rect;
    foreach(GraphMLItem* child, getChildren()){
        if(!child->isEdgeItem()){
            rect = rect.united(child->translatedBoundingRect());
        }
    }
    return rect;
}

QRectF NodeItem::getChildBoundingRect()
{
    QRectF rect;
    rect.setWidth(ITEM_SIZE);
    rect.setHeight(ITEM_SIZE);
    return rect;
}

VIEW_ASPECT NodeItem::getViewAspect()
{
    return viewAspect;
}

void NodeItem::setViewAspect(VIEW_ASPECT aspect)
{
    viewAspect = aspect;
}

QRectF NodeItem::minimumRect() const
{
    return boundingRect();
}

QPointF NodeItem::getCenterOffset() const
{
    return boundingRect().center();
}


QPointF NodeItem::getMinimumRectCenter() const
{
    return minimumRect().center();
}

QPointF NodeItem::getMinimumRectCenterPos() const
{
    return getMinimumRectCenter() + pos();
}

void NodeItem::setMinimumRectCenterPos(QPointF pos)
{
    pos -= getMinimumRectCenter();
    setPos(pos);
}

void NodeItem::sortChildren()
{
    //Get the number of un-locked items
    QMap<int, NodeItem*> toSortMap;

    foreach(GraphMLItem* child, getChildren()){
        if(!child->isVisible() || !child->isNodeItem()){
            continue;
        }
        NodeItem* nodeItem = (NodeItem*)child;
        //Find the sortOrder.
        int childSortOrder = toSortMap.size();
        QString sortOrder = nodeItem->getGraphMLDataValue("sortOrder");

        if(sortOrder != ""){
            childSortOrder = sortOrder.toInt();
        }
        toSortMap.insertMulti(childSortOrder, nodeItem);
    }

    QList<NodeItem*> toSortItems = toSortMap.values();

    QPainterPath sortedPath;
    //Sort Items
    while(toSortItems.size() > 0){
        NodeItem* item = toSortItems.takeFirst();
        QPointF nextPos = getNextChildPos(item->boundingRect(), sortedPath, true);
        item->setMinimumRectCenterPos(nextPos);
        item->updatePositionInModel();
        sortedPath.addRect(item->translatedBoundingRect());
    }
}

void NodeItem::updatePositionInModel(bool directUpdate)
{
    if(isAspectItem()){
        //Don't set Position for Aspects
        return;
    }

    if(getParent() && getParent()->isNodeItem()){
        getParentNodeItem()->hideChildGridOutline(getID());
        getParentNodeItem()->updateSizeInModel(directUpdate);
    }

    //Update the Size in the model.
    QPointF center = getMinimumRectCenterPos();

    if(directUpdate){
        setGraphMLData("x", center.x());
        setGraphMLData("y", center.y());
    }else{
        emit GraphMLItem_SetGraphMLData(getID(), "x", center.x());
        emit GraphMLItem_SetGraphMLData(getID(), "y", center.y());
    }



}

void NodeItem::updateSizeInModel(bool directUpdate)
{
    //If we have a NodeItem
    if(getParentNodeItem()){
        getParentNodeItem()->updateSizeInModel(directUpdate);
    }

    if(directUpdate){
        setGraphMLData("width", getWidth());
        setGraphMLData("height", getHeight());
    }else{
        emit GraphMLItem_SetGraphMLData(getID(), "width", getWidth());
        emit GraphMLItem_SetGraphMLData(getID(), "height", getHeight());
    }


}



void NodeItem::snapToGrid()
{
    if(!getParentNodeItem()){
        return;
    }

    QPointF gridPoint = getParentNodeItem()->getClosestGridPoint(getMinimumRectCenterPos());

    if(!gridPoint.isNull()){
        setMinimumRectCenterPos(gridPoint);
    }
    updatePositionInModel();
}

void NodeItem::addEdge(EdgeItem *edge)
{
    int ID = edge->getID();
    connectedEdges[ID] = edge;
}

void NodeItem::removeEdge(int ID)
{
    connectedEdges.remove(ID);
    if(connectedEdges.isEmpty()){
        emit lastEdgeRemoved();
    }
}

bool NodeItem::hasEdges()
{
    return !connectedEdges.isEmpty();
}

void NodeItem::addEdgeAsChild(EdgeItem *edge)
{
    int ID = edge->getID();
    childEdges[ID] = edge;
}

void NodeItem::removeChildEdge(int ID)
{
    childEdges.remove(ID);
}

QList<EdgeItem *> NodeItem::getEdges()
{
    return connectedEdges.values();
}

void NodeItem::resizeToOptimumSize(NodeItem::RESIZE_TYPE rt)
{
    QRectF rect = childrenBoundingRect();
    switch(rt){
    case RESIZE:
        setWidth(rect.right());
        setHeight(rect.bottom());
        break;
    case HORIZONTAL_RESIZE:
        setWidth(rect.right());
        break;
    case VERTICAL_RESIZE:
        setHeight(rect.bottom());
        break;
    default:
        break;
    }
}


void NodeItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getWidth() + delta.width();
    qreal newHeight = getHeight() + delta.height();
    setWidth(newWidth);
    setHeight(newHeight);
}

void NodeItem::showChildGridOutline(NodeItem *item, QPointF gridPoint)
{
    QRectF nodeRect = item->boundingRect();
    qreal halfPenWidth = 5 ;
    nodeRect.adjust(-halfPenWidth, -halfPenWidth, halfPenWidth, halfPenWidth);
    QPointF deltaPos = gridPoint - item->minimumRect().center();
    nodeRect.translate(deltaPos);

    bool updateOld = true;
    bool updateNew = true;

    int ID = item->getID();
    QRectF oldRect = childrenGridOutlines[ID];

    if(oldRect.isNull()){
        updateOld = false;
    }else if(oldRect == nodeRect){
        updateOld = false;
        updateNew = false;
    }

    childrenGridOutlines[item->getID()] = nodeRect;

    if(updateOld){
        update(oldRect);
    }
    if(updateNew){
        update(nodeRect);
    }
}

void NodeItem::hideChildGridOutline(int ID)
{
    if(childrenGridOutlines.contains(ID)){
        childrenGridOutlines.remove(ID);
        update();
    }
}

QList<QRectF> NodeItem::getChildrenGridOutlines()
{
    return childrenGridOutlines.values();
}

void NodeItem::clearChildrenGridOutlines()
{
    if(!childrenGridOutlines.isEmpty()){
        childrenGridOutlines.clear();
        update();
    }
}

void NodeItem::adjustPos(QPointF delta)
{
    setPos(pos() + delta);
}

QPointF NodeItem::getNextChildPos(QRectF itemRect, QPainterPath childrenPath, bool usePainterPath)
{

    bool useItemRect = !itemRect.isNull();

    //Don't use the provided path, construct a new path.
    if(!usePainterPath){
        childrenPath = QPainterPath();

        // add the children's bounding rectangles to the children path
        foreach (GraphMLItem* child, getChildren()) {
            if (child) {
                childrenPath.addRect(child->translatedBoundingRect());
            }
        }
    }

    // work out how many grid cells are needed per child item
    // divide it by 2 - only need half the number of cells to fit the center of the item
    double startingGridPoint = ceil(getChildBoundingRect().width()/getGridSize()) / 2;

    double currentX = startingGridPoint;
    double currentY = startingGridPoint;

    double maxX = 0;
    bool xOutsideOfGrid = false;
    bool yOutsideOfGrid = false;

    while (true) {

        // get the next position; construct a child rect at that position
        QPointF nextPosition = getGridPosition(currentX, currentY);
        QRectF childRect = getChildBoundingRect();

        if (useItemRect) {
            childRect = itemRect;
            childRect.translate(nextPosition - getChildBoundingRect().center());
        } else {
            //Translate to the center of the normal childBR
            childRect.translate(nextPosition - childRect.center());
        }

        // check if the child rect collides with an existing child item
        if (childrenPath.intersects(childRect)) {
            // if so, check the next x position
            currentX++;
            // collision means that current position is inside the grid
            xOutsideOfGrid = false;
            yOutsideOfGrid = false;
        } else {
            // if there is no collision and the current position is inside the grid
            // it's a valid position - return it
            //if (gridRect().contains(nextPosition)) {
            if (gridRect().contains(childRect)) {
                return nextPosition;
            }

            // if both currentX and currentY are outside of the grid,
            // it means that there is no available spot in the grid
            if (xOutsideOfGrid && yOutsideOfGrid) {
                // return a new position depending on which of the width/height is bigger
                QPointF finalPosition = getGridPosition(maxX, startingGridPoint);
                if (boundingRect().width() > boundingRect().height()) {
                    finalPosition = getGridPosition(startingGridPoint, currentY);
                }
                return finalPosition;
            }

            // because currentY was incremented when currentX was outside
            // of the grid, it means that it is now also out of the grid
            if (xOutsideOfGrid) {
                yOutsideOfGrid = true;
            } else {
                // when it gets into this case, it means that currentX is outside of the grid
                xOutsideOfGrid = true;
                // store the maximum x
                if (currentX > maxX) {
                    maxX = currentX;
                }
                // reset currentX then check the next y position
                currentX = startingGridPoint;
                currentY++;
            }
        }
    }
}


void NodeItem::toggleGridMode(bool visible)
{
    if(IS_GRID_MODE_ON != visible){
        IS_GRID_MODE_ON = visible;
        updateGrid();
        if(drawGridLines()){
            update();
        }
    }
}

QPointF NodeItem::getClosestGridPoint(QPointF childCenterPos)
{
    //Get the gridSize
    qreal gridSize = getGridSize();

    //Offset the childCenterPos by the starting position of the gridRect so we can do easy division.
    QPointF gridOffset = gridRect().topLeft();
    childCenterPos -= gridOffset;

    //Find the closest gridline to the childCenterPos, then add the offset again.
    childCenterPos.setX((qRound(childCenterPos.x() / gridSize) * gridSize) + gridOffset.x());
    childCenterPos.setY((qRound(childCenterPos.y() / gridSize) * gridSize) + gridOffset.y());

    //Calculate the Bounding Rect of the the child.
    QPointF childOffset = QPointF(ITEM_SIZE / 2, ITEM_SIZE / 2);
    QRectF childRect = QRectF(childCenterPos - childOffset, childCenterPos + childOffset);

    //Check for collision with the parent boudning box.
    if(!gridRect().contains(childRect)){
        //Make sure the Top of the child is fully contained!
        while(childRect.top() <= gridRect().top()){
            childRect.translate(0, gridSize);
        }
        //Make sure the Left of the child is fully contained!
        while(childRect.left() <= gridRect().left()){
            childRect.translate(gridSize, 0);
        }
        return childRect.center();
    }

    return childCenterPos;
}

QPointF NodeItem::getGridPosition(int x, int y)
{
    QPointF topLeft = gridRect().topLeft();
    topLeft.rx() += (x * getGridSize());
    topLeft.ry() += (y * getGridSize());
    return topLeft;
}

void NodeItem::updateGrid()
{
    if(IS_GRID_MODE_ON){
        QRectF boundingGridRect = gridRect();
        verticalGridLines.clear();
        horizontalGridLines.clear();


        for(qreal x = boundingGridRect.left() + getGridSize(); x <= boundingGridRect.right(); x += getGridSize()){
            verticalGridLines << QLineF(x, boundingGridRect.top(), x, boundingGridRect.bottom());
        }

        for(qreal y = boundingGridRect.top() + getGridSize(); y <= boundingGridRect.bottom(); y += getGridSize()){
            horizontalGridLines << QLineF(boundingGridRect.left(), y, boundingGridRect.right(), y);
        }
    }
}

QVector<QLineF> NodeItem::getGridLines()
{
    return verticalGridLines + horizontalGridLines;
}

qreal NodeItem::getGridSize()
{
    return GRID_SIZE;
}

qreal NodeItem::getGridMargin()
{
    return GRID_MARGIN;
}

bool NodeItem::drawGridLines()
{
    return IS_GRID_MODE_ON && DRAW_GRID;
}

void NodeItem::setDrawGrid(bool drawGrid)
{
    if(DRAW_GRID != drawGrid){
        DRAW_GRID = drawGrid;
        if(DRAW_GRID){
            clearChildrenGridOutlines();
        }
        update();
    }
}

bool NodeItem::isGridModeOn()
{
    return IS_GRID_MODE_ON;
}

NodeItem *NodeItem::getParentNodeItem()
{
    if(getParent() && getParent()->isNodeItem()){
        return (NodeItem*)getParent();
    }
    return 0;
}

Node *NodeItem::getNode()
{
    return (Node*)getGraphML();
}

NodeItem::RESIZE_TYPE NodeItem::getResizeMode()
{
    return resizeMode;
}

void NodeItem::setResizeMode(NodeItem::RESIZE_TYPE mode)
{
    resizeMode = mode;
}


NodeItem::RESIZE_TYPE NodeItem::resizeEntered(QPointF mousePosition)
{
    int cornerRadius = getItemMargin() * 2;

    QRectF cornerRect(0,0, cornerRadius, cornerRadius);
    cornerRect.moveBottomRight(boundingRect().bottomRight());

    //Check if the Mouse is in the Bottom Right Corner.
    if(cornerRect.contains(mousePosition)){
        return RESIZE;
    }

    //Calculate the Corners for the Horizontal resize
    QPointF topLeft = boundingRect().topRight() + QPointF(-cornerRadius, cornerRadius);
    QPointF bottomRight = boundingRect().bottomRight() - QPointF(0, cornerRadius);

    //Check if the mouse is contained in the rectangle on the right of the EntityItem.
    QRectF horizontalResizeRectangle = QRectF(topLeft, bottomRight);
    if(horizontalResizeRectangle.contains(mousePosition)){
        return HORIZONTAL_RESIZE;
    }

    //Calculate the Corners for the Vertical resize
    QPointF bottomLeft = boundingRect().bottomLeft() + QPointF(cornerRadius, -cornerRadius);
    bottomRight = boundingRect().bottomRight() + QPointF(-cornerRadius, 0);

    //Check if the mouse is contained in the rectangle on the bottom of the EntityItem.
    QRectF verticalRect = QRectF(bottomLeft, bottomRight);
    if(verticalRect.contains(mousePosition)){
        return VERTICAL_RESIZE;
    }

    return NO_RESIZE;
}
