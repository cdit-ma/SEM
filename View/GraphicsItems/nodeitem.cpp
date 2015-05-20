#include "nodeitem.h"
#include "edgeitem.h"
#include "../nodeview.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QRubberBand>
#include <QPixmap>
#include <QBitmap>

#include <QInputDialog>
#include <QTextBlockFormat>

#include <QTextCursor>
#include <math.h>

#define MODEL_WIDTH 1920
#define MODEL_HEIGHT 1080


//#define MARGIN_RATIO 0.15 //LARGE
#define MARGIN_RATIO 0.1 //NORMAL
//#define MARGIN_RATIO 0.05 //COMPACT

//#define ICON_RATIO 0.7 //LARGE
#define ICON_RATIO 0.8 //NORMAL
//#define ICON_RATIO 0.9 //COMPACT


//RATIO FROM ASPECT TO FIRST ENTITY.
#define ENTITY_SIZE_RATIO 10
//RATIO FROM MODEL TO ASPECTS
#define ASPECT_SIZE_RATIO 3

#define LABEL_RATIO (1 - ICON_RATIO)

#define SNAP_PERCENTAGE .25

#define GRID_RATIO 1.75

/**
 * @brief NodeItem::NodeItem
 * @param node
 * @param parent
 * @param aspects
 * @param IN_SUBVIEW
 */
NodeItem::NodeItem(Node *node, NodeItem *parent, QStringList aspects, bool IN_SUBVIEW):  GraphMLItem(node, GraphMLItem::NODE_ITEM)
{
    Q_INIT_RESOURCE(resources);
    setParentItem(parent);

    parentNodeItem = parent;
    isNodeSelected = false;
    setNodeResizing(false);
    setNodeMoving(false);

    isNodeSorted = false;

    isInSubView = IN_SUBVIEW;
    parentView = 0;

    currentResizeMode = NodeItem::NO_RESIZE;
    LOCKED_POSITION = false;
    GRIDLINES_ON = false;
    isGridVisible = false;
    isNodeOnGrid = false;

    isNodePressed = false;
    isNodeCentralized = false;
    isNodeExpanded = true;
    hidden = false;
    hasSelectionMoved = false;
    hasSelectionResized = false;

    hasDefinition = false;

    icon = 0;
    lockIcon = 0;
    textItem = 0;
    width = 0;
    height = 0;
    expandedWidth = 0;
    expandedHeight = 0;
    minimumWidth = 0;
    minimumHeight = 0;

    nodeKind = getGraphML()->getDataValue("kind");

    QString parentNodeKind = "";
    if (parent) {
        setVisible(parent->isExpanded());

        setWidth(parent->getChildWidth());
        setHeight(parent->getChildHeight());

        parentNodeKind = parent->getGraphML()->getDataValue("kind");

    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }

    //Set Minimum Size.
    if(nodeKind == "Model"){
        minimumWidth = getChildWidth();
        minimumHeight = getChildHeight();
    }else{
        minimumWidth = width;
        minimumHeight = height;
    }
    minimumHeightStr = QString::number(minimumHeight);

    //Set Maximum Size
    expandedWidth = width;
    expandedHeight = height;


    //Update Width and Height with values from the GraphML Model If they have them.
    retrieveGraphMLData();

    setupGraphMLConnections();

    setupAspect();
    setupBrushes();
    setupIcon();
    setupLabel();
    setupLockMenu();

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    //QGraphicsItem::setAcceptsHoverEvents

    setCacheMode(QGraphicsItem::NoCache);

    if(getGraphML()->getDataValue("kind") == "Model" || getGraphML()->getDataValue("kind") == "DeploymentDefinitions"){
        setPaintObject(false);
    }else{
        setPaintObject(true);
    }



    updateParentModel();

    if(IN_SUBVIEW){
        setVisible(true);
    }else{
        updateModelData();
    }
}


/**
 * @brief NodeItem::~NodeItem
 */
NodeItem::~NodeItem()
{
    if (parentNodeItem) {
        parentNodeItem->removeChildNodeItem(this);
    }
    delete textItem;
}


NodeItem *NodeItem::getParentNodeItem()
{
    return dynamic_cast<NodeItem*>(parentItem());
}


QList<EdgeItem *> NodeItem::getEdgeItems()
{
    return this->connections;
}


void NodeItem::setParentItem(QGraphicsItem *parent)
{
    NodeItem* nodeItem = dynamic_cast<NodeItem*>(parent);
    if(nodeItem){
        nodeItem->addChildNodeItem(this);
        connect(nodeItem, SIGNAL(nodeItemMoved()), this, SLOT(parentNodeItemMoved()));
    }
    QGraphicsItem::setParentItem(parent);
}


QRectF NodeItem::boundingRect() const
{
    qreal topLeftX = 0;
    qreal topLeftY = 0;
    qreal bottomRightX = width;
    qreal bottomRightY = height;

    float itemMargin = getItemMargin();
    bottomRightX += itemMargin;
    bottomRightY += itemMargin;
    bottomRightX += itemMargin;
    bottomRightY += itemMargin;

    return QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
}


QRectF NodeItem::minimumVisibleRect()
{

    qreal totalMargin = getItemMargin() * 2;
    return QRectF(QPointF(0, 0), QPointF(minimumWidth + totalMargin, minimumHeight + totalMargin));
}

QRectF NodeItem::expandedVisibleRect()
{
    return QRectF(QPointF(getItemMargin(), getItemMargin()), QPointF(expandedWidth + getItemMargin(), expandedHeight + getItemMargin()));
}

QRectF NodeItem::currentItemRect()
{
    return QRectF(QPointF(getItemMargin(), getItemMargin()), QPointF(width + getItemMargin(), height + getItemMargin()));
}

int NodeItem::getEdgeItemIndex(EdgeItem *item)
{
     return connections.indexOf(item);

}

int NodeItem::getEdgeItemCount()
{
    return connections.size();


}

/**
 * @brief NodeItem::gridRect Returns a QRectF which contains the local coordinates of where the Grid lines are to be drawn.
 * @return The grid rectangle
 */
QRectF NodeItem::gridRect()
{
    QPointF topLeft = getMinimumChildRect().topLeft();
    QPointF bottomRight = expandedVisibleRect().bottomRight();

    //If it has an icon,
    if (icon){
        topLeft += QPointF(0, minimumHeight);
    }
    //Enforce at least one grid in height. Size!
    qreal deltaY = bottomRight.y() - topLeft.y();
    if(deltaY <= 0){
        deltaY = qMax(deltaY, getGridSize());
        bottomRight.setY(bottomRight.y() + deltaY);
    }

    return QRectF(topLeft, bottomRight);
}

QRectF NodeItem::getChildBoundingRect()
{
    return QRectF(QPointF(0, 0), QPointF(getChildWidth() + (2* getChildItemMargin()), getChildHeight() + (2 * getChildItemMargin())));
}

void NodeItem::childPosUpdated()
{
    if(!getGraphML()){
        return;
    }

    QSizeF minSize = getMinimumChildRect().size();

    double modelWidth = getGraphML()->getDataValue("width").toDouble();
    double modelHeight = getGraphML()->getDataValue("height").toDouble();

    //Maximize on the current size in the Model and the minimum child rectangle
    if(minSize.width() > modelWidth){
        setWidth(minSize.width());
    }

    if(minSize.height() > modelHeight){
        setHeight(minSize.height());
    }

    if(nodeKind == "Model"){
        //Sort after any model changes!
        newSort();
    }
}


QPointF NodeItem::getGridPosition(int x, int y)
{
    QPointF topLeft = gridRect().topLeft();
    topLeft.setX(topLeft.x() + (x * getGridSize()));
    topLeft.setY(topLeft.y() + (y * getGridSize()));
    return topLeft;
}


bool NodeItem::isSelected()
{
    return isNodeSelected;
}


bool NodeItem::isLocked()
{
    return LOCKED_POSITION;
}

void NodeItem::setLocked(bool locked)
{
    LOCKED_POSITION = locked;
}


bool NodeItem::isPainted()
{
    return PAINT_OBJECT;
}


void NodeItem::addChildNodeItem(NodeItem *child)
{
    if(!childNodeItems.contains(child)){
        childNodeItems.append(child);
    }
}


void NodeItem::removeChildNodeItem(NodeItem *child)
{
    childNodeItems.removeAll(child);
    removeChildOutline(child);
    if(childNodeItems.size() == 0){
        resetSize();
    }

}


