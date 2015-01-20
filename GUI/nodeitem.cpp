#include "nodeitem.h"
#include "nodeedge.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QStyleOptionGraphicsItem>
#include <QRubberBand>
#include <QPixmap>
#include <QBitmap>

#define MODEL_WIDTH 19200
#define MODEL_HEIGHT 10800
#define COLUMN_COUNT 2
#define MINIMUM_HEIGHT_RATIO 7
#define LABEL_LENGTH 16


NodeItem::NodeItem(Node *node, NodeItem *parent, QStringList aspects):  GraphMLItem(node)
{
    Q_INIT_RESOURCE(resources);
    setParentItem(parent);

    nodeSelected = false;
    isNodePressed = false;
    hidden = false;
    expanded = false;
    hasSelectionMoved = false;

    icon = 0;
    proxyWidget = 0;
    expandButton = 0;
    label  = 0;

    nodeKind = "";


    QString parentNodeKind = "";
    if (parent) {
        setWidth(parent->getChildWidth());

        if (getGraphML()->getDataValue("kind").contains("Definitions")) {
            setHeight(width);
            expanded = true;
        } else {
            setHeight(width/7);
        }

        parentNodeKind = parent->getGraphML()->getDataValue("kind");

        // connect this item to its parent item
        connect(this, SIGNAL(addExpandButtonToParent()), parent, SLOT(addExpandButton()));
        connect(this, SIGNAL(updateParentHeight(NodeItem*)), parent, SLOT(updateHeight(NodeItem*)));

    } else {
        setWidth(MODEL_WIDTH);
        setHeight(MODEL_HEIGHT);
    }

    //Make
    if (parentNodeKind== "Component" || parentNodeKind== "ComponentInstance") {
        setWidth(width/2);
        setHeight(height/2);
    }

    initialWidth = width;
    initialHeight = height;

    prevWidth = width;
    prevHeight = height;

    minimumHeight = initialWidth / MINIMUM_HEIGHT_RATIO;
    minimumWidth = initialWidth;

    //Update Width and Height with values from the GraphML Model If they have them.
    retrieveGraphMLData();

    //Update GraphML Model for size/position if they have been changed.
    updateGraphMLSize();
    updateGraphMLPosition();

    setupGraphMLConnections();

    setupAspect();
    setupBrushes();
    setupLabel();
    setupIcon();

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    //setFlag(ItemIsSelectable);
    //setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);

    setCacheMode(QGraphicsItem::NoCache);

    // if this item has a parent and it's the first child of that parent
    // send a signal to the parent to add an expandButton and sort it
    if (parent && parent->getNumberOfChildren() == 1) {
        emit addExpandButtonToParent();
    }

    if(getGraphML()->getDataValue("kind") == "Model" || getGraphML()->getDataValue("kind") == "DeploymentDefinitions"){
        setPaintObject(false);
    }else{
        setPaintObject(true);
    }

    updateViewAspects(aspects);
    resetNextChildPos();
    emit updateParentHeight(this);
}


/**
 * @brief NodeItem::~NodeItem
 * Before deleting this item, check to see if it has a parent item.
 * If it does and this is the only child of that parent remove that
 * parent's exapnd button and reset its size.
 */


NodeItem::~NodeItem()
{
    if (parentItem()) {
        // if this item is its parent's last child,
        // remove the parent's expand button and
        // reset its size and nextChildPosition
        NodeItem* item = dynamic_cast<NodeItem*>(parentItem());
        if (item && item->getNumberOfChildren() == 1) {
            item->removeExpandButton();
            item->resetNextChildPos();
            item->resetSize();

            // update parts dock container if parent kind
            // is File and it no longer has children
            if (item->getGraphML()->getDataValue("kind") == "File") {
                emit updateDockContainer("Parts");
            }
        }
    }
    delete label;
}


