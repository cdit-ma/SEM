#include "edgeitem.h"
#include "graphmlitem.h"
#include <QDebug>
#include <math.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include "../nodeview.h"

#define EDGE_WIDTH 1
#define EDGE_SPACE_RATIO 0.8
#define ARROW_COUNT 6.0
#define ARROW_HEIGHT_RATIO (EDGE_SPACE_RATIO / ARROW_COUNT)
#define ARROW_TAIL_LENGTH ARROW_HEIGHT_RATIO
#define EDGE_GAP_RATIO ((1 - EDGE_SPACE_RATIO)/2)
#define LABEL_RATIO .50

EdgeItem::EdgeItem(Edge* edge, NodeItem* s, NodeItem* d): GraphMLItem(edge, GraphMLItem::NODE_EDGE)
{
    label = 0;

    IS_VISIBLE = true;
    IS_DELETING = false;
    IS_SELECTED = false;
    HAS_MOVED = false;
    CENTER_MOVED = false;


    source = s;
    destination = d;
    visibleDestination = 0;
    visibleSource = 0;


    //Construct lines.
    for(int i=0; i < 6; i++){
        lineSegments.append(new QGraphicsLineItem(this));
    }


    //Setup Sizes.
    circleRadius = (ARROW_HEIGHT_RATIO * source->minimumVisibleRect().height()) / 2;

    //Add the Edge Item to the source/destination.
    source->addEdgeItem(this);
    destination->addEdgeItem(this);


    setupBrushes();
    updateLines();
    updateLabel();

    GraphMLData* descriptionData = edge->getData("label");

    if(descriptionData){
        connect(descriptionData, SIGNAL(dataChanged(GraphMLData* )), this, SLOT(graphMLDataChanged(GraphMLData*)));
    }

    resetEdgeCenter(source, destination);
    updateLines();
    setZValue(1000);
}

EdgeItem::~EdgeItem()
{
    IS_DELETING = true;
    if(getNodeView() && !getNodeView()->isDeleting()){
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
    }

    while(!lineSegments.isEmpty()){
        QGraphicsLineItem *lineI = lineSegments.takeFirst();
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

void EdgeItem::adjustPos(QPointF delta)
{
    /*
    if(CENTER_MOVED){
        updateLines();
    }else{
        setPos(0,0);
        updateLines();
    }
    return;
    QPointF currentPos = pos();
    currentPos += delta;
    setPos(currentPos);
    qCritical() << pos();
    */

}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    Q_UNUSED(widget);


    painter->setClipRect(option->exposedRect);

    if(IS_VISIBLE){
        QPen Pen = pen;
        QBrush Brush = brush;
        QBrush TailBrush = tailBrush;
        QBrush HeadBrush = headBrush;

        //Setup Brushes.
        if(IS_SELECTED){
            Brush = selectedBrush;
            Pen = selectedPen;
            TailBrush = selectedTailBrush;
            HeadBrush = selectedHeadBrush;
        }

        //Calculate the Center Circle.
        QRectF rectangle(QPointF(-circleRadius + (Pen.widthF()/2),-circleRadius+ (Pen.widthF()/2)), QPointF(circleRadius - (Pen.widthF()/2), circleRadius - (Pen.widthF()/2)));
        label->setPos(-(label->boundingRect().width()/2), -(label->boundingRect().height()/2));

        //Paint the Center Circle;
        painter->setPen(Pen);
        painter->setBrush(Brush);
        painter->drawEllipse(rectangle);


        //Paint Tail Brush
        painter->setPen(Qt::NoPen);
        painter->setBrush(TailBrush);
        painter->drawPolygon(mapFromScene(arrowTail));

        //Paint Head Brush
        painter->setPen(Qt::NoPen);
        painter->setBrush(HeadBrush);
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
    if(IS_DELETING){
        return;
    }

    //If the item is already selected
    if(IS_SELECTED == selected){
        return;
    }

    IS_SELECTED = selected;

    foreach(QGraphicsLineItem* line, lineSegments){
        if(selected){
            line->setPen(selectedPen);
        }else{
            line->setPen(pen);
        }
    }

    //Hide/Show the label if selected.
    label->setVisible(selected);
}

void EdgeItem::setVisible(bool visible)
{
    //If we don't have any visible Sources.
    if(!(visibleSource && visibleDestination) || visibleSource == visibleDestination){
        //Force Invisible.
        visible = false;
    }


    //If Item's visibility doesn't match what we are trying to set, update it.
    if(IS_VISIBLE != visible){
        QGraphicsItem::setVisible(visible);
        IS_VISIBLE = visible;
        updateLines();
    }

    //If Line's visibility doesn't match what we are trying to set, update it.
    if(LINES_VISIBLE != visible){
        setLineVisibility(visible);
    }
}

void EdgeItem::updateEdge()
{
    if(source && destination && !IS_DELETING){
        updateLines();
    }
}

void EdgeItem::graphMLDataChanged(GraphMLData *data)
{
    if(IS_DELETING){
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

void EdgeItem::graphMLDataChanged(QString name, QString key, QString value)
{
    //DO NOTHING
}

void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isPointInCircle(event->pos())){
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
            HAS_MOVED = false;
            previousScenePosition = event->scenePos();
            break;
        }
    }
}

void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isPointInCircle(event->pos())){
        event->setAccepted(false);
        return;
    }

    switch (event->button()) {
        case Qt::LeftButton:{
        //Double clicking an edge will reset it.
        resetEdgeCenter(visibleSource, visibleDestination);
        updateLines();
        break;
        }
    }
}

void EdgeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(IS_MOVING){
        //if(!HAS_MOVED){
        //    //Set Lines as invisible.
        //    setLineVisibility(false);
        //    HAS_MOVED = true;
        //}

        QPointF delta = event->scenePos() - previousScenePosition;
        //moveBy(delta.x(),delta.y());
        previousScenePosition = event->scenePos();

        //CENTER_MOVED = true;
    }else{
        if(!isPointInCircle(event->pos())){
            event->setAccepted(false);
        }
    }
}

void EdgeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(IS_MOVING){
        updateLines();
        setLineVisibility(true);
        IS_MOVING = false;
        HAS_MOVED = false;
        return;
    }

    if(!isPointInCircle(event->pos())){
        event->setAccepted(false);
        return;
    }
}

bool EdgeItem::isPointInCircle(QPointF position)
{
    if(abs(position.x()) < circleRadius && abs(position.y()) < circleRadius){
        return true;
    }
    return false;
}

void EdgeItem::resetEdgeCenter(NodeItem* visibleSource, NodeItem* visibleDestination)
{
    QPointF srcCenter = visibleSource->mapToScene(visibleSource->minimumVisibleRect().center());
    QPointF dstCenter = visibleDestination->mapToScene(visibleDestination->minimumVisibleRect().center());

    QPointF centerPoint = (srcCenter + dstCenter) / 2;

    setPos(centerPoint);
    CENTER_MOVED = false;
}