bool NodeItem::intersectsRectangle(QRectF sceneRect)
{
    /*
    QRectF itemRectangle = boundingRect();
    itemRectangle.moveTo(scenePos());
    return sceneRect.contains(itemRectangle);
    */
    return sceneRect.contains(sceneBoundingRect());
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(PAINT_OBJECT){// == PAINT_OBJECT){
        QPen Pen;
        QBrush Brush;

        if(isNodeSelected){
            //Check current View zoom
            Brush = selectedBrush;
            Pen = selectedPen;
            if(parentView){
                qreal scaleX = parentView->transform().m11();
                qreal penWidth = 5.0/scaleX;
                penWidth = qMax(penWidth, 1.0);
                Pen.setWidth(penWidth);
            }

        }else{
            Brush = brush;
            Pen = pen;
        }


        QRectF rectangle = boundingRect();
        rectangle.setWidth(rectangle.width() - Pen.width());
        rectangle.setHeight(rectangle.height() - Pen.width());
        rectangle.translate(Pen.width()/2, Pen.width()/2);



        //If the Node is over a Gridline, set the background Brush to transluscent.
        if(isNodeOnGrid && (isSelectionMoving || isSelectionResizing)){
            QColor brushColor = Brush.color();
            if(brushColor.alpha() > 120){
                brushColor.setAlpha(120);
            }
            Brush.setColor(brushColor);
        }

        if(!hasVisibleChildren() && !nodeKind.endsWith("Definitions")){
            Brush.setColor(Qt::transparent);
        }

        Node* node = getNode();

        if(node && (node->isInstance() || node->isImpl())){
            if(node->getDefinition()){
                hasDefinition = true;
                lockIcon->setVisible(true);
            }else{
                Brush.setStyle(Qt::BDiagPattern);
                hasDefinition = false;
                lockIcon->setVisible(false);
            }
        }

        painter->setPen(Pen);
        painter->setBrush(Brush);

        painter->drawRoundedRect(rectangle, getCornerRadius(), getCornerRadius());


        if(hasVisibleChildren() && textItem){
            QBrush brush;
            brush.setColor(QColor(100,100,100));
            brush.setStyle(Qt::SolidPattern);
            painter->setBrush(brush);


            painter->setPen(pen);
            QPen altPen = pen;
            altPen.setColor(brush.color());

            if(isExpanded()){
                //FROM THE BOTTM
                qreal yBot = gridRect().bottom();
                //FROM THE TOP
                //qreal yBot = gridRect().top();
                qreal yTop = yBot - getItemMargin();
                qreal xRight = boundingRect().right() - getItemMargin();
                qreal xLeft = xRight - getItemMargin();

                QLineF line = QLineF(gridRect().left(), gridRect().top(), xRight, gridRect().top());
                QPolygonF triangle;
                triangle.append(QPointF(xLeft,yTop));
                triangle.append(QPointF(xRight,yTop));
                triangle.append(QPointF(xLeft,yBot));
                painter->drawLine(line);
                painter->setPen(pen);
                painter->setPen(altPen);
                painter->drawPolygon(triangle,Qt::WindingFill);
            }else{


                qreal yBot = gridRect().top();
                qreal yTop = yBot - getItemMargin();
                qreal xRight = getMinimumChildRect().right();
                qreal xLeft = xRight - getItemMargin();

                QLineF line = QLineF(gridRect().left(), yBot, xRight, yBot);
                QPolygonF triangle;
                triangle.append(QPointF(xRight,yTop));
                triangle.append(QPointF(xRight,yBot));
                triangle.append(QPointF(xLeft,yBot));
                painter->setPen(pen);
                //painter->drawLine(line);
                painter->setPen(altPen);
                painter->drawPolygon(triangle,Qt::WindingFill);
            }
        }

        //New Code
        if(drawGridlines()){
            painter->setClipping(false);
            painter->setPen(pen);
            QPen linePen = painter->pen();

            linePen.setStyle(Qt::DashLine);
            linePen.setWidth(minimumWidth / 1000);
            painter->setPen(linePen);

            painter->drawLines(xGridLines);
            painter->drawLines(yGridLines);


        }



        //Draw the Outlines
        if(this->isExpanded() && GRIDLINES_ON){
            foreach(QRectF rect, outlineMap){
                QRectF newRectangle = rect;
                painter->setBrush(Qt::NoBrush);

                QPen linePen = pen;
                linePen.setColor(Qt::white);
                linePen.setStyle(Qt::DotLine);
                linePen.setWidth(linePen.width() *2);

                newRectangle.setWidth(rect.width() + (linePen.widthF()/2));
                newRectangle.setHeight(rect.height() + (linePen.widthF()/2));
                newRectangle.translate(-linePen.widthF()/2,-linePen.widthF()/2);

                painter->setPen(linePen);
                double radius = getChildCornerRadius();
                painter->drawRoundedRect(newRectangle, radius, radius);
            }
        }
    }

    //painter->setBrush(Qt::NoBrush);
    //painter->drawRect(getMinimumChildRect());

    //painter->drawRect(gridRect());


    if(nodeKind == "Model" && !modelCenterPoint.isNull()){



        int offSet = 100;
        double radius = getItemMargin() + offSet;
        QRect clippingCircle(QPoint(modelCenterPoint.toPoint() - QPoint(radius,radius)), QPoint(modelCenterPoint.toPoint() + QPoint(radius,radius)));

        QBrush circleBrush;
        painter->setClipping(true);
        QRegion clipCircle(clippingCircle, QRegion::Ellipse);
        painter->setClipRegion(clipCircle);

        circleBrush.setColor(Qt::white);
        circleBrush.setStyle(Qt::SolidPattern);
        painter->setBrush(circleBrush);


        double middleButton = radius / 2;

        QRectF tLR(modelCenterPoint - QPointF(radius,radius), modelCenterPoint);
        QRectF tRR(modelCenterPoint - QPointF(0,radius), modelCenterPoint + QPointF(radius,0));
        QRectF bLR(modelCenterPoint - QPointF(radius,0), modelCenterPoint + QPointF(0,radius));
        QRectF bRR(modelCenterPoint,  modelCenterPoint + QPointF(radius,radius));

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(110,210,210));
        painter->drawRect(tLR);
        painter->setBrush(QColor(254,184,126));
        painter->drawRect(tRR);
        painter->setBrush(QColor(255,160,160));
        painter->drawRect(bLR);
        painter->setBrush(QColor(110,170,220));
        painter->drawRect(bRR);


        circleBrush.setColor(Qt::gray);

        painter->setBrush(circleBrush);


        if(isNodeSelected){
            QPen pen = selectedPen;
            if(parentView){
                qreal scaleX = parentView->transform().m11();
                qreal penWidth = 5.0/scaleX;
                penWidth = qMax(penWidth, 1.0);
                pen.setWidth(penWidth);
                painter->setPen(pen);
            }
        }



        painter->drawEllipse(modelCenterPoint, middleButton, middleButton);


        if(textItem){

            painter->setPen(Qt::NoPen);

            /*
             * This draws 2 gray rectangles; it makes it look
             * like the aspects are attached to the model
             */

            /*
            double gapSize = MODEL_WIDTH / 128;

            // horizontal rect
            QRectF trh(0, 0, radius*2, gapSize*2+5);
            trh.translate(textItem->pos().x() -offSet, textItem->pos().y() - 44);
            painter->drawRect(trh);

            // vertical rect
            QRectF trv(0, 0, trh.height(), trh.width());
            trv.translate(modelCenterPoint - QPointF(trh.height()/2, trh.width()/2));
            painter->drawRect(trv);



            // this evens out the colours around the model button
            QRectF textRect = textItem->boundingRect();
            textRect.translate(textItem->pos() + QPoint(textRect.width()/4, 0));
            textRect.setWidth(textRect.width()/2);
            painter->drawRect(textRect);
            */
            QRectF textRect = textItem->boundingRect();
            textRect.translate(textItem->pos());
            painter->drawRect(textRect);

        }


    }
}


bool NodeItem::hasVisibleChildren()
{
    foreach(NodeItem* child, childNodeItems){
        if(!child->isHidden()){
            return true;
        }
    }
    return false;
}

bool NodeItem::modelCirclePressed(QPointF mousePosition)
{
    if(modelCenterPoint.isNull()){
        return false;
    }

    QLineF distance = QLineF(mousePosition, modelCenterPoint);
    if(distance.length() < getItemMargin()/2){
        return true;
    }
    return false;

}


bool NodeItem::labelPressed(QPointF mousePosition)
{
    if(textItem){
        QRectF labelRect = textItem->boundingRect();
        labelRect.translate(textItem->pos());
        if(labelRect.contains(mousePosition)){
            return true;
        }
    }
    return false;
}


bool NodeItem::iconPressed(QPointF mousePosition)
{
    if(icon){
        QRectF labelRect = icon->boundingRect();
        labelRect.setWidth(labelRect.width() * icon->scale());
        labelRect.setHeight(labelRect.height() * icon->scale());
        labelRect.translate(icon->pos());

        if(labelRect.contains(mousePosition)){
            return true;
        }
    }
    return false;
}