QRectF NodeItem::boundingRect() const
{
    int brushSize = selectedPen.width();
    if(nodeKind == "Model"){
        brushSize = 0;
    }
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}

bool NodeItem::isSelected()
{
    return nodeSelected;
}

bool NodeItem::isPainted()
{
    return PAINT_OBJECT;
}

bool NodeItem::intersectsRectangle(QRectF sceneRect)
{
    QRectF itemRectangle = boundingRect();
    itemRectangle.moveTo(scenePos());
    return sceneRect.contains(itemRectangle);
}



void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if(PAINT_OBJECT){
        QRectF rectangle(0, 0, width, height);


        float headerWidth = width;
        float headerHeight = minimumHeight;

        QPen Pen;
        QBrush Brush;


        if(nodeSelected){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        QBrush HeaderBrush = Brush;
        QColor HeaderBrushColor = HeaderBrush.color();

        HeaderBrushColor.setAlpha(255);
        HeaderBrush.setColor(HeaderBrushColor);

        Node* node = getNode();
        if(node->isInstance() || node->isImpl()){
            if(!node->getDefinition()){
                Brush.setStyle(Qt::DiagCrossPattern);
            }
        }

        qreal cornerRadius = getCornerRadius();

        painter->setPen(Pen);
        painter->setBrush(Brush);

        painter->drawRoundedRect(rectangle, cornerRadius, cornerRadius);

        //painter->setPen(QPen(Qt::red, 10));
        //painter->drawRoundedRect(rectangle, cornerRadius, cornerRadius);

        QPainterPath UpperSidePath;
        QPolygonF UpperPolygon;
        UpperPolygon << QPointF(0,0) << QPointF(headerWidth,0) << QPointF(headerWidth,headerHeight) << QPointF(0,headerHeight);
        UpperSidePath.addPolygon(UpperPolygon);

        painter->setBrush(HeaderBrush);
        painter->setClipping(true);
        painter->setClipPath(UpperSidePath);

        painter->drawRoundedRect(rectangle, cornerRadius, cornerRadius);

        if(expanded){
            if(headerHeight + selectedPen.width()  < height){
                QPen linePen = selectedPen;
                linePen.setStyle(Qt::DotLine);
                linePen.setColor(QColor(100,100,100));
                painter->setPen(linePen);
                painter->drawLine(QPointF(0,headerHeight), QPointF(width, headerHeight));
            }
        }

    }

}

double NodeItem::getWidth()
{
    return width;

}

double NodeItem::getHeight()
{
    return height;
}


void NodeItem::addNodeEdge(NodeEdge *line)
{
    NodeItem* item = this;
    while(item){
        //Connect the UpdateEdge of all parents of this, to the edge.
        connect(item, SIGNAL(updateEdgePosition()), line, SLOT(updateEdge()));
        //Connect the Visibility of the edges of this node, so that if this node's parent was to be set invisbile, we any edges would be invisible.
        connect(item, SIGNAL(setEdgeVisibility(bool)), line, SLOT(setVisible(bool)));
        item = dynamic_cast<NodeItem*>(item->parentItem());
    }
    connect(this, SIGNAL(setEdgeSelected(bool)), line, SLOT(setSelected(bool)));

    connections.append(line);
}

void NodeItem::removeNodeEdge(NodeEdge *line)
{
    int position = connections.indexOf(line);
    connections.remove(position);
}


double NodeItem::getChildWidth()
{
    return initialWidth / COLUMN_COUNT + 1;
}


/**
 * @brief NodeItem::getNextChildPos
 * @return
 */
QPointF NodeItem::getNextChildPos()
{
    QPointF nextPos = nextChildPosition;

    // update nexChildPosition to the next available position
    nextChildPosition.setX(nextChildPosition.x() + (getChildWidth()*0.05));
    nextChildPosition.setY(nextChildPosition.y() + (getChildWidth()/14));

    // reset nextChildPosition.x if child is going to be out of bounds
    if ((nextChildPosition.x() + getChildWidth() + (getCornerRadius()/2)) > width) {
        nextChildPosition.setX(getCornerRadius()/2);
    }

    return nextPos;
}