void EdgeItem::updateLabel()
{
    QString labelText = getGraphML()->getDataValue("description");

    if(labelText == ""){
        labelText = getGraphML()->getID();
    }

    if(!label){
        label = new QGraphicsTextItem(this);
        QFont font("Arial");
        float fontSize = LABEL_RATIO * circleRadius;
        font.setPointSize(fontSize);
        label->setFont(font);
    }

    if(label){
        label->setPlainText(labelText);
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
    selectedHeadBrush = QBrush(selectedColor);
    tailBrush = QBrush(color);
    selectedTailBrush = QBrush(selectedColor);

    if(source->getNodeKind() == "OutEventPortInstance" || source->getNodeKind() == "OutEventPortDelegate"){
        tailBrush = QBrush(QColor(0,200,0));
        selectedTailBrush = tailBrush;
    }
    if(destination->getNodeKind() == "OutEventPortInstance" || destination->getNodeKind() == "OutEventPortDelegate"){
        headBrush = QBrush(QColor(0,200,0));
        selectedHeadBrush = headBrush;
    }

    if(source->getNodeKind() == "InEventPortInstance" || source->getNodeKind() == "InEventPortDelegate"){
        tailBrush = QBrush(QColor(200,0,0));
        selectedTailBrush = tailBrush;
    }

    if(destination->getNodeKind() == "InEventPortInstance" || destination->getNodeKind() == "InEventPortDelegate"){
        headBrush = QBrush(QColor(200,0,0));
        selectedHeadBrush = headBrush;
    }
}

void EdgeItem::setLineVisibility(bool visible)
{
    foreach(QGraphicsLineItem* line, lineSegments){
        line->setVisible(visible);
    }
    LINES_VISIBLE = visible;
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

        setVisible(false);
        if(visibleSrc){
            visibleSrc->removeVisibleParentForEdgeItem(getID());
            visibleSource = 0;
        }
        if(visibleDst){
            visibleDst->removeVisibleParentForEdgeItem(getID());
            visibleDestination = 0;
        }
        return;
    }

    if(visibleSrc == visibleDst){
        setVisible(false);
        visibleSrc->removeVisibleParentForEdgeItem(getID());
        visibleDst->removeVisibleParentForEdgeItem(getID());
        visibleDestination = 0;
        visibleSource  = 0;
        return;
    }





    QRectF srcRect = visibleSrc->sceneBoundingRect();
    QRectF dstRect = visibleDst->sceneBoundingRect();

    LINE_SIDE srcSide = LEFT;
    LINE_SIDE dstSide = LEFT;
    LINE_DIRECTION srcDir = DOWN;
    LINE_DIRECTION dstDir = DOWN;

    //Work out if the Source's Center Point is on the left or the right of the EdgeCenterPoint
    if(srcRect.center().x() <= scenePos().x()){
        srcSide = RIGHT;
    }

    //Work out if the Destination's Center Point is on the left or the right of the EdgeCenterPoint
    if(dstRect.center().x() <= scenePos().x()){
        dstSide = RIGHT;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(srcRect.center().y() <= scenePos().y()){
        srcDir = UP;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(dstRect.center().y() <= scenePos().y()){
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
            //qCritical() << "Removing Source Edge";
            removeSrcEdge = true;
        }
    }

    if(removeSrcEdge){
        visibleSource->removeVisibleParentForEdgeItem(getID());
        disconnect(visibleSource, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
    }

    visibleSource = visibleSrc;
    if(visibleSource){
        disconnect(visibleSource, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
        connect(visibleSource, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
    }

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
        disconnect(visibleDestination, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
    }




    visibleDestination = visibleDst;
    visibleDst->setVisibleParentForEdgeItem(getID(), dstSide == RIGHT);

    if(visibleDestination){
        disconnect(visibleDestination, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
        connect(visibleDestination, SIGNAL(nodeItemMoved()), this, SLOT(updateEdge()));
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

    lineSegments[0]->setLine(QLineF(arrowTailMR, arrowTailEnd));
    lineSegments[1]->setLine(QLineF(arrowTailEnd, headPoint));
    lineSegments[2]->setLine(QLineF(headPoint, QPointF(-circleRadius,0)));
    lineSegments[3]->setLine(QLineF(QPointF(circleRadius,0), tailPoint));
    lineSegments[4]->setLine(QLineF(tailPoint, arrowHeadEnd));
    lineSegments[5]->setLine(QLineF(arrowHeadEnd, arrowHeadMR));


    if(!CENTER_MOVED){
        resetEdgeCenter(visibleSrc, visibleDst);
    }


    setVisible(true);
    //setLineVisibility(true);
    prepareGeometryChange();
}
