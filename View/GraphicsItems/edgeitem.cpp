#include "edgeitem.h"
#include "graphmlitem.h"
#include <QDebug>
#include <math.h>

#define EDGE_WIDTH 1
#define EDGE_SPACE_RATIO 0.8
#define ARROW_COUNT 6.0
#define ARROW_HEIGHT_RATIO (EDGE_SPACE_RATIO / ARROW_COUNT)
#define ARROW_TAIL_LENGTH ARROW_HEIGHT_RATIO
#define EDGE_GAP_RATIO ((1 - EDGE_SPACE_RATIO)/2)

EdgeItem::EdgeItem(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge, GraphMLItem::NODE_EDGE)
{
    deleting = false;
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

    for(int i=0; i < 6; i++){
        lineItems.append(new QGraphicsLineItem(this));
        lineItems[i]->setZValue(999);
    }


    //Setup Sizes.
    circleRadius = (ARROW_HEIGHT_RATIO * source->minimumVisibleRect().height()) / 2;

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
    setZValue(1000);
}

EdgeItem::~EdgeItem()
{

    deleting = true;
    if(source){
        source->removeVisibleParentForEdgeItem(getID());
        source->removeEdgeItem(this);
    }
    if(destination){
        destination->removeVisibleParentForEdgeItem(getID());
        destination->removeEdgeItem(this);
    }
    if(visibleSource){
        visibleSource->removeVisibleParentForEdgeItem(getID());
    }

    if(visibleDestination){
        visibleDestination->removeVisibleParentForEdgeItem(getID());
    }

    while(!lineItems.isEmpty()){
        QGraphicsLineItem *lineI = lineItems.takeFirst();
        delete lineI;
    }


    delete label;
}