/**
 * @brief NodeItem::lockPressed
 * @param mousePosition
 * @return
 */
bool NodeItem::lockPressed(QPointF mousePosition)
{
    if (lockIcon && lockIcon->isVisible()) {
        if (lockIcon->sceneBoundingRect().contains(mousePosition)) {
            return true;
        }
    }
    return false;
}
NodeItem::RESIZE_TYPE NodeItem::resizeEntered(QPointF mousePosition)
{
    if(getResizePolygon().containsPoint(mousePosition, Qt::WindingFill)){
        return RESIZE;
    }

    qreal rectSize =  selectedPen.widthF() * 2;
    qreal cornerRadius = getCornerRadius();

    QPointF topLeft = boundingRect().topRight() + QPointF(-rectSize, cornerRadius);
    QPointF bottomRight = boundingRect().bottomRight() - QPointF(0, cornerRadius);

    QRectF horizontalRect = QRectF(topLeft, bottomRight);
    if(horizontalRect.contains(mousePosition)){
        return HORIZONTAL_RESIZE;
    }

    QPointF bottomLeft = boundingRect().bottomLeft() + QPointF(cornerRadius, -rectSize);
    bottomRight = boundingRect().bottomRight() + QPointF(-cornerRadius, 0);

    QRectF verticalRect = QRectF(bottomLeft, bottomRight);
    if(verticalRect.contains(mousePosition)){
        return VERTICAL_RESIZE;
    }

    return NO_RESIZE;
}

void NodeItem::setNodeMoving(bool moving)
{
    isSelectionMoving = moving;
}

void NodeItem::setNodeResizing(bool resizing)
{
    isSelectionResizing = resizing;
    if(!isSelectionResizing){
        currentResizeMode = NO_RESIZE;
    }
}


/**
 * @brief NodeItem::isHidden
 * @return
 */
bool NodeItem::isHidden()
{
    return hidden;
}


double NodeItem::getWidth()
{
    return width;

}


double NodeItem::getHeight()
{
    return height;
}


void NodeItem::addEdgeItem(EdgeItem *line)
{
    connect(this, SIGNAL(nodeItemMoved()), line, SLOT(updateEdge()));
    NodeItem* item = this;
    while(item){
        //Connect the Visibility of the edges of this node, so that if this node's parent was to be set invisbile, we any edges would be invisible.
        connect(item, SIGNAL(setEdgesVisibility(bool)), line, SLOT(setVisible(bool)));
        item = dynamic_cast<NodeItem*>(item->parentItem());
    }
    connect(this, SIGNAL(setEdgesSelected(bool)), line, SLOT(setSelected(bool)));

    connections.append(line);
    nodeItemMoved();
}


void NodeItem::removeEdgeItem(EdgeItem *line)
{
    connections.removeAll(line);
    nodeItemMoved();
}


void NodeItem::setCenterPos(QPointF pos)
{
    //pos is the new center Position.
    pos -= minimumVisibleRect().center();
    //QGraphicsItem::setPos(pos);

    setPos(pos);
}

QPointF NodeItem::centerPos()
{
    return pos() + minimumVisibleRect().center();
}


void NodeItem::adjustPos(QPointF delta)
{
    setLocked(false);
    QPointF currentPos = pos();
    currentPos += delta;
    //hasSelectionMoved = true;
    setPos(currentPos);
}

void NodeItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getWidth() + delta.width();
    qreal newHeight = getHeight() + delta.height();

    setWidth(newWidth);
    setHeight(newHeight);
}


void NodeItem::addChildOutline(NodeItem *nodeItem, QPointF gridPoint)
{
    prepareGeometryChange();
    QRectF nodeRect = nodeItem->boundingRect();
    QPointF deltaPos = gridPoint - nodeItem->minimumVisibleRect().center();
    nodeRect.translate(deltaPos);
    outlineMap.insert(nodeItem->getID(), nodeRect);
}

void NodeItem::removeChildOutline(NodeItem *nodeItem)
{
    if(outlineMap.contains(nodeItem->getID())){
        prepareGeometryChange();
        outlineMap.remove(nodeItem->getID());
    }
}



double NodeItem::getChildWidth()
{
    // added an offset of 0.35 here and in getChildHeight
    // to make the gap between the view aspects uniform
    if (nodeKind == "Model") {
        return MODEL_WIDTH / ASPECT_SIZE_RATIO;
    } else if(nodeKind.endsWith("Definitions")) {
        return minimumWidth / ENTITY_SIZE_RATIO;
    }else{
        return minimumWidth;
    }
}

double NodeItem::getChildHeight()
{
    if (nodeKind == "Model") {
        return MODEL_HEIGHT / ASPECT_SIZE_RATIO;
    } else {
        return getChildWidth();
    }

}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos(bool currentlySorting)
{
    QPainterPath childrenPath = QPainterPath();
    bool hasChildren = false;

    // add the children's bounding rectangles to the children path
    foreach(NodeItem* child, getChildNodeItems()){
        if(child->isInAspect() && !child->isHidden()){
            if(!currentlySorting || (currentlySorting && child->isSorted())){
                hasChildren = true;
                QRectF childRect =  child->boundingRect();
                childRect.translate(child->pos());
                childrenPath.addRect(childRect);
            }
        }
    }

    /*
    //Based on BoundingRect
    bool growWidth = boundingRect().width() < boundingRect().height();
    //grow width based on GridRect.
    // bool growWidth = gridRect().width() < gridRect().height();

    int currentX = 0;
    int currentY = 0;
    bool minXSet = false;
    bool minYSet = false;
    int minX = 0;
    int minY = 0;
    bool gridFull = false;

    while(true){
        QPointF newPos = getGridPosition(currentX, currentY);
        //Get the size of the child. In most cases it will be getChildWidth(), getChildHeight();
        QRectF childRect = getChildBoundingRect();
        //Translate to the center of the normal childBR
        childRect.translate(newPos - childRect.center());

        bool itemCollision = childrenPath.intersects(childRect);

        bool inGrid = childRect.top() >= gridRect().top() && childRect.left() >= gridRect().left() ;
        bool rightOfGrid = childRect.right() >= gridRect().right();
        bool belowGrid = childRect.bottom() >= gridRect().bottom();

        if(inGrid || gridFull){
            if(!itemCollision){
                //If there is no collision and we are in the Grid, or there is no room in the grid. Return!
                return newPos;
            }
        }

        //If we are either in the Grid, or we have no Children, we should keep track of the MinX and MinY
        if(inGrid || !hasChildren){
            if(!minXSet){
                minX = currentX;
                minXSet = true;
            }
            if(!minYSet){
                minY = currentY;
                minYSet = true;
            }
            minX = qMin(currentX, minX);
            minY = qMin(currentY, minY);
        }

        //Try Next Column
        //If we aren't Right of the grid, or the grid is full, and we should grow right.
        if(!rightOfGrid || (gridFull && growWidth)){
            currentX++;
            continue;
        }

        //Try Next Row
        //If we aren't Right of the grid, or the grid is full, and we are growing down.
        if(!belowGrid || (gridFull && !growWidth)){
            currentY++;
            currentX=minX;
            continue;
        }

        //If we get here we are out of Grid Spaces, and should set our currentX/currentY
        if(!gridFull){
            gridFull = true;
            if(growWidth){
                currentY = minY;
            }else{
                currentX = minX;
            }
        }
    }
    */
    ///*
    // CATHLYNS CODE FOR THE SAME THING. TEST
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
        childRect.translate(nextPosition - childRect.center());

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
    //*/
}




void NodeItem::setOpacity(qreal opacity)
{
    QGraphicsItem::setOpacity(opacity);
    emit updateOpacity(opacity);

    foreach(EdgeItem* edge, connections){
        if(edge->getSource()->opacity() != 0 && edge->getDestination()->opacity() != 0){

            edge->setOpacity(opacity);
        }else{
            edge->setOpacity(0);
        }
    }
}


/**
 * @brief NodeItem::setSelected Sets the NodeItem as selected, notifies connected edges to highlight.
 * @param selected
 */
void NodeItem::setSelected(bool selected)
{
    if(isNodeSelected != selected){
        isNodeSelected = selected;

        //Bring the item to the front.
        if(isNodeSelected){
            if(parentNodeItem){
                parentNodeItem->setZValue(1);
            }
            setZValue(1);
        }else{
            setZValue(0);
        }

        update();
        setEdgesSelected(selected);
    }
}

/**
 * @brief NodeItem::setVisible Changes the Visibility of the NodeItem, notifies connected edges.
 * @param visible
 */
void NodeItem::setVisible(bool visible)
{
    QGraphicsItem::setVisible(visible);
    setEdgesVisibility(visible);
}