/**
 * @brief NodeItem::resetNextChildPos
 */
void NodeItem::resetNextChildPos()
{
    nextChildPosition = QPointF(getCornerRadius()/2, minimumHeight);
}

/*
int NodeItem::getDepth()
{
    return depth;
}
*/

void NodeItem::setOpacity(qreal opacity)
{
    QGraphicsItem::setOpacity(opacity);
    emit updateOpacity(opacity);

    foreach(NodeEdge* edge, connections){
        if(edge->getSource()->opacity() != 0 && edge->getDestination()->opacity() != 0){
            edge->setOpacity(opacity);
        }else{
            edge->setOpacity(0);
        }
    }
    this->update();
}


void NodeItem::setSelected(bool selected)
{
    if(nodeSelected != selected){
        nodeSelected = selected;

        update();
        emit setEdgeSelected(selected);

        // update corresponding dock node item
        emit updateDockNodeItem(selected);
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


void NodeItem::graphMLDataUpdated(GraphMLData* data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "x" || dataKey == "y"){
            //Update the Position
            QPointF newPosition = pos();

            if(dataKey == "x"){
                newPosition.setX(dataValue.toFloat());
            }else if(dataKey == "y"){
                newPosition.setY(dataValue.toFloat());
            }
            setPos(newPosition);

        }else if(dataKey == "width" || dataKey == "height"){
            //Update the Size
            if(dataKey == "width"){
                setWidth(dataValue.toFloat());
            }else if(dataKey == "height"){
                setHeight(dataValue.toFloat());
            }
            prepareGeometryChange();
            update();
        }else if(dataKey == "label"){
            if(dataValue != ""){
                updateTextLabel(dataValue);
            }

            // update connected dock node item
            emit updateDockNodeItem();
        }
    }
}


/**
 * @brief NodeItem::updateViewAspects
 * This method checks to see if this item should be visible or not.
 * @param aspects
 */
