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
#define LABEL_LINE_LENGTH 12
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

    LOCKED_POSITION = false;
    GRIDLINES_VISIBLE = false;
    drawGrid = false;
    onGrid = false;

    nextX = 1;
    nextY = 1;

    nodeSelected = false;
    isNodePressed = false;
    permanentlyCentralized = false;
    permanentlyInvisible = false;
    hidden = false;
    expanded = true;
    hasSelectionMoved = false;
    hasDefinition = false;

    icon = 0;
    lockIcon = 0;
    textItem = 0;
    labelButton = 0;
    labelWidget = 0;
    proxyWidget = 0;
    initialWidth = 0;
    initialHeight = 0;

    nodeKind = getGraphML()->getDataValue("kind");

    QString parentNodeKind = "";
    if (parent) {
        setWidth(parent->getChildWidth());
        setHeight(width);

        /*
        if (nodeKind == "DeploymentDefinitions") {
            setWidth(MODEL_WIDTH);
            setHeight(MODEL_HEIGHT);
        } else if (nodeKind.endsWith("Definitions")) {
            setHeight(width);
            //expanded = true;
        } else {
            //setHeight(width/7);
            setHeight(width);
        }
        */

        parentNodeKind = parent->getGraphML()->getDataValue("kind");

        // connect this item to its parent item
        connect(this, SIGNAL(updateParentHeight(NodeItem*)), parent, SLOT(updateHeight(NodeItem*)));

    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }

    initialWidth = width;
    initialHeight = height;

    prevWidth = width;
    prevHeight = height;

    minimumHeight = initialWidth;
    minimumWidth = initialWidth;

    //Update Width and Height with values from the GraphML Model If they have them.
    retrieveGraphMLData();

    if(!IN_SUBVIEW){
        if (width < initialWidth) {
            width = initialWidth;
        }
        if (height < initialHeight) {
            height = initialHeight;
        }
    }

    //Update GraphML Model for size/position if they have been changed.
    updateGraphMLSize();
    updateGraphMLPosition();

    setupGraphMLConnections();

    setupAspect();
    setupBrushes();
    setupIcon();
    setupLabel();

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemSendsGeometryChanges);

    setCacheMode(QGraphicsItem::NoCache);

    if(getGraphML()->getDataValue("kind") == "Model" || getGraphML()->getDataValue("kind") == "DeploymentDefinitions"){
        setPaintObject(false);
    }else{
        setPaintObject(true);
    }

    if (parentNodeKind.endsWith("EventPortInstance")){
        setPermanentlyInvisible(true);
    }

    aspectsChanged(aspects);
    updateParentHeight(this);
    updateGridLines(true,true);
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

    /*
    if (nodeKind == "Model") {
        qDebug() << "------ boundingRect() ------";
        qDebug() << "width: " << width;
        qDebug() << "height: " << height;
    }
    */

    float itemMargin = getItemMargin();

    bottomRightX += itemMargin/2;
    bottomRightY += itemMargin/2;

    if (width <= minimumWidth) {
        // what if after the itemMargin is added width is still <= minWidth?
        bottomRightX += itemMargin/2;
        bottomRightY += itemMargin/2;
    }

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
        }

        QPointF bottomRight = boundingRect().bottomRight();

        topLeft += QPointF(getGridSize(), getGridSize());
        bottomRight -= QPointF(getGridSize(), getGridSize());
        rectangle = QRectF(topLeft, bottomRight);

        int xGrids = qRound(rectangle.width() / getGridSize());
        int yGrids = qRound(rectangle.height() / getGridSize());
        rectangle.setWidth(xGrids * getGridSize());
        rectangle.setHeight(yGrids * getGridSize());
    }
    return rectangle;
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
    QRectF itemRectangle = boundingRect();
    itemRectangle.moveTo(scenePos());
    return sceneRect.contains(itemRectangle);
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

        if(this->childNodeItems.size() == 0 && !nodeKind.endsWith("Definitions")){
            Brush.setColor(Qt::transparent);
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
            if(lockIcon && lockIcon->isVisible() != hasDefinition){
                lockIcon->setVisible(hasDefinition);
            }
        }

        qreal cornerRadius = getCornerRadius();

        if(onGrid){
            Pen.setColor(invertColor(Pen.color()));
        }

        painter->setPen(Pen);
        painter->setBrush(Brush);
        painter->drawRoundedRect(rectangle, cornerRadius, cornerRadius);

        /*
    //Show bounding Rectangle for size
    if(textItem){
        QRectF textRect = textItem->boundingRect();
        textRect.translate(textItem->pos());
        painter->drawRect(textRect);
    }
    */

        /*
    // show MINIMUM visible rect
    if(textItem){
        QRectF textRect = minimumVisibleRect();
        //textRect.translate(textItem->pos());
        painter->drawRect(textRect);
    }
    */

        if(isExpanded() && textItem && (width != minimumWidth)){
            painter->setPen(pen);
            qreal yPos = minimumHeight + textItem->boundingRect().height();
            QLineF line = QLineF(Pen.width(), yPos, boundingRect().width() - Pen.width(), yPos);
            painter->drawLine(line);
        }

    }

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
}