void NodeItem::setPermanentlyCentralized(bool centralized)
{
    isNodeCentralized = centralized;
}


void NodeItem::graphMLDataChanged(GraphMLData* data)
{
    if(data){
        QString dataKey = data->getKeyName();
        QString dataValue = data->getValue();
        QString previousValue;

        if(dataKey == "x" || dataKey == "y"){

            //Update the Position
            QPointF newCenter = centerPos();

            if(dataKey == "x"){
                previousValue = QString::number(newCenter.x());
                newCenter.setX(dataValue.toDouble());
            }else if(dataKey == "y"){
                previousValue = QString::number(newCenter.y());
                newCenter.setY(dataValue.toDouble());
            }

            setCenterPos(newCenter);

            if(previousValue != dataValue){
                updateModelPosition();
            }


        }else if(dataKey == "width" || dataKey == "height"){
            if(dataValue == "inf"){
                dataValue = QString::number(MODEL_WIDTH);
            }

            if(dataKey == "width"){
                previousValue = QString::number(width);
                setWidth(dataValue.toDouble());
            }else if(dataKey == "height"){
                previousValue = QString::number(height);

                bool expand = dataValue.toDouble() > height;
                bool contract = (dataValue == minimumHeightStr);

                //If the value of the height is bigger than the minimumHeight, we should expand.
                if(!isExpanded() && expand){
                    //qCritical() << "Expanding";
                    setNodeExpanded(true);
                }else if(isExpanded() && contract){
                    //qCritical() << "Contracting";
                    setNodeExpanded(false);
                }

                //Then set the height.
                setHeight(dataValue.toDouble());

            }
            if(previousValue != dataValue){
                updateModelSize();
            }

        }
        else if(dataKey == "label"){

            if(dataValue != ""){
                updateTextLabel(dataValue);
            }

            // update connected dock node item
            emit updateDockNodeItem();
        }

    }
}


/**
 * @brief NodeItem::newSort
 */
void NodeItem::newSort()
{
    if(nodeKind == "Model"){
        modelSort();
        return;
    }

    // added this so sort can be un-done
    GraphMLItem_TriggerAction("NodeItem: Sorting Node");

    //Get the number of un-locked items
    QMap<int, NodeItem*> toSortMap;
    QList<NodeItem*> lockedItems;

    foreach(NodeItem* child, getChildNodeItems()){
        Node* childNode = child->getNode();
        if(child->getChildNodeItems().size() == 0){
            //RESET SIZE.
            child->setWidth(getChildWidth());
            child->setHeight(getChildHeight());
            //child->updateModelSize();
        }
        if(!child->isVisible()){ //&& nodeKind != "Model"){
            continue;
        }
        if(child->isLocked() && GRIDLINES_ON){
            child->setSorted(true);
            lockedItems.append(child);
            continue;
        }

        Node* childParent = childNode->getParentNode();

        int currentSortOffset = 0;
        while(childParent){
            if(childParent == getNode()){
                break;
            }else{
                bool isInt;
                int childParentSortOrder = childParent->getDataValue("sortOrder").toInt(&isInt);
                if(isInt){
                    currentSortOffset += childParentSortOrder;
                }
            }
            childParent = childParent->getParentNode();
        }

        bool isInt;
        int sortPosition = childNode->getDataValue("sortOrder").toInt(&isInt);
        if(isInt){
            toSortMap.insertMulti(currentSortOffset + sortPosition, child);
        }
    }

    QList<NodeItem*> toSortItems = toSortMap.values();

    //Sort Items
    while(toSortItems.size() > 0){
        NodeItem* item = toSortItems.takeFirst( );
        item->setCenterPos(getNextChildPos(true));
        item->updateModelPosition();
        item->setSorted(true);
    }

    foreach(NodeItem* child, getChildNodeItems()){
        child->setSorted(false);
    }

}

void NodeItem::modelSort()
{
    if(childNodeItems.size() != 4 || nodeKind != "Model"){
        return;
    }

    NodeItem* topLeft = childNodeItems[0];
    NodeItem* topRight = childNodeItems[1];
    NodeItem* bottomLeft = childNodeItems[2];
    NodeItem* bottomRight = childNodeItems[3];

    QPointF gapSize = QPointF(MODEL_WIDTH / 128, MODEL_WIDTH / 128);

    //double margin = qMax(topLeft->boundingRect().width(), topLeft->boundingRect().height());
    //QPointF topLeftPos = QPointF(margin, margin);

    QPointF topLeftPos = QPointF(getItemMargin(), getItemMargin());
    qreal deltaX = topLeft->boundingRect().width() - bottomLeft->boundingRect().width();
    qreal deltaY = topLeft->boundingRect().height() - topRight->boundingRect().height();

    if(deltaX < 0){
        topLeftPos.setX(topLeftPos.x() + abs(deltaX));
    }
    if(deltaY < 0){
        topLeftPos.setY(topLeftPos.y() + abs(deltaY));
    }

    //Move top Left Item to top of Model
    topLeft->setPos(topLeftPos);
    modelCenterPoint = topLeftPos + topLeft->boundingRect().bottomRight() + gapSize;

    //UPDATE TEXT
    if(textItem){
        qreal labelX = modelCenterPoint.x() - (textItem->boundingRect().width()/2);
        qreal labelY = modelCenterPoint.y() - (textItem->boundingRect().height()/2);

        textItem->setPos(labelX, labelY);
    }





    //Move Top Right
    QPointF topRightPos = modelCenterPoint + QPointF(gapSize.x(), - gapSize.y() - topRight->boundingRect().height());
    topRight->setPos(topRightPos);

    //Move Top Right
    QPointF botLeftPos = modelCenterPoint + QPointF(-gapSize.x() - bottomLeft->boundingRect().width(),gapSize.y());
    bottomLeft->setPos(botLeftPos);


    //Move Bottom Right
    QPointF botRightPos = modelCenterPoint + gapSize;
    bottomRight->setPos(botRightPos);

    //Update the model size to contain the new size.
    resizeToOptimumSize();
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!contains(event->pos())){
        QGraphicsItem::mousePressEvent(event);
        return;
    }

    if(!PAINT_OBJECT){
        if(nodeKind == "Model" && !modelCirclePressed(event->pos())){\
            GraphMLItem_ClearSelection(true); // need to update table!
            QGraphicsItem::mousePressEvent(event);
            return;
        }
    }

    switch (event->button()) {

    case Qt::MiddleButton:{
        break;
    }
    case Qt::LeftButton:{


        // check if the lock icon was clicked
        if (lockPressed(event->scenePos())) {
            emit NodeItem_showLockMenu(this);
            return;
        }

        previousScenePosition = event->scenePos();
        hasSelectionMoved = false;
        hasSelectionResized = false;
        isNodePressed = true;


        if(!isSelected()){
            if (event->modifiers().testFlag(Qt::ControlModifier)){
                //CONTROL PRESSED
                GraphMLItem_AppendSelected(this);
            }else{
                GraphMLItem_ClearSelection(true);
                GraphMLItem_AppendSelected(this);
            }
        }else{
            if (event->modifiers().testFlag(Qt::ControlModifier)){
                //Check for parent selection.
                GraphMLItem_RemoveSelected(this);
            }
        }
        break;
    }
    case Qt::RightButton:{
        if(nodeKind == "Model" && !modelCirclePressed(event->pos())){
            return;
        }

        if(!isSelected()){
            if (!event->modifiers().testFlag(Qt::ControlModifier)){
                //Check for parent selection.
                GraphMLItem_ClearSelection(false);
            }
            GraphMLItem_AppendSelected(this);
        }
        break;
    }

    default:
        break;
    }
}


void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isInSubView){
        return;
    }
    switch (event->button()) {
    case Qt::LeftButton:

        // added this to center aspects when double-clicking on !PAINTED items
        //if(!PAINT_OBJECT && nodeKind != "Model"){

        // needed to change it to this other wise you can't center
        // the aspects by double clicking on the model
        if(!PAINT_OBJECT && !modelCirclePressed(event->pos())){
            GraphMLItem_CenterAspects();
            return;
        }

        if(labelPressed(event->pos())){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                //Make sure we set the real current value.
                //textItem->setPlainText(getGraphML()->getDataValue("label"));
                textItem->setEditMode(true);
            }
            break;
        }

        if(iconPressed(event->pos())){
            if(isExpanded()){
                GraphMLItem_TriggerAction("Contracted Node Item");
            }else{
                GraphMLItem_TriggerAction("Expanded Node Item");
            }

            setNodeExpanded(!isExpanded());
            updateModelSize();

            break;
        }
        if(hasVisibleChildren() && resizeEntered(event->pos()) == RESIZE){
            GraphMLItem_TriggerAction("Optimizes Size of NodeItem");
            resizeToOptimumSize();
            updateModelSize();
            break;
        }

        GraphMLItem_SetCentered(this);

    }
}