QRectF EdgeItem::boundingRect() const
{
    QRectF bRect;
    bRect.setTopLeft(QPointF(-circleRadius,-circleRadius));
    bRect.setBottomRight(QPointF(circleRadius, circleRadius));

    QRectF tailBR = mapFromScene(arrowTail).boundingRect();
    QRectF headBR = mapFromScene(arrowHead).boundingRect();

    QPointF tL;
    QPointF bR;
    tL.setX(qMin(qMin(tailBR.left(), headBR.left()), bRect.left()));
    tL.setY(qMin(qMin(tailBR.top(), headBR.top()), bRect.top()));

    bR.setX(qMax(qMax(tailBR.right(), headBR.right()), bRect.right()));
    bR.setY(qMax(qMax(tailBR.bottom(), headBR.bottom()), bRect.bottom()));

    return QRectF(tL, bR);
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

        QRectF rectangle(QPointF(-circleRadius + (Pen.widthF()/2),-circleRadius+ (Pen.widthF()/2)), QPointF(circleRadius - (Pen.widthF()/2), circleRadius - (Pen.widthF()/2)));
        label->setPos(- (label->boundingRect().width()/2), - (label->boundingRect().height()/2));


        painter->drawEllipse(rectangle);
        painter->setPen(Qt::NoPen);


        painter->setBrush(tailBrush);
        painter->drawPolygon(mapFromScene(arrowTail));

        painter->setBrush(headBrush);
        painter->drawPolygon(mapFromScene(arrowHead));

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


void EdgeItem::setSelected(bool selected)
{
    if(deleting){
        return;
    }
    IS_SELECTED = selected;


    foreach(QGraphicsLineItem* line, lineItems){
        if(selected){
            line->setPen(selectedPen);
        }else{
            line->setPen(pen);
        }
        line->update();
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
    if(source && destination && !deleting){
        updateLines();
    }
}

void EdgeItem::graphMLDataChanged(GraphMLData *data)
{
    if(deleting){
        return;
    }
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
    QRectF itemCircle = QRectF(-circleRadius,-circleRadius,width,height);

    if(!itemCircle.contains(event->pos())){
        event->setAccepted(false);
        return;
    }
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
    QRectF itemCircle = QRectF(-circleRadius,-circleRadius,width,height);


    if(IS_MOVING){
        foreach(QGraphicsLineItem* line, lineItems){
            line->setVisible(false);
        }


        QPointF delta = (event->scenePos() - previousScenePosition);
        this->moveBy(delta.x(),delta.y());
        previousScenePosition = event->scenePos();
        hasMovedFromCenter = true;
        return;
    }else{
        if(!itemCircle.contains(event->pos())){
            event->setAccepted(false);
            return;
        }
    }

}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QRectF itemCircle = QRectF(-circleRadius,-circleRadius,width,height);



    if(!itemCircle.contains(event->pos())){
        event->setAccepted(false);
        return;
    }

    if(IS_MOVING){
        foreach(QGraphicsLineItem* line, lineItems){
            line->setVisible(true);
        }
    }
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


    QColor selectedColor;
    QColor color;

    pen.setStyle(Qt::SolidLine);
    color = QColor(100,100,100);

    selectedColor = Qt::blue;

    brush = QBrush(color);
    selectedBrush = QBrush(selectedColor);

    pen.setColor(color);
    pen.setWidth(EDGE_WIDTH);

    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(EDGE_WIDTH);

    headBrush = QBrush(color);
    tailBrush = QBrush(color);

    if(source->getNodeKind() == "OutEventPortInstance"){
        headBrush = QBrush(QColor(200,0,0));
    }

    if(destination->getNodeKind() == "InEventPortInstance"){
        tailBrush = QBrush(QColor(0,200,0));
    }
}

void EdgeItem::forceVisible(bool visible)
{
    foreach(QGraphicsLineItem* line, lineItems){
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
    if(IS_IMPL_LINK || IS_INSTANCE_LINK){
        forceVisible(false);
        return;
    }


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


    bool removeSrcEdge = false;
    if(visibleSource){
        //If our new visual parent is differing from our previous, remove the edge.
        if(visibleSrc != visibleSource){
            removeSrcEdge = true;
        }
        //If the edge has changed sides on its visual parent, remove the edge, so we can re-add it on the right side.
        if(visibleSource->getIndexOfEdgeItem(getID(), srcSide == RIGHT) == -1){
            removeSrcEdge = true;
        }
    }

    if(removeSrcEdge){
        visibleSource->removeVisibleParentForEdgeItem(getID());
    }

    visibleSource = visibleSrc;
    visibleSrc->setVisibleParentForEdgeItem(getID(), srcSide == RIGHT);

    bool removeDstEdge = false;
    if(visibleDestination){
        //If our new visual parent is differing from our previous, remove the edge.
        if(visibleDst != visibleDestination){
            removeDstEdge = true;
        }
        //If the edge has changed sides on its visual parent, remove the edge, so we can re-add it on the right side.
        if(visibleDestination->getIndexOfEdgeItem(getID(), dstSide == RIGHT) == -1){
            removeDstEdge = true;
        }
    }

    if(removeDstEdge){
        visibleDestination->removeVisibleParentForEdgeItem(getID());
    }

    visibleDestination = visibleDst;
    visibleDst->setVisibleParentForEdgeItem(getID(), dstSide == RIGHT);


    if(visibleDst == visibleSrc){
        forceVisible(false);
        return;
    }

    //Get the start/end points.
    QPointF edgeSrc;
    QPointF edgeDst;

    int srcArrowSideFlip = 1;
    int dstArrowSideFlip = 1;


    //Set X for edgeSrc
    if(srcSide == LEFT){
        edgeSrc.setX(srcRect.left());
        srcArrowSideFlip = -1;
    }else{
        edgeSrc.setX(srcRect.right());
    }

    //Set Y for edgeSrc
    int srcEdgeCount = visibleSrc->getNumberOfEdgeItems(srcSide == RIGHT);
    int srcIndex = visibleSrc->getIndexOfEdgeItem(getID(), srcSide == RIGHT);


    qreal srcYOffset = EDGE_GAP_RATIO * srcRect.height() + (((srcIndex + 1.0) / (srcEdgeCount + 1.0)) * (EDGE_SPACE_RATIO * srcRect.height()));
    edgeSrc.setY(srcRect.top() + srcYOffset);



    //Set X for edgeDst
    if(dstSide == LEFT){
        edgeDst.setX(dstRect.left());
    }else{
        edgeDst.setX(dstRect.right());
        dstArrowSideFlip = -1;
    }

    //Set Y for edgeSrc
    int dstEdgeCount = visibleDst->getNumberOfEdgeItems(dstSide == RIGHT);
    int dstIndex = visibleDst->getIndexOfEdgeItem(getID(), dstSide == RIGHT);

    qreal dstYOffset = (EDGE_GAP_RATIO * dstRect.height()) + (((dstIndex + 1.0) / (dstEdgeCount + 1.0)) * (EDGE_SPACE_RATIO * dstRect.height()));
    edgeDst.setY(dstRect.top() + dstYOffset);


    //Calculate Arrow Heads
    QPointF srcItemPos = mapFromScene(edgeSrc);
    QPointF dstItemPos = mapFromScene(edgeDst);

    qreal arrowHeight = ARROW_HEIGHT_RATIO * visibleSrc->minimumVisibleRect().height();

    QPointF arrowTailTL = edgeSrc -  QPointF(0, (arrowHeight / 2));
    QPointF arrowTailMR = edgeSrc +  (srcArrowSideFlip * QPointF(arrowHeight, 0));
    QPointF arrowTailBL = edgeSrc +  QPointF(0, (arrowHeight / 2));
    arrowTail.clear();
    arrowTail.append(arrowTailTL);
    arrowTail.append(arrowTailMR);
    arrowTail.append(arrowTailBL);
    arrowTail.append(arrowTailTL);



    QPointF xOffset = QPointF((-dstArrowSideFlip) * arrowHeight, 0);
    QPointF arrowHeadTL = edgeDst + xOffset -  QPointF(0, (arrowHeight / 2));
    QPointF arrowHeadMR = edgeDst + xOffset + (dstArrowSideFlip * QPointF(arrowHeight, 0));
    QPointF arrowHeadBL = edgeDst + xOffset + QPointF(0, (arrowHeight / 2));
    arrowHead.clear();
    arrowHead.append(arrowHeadTL);
    arrowHead.append(arrowHeadMR);
    arrowHead.append(arrowHeadBL);
    arrowHead.append(arrowHeadTL);


    arrowHeadMR = mapFromScene(arrowHeadMR + xOffset);
    arrowTailMR = mapFromScene(arrowTailMR);


    qreal arrowEndWidth = ARROW_TAIL_LENGTH * visibleDst->minimumVisibleRect().height();
    QPointF arrowTailEnd = arrowTailMR + QPointF((srcArrowSideFlip) * arrowEndWidth, 0);

    QPointF arrowHeadEnd = arrowHeadMR + QPointF((-dstArrowSideFlip) * arrowEndWidth, 0);

    QPointF headPoint = QPointF(arrowTailEnd.x(), 0);
    QPointF tailPoint = QPointF(arrowHeadEnd.x(), 0);

    lineItems[0]->setLine(QLineF(arrowTailMR, arrowTailEnd));
    lineItems[1]->setLine(QLineF(arrowTailEnd, headPoint));
    lineItems[2]->setLine(QLineF(headPoint, QPointF(-circleRadius,0)));
    lineItems[3]->setLine(QLineF(QPointF(circleRadius,0), tailPoint));
    lineItems[4]->setLine(QLineF(tailPoint, arrowHeadEnd));
    lineItems[5]->setLine(QLineF(arrowHeadEnd, arrowHeadMR));


    if(!hasMovedFromCenter){
        resetEdgeCenter(visibleSrc, visibleDst);
        //Recalulate
    }
    setZValue(1000);

    forceVisible(true);
    prepareGeometryChange();
}
