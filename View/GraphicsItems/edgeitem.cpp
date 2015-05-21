#include "edgeitem.h"
#include "graphmlitem.h"
#include <QDebug>
#include <math.h>

#define EDGE_SPACE_RATIO 0.8
#define EDGE_GAP_RATIO ((1 - EDGE_SPACE_RATIO)/2)

EdgeItem::EdgeItem(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge, GraphMLItem::NODE_EDGE)
{
    label = 0;
    IS_VISIBLE = true;
    IS_SELECTED = false;

    IS_INSTANCE_LINK = edge->isInstanceLink();
    IS_IMPL_LINK = edge->isImplLink();
    IS_AGG_LINK = edge->isAggregateLink();
    IS_DEPLOYMENT_LINK = edge->isDeploymentLink();
    IS_COMPONENT_LINK = edge->isComponentLink() || edge->isAssemblyLink();

    source = s;
    destination = d;

    visibleDestination = 0;
    visibleSource = 0;
    sourceParent = 0;
    destinationParent = 0;

    if(s->parentItem()){
        QGraphicsItem* sParent = s->parentItem();
        sourceParent = dynamic_cast<NodeItem*>(sParent);
    }

    if(d->parentItem()){
        QGraphicsItem* dParent = d->parentItem();
        destinationParent = dynamic_cast<NodeItem*>(dParent);
    }

    if(IS_INSTANCE_LINK || IS_IMPL_LINK){
        Node* src = edge->getSource();
        Node* dst = edge->getDestination();

        if(dst->getParentNode()->isDefinition() && (src->getParentNode()->isImpl() || src->getParentNode()->isInstance())){
            //Don't show Non-Top Most Instance Links
            IS_VISIBLE = false;
        }
        if(dst->getDataValue("kind") == "AggregateInstance"){
            IS_VISIBLE = false;
        }
        if(src->getDataValue("kind") == "MemberInstance"){
            IS_VISIBLE = false;
        }
        if(src->getDataValue("kind") == "AggregateInstance"){
            IS_VISIBLE = false;
        }
    }

    if(IS_AGG_LINK){
        Node* src = edge->getSource();
        if(src->isInstance()){
            //Don't show Aggregate Links to Instances.
            IS_VISIBLE = false;
        }
    }


    //Construct lines.

    for(int i=0; i < 3; i++){
        lineItems.append(new QGraphicsLineItem(this));
        lineItems.append(new QGraphicsLineItem(this));
        arrowHeadLines.append(new QGraphicsLineItem(this));
        arrowTailLines.append(new QGraphicsLineItem(this));
    }


    //Setup Sizes.
    circleRadius = (s->boundingRect().width() + d->boundingRect().width()) / 10;
    width = circleRadius * 2;
    height =  width;

    source->addEdgeItem(this);
    destination->addEdgeItem(this);




    setupBrushes();
    updateLines();
    updateLabel();
    setLabelFont();

    GraphMLData* descriptionData = edge->getData("label");

    if(descriptionData)
        connect(descriptionData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));



    hasMovedFromCenter = false;
    //Set Flags
    setFlag(ItemDoesntPropagateOpacityToChildren);
    setFlag(ItemIgnoresParentOpacity);
    setFlag(ItemIsSelectable);

    setVisible(IS_VISIBLE);
}

EdgeItem::~EdgeItem()
{
    if(source){
        source->removeEdgeItem(this);
    }
    if(destination){
        destination->removeEdgeItem(this);
    }

    while(!lineItems.isEmpty()){
        QGraphicsLineItem *lineI = lineItems.takeFirst();
        delete lineI;
    }
    while(!arrowHeadLines.isEmpty()){
        QGraphicsLineItem *lineI = arrowHeadLines.takeFirst();
        delete lineI;
    }
    while(!arrowTailLines.isEmpty()){
        QGraphicsLineItem *lineI = arrowTailLines.takeFirst();
        delete lineI;
    }

    delete label;
}

QRectF EdgeItem::boundingRect() const
{
    int brushSize = selectedPen.width();
    return QRectF(-brushSize, -brushSize, width + (brushSize *2), height + (brushSize *2));
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);


    if(IS_VISIBLE){
        QPen Pen;
        QBrush Brush;

        if(IS_SELECTED){
            Brush = selectedBrush;
            Pen = selectedPen;
        }else{
            Brush = brush;
            Pen = pen;
        }

        painter->setBrush(Brush);
        painter->setPen(Pen);

        QPainterPath circle_path;

        QRectF rectangle(QPointF(0,0), QPointF(circleRadius * 2, circleRadius * 2));



        label->setPos(circleRadius - (label->boundingRect().width()/2), circleRadius - (label->boundingRect().height()/2));


        painter->drawEllipse(rectangle);

    }
}

NodeItem *EdgeItem::getSource()
{
    return source;
}

NodeItem *EdgeItem::getDestination()
{
    return destination;
}


void EdgeItem::setOpacity(qreal opacity)
{
    Q_UNUSED(opacity);
}


