#include "nodeitem.h"
#include "nodeedge.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QStyleOptionGraphicsItem>
#include <QRubberBand>

#define MODEL_WIDTH 19200
#define MODEL_HEIGHT 10800
#define COLUMN_COUNT 2


NodeItem::NodeItem(Node *node, NodeItem *parent):  GraphMLItem(node)
{
    Q_INIT_RESOURCE(resources);


    USING_RUBBERBAND_SELECTION = false;

    isSelected = false;
    hasSelectionMoved = false;

    rubberBand = new QRubberBand(QRubberBand::Rectangle,0);
    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);

    this->isNodePressed = false;

    setParentItem(parent);

    label  = new QGraphicsTextItem(this);
    icon = 0;



    proxyWidget = 0;
    expandButton = 0;
    hidden = false;
    expanded = false;

    icon = 0;
    parentNodeKind= "";

    setHidden(false);
    expanded  = false;
    setHideChildren(false);


    // initially set width and height ot be the default size
    if (!parent) {
        depth = 1;
        width = MODEL_WIDTH;
        height = MODEL_HEIGHT;
    } else {
        parentNodeKind= parent->getGraphML()->getDataValue("kind");
        depth = parent->depth + 1;

        width = parent->getChildSize();
        height = parent->getChildSize();
        //width = dynamic_cast<NodeItem*>(parentItem())->width/3;
        //height = width / 7;
        //height /= 7;

        depth = parent->getDepth() + 1;
        connect(this, SIGNAL(addExpandButtonToParent()), parent, SLOT(addExpandButton()));
    }

    if (parentNodeKind== "Component" || parentNodeKind== "ComponentInstance") {
        width /= 2;
        height /= 2;
    }

    // store original/default width and height

    initialWidth = width;
    initialHeight = height;
    prevWidth = width;
    prevHeight = height;



    GraphMLData* xData = node->getData("x");
    GraphMLData* yData = node->getData("y");
    GraphMLData* kindData = node->getData("kind");
    GraphMLData* labelData = node->getData("label");

    GraphMLData* hData = node->getData("height");
    GraphMLData* wData = node->getData("width");

    if(xData)
        connect(xData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(yData)
        connect(yData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(hData)
        connect(hData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(wData)
        connect(wData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(labelData)
        connect(labelData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));

    if(kindData){
        nodeKind= kindData->getValue();
        connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));
    }



    if(hData && hData->getValue() != ""){
        qCritical() << getNode()->toString() << "Getting height value From Model.";
        height = hData->getValue().toFloat();
    }else{
        hData->setValue(QString::number(height));
    }

    if(wData && wData->getValue() != ""){
        qCritical() << getNode()->toString() << "Getting Width value From Model.";
        qCritical() << width;
        width = wData->getValue().toFloat();
        qCritical() << width;

    }else{
        wData->setValue(QString::number(width));
    }


    if(kindData){
        QImage image( ":/Resources/Icons/" + kindData->getValue() + ".png");
        if (!image.isNull()) {
            icon = new QGraphicsPixmapItem(QPixmap::fromImage(image), this);
        }
    }

    // bRec = QRectF(0,0,this->width, this->height);

    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    color = QColor(255,255,255,255);
    selectedColor = QColor(0,0,255,180);

    brush.setColor(color);
    selectedBrush.setColor(selectedColor);

    if(xData)
        graphMLDataUpdated(xData);
    if(yData)
        graphMLDataUpdated(yData);
    //if(wData)
    //    graphMLDataUpdated(wData);
    //if(hData)
    //    graphMLDataUpdated(hData);
    if(kindData)
        graphMLDataUpdated(kindData);
    if(labelData)
        graphMLDataUpdated(labelData);

    updateBrushes();
    if(kindData->getValue() == "Model" || kindData->getValue() == "DeploymentDefinitions"){
        setHidden(true);
        //setHideChildren(false);

        //drawObject = false;
        //drawDetail = false;
    }


    setCacheMode(QGraphicsItem::NoCache);
    //updateViewAspects(QStringList());


    // this will stop the width of this item and its children from
    // shrinking when it, or one of its child is resized.
    setLabelFont();
    setupIcon();

    if(hData) {
        hData->setValue(QString::number(height));
    }
    if(wData) {
        wData->setValue(QString::number(width));
    }


    setCacheMode(QGraphicsItem::NoCache);
    updateViewAspects(QStringList());

    // if this item has a parent and it's the first child of that parent
    // send a signal to the parent to add an expandButton and sort it
    if (parent && parent->getNumberOfChildren() == 1) {
        emit addExpandButtonToParent();
        //parent->sort();
    }
    //qDebug() << "width = " << width;
    //qDebug() << "origWidth = " << origWidth;

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
        NodeItem* item = dynamic_cast<NodeItem*>(parentItem());
        if (item && item->getNumberOfChildren() == 1) {
            item->removeExpandButton();
            item->resetSize();
        }
    }
    delete rubberBand;
    delete label;
}