void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton:{
        if(!PAINT_OBJECT && nodeKind != "Model"){
            GraphMLItem_ClearSelection(true);
            return;
        }

        if (hasSelectionMoved){

            if(parentNodeItem){
                parentNodeItem->setGridVisible(false);
            }

            if (!currentSceneRect.contains(scenePos()) ||
                    !currentSceneRect.contains(scenePos().x()+width, scenePos().y()+height)) {
                //GraphMLItem_MovedOutOfScene(this);
            }

            isSelectionMoving = false;
            NodeItem_MoveFinished();
        }

        if(hasSelectionResized){
            isSelectionResizing = false;
            NodeItem_ResizeFinished();

        }
        currentResizeMode = NO_RESIZE;


        // have to reset cursor here otherwise it's stuck on Qt::SizeAllCursor after being moved
        setCursor(Qt::OpenHandCursor);



        isNodePressed = false;
        break;
    }
    case Qt::MiddleButton:{
        if(!PAINT_OBJECT && nodeKind != "Model"){
            //emit centerViewAspects();
            GraphMLItem_CenterAspects();
            return;
        }

        if (event->modifiers().testFlag(Qt::ControlModifier)) {
            GraphMLItem_TriggerAction("Sorting Node");
            newSort();
        } else {
            GraphMLItem_SetCentered(this);
        }

        break;
    }

    default:
        break;
    }
}


void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!PAINT_OBJECT || nodeKind.endsWith("Definitions")){
        return;
    }

    if(isNodePressed && isNodeSelected){

        if(currentResizeMode != NO_RESIZE){
            if(!hasSelectionResized){
                GraphMLItem_TriggerAction("Resizing Selection");
                hasSelectionResized = true;
                isSelectionResizing = true;
            }
            if(isSelectionResizing){
                QPointF delta = (event->scenePos() - previousScenePosition);
                QSizeF dSize(delta.x(), delta.y());

                if(currentResizeMode == HORIZONTAL_RESIZE){
                    dSize.setHeight(0);
                }else if(currentResizeMode == VERTICAL_RESIZE){
                    dSize.setWidth(0);
                }

                NodeItem_ResizeSelection(dSize);
            }

        }else{
            if(hasSelectionMoved == false){
                GraphMLItem_TriggerAction("Moving Selection");
                setCursor(Qt::SizeAllCursor);
                if(parentNodeItem){
                    parentNodeItem->setGridVisible(true);
                }
                hasSelectionMoved = true;
                isSelectionMoving = true;
            }
            if(isSelectionMoving){
                QPointF delta = (event->scenePos() - previousScenePosition);

                NodeItem_MoveSelection(delta);
            }
        }
        previousScenePosition = event->scenePos();

    }
}

void NodeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    bool changedCursor = false;
    if(hasVisibleChildren() && iconPressed(event->pos())){
        setCursor(Qt::PointingHandCursor);
        changedCursor = true;
    }


    if(!isExpanded() || !hasVisibleChildren()){
        return;
    }

    if(isNodeSelected){
        currentResizeMode = resizeEntered(event->pos());

        if(currentResizeMode == RESIZE){
            setCursor(Qt::SizeFDiagCursor);
            changedCursor = true;
        }else if(currentResizeMode == HORIZONTAL_RESIZE){
            setCursor(Qt::SizeHorCursor);
            changedCursor = true;
        }else if(currentResizeMode == VERTICAL_RESIZE){
            setCursor(Qt::SizeVerCursor);
            changedCursor = true;
        }
    }

    if(!changedCursor){
        setCursor(Qt::OpenHandCursor);
    }

}

bool NodeItem::compareTo2Decimals(qreal num1, qreal num2)
{
    int number1To3 = qRound(num1 * 100.0);
    int number2To3 = qRound(num2 * 100.0);

    return number1To3 == number2To3;
}


void NodeItem::updateModelData()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* wData = modelEntity->getData("width");
        GraphMLData* hData = modelEntity->getData("height");
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        wData->setValue(QString::number(width));
        hData->setValue(QString::number(height));
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }
}

void NodeItem::resizeToOptimumSize()
{
    setWidth(getMinimumChildRect().width());
    setHeight(getMinimumChildRect().height());
    //updateModelSize();
}

NodeItem *NodeItem::getChildNodeItemFromNode(Node *child)
{
    foreach(NodeItem* childNI , childNodeItems){
        if(childNI->getNode() == child){
            return childNI;
        }
    }
    return 0;
}




void NodeItem::setWidth(qreal w)
{   
    if(isExpanded()){
        w = qMax(w, getMinimumChildRect().width());
        expandedWidth = w;
    }else{
        w = minimumWidth;
    }


    prepareGeometryChange();
    width = w;

    updateTextLabel();

    updateChildrenOnChange();
    calculateGridlines();
    updateParent();
    isOverGrid(centerPos());

    //emit NodeItem_resized(this);
}


void NodeItem::setHeight(qreal h)
{
    if(isExpanded()){
        h = qMax(h, getMinimumChildRect().height());
        expandedHeight = h;
    }else{
        h = minimumHeight;
    }

    prepareGeometryChange();
    height = h;

    calculateGridlines();
    updateChildrenOnChange();
    updateParent();
    isOverGrid(centerPos());
}

void NodeItem::setSize(qreal w, qreal h)
{

    QRectF childRect = getMinimumChildRect();

    setWidth(w);
    setHeight(h);
    return;

    if(w != width || height != h ){
        //if()
        //w = qMax(childRect.width(), w);
        //h = qMax(childRect.height(), h);

        prepareGeometryChange();

        height = h;
        width = w;

        if(isExpanded()){
            expandedWidth = w;
            expandedHeight = h;
        }

        updateTextLabel();
        calculateGridlines();

        updateParent();
    }
}

/**
 * @brief NodeItem::calculateGridlines Calculates the horizontal and vertical gridlines to be drawn by paint.
 */
void NodeItem::calculateGridlines()
{
    if(GRIDLINES_ON){
        QRectF boundingGridRect = gridRect();
        xGridLines.clear();
        yGridLines.clear();

        //Don't draw first.
        for(qreal x = boundingGridRect.left() + getGridSize(); x <= boundingGridRect.right(); x += getGridSize()){
            xGridLines << QLineF(x, boundingGridRect.top(), x, boundingGridRect.bottom());
        }

        for(qreal y = boundingGridRect.top() + getGridSize(); y <= boundingGridRect.bottom(); y += getGridSize()){
            yGridLines << QLineF(boundingGridRect.left(), y, boundingGridRect.right(), y);
        }
    }
}


void NodeItem::setPaintObject(bool paint)
{
    PAINT_OBJECT = paint;

    if(icon){
        icon->setVisible(paint);
    }
    if(lockIcon){
        if(hasDefinition){
            lockIcon->setVisible(paint);
        }else{
            lockIcon->setVisible(false);
        }
    }
    if(textItem){
        textItem->setVisible(paint);
    }
}


bool NodeItem::isAncestorSelected()
{
    if(parentNodeItem){
        if(parentNodeItem->isSelected()){
            return true;
        }else{
            return parentNodeItem->isAncestorSelected();
        }
    }
    return false;
}


/**
 * @brief NodeItem::setGridVisible Sets the grid as visible.
 * @param visible
 */
void NodeItem::setGridVisible(bool visible)
{
    prepareGeometryChange();
    isGridVisible = visible;
}


void NodeItem::updateTextLabel(QString newLabel)
{
    if(!textItem){
        return;
    }

    if(nodeKind != "Model"){
        textItem->setTextWidth(width);
    }else{
        textItem->setTextWidth(getItemMargin() * 2);
    }

    if(newLabel != ""){
        //qCritical() << newLabel;
        textItem->setPlainText(newLabel);
    }
}

QRectF NodeItem::getMinimumChildRect()
{
    qreal itemMargin = getItemMargin();
    qreal childItemMargin = getChildItemMargin(); //Use this to grow to be square.

    QPointF topLeft(itemMargin, itemMargin);
    QPointF bottomRight((itemMargin) + minimumWidth, (itemMargin) + minimumHeight);

    foreach(NodeItem* child, childNodeItems){
        if(child->isVisible() || isExpanded()){
            qreal childMaxX = child->pos().x() + child->boundingRect().width() + childItemMargin;
            qreal childMaxY = child->pos().y() + child->boundingRect().height() + childItemMargin;

            if(childMaxX >= bottomRight.x()){
                bottomRight.setX(childMaxX);
            }
            if(childMaxY >= bottomRight.y()){
                bottomRight.setY(childMaxY);
            }
        }
    }

    QRectF rectangle = QRectF(topLeft, bottomRight);
    return rectangle;
}