bool NodeItem::hasChildren()
{
    return childNodeItems.size() > 0;
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

    setPos(pos);
}

QPointF NodeItem::centerPos()
{
    return pos() + minimumVisibleRect().center();
}


void NodeItem::adjustPos(QPointF delta)
{
    QPointF currentPos = pos();
    currentPos += delta;
    setPos(currentPos);
}


double NodeItem::getChildWidth()
{
    return initialWidth / MINIMUM_HEIGHT_RATIO;
}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos()
{
    QPointF position = getGridPosition(nextX, nextY);
    nextX += 3;

    QPointF nextPosition = getGridPosition(nextX, nextY);

    if ((nextPosition.x() + getChildWidth()) > boundingRect().width()) {
        nextX = 1;
        nextY += 3;
    }

    nextPosition.setX(nextPosition.x() - getChildWidth()/2);
    nextPosition.setY(nextPosition.y() - getChildWidth()/2);
    return position;
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
    bool isCurrentlyVisible = isVisible();
    if(isCurrentlyVisible != visible){
        QGraphicsItem::setVisible(visible);
        emit setEdgeVisibility(visible);
    }
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
            //Update the Size
            prepareGeometryChange();

            if(dataKey == "width"){
                //qCritical() << "Width";
                setWidth(dataValue.toFloat());
            }else if(dataKey == "height"){
                //qCritical() << "Height" << dataValue.toFloat();
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
 * @brief NodeItem::sort
 * This methods sorts this node item's children one by one, from left to right
 * until it can't fit it inside this item's origWidth in which case it moves it
 * to the next row. The maxHeight per row is being used to get the new y pos for
 * the next row. The maxWidth per column isn't currently being stored but it will
 * need to be for future sorting.
 *
 * The sorting for Components/ComponentInstances is different. The in/out event
 * ports are placed on the item's left/right edge respectively and the attributes
 * in the middle. A bigger gap is also required for nodes containing Components
 * to prevent the in/out event ports from overlapping each other.
 */
void NodeItem::sort()
{
    //emit triggerAction("Sorting Children");

    bool permanentlyHiddenContainer = (nodeKind == "Model" || nodeKind == "DeploymentDefinitions");

    bool componentLayout = (nodeKind.contains("Component"));
    bool componentAssembly = (nodeKind == "ComponentAssembly");
    bool fileContainsComponents = (nodeKind == "File" && getChildrenKind().contains("Component"));
    bool componentHasChildren = false;

    // if this item is a File/ComponentAssembly and contains Components for its children,
    // check if any of them has children and increase gapX accordingly
    if (fileContainsComponents || componentAssembly) {
        for(int i = 0 ; i < childNodeItems.size() ; i++){
            NodeItem* nodeItem = childNodeItems[i];
            if (nodeItem && (nodeItem->getChildNodeItems().count() > 0)) {
                componentHasChildren = true;
                break;
            }
        }
    }

    //float topY = getCornerRadius();
    float topY = getGridSize()/2;
    float gapY = topY;
    float gapX = gapY;

    float labelHeight = 3 * FONT_RATIO * minimumHeight;
    float colHeight = minimumHeight + labelHeight;
    float rowWidth = gapX;

    float inCol = colHeight;
    float outCol = colHeight;
    float attCol = colHeight;

    float maxHeight = 0;
    float maxWidth = 0;

    int numberOfItems = 0;

    /*
    // if the node item's children are Components or ComponentInstances
    // leave more gap for the in/out event ports along its edges
    if ((fileContainsComponents || componentAssembly) && componentHasChildren) {
        gapY = topY;
        gapX = gapY;
        rowWidth = gapX;
    }
    */

    // position children differently for Model and DeploymentDefinitions
    if (permanentlyHiddenContainer) {
        //rowWidth = 0;
        colHeight = 0;
    } else if (nodeKind.endsWith("Definitions")) {
        colHeight = topY;
    }

    foreach (Node* child, getNode()->getChildren(0)) {

        NodeItem* nodeItem = getChildNodeItemFromNode(child);

        // check that it's a NodeItem and that it's visible
        if (nodeKind == "Model" || (nodeItem && nodeItem->isVisible() && !nodeItem->isLocked())) {

            // if child == DeploymentDefinitions and all of it's children are invisible, don't sort it
            if (nodeItem->getNodeKind() == "DeploymentDefinitions") {
                bool childrenAreInAspect = false;
                for(int i = 0; i < childNodeItems.size(); i++){
                    NodeItem *nodeItm = childNodeItems[i];
                    if (nodeItm && nodeItm->isVisible()) {
                        childrenAreInAspect = true;
                        break;
                    }
                }
                if (!childrenAreInAspect) {
                    break;
                }
            }

            int childWidth = nodeItem->boundingRect().width();
            int childHeight = nodeItem->boundingRect().height();

            // if the origWidth is not used, when a node is sorted and it
            // only had one child to begin with, it will always only have
            // one node per row and hence one column, once sorted
            // this allows there to be at most 2 child nodes per row

            if ((rowWidth + childWidth) > (minimumWidth*1.5)) {
                colHeight += maxHeight + gapY;

                if (rowWidth > maxWidth) {
                    maxWidth = rowWidth - gapX;
                }

                maxHeight = childHeight;

                if (permanentlyHiddenContainer) {
                    rowWidth = 0;
                } else {
                    rowWidth = gapX;
                }
            }

            // store the maximum height for each row
            if (childHeight > maxHeight) {
                maxHeight = childHeight;
            }

            if (componentLayout) {

                QString nodeKind = nodeItem->getGraphML()->getDataValue("kind");
                float newX = 0;
                float newY = 0;

                if (nodeKind.startsWith("InEvent")) {
                    newY = inCol;
                    newX = 0 - (childWidth/2);
                    inCol += childHeight + gapY;
                } else if (nodeKind.startsWith("OutEvent")) {
                    newY = outCol;
                    newX = width - (childWidth/2);
                    outCol += childHeight + gapY;
                } else {
                    newX = (width/2) - (childWidth/2);
                    newY = attCol;
                    attCol += childHeight + gapY;
                }

                GraphMLItem_SetGraphMLData(nodeItem->getGraphML(), "x", QString::number(newX));
                GraphMLItem_SetGraphMLData(nodeItem->getGraphML(), "y", QString::number(newY));

            } else {
                GraphMLItem_SetGraphMLData(nodeItem->getGraphML(), "x", QString::number(rowWidth));
                GraphMLItem_SetGraphMLData(nodeItem->getGraphML(), "y", QString::number(colHeight));
                rowWidth += childWidth + gapX;
            }

            numberOfItems++;
        }
    }

    if (numberOfItems > 0) {

        if (componentLayout) {

            float max_height = inCol;
            if (outCol > max_height) {
                max_height = outCol;
            }
            if (attCol > max_height) {
                max_height = attCol;
            }

            if (max_height < minimumHeight) {
                GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(minimumHeight));
            } else {
                GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(max_height));
            }

        } else {

            if (rowWidth > maxWidth) {
                maxWidth = rowWidth - gapX;
            }

            if (permanentlyHiddenContainer) {
                GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(maxWidth));
                GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(colHeight + maxHeight));
                return;
            }

            if ((maxWidth + gapX) < minimumWidth) {
                GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(minimumWidth));
            } else {
                //GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(maxWidth + gapX));
                GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(maxWidth - gapX));
            }

            if ((colHeight + maxHeight + gapY) < minimumHeight) {
                GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(minimumHeight));
            } else {
                //GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(colHeight + maxHeight + gapY));
                GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(colHeight + maxHeight - gapY));
            }
        }
    }
}