void NodeItem::updateViewAspects(QStringList aspects)
{
    if(hidden || !PAINT_OBJECT){
        return;
    }

    bool allMatched = true;
    foreach(QString aspect, viewAspects){
        if(!aspects.contains(aspect)){
            allMatched = false;
            break;
        }
    }

    setVisible(allMatched && (viewAspects.size() > 0));

    // if not visible, unselect node item
    if (!isVisible()) {
        setSelected(false);
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
    emit triggerAction("Sorting Children");

    float topY;
    float gapY;

    // if it's a main node item, it will have no icon
    if (!PAINT_OBJECT || nodeKind.endsWith("Definitions")) {
        topY = label->x() + label->boundingRect().height()  + getCornerRadius();
        gapY = topY;
    } else {
        if(icon){
            topY = (icon->boundingRect().height()*icon->scale()) + getCornerRadius();
        }
        gapY = topY/1.8;
    }

    //float gapY = topY;
    float gapX = gapY;

    float inCol = topY;
    float outCol = topY;
    float attCol = topY;

    float rowWidth = gapX;
    float colHeight = topY;

    float maxHeight = 0;
    float maxWidth = 0;

    int numberOfItems = 0;

    //bool componentLayout = (nodeKind == "Component" || nodeKind == "ComponentInstance");
    bool componentLayout = (nodeKind.contains("Component"));
    bool fileContainsComponents = (nodeKind == "File" && getChildrenKind().contains("Component"));
    bool componentAssembly = (nodeKind == "ComponentAssembly");
    bool componentHasChildren = false;

    // if this item is a File and contains Components for its children,
    // check if any of them has children and increase gapX accordingly
    if (fileContainsComponents || componentAssembly) {
        foreach (QGraphicsItem* child, this->childItems()) {
            NodeItem* nodeItem = dynamic_cast<NodeItem*>(child);
            if (nodeItem && (nodeItem->getNumberOfChildren() > 0)) {
                componentHasChildren = true;
                break;
            }
        }
    }

    // if the node item's children are Components or ComponentInstances
    // leave more gap for the in/out event ports along its edges
    if ((fileContainsComponents || componentAssembly) && componentHasChildren) {
        //gapY = topY;  // testing with this value to sort an imported file
        gapY *= 1.25;
        gapX = gapY;
        rowWidth = gapX;
    }

    foreach (QGraphicsItem* child, this->childItems()) {

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(child);

        // check that it's a NodeItem and that it's visible
        if (nodeItem != 0 && nodeItem->isVisible()) {

            int childWidth = nodeItem->boundingRect().width();
            int childHeight = nodeItem->boundingRect().height();

            // if the origWidth is not used, when a node is sorted and it
            // only had one child to begin with, it will always only have
            // one node per row and hence one column, once sorted
            // this allows there to be at most 2 child nodes per row

            if ((rowWidth + childWidth) > (initialWidth*1.5)) {
                colHeight += maxHeight + gapY;

                if (rowWidth > maxWidth) {
                    maxWidth = rowWidth - gapX;
                }

                maxHeight = childHeight;
                rowWidth = gapX;
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
                } else { //if (nodeKind.startsWith("Attribute")) {
                    newX = (width/2) - (childWidth/2);
                    newY = attCol;
                    attCol += childHeight + gapY;

                }

                emit updateGraphMLData(nodeItem->getGraphML(),"x", QString::number(newX));
                emit updateGraphMLData(nodeItem->getGraphML(),"y", QString::number(newY));

            } else {

                emit updateGraphMLData(nodeItem->getGraphML(),"x", QString::number(rowWidth));
                emit updateGraphMLData(nodeItem->getGraphML(),"y", QString::number(colHeight));
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
            emit updateGraphMLData(getGraphML(),"height", QString::number(max_height));
        } else {
            if (maxWidth == 0) {
                maxWidth = rowWidth - gapX;
            }
            if ((maxWidth+gapX) > initialWidth) {
                emit updateGraphMLData(getGraphML(), "width", QString::number(maxWidth + gapX));
            } else {
                emit updateGraphMLData(getGraphML(), "width", QString::number(initialWidth));
            }
            emit updateGraphMLData(getGraphML(), "height", QString::number(colHeight + maxHeight + gapY));
        }
    }

    resetNextChildPos();
}



void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {

    case Qt::MiddleButton:{
        if (event->modifiers().testFlag(Qt::ControlModifier)) {
            if (!PAINT_OBJECT) {
                emit sortModel();
            } else {
                sort();
            }
        } else {
            if (PAINT_OBJECT) {
                emit triggerCentered(getGraphML());
            } else {
                emit centerViewAspects();
            }
        }
        break;
    }
    case Qt::LeftButton:{

        // unselect any selected node item
        // when the model is pressed
        if(!PAINT_OBJECT){
            emit clearSelection();
            //event->setAccepted(false);
            return;
        }

        //Left and Right buttons should target this line.
        previousScenePosition = event->scenePos();

        hasSelectionMoved = false;
        isNodePressed = true;

        emit triggerSelected(getGraphML());

        break;
    }
    case Qt::RightButton:{
        if(!PAINT_OBJECT){
            event->setAccepted(false);
            return;
        }
        //Select this node, and construct a child node.
        emit triggerSelected(getGraphML());
        break;

    }

    default:
        break;
    }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(!PAINT_OBJECT){
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:{

        hasSelectionMoved = false;
        isNodePressed = false;
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
            emit triggerAction("Moving Selection");
            hasSelectionMoved = true;
        }
        QPointF delta = (event->scenePos() - previousScenePosition);
        previousScenePosition = event->scenePos();

        emit moveSelection(delta);
    }
}