void EdgeItem::setSelected(bool selected)
{
    IS_SELECTED = selected;


    foreach(QGraphicsLineItem* line, lineItems){
        if(selected){
            line->setPen(selectedPen);
        }else{
            line->setPen(pen);
        }
        line->update();
    }

    foreach(QGraphicsLineItem* line, arrowHeadLines){
        //if(selected){
        //    line->setPen(selectedArrowPen);
       // }else{
            line->setPen(arrowHeadPen);
       // }
        //line->update();
    }
    foreach(QGraphicsLineItem* line, arrowTailLines){

         line->setPen(arrowTailPen);
         /*
        if(selected){
            line->setPen(selectedArrowPen);
        }else{
            line->setPen(arrowPen);
        }
        line->update();
        */
    }

    label->setVisible(selected);

    prepareGeometryChange();
    update();
}

void EdgeItem::setVisible(bool visible)
{
    NodeItem* start = source;
    NodeItem* finish = destination;
    if(!source->isVisible()){
        if(sourceParent){
            if(sourceParent->isVisible()){
                start = sourceParent;
            }
        }
    }

    if(!destination->isVisible()){
        if(destinationParent){
            if(destinationParent->isVisible()){
                finish = destinationParent;
            }
        }
    }


    bool isVisible = false;
    if(IS_VISIBLE && start->isVisible() && finish->isVisible() && (start != finish)){
        isVisible = true;
    }

    if(isVisible){
        //UPDATE EDGE
        updateEdge();
    }
    forceVisible(isVisible);

}

void EdgeItem::updateEdge()
{
    if(source && destination){
        updateLines();
    }
}

void EdgeItem::graphMLDataChanged(GraphMLData *data)
{
    if(data){
        QString dataKey = data->getKey()->getName();
        QString dataValue = data->getValue();

        if(dataKey == "description"){
            updateLabel();
        }
    }
}

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!IS_VISIBLE){
        event->setAccepted(false);
        return;
    }
    switch (event->button()) {

    case Qt::MiddleButton:{
        GraphMLItem_SetCentered(this);
        break;
    }
    case Qt::LeftButton:{

        if (!event->modifiers().testFlag(Qt::ControlModifier)){
            //Clear First if Control isn't pressed!
            GraphMLItem_ClearSelection(false);
        }
        GraphMLItem_AppendSelected(this);
        IS_MOVING = true;

        previousScenePosition = event->scenePos();
        break;
    }
    };
}

void EdgeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{

    if(IS_MOVING){

        QPointF delta = (event->scenePos() - previousScenePosition);
        this->moveBy(delta.x(),delta.y());
        previousScenePosition = event->scenePos();
        hasMovedFromCenter = true;
        return;
    }

}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    IS_MOVING = false;
    updateLines();
}

void EdgeItem::resetEdgeCenter(NodeItem* visibleSource, NodeItem* visibleDestination)
{
    QPointF centerPoint = (visibleSource->sceneBoundingRect().center() + visibleDestination->sceneBoundingRect().center()) /2;
    centerPoint -= QPointF(circleRadius, circleRadius);
    setPos(centerPoint);

}

void EdgeItem::setLabelFont()
{
    QFont font("Arial");
    float fontSize = qMax(.25 * 2*circleRadius, 1.0);

    font.setPointSize(fontSize);

    label->setFont(font);

}

void EdgeItem::updateLabel()
{
    QString labelText = getGraphML()->getDataValue("description");
    if(labelText == ""){
        labelText = getGraphML()->getID();
    }

    if(label){
        //label->setDefaultTextColor(QColor(Qt::white));
        label->setPlainText(labelText);
    }else{
        label = new QGraphicsTextItem(labelText, this);
        label->setVisible(false);
    }
}

void EdgeItem::setupBrushes()
{
    int penWidth = 2;

    QColor selectedColor;
    QColor color;

    pen.setStyle(Qt::DashLine);
    if(IS_INSTANCE_LINK){
        color = QColor(0, 0, 180);
    }else if(IS_IMPL_LINK){
        color = QColor(0, 180, 0);
    }else if(IS_AGG_LINK){
        color = QColor(180, 0, 0);
    }else if(IS_DEPLOYMENT_LINK){
        color = QColor(180, 0, 180);
    }else if(IS_COMPONENT_LINK){
        color = QColor(0, 180 , 180);
        //penWidth *= 2;
        pen.setStyle(Qt::SolidLine);
    }else{
        color = QColor(50, 50, 50);
    }

    selectedColor = color;
    //color.setAlpha(150);
    //selectedColor.setAlpha(250);

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(color);
    pen.setWidth(penWidth);

    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(penWidth);

    if(source->getNodeKind() == "OutEventPortInstance"){
        arrowTailPen.setColor(Qt::red);
    }

    if(destination->getNodeKind() == "InEventPortInstance"){
        arrowHeadPen.setColor(Qt::green);
    }

    arrowPen.setColor(selectedColor);
    arrowPen.setStyle(Qt::SolidLine);
    arrowPen.setWidth(penWidth);

    selectedArrowPen.setColor(selectedColor);
    selectedArrowPen.setWidth(penWidth);
}

