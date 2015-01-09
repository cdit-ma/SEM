#include "nodeitem.h"
#include "nodeedge.h"

#include <QGraphicsTextItem>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QRubberBand>

NodeItem::NodeItem(Node *node, NodeItem *parent):  GraphMLItem(node)
{
    Q_INIT_RESOURCE(resources);

    isSelected = false;
    drawDetail = true;
    drawObject = true;
    USING_RUBBERBAND_SELECTION = false;

    ID = node->getID();

    CONTROL_DOWN = false;
    hasMoved = false;

    rubberBand = new QRubberBand(QRubberBand::Rectangle,0);
    QPalette palette;
    palette.setBrush(QPalette::Foreground, QBrush(Qt::green));
    palette.setBrush(QPalette::Base, QBrush(Qt::red));

    rubberBand->setPalette(palette);
    rubberBand->resize(500, 500);

    this->isPressed = false;
    this->node = node;

    setParentItem(parent);

    label  = new QGraphicsTextItem("NULL",this);
    icon = 0;

    // initially set width and height ot be the default size
    if (!parent) {
        depth = 1;
        width = 19200;
        height = 10800;
    } else {
        parentKind = parent->getGraphML()->getDataValue("kind");
        width = parent->getChildSize();
        height = parent->getChildSize();
    }

    if (!parentKind.isNull()) {
        if (parentKind == "Component" || parentKind == "ComponentInstance") {
            width /= 2;
            height /= 2;
        }
    }

    // store original/default width and height
    origWidth = width;
    origHeight = height;
    hidden = false;


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

    if(kindData)
        connect(kindData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataUpdated(GraphMLData*)));



    if(wData && wData->getValue() == ""){
        wData->setValue(QString::number(width));
    }else{
        if(wData)
            width = wData->getValue().toFloat();
    }

    if(hData && hData->getValue() == ""){
        hData->setValue(QString::number(height));
    }else{
        if(hData)
            height = hData->getValue().toFloat();
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
    if(kindData)
        graphMLDataUpdated(kindData);
    if(labelData)
        graphMLDataUpdated(labelData);

    updateBrushes();
    if(kindData->getValue() == "Model" || kindData->getValue() == "DeploymentDefinitions"){
        drawObject = false;
        drawDetail = false;
    }

    // this will stop the width of this item and its children from
    // shrinking when it, or one of its child is resized.
    setLabelFont();
    setupIcon();


     setCacheMode(QGraphicsItem::NoCache);
}


NodeItem::~NodeItem()
{
    delete rubberBand;
    delete label;
}


QRectF NodeItem::boundingRect() const
{
    int brushSize = selectedPen.width();
    if(kind == "Model"){
        brushSize = 0;
    }
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}


void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    label->setVisible(drawDetail);

    if(drawObject){
        //qCritical() << "REPAINTING!";
        QRectF rectangle(0, 0, width, height);

        QPen Pen;
        QBrush Brush;

        if(isSelected){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        if(node->isInstance() || node->isImpl()){
            if(!node->getDefinition()){
                Brush.setStyle(Qt::DiagCrossPattern);
            }
        }

        qreal symetricRound = width > height ? width/10 : height/10;

        painter->setPen(Pen);
        painter->setBrush(Brush);
        painter->drawRoundedRect(rectangle, symetricRound, symetricRound);
    }



    */
}

void NodeItem::notifyEdges()
{
    foreach(NodeEdge* edge, connections){
        edge->updateLine();
    }
    foreach(QGraphicsItem* child, childItems()){
        NodeItem * childNode = dynamic_cast<NodeItem*>(child);
        if(childNode){
            childNode->notifyEdges();
        }
    }

}

void NodeItem::addConnection(NodeEdge *line)
{
    connections.append(line);
}

void NodeItem::deleteConnnection(NodeEdge *line)
{
    int position = connections.indexOf(line);
    connections.remove(position);
}


float NodeItem::getChildSize()
{
    return childSize;
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
    isSelected = selected;
    for(int i =0;i< connections.size();i++){
        if(connections[i] != 0){
            connections[i]->setSelected(selected);
        }
    }

    this->update();

    // update corresponding dock node item
    emit updateDockNodeItem(selected);
}


void NodeItem::toggleDetailDepth(int level)
{

    if(level>=depth){
        drawDetail = true;
    }else{
        if(level + 1 < depth){
            drawObject = false;

        }else{
            drawObject = true;
        }
        drawDetail = false;
    }
    notifyEdges();
    update();
}


