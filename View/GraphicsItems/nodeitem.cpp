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

#define MODEL_WIDTH 19200
#define MODEL_HEIGHT 10800
#define COLUMN_COUNT 2
#define MINIMUM_HEIGHT_RATIO 4
#define LABEL_LINE_LENGTH 10
#define LABEL_RATIO 1.1
#define FONT_RATIO 0.1

#define GRID_RATIO 7
#define SNAP_PERCENTAGE .25


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

    inSubView = IN_SUBVIEW;
    nodeResizing = false;

    currentResizeMode = NodeItem::NO_RESIZE;
    LOCKED_POSITION = false;
    GRIDLINES_VISIBLE = false;
    drawGrid = false;
    onGrid = false;
    firstReposition = true;

    nextX = 1;
    nextY = 1;

    nodeSelected = false;
    isNodePressed = false;
    permanentlyCentralized = false;
    permanentlyInvisible = false;
    expanded = true;
    hidden = false;
    hasSelectionMoved = false;
    hasDefinition = false;

    icon = 0;
    lockIcon = 0;
    textItem = 0;
    labelButton = 0;
    labelWidget = 0;
    proxyWidget = 0;
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

        if (nodeKind.endsWith("Definitions")) {
            setWidth(parent->getWidth()/2);
            setHeight(parent->getHeight()/2);
        } else {

            setWidth(parent->getChildWidth());
            setHeight(width);


        }

        parentNodeKind = parent->getGraphML()->getDataValue("kind");

        // connect this item to its parent item
        //connect(this, SIGNAL(updateParentHeight(NodeItem*)), parent, SLOT(updateHeight(NodeItem*)));

    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }

    //Set Minimum Size.
    minimumWidth = width;
    minimumHeight = height;

    //Set Maximum Size
    expandedWidth = width;
    expandedHeight = height;

    //Update Width and Height with values from the GraphML Model If they have them.
    retrieveGraphMLData();

    //Update GraphML Model for size/position if they have been changed.
    updateGraphMLSize();

    updateModelPosition();
    //updateGraphMLPosition();

    setupGraphMLConnections();

    setupAspect();
    setupBrushes();
    setupIcon();
    setupLabel();

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

    // why is the EventPortInstance permanently invisible?
    //if (parentNodeKind.endsWith("EventPortInstance")){
    //    setPermanentlyInvisible(true);
    //}

    aspectsChanged(aspects);

    if(parentNodeItem){
        parentNodeItem->childPosUpdated();
    }
    outlines.clear();

    if(IN_SUBVIEW){
        setVisible(true);
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
    bottomRightX += itemMargin/2;
    bottomRightY += itemMargin/2;
    bottomRightX += itemMargin/2;
    bottomRightY += itemMargin/2;

    return QRectF(QPointF(topLeftX, topLeftY), QPointF(bottomRightX, bottomRightY));
}


QRectF NodeItem::minimumVisibleRect()
{
    return QRectF(QPointF(0, 0), QPointF(minimumWidth + getItemMargin(), minimumHeight + getItemMargin()));
}


QRectF NodeItem::gridRect()
{
    QRectF rectangle;

    if (isExpanded()) {

        QPointF topLeft = minimumVisibleRect().bottomLeft();

        if (nodeKind == "Model" || nodeKind.endsWith("Definitions")) {
            topLeft = minimumVisibleRect().topLeft();
            topLeft += QPointF(getGridSize(), getGridSize());
        }else{
            topLeft += QPointF(getGridSize(), 0);
        }

        QPointF bottomRight = boundingRect().bottomRight();

        // topLeft += QPointF(getGridSize(), getGridSize());
        bottomRight -= QPointF(getGridSize(), getGridSize());
        rectangle = QRectF(topLeft, bottomRight);

        int xGrids = qRound(rectangle.width() / getGridSize());
        int yGrids = qRound(rectangle.height() / getGridSize());
        rectangle.setWidth(xGrids * getGridSize());
        rectangle.setHeight(yGrids * getGridSize());
    }

    return rectangle;
}