void NodeItem::setWidth(qreal width)
{
    this->width = width;
    updateExpandButton();
    updateTextLabel();
}

void NodeItem::setHeight(qreal height)
{
    this->height = height;

}


void NodeItem::setPaintObject(bool paint)
{
    PAINT_OBJECT = paint;

    if(icon){
        icon->setVisible(paint);
    }
    if(label){
        label->setVisible(paint);
    }
}

void NodeItem::updateTextLabel(QString text)
{
    if(label){
        if(text == 0){
            if(getGraphML()){
                text = getGraphML()->getDataValue("label");
            }
        }
        label->setPlainText(text);

        int brushSize = selectedPen.width();
        qreal availableWidth = width - (getCornerRadius()/2 + minimumHeight + brushSize);
        if(icon){
            availableWidth -= (getCornerRadius()/2 + minimumHeight + brushSize);
        }

        qreal labelWidth = label->boundingRect().width();
        if(labelWidth > availableWidth){
            qreal ratio = availableWidth / labelWidth;
            qreal fitChars = (text.size() * ratio) - 2;
            text.truncate(fitChars);
            label->setPlainText(text + "..");
        }
    }


}

void NodeItem::updateExpandButton()
{
    if(expandButton){
        int brushSize = selectedPen.width();
        QPoint currentPos = expandButton->pos();
        qreal buttonSize = expandButton->width();

        //New Position
        QPoint newPos(width - (getCornerRadius()/2) - buttonSize - brushSize, (minimumHeight - buttonSize)/2);

        expandButton->move(newPos);
    }
}