/**
 * @brief NodeItem::newSort
 */
void NodeItem::newSort()
{
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
    int y = 1;


    NodeItem* nextItem = 0;
    for (y; y <= (gridSize * 3) - 1; y += 3) {
        x = 1;
        for (x; x <= (gridSize * 3) - 1; x += 3) {
            if (toSortItems.size() > 0) {
                nextItem = toSortItems.takeFirst();
                nextItem->setCenterPos(getGridPosition(x,y));
                nextItem->updateParentHeight(nextItem);
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
    if(!PAINT_OBJECT){
        return;
    }
    switch (event->button()) {
    case Qt::LeftButton:{
        if(labelPressed(event->pos())){
            if(getGraphML() && !getGraphML()->getData("label")->isProtected()){
                textItem->setEditable(true);
                textItem->forceMousePress(event);
            }
        }

        if(iconPressed(event->pos())){
            if(isExpanded()){
                expandItem(false);
            }else{
                expandItem(true);
            }
        }
        break;
    }
    default:{

    }
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
        if(hasSelectionMoved == false){
            emit GraphMLItem_TriggerAction("Moving Selection");
            if(parentNodeItem){
                parentNodeItem->setDrawGrid(true);
            }
            hasSelectionMoved = true;
        }
        QPointF delta = (event->scenePos() - previousScenePosition);
        previousScenePosition = event->scenePos();

        NodeItem_MoveSelection(delta);
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


void NodeItem::setWidth(qreal width)
{
    if(this->width != width){

        bool updateModel = false;
        if(width < minimumWidth){
            width = minimumWidth;
            updateModel = true;
        }

        this->width = width;

        updateTextLabel();
        updateChildrenOnChange();

        if(updateModel){
            updateGraphMLSize();
        }

        updateGridLines(true, true);
    }

}


void NodeItem::setHeight(qreal height)
{
    if(this->height != height){
        bool updateModel = false;

        if(height < minimumHeight){
            height = minimumHeight;
            updateModel = true;
        }

        this->height = height;
        updateChildrenOnChange();

        if(updateModel){
            updateGraphMLSize();
        }

        updateGridLines(true, true);
    }
}


void NodeItem::updateGridLines(bool updateX, bool updateY)
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


void NodeItem::updateTextLabel(QString text)
{
    if(!textItem){
        return;
    }

    if(text == ""){
        text = getGraphML()->getDataValue("label");
    }

    QString remainingText = text;
    QString line1 = text;
    QString line2 = "";

    bool showSecondLine = isSelected() || (isExpanded() && childNodeItems.size() > 0);

    qreal margin = 0;
    if(width != minimumWidth){
        margin = getItemMargin()/2;
    }
    textItem->setTextWidth( width - margin);

    if(!isExpanded()){
        if(remainingText.length() > LABEL_LINE_LENGTH){
            line1.truncate(LABEL_LINE_LENGTH);

            if(!showSecondLine){
                line1.truncate(LABEL_LINE_LENGTH -2);
                line1 = line1 + "..";
            }
        }
        remainingText.remove(0, LABEL_LINE_LENGTH);

        if(remainingText.length() > 0){
            if(showSecondLine){
                line2 = remainingText;
                line2.truncate(LABEL_LINE_LENGTH);
            }
        }
        textItem->setHtml("<left>" + line1 + "<br / >"+ line2+ "</left>");
    }else{
        //textItem->setTextWidth(LABEL_RATIO * width);
        textItem->setHtml("<left>" + line1 + "<br / >"+ line2+ "</left>");
    }




    //qreal xPos = ((width + getItemMargin()) - LABEL_RATIO * width) /2;
    qreal xPos = (boundingRect().width() - textItem->boundingRect().width()) /2;
    textItem->setPos(xPos, minimumHeight);
    textItem->update();

}


void NodeItem::setupAspect()
{
    Node* node = getNode();

    while(node){
        QString nodeKind = node->getDataValue("kind");
        if(nodeKind == "ManagementComponent"){
            viewAspects.append("Hardware");
            viewAspects.append("Assembly");
        }
        else if(nodeKind == "HardwareDefinitions"){
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
        color = QColor(0,50,100);
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


    if(nodeKind.endsWith("Definitions")){
        selectedColor = color;
        //color.setAlpha(50);
        //selectedColor.setAlpha(150);
    }else{
        selectedColor = color;
        //color.setAlpha(200);
        //selectedColor.setAlpha(250);

    }

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
        //Get the initial Width
        QGraphicsItem::setPos(pos);
        updateChildrenOnChange();
        updateGraphMLPosition();
    }
}


void NodeItem::aspectsChanged(QStringList aspects)
{
    if(hidden || !PAINT_OBJECT || permanentlyInvisible){
        return;
    }
    if(this->getParentNodeItem() && !getParentNodeItem()->isExpanded()){
        return;
    }

    bool allMatched = true;
    foreach(QString aspect, viewAspects){
        if(!aspects.contains(aspect)){
            allMatched = false;
            break;
        }
    }

    bool prevVisible = isVisible();

    setVisible(allMatched && (viewAspects.size() > 0));

    // if the view aspects have been changed, update pos of edges
    //if(prevVisible != isVisible()){
    //    emit update
    //    emit updateEdgePosition();
    //}

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
            lockIcon->setPos(brushSize, (iconHeight/2) - (lockHeight/2));
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
    connect(textItem, SIGNAL(textUpdated(QString)),this, SLOT(setNewLabel(QString)));
    //textItem->setTextInteractionFlags(Qt::NoTextInteraction);

    textItem->setToolTip("Double Click to Edit Label");
    textItem->setTextWidth(LABEL_RATIO * minimumWidth);
    textItem->setPos(0, height);
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


void NodeItem::setNewLabel(QString newLabel)
{
    if(newLabel == ""){
        if(getGraphML() && textItem){
            GraphMLData* labelData = getGraphML()->getData("label");
            if(labelData && !labelData->isProtected()){
                QString currentLabel = labelData->getValue();
                QColor oldColor = this->textItem->defaultTextColor();

                textItem->setDefaultTextColor(QColor(0,0,255));

                bool ok;
                newLabel = QInputDialog::getText(0, "Set New Label", "Value:", QLineEdit::Normal, currentLabel, &ok);

                textItem->setDefaultTextColor(oldColor);
                if(!ok){
                    return;
                }

            }
        }
    }
    GraphMLItem_TriggerAction("Set New Label");
    GraphMLItem_SetGraphMLData(getGraphML(), "label", newLabel);

}


void NodeItem::toggleGridLines(bool on)
{
    GRIDLINES_VISIBLE = on;
    if(on){
        updateGridLines(true,true);
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

    setCenterPos(QPointF(graphmlX, graphmlY));

    if(graphmlWidth != 0 && graphmlHeight != 0){
        setWidth(graphmlWidth);
        setHeight(graphmlHeight);
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
    return minimumVisibleRect().width() / GRID_RATIO;// -1);
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
 * @brief NodeItem::getCurvedCornerWidth
 * @return
 */
double NodeItem::getCornerRadius()
{
    return initialWidth / (2 * MINIMUM_HEIGHT_RATIO);
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
    for(int i = 0 ; i < childNodeItems.size() ; i++){
        NodeItem* nodeItem = childNodeItems[i];
        if (nodeItem) {
            nodeItem->setVisible(show);
        }
    }

    GraphML* modelEntity = getGraphML();
    GraphMLData* wData = 0;
    GraphMLData* hData = 0;

    if (modelEntity) {
        wData = modelEntity->getData("width");
        hData = modelEntity->getData("height");
    }

    if (show) {
        if (wData) {
            wData->setValue(QString::number(prevWidth));
        }
        if (hData) {
            hData->setValue(QString::number(prevHeight));
        }

    } else {

        prevWidth = width;
        prevHeight = height;

        if (wData) {
            wData->setValue(QString::number(minimumWidth));
        }
        if (hData) {
            hData->setValue(QString::number(minimumHeight));
        }
    }

    expanded = show;

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
    qreal gridBottomY = gridRect().height() + gridRect().top();
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
            emit updateParentHeight(this);
        }
    }
    //}
}


void NodeItem::updateModelPosition()
{
    GraphMLItem_SetGraphMLData(getGraphML(), "x", QString::number(centerPos().x()));
    GraphMLItem_SetGraphMLData(getGraphML(), "y", QString::number(centerPos().y()));
    onGrid = false;
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
    hidden  = h;
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
        if(initialHeight != height || initialWidth != width){
            //emit triggerAction("Resetting Size!");
        }
        if(initialHeight != height){
            GraphMLItem_SetGraphMLData(this->getGraphML(), "height", QString::number(initialHeight));
        }
        if(initialWidth != width){
            GraphMLItem_SetGraphMLData(this->getGraphML(), "width", QString::number(initialWidth));
        }
    }
}


/**
 * @brief NodeItem::isExpanded
 */
bool NodeItem::isExpanded()
{
    return expanded ;
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