void EdgeItem::forceVisible(bool visible)
{
    foreach(QGraphicsLineItem* line, lineItems){
        line->setVisible(visible);
    }
    foreach(QGraphicsLineItem* line, arrowHeadLines){
        line->setVisible(visible);
    }
    foreach(QGraphicsLineItem* line, arrowTailLines){
        line->setVisible(visible);
    }

    QGraphicsItem::setVisible(visible);
}

QPointF EdgeItem::getEdgeCenterPoint()
{
    return pos() + QPointF(circleRadius, circleRadius);
}

void EdgeItem::aspectsChanged(QStringList aspects)
{
    //Do Nothing.
}


void EdgeItem::updateLines()
{

    NodeItem* visibleSrc = source;
    NodeItem* visibleDst = destination;

    //Get current visibleSrc
    while(visibleSrc && !visibleSrc->isVisible()){
        visibleSrc = visibleSrc->getParentNodeItem();
    }

    //Get current visibleDst
    while(visibleDst && !visibleDst->isVisible()){
        visibleDst = visibleDst->getParentNodeItem();
    }

    //If we don't have both end points, set edge as invisible, and do nothing.
    if(!visibleSrc || !visibleDst){
        forceVisible(false);
        return;
    }


    QRectF srcRect = visibleSrc->sceneBoundingRect();
    QRectF dstRect = visibleDst->sceneBoundingRect();

    LINE_SIDE srcSide = LEFT;
    LINE_SIDE dstSide = LEFT;
    LINE_DIRECTION srcDir = DOWN;
    LINE_DIRECTION dstDir = DOWN;

    //Work out if the Source's Center Point is on the left or the right of the EdgeCenterPoint
    if(srcRect.center().x() <= getEdgeCenterPoint().x()){
        srcSide = RIGHT;
    }

    //Work out if the Destination's Center Point is on the left or the right of the EdgeCenterPoint
    if(dstRect.center().x() <= getEdgeCenterPoint().x()){
        dstSide = RIGHT;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(srcRect.center().y() <= getEdgeCenterPoint().y()){
        srcDir = UP;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(dstRect.center().y() <= getEdgeCenterPoint().y()){
        dstDir = UP;
    }

    //If our new parent is different to our previous, remove the edge from the listings of the previous.
    if(visibleSrc != visibleSource){
        if(visibleSource){
            visibleSource->removeVisibleParentForEdgeItem(this);
        }
        visibleSource = visibleSrc;
        visibleSrc->setVisibleParentForEdgeItem(this, srcSide == RIGHT);
    }

    //If our new parent is different to our previous, remove the edge from the listings of the previous.
    if(visibleDst != visibleDestination){
        if(visibleDestination){
            visibleDestination->removeVisibleParentForEdgeItem(this);
        }
        visibleDestination = visibleDst;
        visibleDst->setVisibleParentForEdgeItem(this, dstSide == RIGHT);
    }

    //Get the start/end points.
    QPointF edgeSrc;
    QPointF edgeDst;

    //Set X for edgeSrc
    if(srcSide == LEFT){
        edgeSrc.setX(srcRect.left());
    }else{
        edgeSrc.setX(srcRect.right());
    }

    //Set Y for edgeSrc
    int srcEdgeCount = visibleSrc->getNumberOfEdgeItems(srcSide == RIGHT);
    int srcIndex = visibleSrc->getIndexOfEdgeItem(this, srcSide == RIGHT);


    qreal srcYOffset = EDGE_GAP_RATIO * srcRect.height() + (((srcIndex + 1.0) / (srcEdgeCount + 1.0)) * (EDGE_SPACE_RATIO * srcRect.height()));
    edgeSrc.setY(srcRect.top() + srcYOffset);

     qCritical() << "srcEdgeCount: " << srcEdgeCount;
      qCritical() << "srcIndex: " << srcIndex;

     qCritical() << "srcYOffset: " << srcYOffset;

    //Set X for edgeDst
    if(dstSide == LEFT){
        edgeDst.setX(dstRect.left());
    }else{
        edgeDst.setX(dstRect.right());
    }

    //Set Y for edgeSrc
    int dstEdgeCount = visibleDst->getNumberOfEdgeItems(dstSide == RIGHT);
    int dstIndex = visibleDst->getIndexOfEdgeItem(this, dstSide == RIGHT);

    qreal dstYOffset = (EDGE_GAP_RATIO * dstRect.height()) + (((dstIndex + 1.0) / (dstEdgeCount + 1.0)) * (EDGE_SPACE_RATIO * dstRect.height()));
    edgeDst.setY(dstRect.top() + dstYOffset);


    QPointF srcTest = mapFromScene(edgeSrc);
    QPointF dstTest = mapFromScene(edgeDst);
    lineItems[0]->setLine(QLineF(srcTest, dstTest));



    //Find our current Visible Parent

    if(!hasMovedFromCenter){
        resetEdgeCenter(visibleSrc, visibleDst);
        //Recalulate
    }
    setZValue(qMax(visibleSrc->zValue(), visibleDst->zValue()) +1);

    forceVisible(true);
}