void NodeItem::setupAspect()
{
    Node* node = getNode();

    while(node){
        QString nodeKind = node->getDataValue("kind");
        if(nodeKind == "ManagementComponent"){
            viewAspects.append("Hardware");
            viewAspects.append("Assembly");
        }else if(nodeKind == "HardwareDefinitions"){
            viewAspects.append("Hardware");
        }else if(nodeKind == "AssemblyDefinitions"){
            viewAspects.append("Assembly");
        }else if(nodeKind == "BehaviourDefinitions"){
            viewAspects.append("Workload");
        }else if(nodeKind == "InterfaceDefinitions"){
            viewAspects.append("Definitions");
        }

        if (nodeKind == "DeploymentDefintions") {
            if (!viewAspects.contains("Hardware")) {
                viewAspects.append("Hardware");
            }
            if (!viewAspects.contains("Assembly")) {
                viewAspects.append("Assembly");
            }
        }

        node = node->getParentNode();
    }
}


void NodeItem::setupBrushes()
{
    QString nodeKind= getGraphML()->getDataValue("kind");


    if(nodeKind == "BehaviourDefinitions"){
        color = QColor(254,184,126);
    }
    else if(nodeKind == "InterfaceDefinitions"){
        color = QColor(110,210,210);
    }
    else if(nodeKind == "HardwareDefinitions"){
        color = QColor(110,170,220);
    }
    else if(nodeKind == "AssemblyDefinitions"){
        color = QColor(255,160,160);
    }else{
        color = QColor(200,200,200);
    }
    /*

    if(nodeKind== "OutEventPort"){
        color = QColor(0,250,0);
    }
    else if(nodeKind== "OutEventPortInstance"){
        color = QColor(0,200,0);
    }
    else if(nodeKind== "OutEventPortImpl"){
        color = QColor(0,150,0);
    }
    else if(nodeKind== "OutEventPortDelegate"){
        color = QColor(0,100,0);
    }
    else if(nodeKind== "InEventPort"){
        color = QColor(250,0,0);
    }
    else if(nodeKind== "InEventPortInstance"){
        color = QColor(200,0,0);
    }
    else if(nodeKind== "InEventPortImpl"){
        color = QColor(150,0,0);
    }
    else if(nodeKind== "InEventPortDelegate"){
        color = QColor(100,0,0);
    }
    else if(nodeKind== "Component"){
        color = QColor(200,200,200);
    }
    else if(nodeKind== "ComponentInstance"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "ComponentImpl"){
        color = QColor(100,100,100);
    }
    else if(nodeKind== "Attribute"){
        color = QColor(0,0,250);
    }
    else if(nodeKind== "AttributeInstance"){
        color = QColor(0,0,200);
    }
    else if(nodeKind== "AttributeImpl"){
        color = QColor(0,0,150);
    }
    else if(nodeKind== "HardwareNode"){
        color = QColor(0,250,250);
    }
    else if(nodeKind== "HardwareCluster"){
        color = QColor(200,200,200);
    }


    else if(nodeKind == "BehaviourDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(254,184,126);
    }
    else if(nodeKind == "InterfaceDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(110,210,210);
    }
    else if(nodeKind == "HardwareDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(110,170,220);
    }
    else if(nodeKind == "AssemblyDefinitions"){
        //color = QColor(240,240,240);
        color = QColor(255,160,160);
    }

    else if(nodeKind== "File"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "ComponentAssembly"){
        color = QColor(200,200,200);
    }

    else if(nodeKind== "Aggregate"){
        color = QColor(200,200,200);
    }
    else if(nodeKind== "AggregateMember"){
        color = QColor(150,150,150);
    }
    else if(nodeKind== "Member"){
        color = QColor(100,100,100);
    }else{
        color = QColor(0,100,0);
    }*/


    selectedColor = color;

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(Qt::gray);
    pen.setWidth(1);
    selectedPen.setColor(Qt::blue);
    selectedPen.setWidth(24);

}


void NodeItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}


void NodeItem::setPos(const QPointF &pos)
{
    if(pos != this->pos()){
        prepareGeometryChange();
        QGraphicsItem::setPos(pos);

        // need to check if GRID is turned on
        isOverGrid(centerPos());

        updateChildrenOnChange();
        updateParent();

        //if (!parentView->sceneRect().contains(scenePos()) ||
        //        !parentView->sceneRect().contains(scenePos().x()+width, scenePos().y()+height)) {
        //    GraphMLItem_MovedOutOfScene(this);
        //}
    }
}



/**
 * @brief NodeItem::setupLockMenu
 */
void NodeItem::setupLockMenu()
{
    lockMenu = new QMenu();
    lockPos = new QWidgetAction(this);
    lockSize = new QWidgetAction(this);
    lockLabel = new QWidgetAction(this);
    lockSortOrder = new QWidgetAction(this);

    QCheckBox* cb1 = new QCheckBox("Position");
    QCheckBox* cb2 = new QCheckBox("Size");
    QCheckBox* cb3 = new QCheckBox("Label");
    QCheckBox* cb4 = new QCheckBox("Sort Order");

    lockPos->setDefaultWidget(cb1);
    lockSize->setDefaultWidget(cb2);
    lockLabel->setDefaultWidget(cb3);
    lockSortOrder->setDefaultWidget(cb4);

    lockMenu->addAction(lockPos);
    lockMenu->addAction(lockSize);
    lockMenu->addAction(lockLabel);
    lockMenu->addAction(lockSortOrder);

    QFont font = lockMenu->font();
    font.setPointSize(9);

    lockMenu->setFont(font);
    lockMenu->setFixedSize(110, 93);
    lockMenu->setAttribute(Qt::WA_TranslucentBackground);
    lockMenu->setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    lockMenu->setStyleSheet("QCheckBox::checked{ color: darkRed; font-weight: bold; }"
                            "QCheckBox{ padding: 3px; }"
                            "QMenu{ padding: 5px;"
                            "border-radius: 8px;"
                            "background-color: rgba(240,240,240,245); }");

    connect(lockMenu, SIGNAL(aboutToHide()), this, SLOT(menuClosed()));

}


void NodeItem::updateParent()
{
    if(parentNodeItem){
        parentNodeItem->childPosUpdated();
        //fitToScreen(this);
    }
}

void NodeItem::updateParentModel()
{
    if(parentNodeItem){
        parentNodeItem->updateModelSize();
    }
}


void NodeItem::aspectsChanged(QStringList aspects)
{
    currentViewAspects = aspects;

    if(hidden || !PAINT_OBJECT){
        return;
    }

    if(this->getParentNodeItem() && !getParentNodeItem()->isExpanded()){
        return;
    }

    //bool prevVisible = isVisible();

    setVisible(isInAspect());

    /*
    // if the view aspects have been changed, update pos of edges
    if(prevVisible != isVisible()){
        emit updateEdgePosition();
    }
    */

    // if not visible, unselect node item
    if (!isVisible()) {
        setSelected(false);
    }
}


/**
 * @brief NodeItem::setupIcon
 * This sets up the scale and tranformation of this item's icon
 * if it has one. It also updates the pos for this item's label.
 */
void NodeItem::setupIcon()
{
    QString nodeKind = getGraphML()->getDataValue("kind");

    // get the icon images
    QImage image( ":/Resources/Icons/" + nodeKind + ".png");
    if (!image.isNull()) {
        icon = new QGraphicsPixmapItem(QPixmap::fromImage(image), this);
        icon->setTransformationMode(Qt::SmoothTransformation);
        icon->setToolTip("Double Click to Expand/Contract Node");
    }
    QImage lockImage (":/Resources/Icons/lock.png");
    if (!lockImage.isNull()){
        lockIcon = new QGraphicsPixmapItem(QPixmap::fromImage(lockImage), this);
        lockIcon->setTransformationMode(Qt::SmoothTransformation);
        lockIcon->setToolTip("Click to see Locked Attributes");
    }

    if (icon) {
        //The amount of space should be 1 - the FONT_RATIO

        qreal iconHeight = icon->boundingRect().height();
        qreal iconWidth = icon->boundingRect().width();

        //Calculate the Scale Factor for the icon.
        qreal scaleFactor = (ICON_RATIO * minimumHeight) / iconHeight;
        icon->setScale(scaleFactor);

        //Update the icon sizes post scale.
        iconWidth *= scaleFactor;
        iconHeight *= scaleFactor;

        //Calculate the x such that it would be in the horizontal center of the minimumVisibleRect
        qreal iconX = (minimumVisibleRect().width() - iconWidth) /2;
        //Calculate the y such that it would be in the vertical center of the iconRatio * minimumVisibleRect
        qreal iconY = getItemMargin() + ((ICON_RATIO * minimumHeight) - iconHeight) /2;

        icon->setPos(iconX, iconY);
    }

    if(lockIcon){
        //ICON SHOULD FIT ON THE LEFT OF
        qreal iconSpace = ((1 - ICON_RATIO) * minimumHeight)/2;


        qreal iconWidth = lockIcon->boundingRect().width();
        qreal iconHeight = lockIcon->boundingRect().height();

        //Calculate the Scale Factor for the icon.
        qreal scaleFactor = (iconSpace / iconWidth);
        lockIcon->setScale(scaleFactor);

        //Update the icon sizes post scale.
        iconWidth *= scaleFactor;
        iconHeight *= scaleFactor;

        lockIcon->setPos(getItemMargin(), getItemMargin());
    }




}