QRectF NodeItem::boundingRect() const
{
    int brushSize = selectedPen.width();
    if(nodeKind== "Model"){
        brushSize = 0;
    }
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setClipRect( option->exposedRect );

    if(DRAW_OBJECT){

        QRectF rectangle(0, 0, width, height);
        //Get Height from Icon\

        float headerWidth = width;
        float headerHeight = label->boundingRect().height()*label->scale();
        if(icon){
            headerHeight = icon->boundingRect().height()*icon->scale();
            float gapY = headerHeight/1.8;
            headerHeight += gapY;

        }

        QPen Pen;
        QBrush Brush;

        if(isSelected){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        QBrush HeaderBrush;
        HeaderBrush = Brush;
        QColor HeaderBrushColor = HeaderBrush.color();
        HeaderBrushColor.setAlpha(255);
        HeaderBrushColor.setRed(255);
        HeaderBrush.setColor(HeaderBrushColor);

        Node* node = getNode();
        if(node->isInstance() || node->isImpl()){
            if(!node->getDefinition()){
                Brush.setStyle(Qt::DiagCrossPattern);
            }
        }

        qreal symetricRound = width > height ? width/15 : height/15;

        painter->setPen(Pen);
        painter->setBrush(Brush);


        painter->drawRoundedRect(rectangle, symetricRound, symetricRound);


        QPainterPath UpperSidePath;
        QPolygonF UpperPolygon;
        UpperPolygon << QPointF(0,0) << QPointF(headerWidth,0) << QPointF(headerWidth,headerHeight) << QPointF(0,headerHeight);
        UpperSidePath.addPolygon(UpperPolygon);


        painter->setBrush(HeaderBrush);
        painter->setClipping(true);
        painter->setClipPath(UpperSidePath);
        painter->drawRoundedRect(rectangle, symetricRound, symetricRound);



    }


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


float NodeItem::getChildSize()
{
    return initialWidth / COLUMN_COUNT + 1;
}

int NodeItem::getDepth()
{
    return depth;
}



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
    if(isSelected != selected){
        isSelected = selected;

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


void NodeItem::toggleDetailDepth(int level)
{

    if(level>=depth){
        setHideChildren(false);
    }else{
        if(level + 1 < depth){
            setHidden(true);
        }else{
            setHidden(false);
        }
        setHideChildren(true);
    }
    update();
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
                width = dataValue.toFloat();
            }else if(dataKey == "height"){
                height = dataValue.toFloat();
            }
            prepareGeometryChange();
            update();
        }else if(dataKey == "label"){

            Node* node = (Node*) this->getGraphML();

            dataValue = dataValue;// +  " [" + node->getID()+"]";

            if(dataValue != ""){
                label->setPlainText(dataValue);
            }

            // there has been a change to this item's graphml data
            // update connected dock node item
            //qDebug() << "NodeItem: graphMLDataUpdated";
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
    // check to see if this item is meant to be hidden
    if (hidden) {
        setVisible(false);
        return;
    }

    // only show Assembly when the Assembly view aspect is turned on
    if (nodeKind == "AssemblyDefinitions" && !aspects.contains("Assembly")) {
        setVisible(false);
        return;
    }

    // only show ManagementComponents and HardwareDefinitions
    // when the Hardware view aspect is turned on
    if ((nodeKind == "ManagementComponent" || nodeKind == "HardwareDefinitions")
            && !aspects.contains("Hardware")) {
        setVisible(false);
        return;
    }

    NodeItem* parentNodeItem = dynamic_cast<NodeItem*>(parentItem());

    // if this item has a parent and that parent is not expanded, hide this item
    if (parentNodeItem && parentNodeItem->hasExpandButton() && !parentNodeItem->isExpanded()) {
        setVisible(false);
        return;
    }

    bool isVisible = true;


    // if this item is in the currently viewed aspects, show it
    /*
    foreach (QString aspect, aspects) {
        if (getNode()->isInAspect(aspect)) {
            isVisible = true;
            break;
        }
    }
    */

    setVisible(isVisible);

    foreach(NodeEdge* edge, connections){
        edge->setVisible(isVisible);
    }

    //qDebug() << "NodeItem::updateViewAspects";
    //qDebug() << kind << ": " << this->isVisible();
}


/**
 * @brief NodeItem::sortChildren
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
    //qDebug() << "----------------------------";

    emit triggerAction("Sorting Children");

    float topY;
    float gapY;

    // if it's a main node item, it will have no icon
    if (nodeKind == "Model" || nodeKind.endsWith("Definitions")) {
        topY = label->x() + label->boundingRect().height()  + getCurvedCornerWidth();
        gapY = topY;
    } else {
        topY = (icon->boundingRect().height()*icon->scale()) + getCurvedCornerWidth();
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

    bool componentLayout = (nodeKind == "Component" || nodeKind == "ComponentInstance");
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
        gapY = topY;  // testing with this value to sort an imported file
        //gapY *= 1.25;
        gapX = gapY;
        rowWidth = gapX;
    }

    foreach (QGraphicsItem* child, this->childItems()) {

        NodeItem* nodeItem = dynamic_cast<NodeItem*>(child);

        // check that it's a NodeItem and that it's visible
        if (nodeItem != 0 && nodeItem->isVisible()) {

            //int childWidth = nodeItem->boundingRect().width();
            //int childHeight = nodeItem->boundingRect().height();
            int childWidth = nodeItem->width;
            int childHeight = nodeItem->height;

            // if the origWidth is not used, when a node is sorted and it
            // only had one child to begin with, it will always only have
            // one node per row and hence one column, once sorted
            // this allows there to be at most 2 child nodes per row

            if ((rowWidth + childWidth) > (initialWidth*1.5)) {
                //if ((rowWidth + childWidth) > origWidth) {
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
                } else if (nodeKind.startsWith("Attribute")) {
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
    } else {
        emit updateGraphMLData(getGraphML(), "width", QString::number(initialWidth));
        emit updateGraphMLData(getGraphML(), "height", QString::number(initialHeight));
    }
}

void NodeItem::setHidden(bool hidden)
{
    DRAW_OBJECT = !hidden;
    if(label){
        label->setVisible(DRAW_OBJECT);
    }
    setVisible(DRAW_OBJECT);
}


void NodeItem::setHideChildren(bool hideChildren)
{
    DRAW_DETAIL = !hideChildren;
    //Hide Label
    if(label){
        label->setVisible(DRAW_DETAIL);
    }
}


void NodeItem::setRubberbandMode(bool On)
{
    USING_RUBBERBAND_SELECTION = On;
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch (event->button()) {

    case Qt::MiddleButton:{
        if (event->modifiers().testFlag(Qt::ControlModifier)) {
            if (!DRAW_OBJECT) {
                emit sortModel();
            }
            sort();
        } else {
            if (nodeKind!= "DeploymentDefinitions") {
                emit triggerCentered(getGraphML());
            }
        }
        break;
    }
    case Qt::LeftButton:{
        if(!DRAW_OBJECT){
            // unselect any selected node item
            // when the model is pressed
            emit clearSelection();
            event->setAccepted(false);
            return;
        }
        if(USING_RUBBERBAND_SELECTION){
            rubberBand_ScreenOrigin = event->screenPos();
            rubberBand_SceneOrigin = mapToScene(event->pos());
            rubberBand->setGeometry(QRect(rubberBand_ScreenOrigin.toPoint(), QSize()));
            rubberBand->show();
        }else{
            //Left and Right buttons should target this line.
            previousScenePosition = event->scenePos();
            hasSelectionMoved = false;
            isNodePressed = true;
            emit triggerSelected(getGraphML());
        }
        break;
    }
    case Qt::RightButton:{
        if(!DRAW_OBJECT){
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
    if(!DRAW_OBJECT){
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            QPointF scenePos = mapToScene(event->pos());
            QRect selectionRectangle(rubberBand_SceneOrigin.toPoint(), scenePos.toPoint());
            QPainterPath pp;
            pp.addRect(selectionRectangle);

            scene()->setSelectionArea(pp, Qt::ContainsItemBoundingRect);
            rubberBand->hide();
        }else{
            hasSelectionMoved = false;
            isNodePressed = false;
        }
        break;
    }
    default:
        break;
    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!DRAW_OBJECT){
        return;
    }

    if(USING_RUBBERBAND_SELECTION){
        if(boundingRect().contains(event->pos())){
            rubberBand->setGeometry(QRect(rubberBand_ScreenOrigin.toPoint(), event->screenPos()).normalized());
        }
    }else if(isNodePressed && isSelected){
        if(hasSelectionMoved == false){
            emit triggerAction("Moving Selection");
        }
        QPointF delta = (event->scenePos() - previousScenePosition);

        //if (parentItem()) {
        //QRectF rec = parentItem()->boundingRect();
        //if (rec.contains(pos()+delta) && rec.contains(pos()+delta+QPoint(width, height))) {
        this->setPos(pos() + delta);
        emit moveSelection(delta);
        hasSelectionMoved = true;
        previousScenePosition = event->scenePos();
    }
}

void NodeItem::updateBrushes()
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
    else if(nodeKind== "DeploymentDefinitions"){
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
    QGraphicsItem::setPos(pos);
    emit updateEdgePosition();
}



/**
 * @brief NodeItem::setLabelFont
 * This sets up the font and size of the label.
 * It also stores a fixed, default size for its children.
 */
void NodeItem::setLabelFont()
{
    QFont font("Arial");
    font.setPointSize(1);
    QFontMetrics fm(font);

    float factor = (width*0.95) / fm.width(QString(16, 'c'));
    font.setPointSizeF(font.pointSizeF() * factor);
    label->setFont(font);

    // move the label away from the curved corner
    label->setPos(getCurvedCornerWidth(), getCurvedCornerWidth()/2);

    // set fixed child size
    defaultChildSize = label->boundingRect().height() * 4;
}


/**
 * @brief NodeItem::setupIcon
 * This sets up the scale and tranformation of this item's icon
 * if it has one. It also sets the pos for this item's label.
 */
void NodeItem::setupIcon()
{
    if(icon != 0){
        qreal labelHeight = label->boundingRect().height();
        qreal iconHeight = icon->boundingRect().height();
        qreal iconWidth = icon->boundingRect().width();

        qreal scaleFactor = (labelHeight / iconHeight)*1.5;
        icon->setScale(scaleFactor);
        icon->setPos(getCurvedCornerWidth(), getCurvedCornerWidth()/2);

        qreal diffHeight = (iconHeight*scaleFactor) - labelHeight;
        label->setX(icon->x() + (iconWidth * scaleFactor));
        label->setY(icon->y() + (diffHeight/2));

        // this prevents the icon from looking pixelated
        icon->setTransformationMode(Qt::SmoothTransformation);
         height = (icon->boundingRect().height()*icon->scale())
                + getCurvedCornerWidth();
        initialHeight = height;
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
double NodeItem::getCurvedCornerWidth()
{
  /**
    if (width > height) {
        return width/20;
    } else {
        return height/20;
    }
    */
    return initialWidth/20;
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
        if (nodeKind!= "Hardware" && nodeKind!= "ManagementComponent") {

            QFont font = label->font();
            QFontMetrics fm(font);
            QPointF point = QPointF(label->x()+label->boundingRect().width(), icon->y());
            QSize size = QSize(icon->boundingRect().width()*icon->scale(), icon->boundingRect().height()*icon->scale());
            double ratio = 0.8;

            font.setPointSize(font.pointSize()*ratio);
            point.setX(point.x()+(size.width()*(1-ratio)));
            point.setY(point.y()+(size.height()*(1-ratio)/2));
            size.setWidth(size.width()*ratio);
            size.setHeight(size.height()*ratio);

            expandButton = new QPushButton("-");
            expandButton->setStyleSheet("padding: 0px;");
            expandButton->setFont(font);
            expandButton->setFixedSize(size.width(), size.height());
            //expandButton->move(point.x(), point.y());
            expandButton->move(label->x()+fm.width(QString(16, 'c')), point.y());

            proxyWidget = new QGraphicsProxyWidget(this);
            proxyWidget->setWidget(expandButton);

            // setup and connect button
            expanded = true;
            expandButton->setCheckable(true);
            expandButton->setChecked(true);
            connect(expandButton, SIGNAL(clicked(bool)), this, SLOT(expandItem(bool)));

            // add button's width to this item's origWidth
            //origWidth += expandButton->width();
            //qDebug() << "origWidth: " << origWidth;
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

    if (show) {
        expandButton->setText("-");
        width = prevWidth;
        height = prevHeight;
    } else {
        expandButton->setText("+");
        prevWidth = width;
        prevHeight = height;
        width = initialWidth;
        height = initialHeight;
    }

    expanded = show;
    prepareGeometryChange();
    update();
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
/*
void NodeItem::setHidden(bool h)
{
    hidden  = h;
    setVisible(!h);
}
*/


/**
 * @brief NodeItem::resetSize
 * Reset this node item's size to its default size
 * and sort its children if there are any.
 */
void NodeItem::resetSize()
{
    /*
    GraphMLData* hData = getGraphML()->getData("height");
    GraphMLData* wData = getGraphML()->getData("width");

    if(hData && wData){
        wData->setValue(QString::number(initialWidth));
        hData->setValue(QString::number(initialHeight));
    }*/

    emit updateGraphMLData(getGraphML(),"height", QString::number(initialHeight));
    emit updateGraphMLData(getGraphML(),"width", QString::number(initialWidth));

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