void NodeItem::graphMLDataUpdated(GraphMLData* data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "x"){
            updatePosition(dataValue,0);
        }else if(dataKey == "y"){
            updatePosition(0,dataValue);
        }else if(dataKey == "label"){

            Node* node = (Node*) this->getGraphML();

            dataValue = dataValue;// +  " [" + node->getID()+"]";

            if(dataValue != ""){
                label->setPlainText(dataValue);
            }

            /**
            if(icon){
                qreal labelHeight = label->boundingRect().height();
                qreal iconHeight = icon->boundingRect().height();
                qreal iconWidth = icon->boundingRect().width();
                qreal scaleFactor = labelHeight / iconHeight;
                icon->setScale(scaleFactor);

                label->setX(icon->x() + (iconWidth * scaleFactor) );

                // added this to prevent the icon from looking pixelated
                icon->setTransformationMode(Qt::SmoothTransformation);
            }
            */

            // there has been a change to this item's graphml data
            // update connected dock node item
            //qDebug() << "NodeItem: graphMLDataUpdated";
            emit updateDockNodeItem();

        }else if(dataKey == "kind"){
            kind = dataValue;
            update();
        }
        else if(dataKey == "width"){
            updateSize(dataValue, 0);
            graphMLDataUpdated(node->getData("label"));
        }
        else if(dataKey == "height"){
            updateSize(0, dataValue);
            graphMLDataUpdated(node->getData("label"));
        }

        if(dataKey == "x" || dataKey == "y"){
            notifyEdges();
        }
    }
}

void NodeItem::recieveData()
{
    name = node->getDataValue("label");

    QString x = node->getDataValue("x");
    QString y = node->getDataValue("y");

    this->setPos(QPointF(x.toDouble(),y.toDouble()));
    notifyEdges();

}

void NodeItem::destructNodeItem()
{
    this->~NodeItem();
}

void NodeItem::updateChildNodeType(QString type)
{
    toBuildType = type;
}