/**
 * @brief NodeItem::setupLabel
 * This sets up the font and size of the label.
 */
void NodeItem::setupLabel()
{
    // this updates this item's label
    if (nodeKind.endsWith("Definitions")) {
        return;
    }


    float fontSize = qMax((LABEL_RATIO / 2) * minimumHeight, 1.0);
    if(nodeKind == "Model"){
        fontSize = qMax(getItemMargin() * LABEL_RATIO, 1.0);
    }

    QFont font("Arial", fontSize);

    textItem = new EditableTextItem(this);
    connect(textItem, SIGNAL(textUpdated(QString)),this, SLOT(labelUpdated(QString)));
    connect(textItem, SIGNAL(editableItem_hasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));
    textItem->setToolTip("Double Click to Edit Label");

    textItem->setTextWidth(minimumWidth);

    if(nodeKind == "Model"){
        textItem->setCenterJustified();
        textItem->setTextWidth(getItemMargin() * 2);
    }

    qreal labelX = (minimumVisibleRect().width() - textItem->boundingRect().width()) /2;
    qreal labelY = getItemMargin() + (ICON_RATIO * minimumHeight);

    textItem->setFont(font);
    textItem->setPos(labelX, labelY);

    updateTextLabel(getGraphML()->getDataValue("label"));
}


/**
 * @brief NodeItem::setupGraphMLConnections
 */
void NodeItem::setupGraphMLConnections()
{
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        GraphMLData* hData = modelEntity->getData("height");
        GraphMLData* wData = modelEntity->getData("width");

        GraphMLData* kindData = modelEntity->getData("kind");
        GraphMLData* labelData = modelEntity->getData("label");


        if(xData){
            connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }

        if(yData){
            connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }

        if(hData){
            connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }

        if(wData){
            connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }

        if(labelData){
            connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }

        if(kindData){
            connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
        }
    }
}

QPointF NodeItem::isOverGrid(const QPointF centerPosition)
{  
    if(!GRIDLINES_ON || !parentNodeItem || nodeKind.endsWith("Definitions")){
        if(parentNodeItem){
            isNodeOnGrid = false;
            parentNodeItem->removeChildOutline(this);
        }
        return QPointF();
    }

    QPointF gridPoint = parentNodeItem->getClosestGridPoint(centerPosition);

    //Calculate the distance between the centerPosition and the closestGrid
    qreal distance = QLineF(centerPosition, gridPoint).length();

    //If the distance is less than the SNAP_PERCENTAGE
    if((distance / minimumWidth) <= SNAP_PERCENTAGE){
        if(isNodeOnGrid || isSelectionMoving || isSelectionResizing){
            isNodeOnGrid = true;
            parentNodeItem->addChildOutline(this, gridPoint);
        }
        return gridPoint;
    }else{
        isNodeOnGrid = false;

        if(isSelectionMoving || isSelectionResizing){
            parentNodeItem->removeChildOutline(this);
        }
        return QPointF();
    }
}


void NodeItem::setNewLabel(QString newLabel)
{
    if(getGraphML()){
        if(newLabel != ""){
            GraphMLItem_TriggerAction("Set New Label");
            GraphMLItem_SetGraphMLData(getGraphML(), "label", newLabel);
        }else{
            if(textItem){
                textItem->setEditMode(true);
            }
        }
    }
}


void NodeItem::toggleGridLines(bool on)
{
    GRIDLINES_ON = on;
    if(on){
        calculateGridlines();
    }
}


void NodeItem::snapToGrid()
{
    if(parentNodeItem && isVisible()){
        QPointF gridPosition = parentNodeItem->getClosestGridPoint(centerPos());

        setCenterPos(gridPosition);
    }
}


void NodeItem::snapChildrenToGrid()
{
    foreach(NodeItem *child, childNodeItems){
        if(child->isVisible()){
            child->snapToGrid();
            /*
            QPointF localPosition = child->minimumVisibleRect().center();
            QPointF childPosition = child->pos() + localPosition;
            QPointF newPosition = getClosestGridPoint(childPosition);

            if(childPosition != newPosition){
                child->setPos(newPosition - localPosition);
                // this needs to be called because setPos isn't immediately updating the node's pos
                // if it's not called, the item jumps back to it's prev pos when dragged
                //child->updateGraphMLPosition();
            }
            */
        }
    }
}


/**
 * @brief NodeItem::menuClosed
 */
void NodeItem::menuClosed()
{
    emit NodeItem_lockMenuClosed(this);
}


void NodeItem::updateGraphMLPosition()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* xData = modelEntity->getData("x");
        GraphMLData* yData = modelEntity->getData("y");
        QPointF center = centerPos();
        xData->setValue(QString::number(center.x()));
        yData->setValue(QString::number(center.y()));
    }

}


void NodeItem::updateChildrenOnChange()
{
    nodeItemMoved();

    if(this->isPermanentlyCentered()){
        emit recentralizeAfterChange(getGraphML());
    }
}


void NodeItem::retrieveGraphMLData()
{
    QString modelHeight = getGraphML()->getDataValue("height");
    QString modelWidth = getGraphML()->getDataValue("width");
    QString modelX = getGraphML()->getDataValue("x");
    QString modelY = getGraphML()->getDataValue("y");

    graphMLDataChanged(getGraphML()->getData("width"));
    graphMLDataChanged(getGraphML()->getData("height"));
    graphMLDataChanged(getGraphML()->getData("x"));
    graphMLDataChanged(getGraphML()->getData("y"));
    /*


    //Update the position with values from the GraphML Model if they have them.
    double graphmlX = getGraphML()->getDataValue("x").toDouble();
    double graphmlY = getGraphML()->getDataValue("y").toDouble();

    if(isInSubView){
        qCritical() << "X: " << graphmlX;
        qCritical() << "Y: " << graphmlY;
        qCritical() << pos();
    }
    setCenterPos(QPointF(graphmlX, graphmlY));
    prepareGeometryChange();

    if(graphmlHeight > height || graphmlWidth > width){
        qCritical() << "STATED AS EXPANDED";
        expandedWidth = graphmlWidth;
        expandedHeight = graphmlHeight;
        //width = expandedWidth;
        //height = expandedHeight;
        //isNodeExpanded = true;
        setNodeExpanded(true);
        //expandItem(true);
    }else if(graphmlHeight != 0 && graphmlWidth != 0){
        //isNodeExpanded = false;
        setNodeExpanded(false);
        //expandItem(false);
    }else{
        //isNodeExpanded = true;
         setNodeExpanded(true);
        //expandItem(true);
    }*/


}


/**
 * @brief NodeItem::getChildren
 * @return
 */
QList<NodeItem *> NodeItem::getChildNodeItems()
{
    return childNodeItems;
}


bool NodeItem::isPermanentlyCentered()
{
    return isNodeCentralized;
}


qreal NodeItem::getGridSize()
{
    return getChildBoundingRect().width() / GRID_RATIO;
}


/**
 * @brief NodeItem::getAspects
 * This returns the view aspects that this node item belongs to.
 * @return
 */
QStringList NodeItem::getAspects()
{
    return viewAspects;
}

void NodeItem::setGraphicsView(QGraphicsView *view)
{
    parentView = view;
}


/**
 * @brief NodeItem::isInAspect
 * This returns whether this node item is in the currently viewed aspects or not.
 * @return
 */
bool NodeItem::isInAspect()
{
    // if this node item doesn't belong to any view aspects,
    // or there are currently no view aspects turned on, return false
    if (viewAspects.count() == 0 || currentViewAspects.count() == 0) {
        return false;
    }

    // otherwise, check the list of currently viewed aspects to see if it contains
    // all of the aspects that need to be turned on for this item to be visible
    bool inAspect = true;
    foreach(QString aspect, viewAspects){
        if(!currentViewAspects.contains(aspect)){
            inAspect = false;
            break;
        }
    }

    return inAspect;
}
/**
 * @brief NodeItem::getLockMenu
 * @return
 */