void NodeItem::setupAspect()
{
    Node* node = getNode();

    while(node){
        QString nodeKind = node->getDataValue("kind");
        if(nodeKind == "ManagementComponent"){
            if(!viewAspects.contains("Hardware")){
                viewAspects.append("Hardware");
            }
            if(!viewAspects.contains("Assembly")){
                viewAspects.append("Assembly");
            }
        }
        else if(nodeKind == "HardwareDefinitions"){
            if(!viewAspects.contains("Hardware")){
                viewAspects.append("Hardware");
            }
        }else if(nodeKind == "AssemblyDefinitions"){
            if(!viewAspects.contains("Assembly")){
                viewAspects.append("Assembly");
            }
        }else if(nodeKind == "BehaviourDefinitions"){
            if(!viewAspects.contains("Workload")){
                viewAspects.append("Workload");
            }
        }else if(nodeKind == "InterfaceDefinitions"){
            if(!viewAspects.contains("Definitions")){
                viewAspects.append("Definitions");
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

    else if(nodeKind== "BehaviourDefinitions"){
        color = QColor(250,250,250);
    }
    else if(nodeKind== "InterfaceDefinitions"){
        color = QColor(250,250,250);
    }
    else if(nodeKind== "HardwareDefinitions"){
        color = QColor(250,250,250);
    }
    else if(nodeKind== "AssemblyDefinitions"){
        color = QColor(250,250,250);
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
        color.setAlpha(50);
        selectedColor.setAlpha(150);
    }else{
        selectedColor = color;
        color.setAlpha(200);
        selectedColor.setAlpha(250);

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
        QGraphicsItem::setPos(pos);
        emit updateEdgePosition();
    }
}



/**
 * @brief NodeItem::setLabelFont
 * This sets up the font and size of the label.
 * It also stores a fixed, default size for its children.
 */
void NodeItem::setupLabel()
{
    QFont font("Arial");
    font.setPointSize(.25 * minimumHeight);

    label = new QGraphicsTextItem(this);
    //label->adjustSize();
    //label->setTextWidth(availableWidth);
    label->setFont(font);

    int brushSize = selectedPen.width();
    label->setPos(getCornerRadius()/2 + brushSize , (minimumHeight - label->boundingRect().height())/2);
    updateTextLabel(getGraphML()->getDataValue("label"));
    //label->setPlainText(getGraphML()->getDataValue("label"));

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
            connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
        }

        if(yData){
            connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
        }

        if(hData){
            connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
        }

        if(wData){
            connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
        }

        if(labelData){
            connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
        }

        if(kindData){
            connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
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
        xData->setValue(QString::number(pos().x()));
        yData->setValue(QString::number(pos().y()));
    }
}

void NodeItem::retrieveGraphMLData()
{
    double graphmlHeight = getGraphML()->getDataValue("height").toDouble();
    double graphmlWidth = getGraphML()->getDataValue("width").toDouble();

    //Update the position with values from the GraphML Model if they have them.
    double graphmlX = getGraphML()->getDataValue("x").toDouble();
    double graphmlY = getGraphML()->getDataValue("y").toDouble();
    nodeKind = getGraphML()->getDataValue("kind");

    setPos(graphmlX, graphmlY);

    if(graphmlWidth != 0 && graphmlHeight != 0){
        setWidth(graphmlWidth);
        setHeight(graphmlHeight);
    }
}


/**
 * @brief NodeItem::setupIcon
 * This sets up the scale and tranformation of this item's icon
 * if it has one. It also sets the pos for this item's label.
 */
void NodeItem::setupIcon()
{
    QString nodeKind = getGraphML()->getDataValue("kind");

    QImage image( ":/Resources/Icons/" + nodeKind + ".png");
    if (!image.isNull()) {
        icon = new QGraphicsPixmapItem(QPixmap::fromImage(image), this);
    }

    if(icon != 0){
        qreal iconHeight = icon->boundingRect().height();
        qreal iconWidth = icon->boundingRect().width();
        qreal scaleFactor = (minimumHeight / iconHeight);
        iconWidth *= scaleFactor;

        icon->setScale(scaleFactor);

        int brushSize = selectedPen.width();

        icon->setPos((getCornerRadius()/2 + brushSize) ,0);

        icon->setTransformationMode(Qt::SmoothTransformation);

        label->setX(label->x() + iconWidth);
    }
}


/**
 * @brief NodeItem::getNumberOfChildren
 * This returns the number of NodeItem children this item has.
 * @return
 */
int NodeItem::getNumberOfChildren()
{
    int childrenCount = 0;
    foreach (QGraphicsItem *itm, childItems()) {
        NodeItem *nodeItm = dynamic_cast<NodeItem*>(itm);
        if (nodeItm) {
            childrenCount++;
        }
    }
    return childrenCount;
}


/**
 * @brief NodeItem::getChildKind
 * This returns the kinds of all this item's children.
 * @return
 */
QStringList NodeItem::getChildrenKind()
{
    QStringList returnable;
    Node *node = dynamic_cast<Node*>(getGraphML());
    if (node) {
        foreach(Node* child, node->getChildren(0)){
            returnable += child->getDataValue("kind");
        }
    }
    return returnable;
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

/**
 * @brief NodeItem::addExpandButton
 */
void NodeItem::addExpandButton()
{
    if (icon != 0) {
        if (nodeKind!= "Hardware" && nodeKind != "ManagementComponent") {

            QFont font("Arial");
            qreal buttonSize = .7 * minimumHeight;
            font.setPointSize(buttonSize/2);

            expandButton = new QPushButton("-");
            expandButton->setFont(font);
            expandButton->setFixedSize(buttonSize, buttonSize);

            //QString testString = QString("This is a test %1, T.txt").arg(expandButton->width());
            QString penWidth = QString::number(pen.width());
            QString borderRadius = QString::number(expandButton->width()/2);
            expandButton->setStyleSheet("QPushButton {"
                                        "background-color: rgba(255,255,255,250);"
                                        "border:" + penWidth + "px solid gray;"
                                        "border-radius:" + borderRadius + "px;"
                                        "padding: 0px;"
                                        "margin: 0px;"
                                        "}");

            /*
            // this makes the corners of the expand button
            // and the proxy widget look rounded
            QPixmap pixmap(expandButton->size());
            QPainter painter(&pixmap);
            QBrush brush(Qt::white);
            painter.setBrush(brush);
            painter.drawRoundRect(pixmap.rect(), 60, 60);
            expandButton->setMask(pixmap.createMaskFromColor(Qt::white, Qt::MaskOutColor));
            */

            QRegion region(expandButton->rect(), QRegion::RegionType::Ellipse);
            expandButton->setMask(region);

            int brushSize = selectedPen.width();
            expandButton->move(width - (getCornerRadius()/2) - buttonSize - brushSize, (minimumHeight - buttonSize)/2);

            proxyWidget = new QGraphicsProxyWidget(this);
            proxyWidget->setWidget(expandButton);

            // setup and connect button
            expanded = true;
            expandButton->setCheckable(true);
            expandButton->setChecked(true);
            connect(expandButton, SIGNAL(clicked(bool)), this, SLOT(expandItem(bool)));
        }
    }
}

/**
 * @brief NodeItem::expandItem
 * @param show
 */
void NodeItem::expandItem(bool show)
{
    foreach (QGraphicsItem* child, this->childItems()) {
        NodeItem* nodeItem = dynamic_cast<NodeItem*>(child);
        if (nodeItem) {
            nodeItem->setVisible(show);
        }
    }

    GraphML* modelEntity = getGraphML();
    GraphMLData* wData = 0;
    GraphMLData* hData = 0;

    if(modelEntity){
        wData = modelEntity->getData("width");
        hData = modelEntity->getData("height");
    }

    if (show) {
        expandButton->setText("-");
        if(wData){
            wData->setValue(QString::number(prevWidth));
        }
        if(hData){
            hData->setValue(QString::number(prevHeight));

        }
    } else {
        expandButton->setText("+");
        prevWidth = width;
        prevHeight = height;

        if(wData){
            wData->setValue(QString::number(initialWidth));
        }
        if(hData){
            hData->setValue(QString::number(initialHeight));

        }
    }

    expanded = show;
    prepareGeometryChange();
    update();
}


/**
 * @brief NodeItem::updateHeight
 * Expand this item's height to fit the newly added child.
 * This only gets called by painted and non-definition items.
 * @param child
 */
void NodeItem::updateHeight(NodeItem *child)
{
    if (PAINT_OBJECT && !nodeKind.endsWith("Definitions")) {

        double diffHeight = (child->pos().y() + child->getHeight()) - height;

        if (diffHeight > 0) {

            setHeight(height + diffHeight + selectedPen.width());
            emit updateGraphMLData(getGraphML(), "height", QString::number(height));

            // recurse while there is a parent node item
            NodeItem* parentNodeItem = dynamic_cast<NodeItem*>(parentItem());
            if (parentNodeItem) {
                emit updateParentHeight(this);
            }

        }
    }
}


Node *NodeItem::getNode()
{
    return dynamic_cast<Node*>(getGraphML());
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
 * Reset this node item's size to its default size
 * and sort its children if there are any.
 */
void NodeItem::resetSize()
{
    emit updateGraphMLData(getGraphML(), "height", QString::number(initialHeight));
    emit updateGraphMLData(getGraphML(), "width", QString::number(initialWidth));
}
/**
 * @brief NodeItem::isExpanded
 */
bool NodeItem::isExpanded()
{
    return expanded;
}


/**
 * @brief NodeItem::hasExpandButton
 * @return
 */
bool NodeItem::hasExpandButton()
{
    if (expandButton == 0) {
        return false;
    } else {
        return true;
    }
}


/**
 * @brief NodeItem::hideExpandButton
 */
void NodeItem::removeExpandButton()
{
    if (expandButton) {
        delete expandButton;
        expandButton = 0;
    }
}