void NodeItem::updateViewAspects(QStringList aspects)
{
    viewAspect = aspects;

    bool isVisible = false;

    foreach(QString aspect, aspects){
        // if this item is in the currently viewed aspects, check to see
        // if this item is meant to be hidden before making it visible
        if(node->isInAspect(aspect) && !hidden){
            isVisible = true;
            break;
        }
    }

    this->setVisible(isVisible);

    foreach(NodeEdge* edge, connections){
        edge->setVisible(isVisible);
    }

    update();
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
void NodeItem::sortChildren()
{
    emit triggerAction("Sorting Children");

    float topY;
    float gapY;

    // if it's a main node item, it will have no icon
    if (kind == "Model" || kind.endsWith("Definitions")) {
        topY = label->boundingRect().height();
        gapY = topY;
    } else {
        topY = icon->boundingRect().height()*icon->scale();
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

    bool componentLayout = (kind == "Component" || kind == "ComponentInstance");
    bool fileContainsComponents = (kind == "File" && getChildrenKind().contains("Component"));
    bool componentAssembly = (kind == "ComponentAssembly");
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

            int childWidth = nodeItem->boundingRect().width();
            int childHeight = nodeItem->boundingRect().height();

            if ((rowWidth + childWidth) > (origWidth*1.25)) {
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

                // if the origWidth is not used, when a node is sorted and it
                // only had one child to begin with, it will always only have
                // one node per row and hence one column, once sorted
                // this allows there to be at most 2 child nodes per row

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
            emit updateGraphMLData(getGraphML(), "width", QString::number(maxWidth + gapX));
            emit updateGraphMLData(getGraphML(), "height", QString::number(colHeight + maxHeight + gapY));

            // this is only used to resize the model
            if (kind == "Model") {
                //updateSize(maxWidth + gapX, colHeight + maxHeight + gapY);
            }
        }
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
        if(event->modifiers().testFlag(Qt::ControlModifier)){
            sortChildren();
            // update scene rect after the model is sorted/resized
            if (kind == "Model") {
                emit updateSceneRect(this);
            }
        }else{
            emit triggerCentered(getGraphML());
        }
        //emit centreNode(this);
        break;
    }
    case Qt::LeftButton:{
        if(!drawObject){
            // unselect any selected node item
            // when the model is pressed
            emit clearSelection();
            event->setAccepted(false);
            return;
        }
        if(USING_RUBBERBAND_SELECTION){
            origin = event->screenPos();
            sceneOrigin = mapToScene(event->pos());
            rubberBand->setGeometry(QRect(origin.toPoint(), QSize()));
            rubberBand->show();
        }else{
            //Left and Right buttons should target this line.
            previousPosition = event->scenePos();
            hasMoved = false;
            isPressed = true;
            emit triggerSelected(getGraphML());
            //emit triggerSelected(this);
            //emit setItemSelected(node);
        }
        break;
    }
    case Qt::RightButton:{
        if(!drawObject){
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
    if(!drawObject){
        return;
    }

    switch (event->button()) {
    case Qt::LeftButton:{
        if(USING_RUBBERBAND_SELECTION){
            QPointF scenePos = mapToScene(event->pos());
            QRect selectionRectangle(sceneOrigin.toPoint(), scenePos.toPoint());
            QPainterPath pp;
            pp.addRect(selectionRectangle);

            scene()->setSelectionArea(pp, Qt::ContainsItemBoundingRect);
            rubberBand->hide();
        }else{
            hasMoved = false;
            isPressed = false;
        }
        break;
    }
    default:
        break;
    }
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(!drawObject){
        return;
    }

    if(USING_RUBBERBAND_SELECTION){
        if(boundingRect().contains(event->pos())){
            rubberBand->setGeometry(QRect(origin.toPoint(), event->screenPos()).normalized());
        }
    }else if(isPressed && isSelected){
        if(hasMoved == false){
            emit triggerAction("Moving Selection");
        }
        QPointF delta = (event->scenePos() - previousPosition);

        //if (parentItem()) {
        //QRectF rec = parentItem()->boundingRect();
        //if (rec.contains(pos()+delta) && rec.contains(pos()+delta+QPoint(width, height))) {
        this->setPos(pos() + delta);
        emit moveSelection(delta);
        hasMoved = true;
        previousPosition = event->scenePos();
        //}
        //}
    }
}

void NodeItem::updateBrushes()
{
    QString nodeKind = getGraphML()->getDataValue("kind");

    if(nodeKind == "OutEventPort"){
        color = QColor(0,250,0);
    }
    else if(kind == "OutEventPortInstance"){
        color = QColor(0,200,0);
    }
    else if(kind == "OutEventPortImpl"){
        color = QColor(0,150,0);
    }
    else if(kind == "OutEventPortDelegate"){
        color = QColor(0,100,0);
    }
    else if(nodeKind == "InEventPort"){
        color = QColor(250,0,0);
    }
    else if(kind == "InEventPortInstance"){
        color = QColor(200,0,0);
    }
    else if(kind == "InEventPortImpl"){
        color = QColor(150,0,0);
    }
    else if(kind == "InEventPortDelegate"){
        color = QColor(100,0,0);
    }
    else if(nodeKind == "Component"){
        color = QColor(200,200,200);
    }
    else if(kind == "ComponentInstance"){
        color = QColor(150,150,150);
    }
    else if(kind == "ComponentImpl"){
        color = QColor(100,100,100);
    }
    else if(nodeKind == "Attribute"){
        color = QColor(0,0,250);
    }
    else if(kind == "AttributeInstance"){
        color = QColor(0,0,200);
    }
    else if(kind == "AttributeImpl"){
        color = QColor(0,0,150);
    }
    else if(nodeKind == "HardwareNode"){
        color = QColor(0,250,250);
    }
    else if(kind == "HardwareCluster"){
        color = QColor(0,50,100);
    }

    else if(nodeKind == "BehaviourDefinitions"){
        color = QColor(250,250,250);
    }
    else if(kind == "InterfaceDefinitions"){
        color = QColor(250,250,250);
    }
    else if(nodeKind == "DeploymentDefinitions"){
        color = QColor(250,250,250);
    }

    else if(kind == "File"){
        color = QColor(150,150,150);
    }
    else if(nodeKind == "ComponentAssembly"){
        color = QColor(200,200,200);
    }

    else if(kind == "Aggregate"){
        color = QColor(200,200,200);
    }
    else if(nodeKind == "AggregateMember"){
        color = QColor(150,150,150);
    }
    else if(nodeKind == "Member"){
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

void NodeItem::updatePosition(QString x, QString y)
{
    qreal xR = pos().x();
    qreal yR = pos().y();
    if(x !=0){
        xR = x.toDouble();
    }
    if(y != 0){
        yR = y.toDouble();
    }
    setPos(xR,yR);

    update();
}

void NodeItem::updateSize(QString w, QString h)
{
    if(w != 0){
        width = w.toDouble();
    }
    if(h != 0){
        height = h.toDouble();
    }

    prepareGeometryChange();
    update();
}


/**
 * @brief NodeItem::updateSize
 * This is called whenever the sortChildren is called on the model.
 * @param w
 * @param h
 */
void NodeItem::updateSize(double w, double h)
{
    //bRec = QRect(0, 0, w, h);
    update();
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
    childSize = label->boundingRect().height() * 4;
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
        icon->setPos(getCurvedCornerWidth(), getCurvedCornerWidth());

        qreal diffHeight = (iconHeight*scaleFactor) - labelHeight;
        label->setX(icon->x() + (iconWidth * scaleFactor));
        label->setY(icon->y() + (diffHeight/2));

        // this prevents the icon from looking pixelated
        icon->setTransformationMode(Qt::SmoothTransformation);
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
    if (width > height) {
        return width/15;
    } else {
        return height/15;
    }
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
    GraphMLData* hData = getGraphML()->getData("height");
    GraphMLData* wData = getGraphML()->getData("width");

    if(hData && wData){
        wData->setValue(QString::number(origWidth));
        hData->setValue(QString::number(origHeight));
    }

    //updateSize(QString::number(origWidth), QString::number(origHeight));
    //sortChildren();
}