void NodeItem::childPosUpdated()
{
    //Force onto the current child.
    //prepareGeometryChange();
    //if()

    QRectF rectf = getMinimumChildRect();

    qreal w = rectf.width();
    qreal h = rectf.height();

    // don't square the Model and Definitions containers
    if(firstReposition && nodeKind != "Model" && !nodeKind.endsWith("Definitions")){
        firstReposition = false;
        //qCritical() << "FORCE SQUARED";
        if(w > h){
            //Grow height to width
            h = w;
        }else{
            //Grow width to height
            w = h;
        }
    }
    w = qMax(w, width);
    h = qMax(h, height);

    //qCritical() << width << " x " << height;

    //w = qMax(w, width);
    //h = qMax(h, height);
    setSize(w, h);
    updateParent();
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
    return nodeSelected;
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
    if(PAINT_OBJECT){
        QPen Pen;
        QBrush Brush;

        if(nodeSelected){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }



        QRectF rectangle = boundingRect();
        rectangle.setWidth(rectangle.width() - Pen.width());
        rectangle.setHeight(rectangle.height() - Pen.width());
        rectangle.translate(Pen.width()/2, Pen.width()/2);

        if(!hasVisibleChildren() && !nodeKind.endsWith("Definitions")){
            Brush.setColor(Qt::transparent);

        }
        if(onGrid){
            QColor brushColor = Brush.color();
            if(brushColor.alpha() > 120){
                brushColor.setAlpha(120);
            }
            Brush.setColor(brushColor);

        }

        QBrush HeaderBrush = Brush;
        QColor HeaderBrushColor = HeaderBrush.color();

        HeaderBrushColor.setAlpha(255);
        HeaderBrush.setColor(HeaderBrushColor);

        Node* node = getNode();
        if(node && (node->isInstance() || node->isImpl())){
            if(!node->getDefinition()){
                Brush.setStyle(Qt::DiagCrossPattern);
                hasDefinition = false;
            }else{
                hasDefinition = true;
            }

        }

        if(lockIcon){
            if((hasDefinition || LOCKED_POSITION) && !nodeKind.endsWith("Definitions")){
                lockIcon->setVisible(true);
            }else{
                lockIcon->setVisible(false);
            }
        }

        qreal cornerRadius = getCornerRadius();

        painter->setPen(Pen);
        painter->setBrush(Brush);
        painter->drawRoundedRect(rectangle, cornerRadius, cornerRadius);



        if(isExpanded() && hasVisibleChildren() && textItem && (width != minimumWidth)){
            painter->setPen(pen);
            qreal yPos = minimumHeight + textItem->boundingRect().height();
            QLineF line = QLineF(Pen.width(), yPos, boundingRect().width() - Pen.width(), yPos);
            painter->drawLine(line);
        }


        //painter->drawRect(getMinimumChildRect());

        //New Code
        if(GRIDLINES_VISIBLE && PAINT_OBJECT && drawGrid){
            painter->setClipping(false);
            QPen linePen = painter->pen();

            linePen.setStyle(Qt::DashLine);
            linePen.setWidth(minimumWidth / 1000);
            linePen.setColor(QColor(0,0,0));
            painter->setPen(linePen);

            painter->drawLines(xGridLines);
            painter->drawLines(yGridLines);

        }
        if(outlines.size() > 0){
            foreach(QRectF childrec, outlines){
                QPen linePen = painter->pen();
                linePen.setColor(Qt::yellow);
                linePen.setWidth(selectedPen.width());

                QBrush paintBrush = painter->brush();
                paintBrush.setColor(Qt::transparent);
                painter->setBrush(paintBrush);

                painter->setPen(linePen);

                double radius = getChildCornerRadius();

                painter->drawRoundedRect(childrec, radius, radius);
            }

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

    /*

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

*/

    //Check for horizontal;


    //Check for vertical

    return NO_RESIZE;
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
        connect(item, SIGNAL(setEdgeVisibility(bool)), line, SLOT(setVisible(bool)));
        item = dynamic_cast<NodeItem*>(item->parentItem());
    }
    connect(this, SIGNAL(setEdgeSelected(bool)), line, SLOT(setSelected(bool)));

    connections.append(line);
}


void NodeItem::removeEdgeItem(EdgeItem *line)
{
    connections.removeAll(line);
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
    hasSelectionMoved = true;
    setPos(currentPos);
}

void NodeItem::adjustSize(QSizeF delta)
{
    qreal newWidth = getWidth() + delta.width();
    qreal newHeight = getHeight() + delta.height();

    prepareGeometryChange();
    setWidth(newWidth);
    setHeight(newHeight);
}

void NodeItem::drawOutline(QRectF translatedOutline)
{
    if(!outlines.contains(translatedOutline)){
        prepareGeometryChange();
        outlines.append(translatedOutline);
    }
}

void NodeItem::clearOutlines()
{
    prepareGeometryChange();
    outlines.clear();
}



double NodeItem::getChildWidth()
{
    if (nodeKind == "Model") {
        return MODEL_WIDTH/2;
    } else {
        return minimumWidth / MINIMUM_HEIGHT_RATIO;
    }
}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos()
{
    int currentX = 1;
    int currentY = 1;

    qreal childWidth = getChildWidth();

    QPainterPath childrenPath;

    foreach(NodeItem* child, getChildNodeItems()){
        if(child->isVisible()){
            QRectF childRect =  child->boundingRect();
            childRect.translate(child->pos());
            childRect.setWidth(childRect.width() + childWidth);
            childRect.setHeight(childRect.height() + childWidth);
            childrenPath.addRect(childRect);
        }
    }
    while(true){

        QPointF nextPosition = getGridPosition(currentX, currentY);

        if(!childrenPath.contains(nextPosition)){
            return nextPosition;
        }else{

            /*
            if((nextPosition.x() + childWidth) > boundingRect().width()){
                currentX = 1;
                currentY += 1;
            }else{
                currentX += 1;
            }
            */

            if ((nextPosition.x() + childWidth) > boundingRect().width()) {
                if (currentX >= currentY) {
                    currentX = 1;
                    currentY += 1;
                } else {
                    //currentY = 1; // this puts it in an infinite loop!
                    currentX += 1;
                }
            } else {
                currentX += 1;
            }
        }
    }

}


/*

    nextX += 3;


    QPointF nextPosition = getGridPosition(nextX, nextY);


    //Check Child Position.

    if ((nextPosition.x() + getChildWidth()) > boundingRect().width()) {
        nextX = 1;
        nextY += 3;
        firstReposition = true;
    }

    nextPosition.setX(nextPosition.x() - getChildWidth()/2);
    nextPosition.setY(nextPosition.y() - getChildWidth()/2);

//    return position;
//}
*/

void NodeItem::itterateNextSpace()
{

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


void NodeItem::setSelected(bool selected)
{
    if(nodeSelected != selected){
        nodeSelected = selected;
        if(nodeSelected){
            this->setZValue(1000);
        }else{
            this->setZValue(0);
        }

        update();
        updateTextLabel();
        emit setEdgeSelected(selected);
    }
}


void NodeItem::setVisible(bool visible)
{
    QGraphicsItem::setVisible(visible);
    emit setEdgeVisibility(visible);
}


void NodeItem::setPermanentlyCentralized(bool centralized)
{
    permanentlyCentralized = centralized;
}


void NodeItem::graphMLDataChanged(GraphMLData* data)
{
    if(data){
        QString dataKey = data->getKeyName();
        QString dataValue = data->getValue();

        if(dataKey == "x" || dataKey == "y"){
            //Update the Position
            QPointF newCenterPos = centerPos();

            if(dataKey == "x"){
                newCenterPos.setX(dataValue.toFloat());
            }else if(dataKey == "y"){
                newCenterPos.setY(dataValue.toFloat());
            }
            setCenterPos(newCenterPos);

        }else if(dataKey == "width" || dataKey == "height"){
            if(dataValue == "inf"){
                dataValue = QString::number(MODEL_WIDTH);
            }
            //Update the Size
            prepareGeometryChange();


            if(dataKey == "width"){
                //qCritical() << dataValue;

                setWidth(dataValue.toFloat());
            }else if(dataKey == "height"){
                //qCritical() << dataValue;
                setHeight(dataValue.toFloat());
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
    // added this so sort can be un-done
    GraphMLItem_TriggerAction("NodeItem: Sorting Node");

    //Get the number of un-locked items
    QMap<int, NodeItem*> toSortMap;
    QList<NodeItem*> lockedItems;

    foreach(NodeItem* child, getChildNodeItems()){
        Node* childNode = child->getNode();
        if(!child->isVisible() && nodeKind != "Model"){
            continue;
        }
        if(child->isLocked()){
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

    QList<NodeItem*>  toSortItems = toSortMap.values();

    //Calculate the grid size required to fit all of sortItems.
    int gridSize = ceil(sqrt((float)toSortItems.size()));

    bool finishedLayout = false;

    int x = 1;
    int y = 2;

    // make the gap between view aspects smaller
    int xIncrement = 3;
    int yIncrement = 3;

    if (nodeKind == "Model") {
        xIncrement = 5;
    }

    NodeItem* nextItem = 0;

    for (y; y <= (gridSize * yIncrement) - 1; y += yIncrement) {

        if (nodeKind == "Model") {
            x = 2;
        } else {
            x = 1;
        }

        for (x; x <= (gridSize * xIncrement) - 1; x += xIncrement) {
            if (toSortItems.size() > 0) {
                nextItem = toSortItems.takeFirst();
                nextItem->setCenterPos(getGridPosition(x,y));
            } else {
                finishedLayout = true;
                break;
            }
        }

        if (finishedLayout) {
            break;
        }
    }

    nextX = x;
    nextY = y;
}


void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!PAINT_OBJECT){
        // clear selection when pressing on a !PAINTED item
        GraphMLItem_ClearSelection(false);
        QGraphicsItem::mousePressEvent(event);
    }

    switch (event->button()) {

    case Qt::MiddleButton:{
        if(!PAINT_OBJECT){
            return;
        }
        break;
    }
    case Qt::LeftButton:{
        // unselect any selected node item
        // when the model is pressed
        if(!PAINT_OBJECT){
            //emit clearSelection();
            return;
        }

        previousScenePosition = event->scenePos();
        hasSelectionMoved = false;
        isNodePressed = true;

        if(!isSelected()){
            if (!event->modifiers().testFlag(Qt::ControlModifier)){
                //Check for parent selection.
                GraphMLItem_ClearSelection(false);
            }
            GraphMLItem_AppendSelected(this);
        }


        break;
    }
    case Qt::RightButton:{
        if(!PAINT_OBJECT){
            return;
        }

        if(!isSelected()){
            if (!event->modifiers().testFlag(Qt::ControlModifier)){
                //Check for parent selection.
                GraphMLItem_ClearSelection(false);
            }
            GraphMLItem_AppendSelected(this);
        }

        //emit setSelection(this);

        //Select this node, and construct a child node.
        //emit triggerSelected(this);
        //emit triggerSelected(getGraphML());
        break;
    }

    default:
        break;
    }
}


void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(inSubView){
        return;
    }
    switch (event->button()) {
    case Qt::LeftButton:

        // added this to center aspects when double-clicking on !PAINTED items
        if(!PAINT_OBJECT){
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

        if(hasVisibleChildren() && iconPressed(event->pos())){

            if(isExpanded()){
                GraphMLItem_TriggerAction("Expanded Item");
                expandItem(false);
            }else{
                GraphMLItem_TriggerAction("Contracted Item");
                expandItem(true);
            }
            break;
        }

        GraphMLItem_SetCentered(this);

    }
}


void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {
    case Qt::LeftButton:{
        if(!PAINT_OBJECT){
            GraphMLItem_ClearSelection(true);
            return;
        }

        // check if item is still inside sceneRect after it's been moved
        if (hasSelectionMoved) {
            //NodeItem_MoveFinished();
            if(parentNodeItem){
                parentNodeItem->setDrawGrid(false);
            }

            if (!currentSceneRect.contains(scenePos()) ||
                    !currentSceneRect.contains(scenePos().x()+width, scenePos().y()+height)) {
                GraphMLItem_MovedOutOfScene(this);
            }
            NodeItem_MoveFinished();
        }
        if(hasSelectionResized){
            NodeItem_ResizeFinished();

        }

        hasSelectionResized = false;

        // have to reset cursor here otherwise it's stuck on Qt::SizeAllCursor after being moved
        setCursor(Qt::OpenHandCursor);

        nodeResizing = false;
        hasSelectionMoved = false;
        isNodePressed = false;
        break;
    }
    case Qt::MiddleButton:{
        if (PAINT_OBJECT) {
            if (event->modifiers().testFlag(Qt::ControlModifier)) {
                GraphMLItem_TriggerAction("Sorting Node");
                newSort();
                //sort();
            } else {
                GraphMLItem_SetCentered(this);
            }
        } else {
            emit centerViewAspects();
        }
        break;
    }

    default:
        break;
    }
}


void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!PAINT_OBJECT){
        return;
    }


    if(isNodePressed && nodeSelected){

        if(currentResizeMode){
            if(hasSelectionResized == false){
                emit GraphMLItem_TriggerAction("Resizing Selection");
                hasSelectionResized = true;

            }
            QPointF delta = (event->scenePos() - previousScenePosition);

            QSizeF dSize(delta.x(), delta.y());

            if(currentResizeMode == HORIZONTAL_RESIZE){
                dSize.setHeight(0);
            }else if(currentResizeMode == VERTICAL_RESIZE){
                dSize.setWidth(0);
            }

            NodeItem_ResizeSelection(dSize);

        }else{
            if(hasSelectionMoved == false){
                qCritical() << "ADD ACTION";
                //qCritical() << "UNDO ACTIONB ADDED";
                emit GraphMLItem_TriggerAction("Moving Selection");
                setCursor(Qt::SizeAllCursor);
                if(parentNodeItem){
                    parentNodeItem->setDrawGrid(true);
                }
                hasSelectionMoved = true;
            }
            QPointF delta = (event->scenePos() - previousScenePosition);

            NodeItem_MoveSelection(delta);
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



    if(nodeSelected){

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
    if(width != w){
        QRectF childRect = getMinimumChildRect();


        bool updateModel = false;
        if(w <= childRect.width()){
            w = childRect.width();
            updateModel = true;
        }

        width = w;
        if(isExpanded()){
            expandedWidth = w;
        }

        updateTextLabel();
        updateChildrenOnChange();
        updateGridLines();
        updateParent();

        if(updateModel){
            updateModelSize();
        }


    }


}


void NodeItem::setHeight(qreal h)
{
    if(height != h){

        QRectF childRect = getMinimumChildRect();

        bool updateModel = false;
        if(h <= childRect.height()){
            //qCritical() << "H:AM I CHANGING";
            h = childRect.height();
            updateModel = true;
        }

        height = h;

        if(isExpanded()){
            expandedHeight = h;

        }

        updateGridLines();
        updateChildrenOnChange();
        updateParent();

        if(updateModel){
            updateModelSize();
        }
    }
}

void NodeItem::setSize(qreal w, qreal h)
{
    QRectF childRect = getMinimumChildRect();

    if(w != width || height != h ){

        if(w <= childRect.width()){
            w = childRect.width();
        }
        if(h <= childRect.height()){
            h = childRect.height();
        }

        prepareGeometryChange();
        height = h;

        width= w;
        if(isExpanded()){
            expandedWidth = w;
        }

        updateTextLabel();
        updateGridLines();

        updateModelSize();
        updateParent();

    }
}


void NodeItem::updateGridLines()
{
    if(GRIDLINES_VISIBLE){
        QRectF boundingGridRect = gridRect();
        qreal errorPercent = .10;
        xGridLines.clear();
        yGridLines.clear();


        for(qreal x = boundingGridRect.left(); x <= boundingGridRect.right() + (errorPercent * getGridSize()); x += getGridSize()){
            xGridLines << QLineF(x, boundingGridRect.top(), x, boundingGridRect.bottom());
        }

        for(qreal y = boundingGridRect.top(); y <= boundingGridRect.bottom() + (errorPercent * getGridSize()); y += getGridSize()){
            yGridLines << QLineF(boundingGridRect.left(), y, boundingGridRect.right(), y);
        }
    }
}


void NodeItem::setPermanentlyInvisible(bool isInvisible)
{
    permanentlyInvisible = isInvisible;
    setVisible(permanentlyInvisible);
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


void NodeItem::setDrawGrid(bool value)
{

    drawGrid= value;
    this->prepareGeometryChange();
}


void NodeItem::updateTextLabel(QString newLabel)
{
    if(!textItem){
        return;
    }

    textItem->setTextWidth(width);

    if(newLabel != ""){
        qCritical() << newLabel;
        textItem->setPlainText(newLabel);
    }
}

QRectF NodeItem::getMinimumChildRect()
{
    qreal itemMargin = getItemMargin()/2;



    QPointF topLeft(itemMargin, itemMargin);
    QPointF bottomRight(itemMargin + minimumWidth, itemMargin + minimumHeight);


    bool hasChildren = false;
    bool hasUncontainedChildren = false;


    foreach(NodeItem* child, childNodeItems){

        if(child->isVisible()){
        //if(!child->isHidden()){
            hasChildren = true;
            qreal childMaxX = child->pos().x() + child->boundingRect().width();
            qreal childMaxY = child->pos().y() + child->boundingRect().height();
            if(childMaxX >= bottomRight.x()){
                bottomRight.setX(childMaxX);
                hasUncontainedChildren=true;
            }
            if(childMaxY >= bottomRight.y()){
                bottomRight.setY(childMaxY);
                hasUncontainedChildren=true;
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

    /*
     * Changed these to match the view button colors
     */
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
    }


    selectedColor = color;

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(Qt::gray);
    pen.setWidth(4);
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

        isOverGrid(pos);

        QGraphicsItem::setPos(pos);

        updateChildrenOnChange();
        updateParent();
    }
}


void NodeItem::updateParent()
{
    if(parentNodeItem){
        parentNodeItem->childPosUpdated();
    }
}


void NodeItem::aspectsChanged(QStringList aspects)
{    
    currentViewAspects = aspects;

    if(hidden || !PAINT_OBJECT || permanentlyInvisible){
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
    }
    QImage lockImage (":/Resources/Icons/lock.png");
    if (!lockImage.isNull()){
        lockIcon = new QGraphicsPixmapItem(QPixmap::fromImage(lockImage), this);
    }

    if (icon) {
        float iconPercent = 1;
        qreal iconHeight = icon->boundingRect().height();
        qreal iconWidth = icon->boundingRect().width();
        qreal scaleFactor = (iconPercent * minimumHeight / iconHeight);

        icon->setScale(scaleFactor);
        icon->setTransformationMode(Qt::SmoothTransformation);

        icon->setToolTip("Double Click to Expand/Contract Node");

        int brushSize = selectedPen.width();
        icon->setPos((getCornerRadius()/2 + brushSize) ,0);

        iconWidth *= scaleFactor;
        iconHeight *= scaleFactor;

        qreal lockScale = .25;
        qreal lockHeight = lockIcon->boundingRect().height();
        qreal lockScaleFactor = lockScale * ((minimumHeight) / lockHeight);

        if (lockIcon){
            lockIcon->setScale(lockScaleFactor);
            lockHeight *= lockScaleFactor;
            lockIcon->setTransformationMode(Qt::SmoothTransformation);
            lockIcon->setPos(brushSize, brushSize);
            //lockIcon->setPos(brushSize, (iconHeight/2) - (lockHeight/2));
        }


        qreal xPos = (minimumVisibleRect().width() - iconWidth)/2;

        icon->setX(xPos);
        icon->setY((minimumHeight - iconHeight) /2);
    }
}


/**
 * @brief NodeItem::setupLabel
 * This sets up the font and size of the label.
 */
void NodeItem::setupLabel()
{
    // this updates this item's label
    if (nodeKind == "Model" || nodeKind.endsWith("Definitions")) {
        return;
    }

    // setup font
    float fontSize = qMax(FONT_RATIO * minimumHeight, 1.0);
    QFont font("Arial", fontSize);

    // setup text item
    //textItem = new QGraphicsTextItem(this);
    textItem = new EditableTextItem(this);
    connect(textItem, SIGNAL(textUpdated(QString)),this, SLOT(labelUpdated(QString)));
    connect(textItem, SIGNAL(editableItem_hasFocus(bool)), this, SIGNAL(Nodeitem_HasFocus(bool)));
    //textItem->setTextInteractionFlags(Qt::NoTextInteraction);

    textItem->setToolTip("Double Click to Edit Label");
    textItem->setTextWidth(minimumWidth);

    //qreal xPos = getItemMargin()/2 + selectedPen.widthF();
    //qreal xPos = (boundingRect().width() - textItem->boundingRect().width())/2;
    textItem->setPos(icon->pos().x(), minimumHeight);
    textItem->setFont(font);

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

QPointF NodeItem::isOverGrid(const QPointF position)
{
    QPointF centerPosition = position + minimumVisibleRect().center();

    QPointF closestGridPoint;
    if(this->parentNodeItem){
        closestGridPoint = parentNodeItem->getClosestGridPoint(centerPosition);
    }



    QLineF line(centerPosition, closestGridPoint);

    if((line.length()/ minimumWidth) <= SNAP_PERCENTAGE){
        //QGraphicsItem::setPos(closestGridPoint - minimumVisibleRect().center());
        if(hasSelectionMoved){
            onGrid = true;
        }
        if(this->parentNodeItem && hasSelectionMoved){
            QRectF rect = boundingRect();
            QPointF newPoint = closestGridPoint - minimumVisibleRect().center();

            rect.translate(newPoint);

            parentNodeItem->drawOutline(rect);
        }
        return closestGridPoint;
    }else{
        onGrid = false;
        if(this->parentNodeItem){
            parentNodeItem->clearOutlines();

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
    GRIDLINES_VISIBLE = on;
    if(on){
        updateGridLines();
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


void NodeItem::updateGraphMLSize()
{
    //Give the current Width and height. update the width/height variable in the GraphML Model.
    GraphML* modelEntity = getGraphML();
    if(modelEntity){
        GraphMLData* wData = modelEntity->getData("width");
        GraphMLData* hData = modelEntity->getData("height");
        wData->setValue(QString::number(width));
        hData->setValue(QString::number(height));
    }
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
    emit nodeItemMoved();

    if(this->isPermanentlyCentered()){
        emit recentralizeAfterChange(getGraphML());
    }
}


void NodeItem::retrieveGraphMLData()
{

    double graphmlHeight = getGraphML()->getDataValue("height").toDouble();
    double graphmlWidth = getGraphML()->getDataValue("width").toDouble();


    //Update the position with values from the GraphML Model if they have them.
    double graphmlX = getGraphML()->getDataValue("x").toDouble();
    double graphmlY = getGraphML()->getDataValue("y").toDouble();

    if(inSubView){
        qCritical() << "X: " << graphmlX;
        qCritical() << "Y: " << graphmlY;
        qCritical() << pos();
    }
    setCenterPos(QPointF(graphmlX, graphmlY));

    if(graphmlHeight > height || graphmlWidth > width){
        //qCritical() << "Expanded Given.";
        expandedWidth = graphmlWidth;
        expandedHeight = graphmlHeight;
        expandItem(true);
    }else if(graphmlHeight != 0 && graphmlWidth != 0){
        //qCritical() << "Not Expanding";
        expandItem(false);
    }else{
        //qCritical() << "No value; Expanding";
        expandItem(true);
    }
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
    return permanentlyCentralized;
}


qreal NodeItem::getGridSize()
{
    if (nodeKind == "Model") { // || nodeKind.endsWith("Definitions")) {
        return minimumVisibleRect().width() / (GRID_RATIO + 2);
    } else {
        return minimumVisibleRect().width() / GRID_RATIO;
    }
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

    emit nodeItemMoved();
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
    return minimumWidth / (2 * MINIMUM_HEIGHT_RATIO);
}

double NodeItem::getChildCornerRadius()
{
    return getChildWidth() / (2 * MINIMUM_HEIGHT_RATIO);

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


QColor NodeItem::invertColor(QColor oldColor)
{
    int red = 255 - oldColor.red();
    int green = 255 - oldColor.green();
    int blue = 255 - oldColor.blue();
    return QColor(red, green, blue);
}


double NodeItem::getItemMargin() const
{
    return (minimumHeight / MINIMUM_HEIGHT_RATIO) * (1 + (3 * FONT_RATIO));
}


/**
 * @brief NodeItem::expandItem
 * @param show
 */
void NodeItem::expandItem(bool show)
{
    if(!getGraphML()){
        return;
    }


    expanded = show;

    if(show){
        QRectF childRec = getMinimumChildRect();

        expandedWidth = qMax(childRec.width(), expandedWidth);
        expandedHeight = qMax(childRec.height(), expandedHeight);

        GraphMLItem_SetGraphMLData(getGraphML(), "width", QString::number(expandedWidth));
        GraphMLItem_SetGraphMLData(getGraphML(), "height", QString::number(expandedHeight));
    } else {

        //qreal mW = minimumVisibleRect().width();
        //qreal mH = minimumVisibleRect().width();
        //GraphMLItem_SetGraphMLData(getGraphML(), "width", QString::number(mW));
        //GraphMLItem_SetGraphMLData(getGraphML(), "height", QString::number(mH));
        GraphMLItem_SetGraphMLData(getGraphML(), "width", QString::number(minimumWidth));
        GraphMLItem_SetGraphMLData(getGraphML(), "height", QString::number(minimumHeight));

    }

    //Hide the children.
    foreach(NodeItem* nodeItem, childNodeItems){
        if (!nodeItem->isHidden()) {
            nodeItem->setVisible(show);
        }
    }


    prepareGeometryChange();
    update();
}


/**
 * @brief NodeItem::updateHeight
 * Expand this item's height to fit the newly added child and grow to the next grid line size.
 * This only gets called by painted items.
 * @param child
 */
void NodeItem::updateHeight(NodeItem *child)
{
    //if (PAINT_OBJECT){
    qreal childBottomY = child->boundingRect().height() + child->pos().y();
    qreal gridBottomY = gridRect().bottom();
    //qreal gridBottomY = gridRect().height() + gridRect().top();

    //qCritical() << ""
    qreal diffHeight = childBottomY - gridBottomY;

    if (diffHeight > 0) {
        qreal requiredHeight = height + diffHeight;
        //Expand to next Grid Line
        qreal gridHeight = ceil(requiredHeight/getGridSize()) * getGridSize();

        GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(gridHeight));
        GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(gridHeight));

        // recurse while there is a parent node item
        NodeItem* parentNodeItem = dynamic_cast<NodeItem*>(parentItem());
        if (parentNodeItem) {

        }
    }
}



void NodeItem::updateModelPosition()
{
    if(hasSelectionMoved){
        setLocked(onGrid);
    }
    if(GRIDLINES_VISIBLE){
        QPointF gridPoint = isOverGrid(pos());
        if(!gridPoint.isNull()){
            setCenterPos(gridPoint);
        }
    }

    GraphMLItem_SetGraphMLData(getGraphML(), "x", QString::number(centerPos().x()));
    GraphMLItem_SetGraphMLData(getGraphML(), "y", QString::number(centerPos().y()));
    onGrid = false;
    if(this->parentNodeItem){
        parentNodeItem->clearOutlines();
        parentNodeItem->updateModelSize();
    }
}

void NodeItem::updateModelSize()
{

    GraphMLItem_SetGraphMLData(getGraphML(), "width", QString::number(width));
    GraphMLItem_SetGraphMLData(getGraphML(), "height", QString::number(height));
    if(this->parentNodeItem){
        //parentNodeItem->updateModelSize();
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
        qCritical() << "Tell Controller nwe label is: " << newLabel;

        //if(currentLabel != newLabel){
            GraphMLItem_TriggerAction("Set New Label");
            GraphMLItem_SetGraphMLData(getGraphML(), "label", newLabel);
        //}
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
    return expanded;
}


QPointF NodeItem::getClosestGridPoint(QPointF referencePoint)
{
    qreal gridSize = getGridSize();

    qreal gridX = qRound(referencePoint.x() / gridSize) * gridSize;
    qreal gridY = qRound(referencePoint.y() / gridSize) * gridSize;

    // bound the closest grid point to the gridRect
    if(referencePoint.x() < gridRect().left()){
        gridX = gridRect().left() + gridSize;
    }else if(referencePoint.x() > gridRect().right()){
        gridX = gridRect().right() - gridSize;
    }

    if(referencePoint.y() < gridRect().top()){
        gridY = gridRect().top() + gridSize;
    }else if(referencePoint.y() > gridRect().bottom()){
        gridY = gridRect().bottom() - gridSize;
    }
    referencePoint.setX(gridX);
    referencePoint.setY(gridY);

    return referencePoint;
}