QMenu* NodeItem::getLockMenu()
{
    return lockMenu;
}


/**
 * @brief NodeItem::getLockIconSceneRect
 * @return
 */
QRectF NodeItem::getLockIconSceneRect()
{
    if (lockIcon) {
        return lockIcon->sceneBoundingRect();
    }
    return QRectF();
}



bool NodeItem::isSorted()
{
    return isNodeSorted;
}

void NodeItem::setSorted(bool isSorted)
{
    isNodeSorted = isSorted;
}


QPolygonF NodeItem::getResizePolygon()
{
    QPointF bottomRight = boundingRect().bottomRight();
    qreal cornerRadius = getCornerRadius();
    qreal extraDistance = cornerRadius * .25;
    QVector<QPointF> points;

    //Top Left
    points << (bottomRight - QPointF(cornerRadius, cornerRadius));
    //Top Right
    points << (bottomRight - QPointF(0, cornerRadius));
    //Top Right Bottom
    points << (bottomRight - QPointF(0, cornerRadius - extraDistance));
    //Bottom Left Right
    points << (bottomRight - QPointF(cornerRadius - extraDistance,0));
    //Bottom Left
    points << (bottomRight - QPointF(cornerRadius, 0));

    return QPolygonF(points);
}


void NodeItem::parentNodeItemMoved()
{

    nodeItemMoved();

    if(isPermanentlyCentered()){
        emit recentralizeAfterChange(getGraphML());
    }
}


/**
 * @brief NodeItem::getChildKind
 * This returns a list of kinds of all this item's children.
 * @return
 */
QStringList NodeItem::getChildrenKind()
{
    QStringList childrenKinds;
    Node *node = dynamic_cast<Node*>(getGraphML());
    if (node) {

        foreach(Node* child, node->getChildren(0)){
            childrenKinds.append(child->getDataValue("kind"));
        }
    }
    return childrenKinds;
}


/**
 * @brief NodeItem::getCornerRadius
 * @return
 */
double NodeItem::getCornerRadius()
{
    double itemMargin = getItemMargin();
    //if(nodeKind.endsWith("Definitions")){
    //    itemMargin *= 2;
    //}
    return itemMargin;
}

double NodeItem::getChildCornerRadius()
{
    double itemMargin = getItemMargin();
    if(nodeKind.endsWith("Definitions")){
        itemMargin /= ENTITY_SIZE_RATIO;
    }
    return itemMargin;
}


/**
 * @brief NodeItem::getMaxLabelWidth
 * @return
 */
double NodeItem::getMaxLabelWidth()
{
    // calculate font metrics here
    return 0;
}

QSizeF NodeItem::getModelSize()
{

    float graphmlHeight = 0;
    float graphmlWidth = 0;
    if(getGraphML()){
        graphmlWidth = getGraphML()->getDataValue("width").toDouble();
        graphmlHeight = getGraphML()->getDataValue("height").toDouble();
    }

    return QSizeF(graphmlWidth, graphmlHeight);
}



bool NodeItem::drawGridlines()
{
    return isGridVisible && GRIDLINES_ON;
}


double NodeItem::getItemMargin() const
{

    return (MARGIN_RATIO * minimumWidth);
}

double NodeItem::getChildItemMargin()
{
    if(nodeKind.endsWith("Definitions")){
        return getItemMargin() / ENTITY_SIZE_RATIO;
    }
    return getItemMargin();

}


/**
 * @brief NodeItem::expandItem
 * @param show
 */
void NodeItem::setNodeExpanded(bool expanded)
{


    //Can't Contract a Definition or Model
    if(!getGraphML() || nodeKind.endsWith("Definitions") || nodeKind == "Model"){
        isNodeExpanded = true;
        return;
    }

    //If our state is already set, don't do anything!
    if(isNodeExpanded == expanded){
        return;
    }



    isNodeExpanded = expanded;

    //Show/Hide the non-hidden children.
    foreach(NodeItem* nodeItem, childNodeItems){
        if (!nodeItem->isHidden()){
            nodeItem->setVisible(expanded);
        }
    }

    if(isExpanded()){
        //Set the width/height to their expanded values.
        setWidth(expandedWidth);
        setHeight(expandedHeight);
    } else {
        //Set the width/height to their minimum values.
        setWidth(minimumWidth);
        setHeight(minimumHeight);
    }
}






void NodeItem::updateModelPosition()
{
    //Update the Parent Model's size first to make sure that the undo states are correct.
    updateParentModel();

    //if we are over a grid line (or within a snap ratio)
    QPointF gridPoint = isOverGrid(centerPos());
    if(!gridPoint.isNull()){
        //Setting new Center Point
        setCenterPos(gridPoint);
        //If the node moved via the mouse, lock it.
        if(hasSelectionResized || hasSelectionMoved){
            setLocked(isNodeOnGrid);
        }
    }

    GraphMLItem_SetGraphMLData(getGraphML(), "x", QString::number(centerPos().x()));
    GraphMLItem_SetGraphMLData(getGraphML(), "y", QString::number(centerPos().y()));


    if(!isNodeOnGrid && parentNodeItem){
        parentNodeItem->removeChildOutline(this);
    }

    GraphMLItem_PositionSizeChanged(this);



}

void NodeItem::updateModelSize()
{
    double prevWidth = getGraphML()->getDataValue("width").toDouble();
    double prevHeight = getGraphML()->getDataValue("height").toDouble();

    //Update the Size in the model.
    GraphMLItem_SetGraphMLData(getGraphML(), "width", QString::number(width));
    GraphMLItem_SetGraphMLData(getGraphML(), "height", QString::number(height));

    //If we are over a gridline already.
    if(isNodeOnGrid){
        //Update the gridPoint.
        QPointF gridPoint = isOverGrid(centerPos());
    }


    //Make sure the parentModel is updated.
    updateParentModel();

    if(hasSelectionResized || hasSelectionMoved){
        setLocked(isNodeOnGrid);
    }

    if (width > prevWidth || height > prevHeight) {
        GraphMLItem_PositionSizeChanged(this, true);
    }
}


/**
 * @brief NodeItem::updateSceneRect
 * This gets called everytime there has been a change to the view's sceneRect.
 * @param sceneRect
 */
void NodeItem::sceneRectChanged(QRectF sceneRect)
{
    currentSceneRect = sceneRect;
}

void NodeItem::labelUpdated(QString newLabel)
{
    if(getGraphML()){

        QString currentLabel = getGraphML()->getDataValue("label");

        if(currentLabel != newLabel){
            GraphMLItem_TriggerAction("Set New Label");
            GraphMLItem_SetGraphMLData(getGraphML(), "label", newLabel);
        }
    }
}


Node *NodeItem::getNode()
{
    return dynamic_cast<Node*>(getGraphML());
}


/**
 * @brief NodeItem::getNodeKind
 * @return
 */
QString NodeItem::getNodeKind()
{
    return nodeKind;
}


/**
 * @brief NodeItem::setHidden
 * This method is used to prevent this item from being shown
 * when the view aspects are changed. If this item is meant to
 * be hidden no matter the view aspect, this keeps it hidden.
 */
void NodeItem::setHidden(bool h)
{
    hidden = h;
    setVisible(!h);
}


/**
 * @brief NodeItem::resetSize
 * Reset this node item's size to its default size.
 */
void NodeItem::resetSize()
{
    GraphML* id = getGraphML();
    if(id){
        GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(minimumHeight));
        GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(minimumWidth));
    }
}


/**
 * @brief NodeItem::isExpanded
 */
bool NodeItem::isExpanded()
{
    return isNodeExpanded;
}

/**
 * @brief NodeItem::getClosestGridPoint
 * @param centerPoint - The Center Point of a newly constructed child
 * @return The closest Grid Point which can contain a child!
 */
QPointF NodeItem::getClosestGridPoint(QPointF centerPoint)
{
    //Get the gridSize
    qreal gridSize = getGridSize();

    //Offset the centerPoint by the starting position of the gridRect so we can do easy division.
    QPointF gridOffset = gridRect().topLeft();
    if(nodeKind == "Model"){
        //qCritical() << gridOffset;
    }
    centerPoint -= gridOffset;

    //Find the closest gridline to the centerPoint, then add the offset again.
    centerPoint.setX((qRound(centerPoint.x() / gridSize) * gridSize) + gridOffset.x());
    centerPoint.setY((qRound(centerPoint.y() / gridSize) * gridSize) + gridOffset.y());

    //Calculate the Bounding Rect of the the child.
    QPointF childOffset = QPointF(getChildWidth()/2, getChildHeight()/2);
    QRectF childRect = QRectF(centerPoint - childOffset, centerPoint + childOffset);

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

    return centerPoint;
}
