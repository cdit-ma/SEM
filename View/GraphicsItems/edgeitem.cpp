#include "edgeitem.h"
#include "graphmlitem.h"
#include <QDebug>
#include <math.h>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include "../nodeview.h"
#include <cmath>
#include <QRegion>

#include "edgeitemarrow.h"

#define CIRCLE_RADIUS 5
#define TEXT_LENGTH 30
#define EDGE_WIDTH 1
#define EDGE_SPACE_RATIO 0.8

#define ARROW_HEIGHT 10

#define EDGE_GAP_RATIO ((1 - EDGE_SPACE_RATIO)/2)

#define LABEL_RATIO .50

EdgeItem::EdgeItem(EdgeAdapter *edge, NodeItem *parent, EntityItem* s, EntityItem* d): GraphMLItem(edge, 0, GraphMLItem::NODE_EDGE)
{
    if(parent){
        setNodeView(parent->getNodeView());
    }
    QGraphicsItem::setParentItem(parent);


    parent->addEdgeAsChild(this);

    textItem = 0;



    source = s;
    destination = d;
    visibleSource = s;
    visibleDestination = d;

    this->parent = parent;


    //Construct lines.
    for(int i=0; i < 6; i++){
        lineSegments.append(new QGraphicsLineItem(this));
        lineSegments[i]->setPen(pen);
        lineSegments[i]->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    }



    connect(source, SIGNAL(GraphMLItem_PositionChanged()), this, SLOT(updateLine()));

    connect(destination, SIGNAL(GraphMLItem_PositionChanged()), this, SLOT(updateLine()));
    connect(source, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(updateLine()));
    connect(destination, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(updateLine()));

    //Add the Edge Item to the source/destination.
    source->addEdgeItem(this);
    destination->addEdgeItem(this);

    setupBrushes();

    listenForData("description");


    //Stepup.
    NodeItem* sParent = source;
    while(sParent != parent){
        connect(sParent, SIGNAL(visibleChanged()), this, SLOT(updateVisibleParents()));
        sParent = sParent->getParentNodeItem();
    }

    NodeItem* dParent = destination;
    while(dParent != parent){
        connect(dParent, SIGNAL(visibleChanged()), this, SLOT(updateVisibleParents()));
        dParent = dParent->getParentNodeItem();
    }

    arrowHeadItem = new EdgeItemArrow(this);
    arrowTailItem = new EdgeItemArrow(this);

    arrowHeadItem->setBrush(headBrush);
    arrowTailItem->setBrush(tailBrush);





    updateLines();
    updateFromData();

    //Update selection state.
    setSelected(false);

    updateVisibleParents();



    setZValue(500);
    updateBrushes();
}

EdgeItem::~EdgeItem()
{

    if(getNodeView() && !getNodeView()->isTerminating()){
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
        if(parent){
            parent->removeChildEdge(getID());
        }
    }

    while(!lineSegments.isEmpty()){
        QGraphicsLineItem *lineI = lineSegments.takeFirst();
        delete lineI;
    }

    delete arrowHeadItem;
    delete arrowTailItem;
    delete textItem;
}

QRectF EdgeItem::boundingRect() const
{
    QPointF modifier = QPointF(TEXT_LENGTH, CIRCLE_RADIUS);
    return QRectF(-modifier, modifier);
}

QRectF EdgeItem::circleRect() const
{

    QPointF modifier = QPointF(CIRCLE_RADIUS, CIRCLE_RADIUS);
    return QRectF(-modifier, modifier);
}

void EdgeItem::setCenterPos(QPointF pos)
{
    pos -= boundingRect().center();
    setPos(pos);
}

QPointF EdgeItem::getCenterPos()
{
    return this->pos() += boundingRect().center();
}


void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    Q_UNUSED(widget);
    switch(getRenderState()){

    case RS_FULL:{
        painter->setClipRect(option->exposedRect);
        painter->setBrush(currentBrush);
        painter->setPen(currentPen);


        QRectF rectangle = circleRect();
        if(isSelected()){
            rectangle = boundingRect();
        }

        painter->setPen((currentPen));
        painter->setBrush(currentBrush);
        rectangle.adjust(selectedPen.widthF(), selectedPen.widthF(), -selectedPen.widthF(), -selectedPen.widthF());
        painter->drawRoundedRect(rectangle, CIRCLE_RADIUS, CIRCLE_RADIUS);



        break;
    }
    case RS_MINIMAL:

    case RS_REDUCED:{
        painter->setClipRect(option->exposedRect);
        painter->setBrush(currentBrush);
        painter->setPen(currentPen);
        painter->drawLine(QPointF(-CIRCLE_RADIUS,0), QPointF(CIRCLE_RADIUS,0));
        break;
    }
    default:
        break;
    }
}

EntityItem *EdgeItem::getSource()
{
    return source;
}

EntityItem *EdgeItem::getDestination()
{
    return destination;
}

EdgeAdapter *EdgeItem::getEdgeAdapter()
{
    return (EdgeAdapter*)getEntityAdapter();
}


void EdgeItem::setHovered(bool highlighted)
{
    GraphMLItem::setHovered(highlighted);
    updateBrushes();
}


void EdgeItem::setSelected(bool selected)
{
    if(isDeleting()){
        return;
    }

    //If the item is already selected
    if(isSelected() == selected){
        return;
    }



    //Hide/Show the label if selected.
    textItem->setVisible(selected);

    //Call base class
    GraphMLItem::setSelected(selected);
    updateBrushes();
}

void EdgeItem::labelUpdated(QString newLabel)
{
    if(getEntityAdapter()){
        QString currentLabel = getDataValue("description").toString();

        if(currentLabel != newLabel){
            if(isDataEditable("description")){
                GraphMLItem_TriggerAction("Set new description");
                GraphMLItem_SetData(getID(), "description", newLabel);
            }
        }
    }
}

void EdgeItem::setVisible(bool visible)
{
    foreach(QGraphicsLineItem* line, lineSegments){
        line->setVisible(visible);
    }

    QGraphicsItem::setVisible(visible);
}

void EdgeItem::updateVisibleParents()
{
    if(parent->isVisible()){
        EntityItem* src = source;
        EntityItem* dst = destination;

        //Get the topmost visible Source
        while(src && !src->isVisibleTo(0)){
            src = src->getParentEntityItem();
            if(src == parent){
                src = 0;
            }
        }

        //Get the topmost visible Destination
        while(dst && !dst->isVisibleTo(0)){
            dst = dst->getParentEntityItem();
            if(dst == parent){
                dst = 0;
            }
        }

        if(src && dst){


            if(visibleSource != src){
                if(visibleSource){
                    //Remove the Edge from it's old visible parent.
                    visibleSource->removeVisibleParentForEdgeItem(getID());
                }
                visibleSource = src;
            }
            if(visibleDestination != dst){
                if(visibleDestination){
                    //Remove the Edge from it's old visible parent.
                    visibleSource->removeVisibleParentForEdgeItem(getID());
                }
                visibleDestination = dst;
            }


            updateLine();
            setVisible(true);
            return;
        }
    }
    setVisible(false);
}

void EdgeItem::updateLine()
{
    if(visibleSource && visibleDestination){
        QPointF sceneSrcPoint = visibleSource->scenePos() + visibleSource->getCenterOffset();
        QPointF sceneDstPoint = visibleDestination->scenePos() + visibleDestination->getCenterOffset();
        QPointF parentSrcPoint = parent->mapFromScene(sceneSrcPoint);
        QPointF parentDstPoint = parent->mapFromScene(sceneDstPoint);


        //This is the line directly in item coordinates.
        QLineF parentLine(parentSrcPoint, parentDstPoint);

        //If the line has changed (relative to the parent)
        if(parentLine != currentParentLine){
            //Set our center point
            //Calculate the center point of the sceneLine.
           // QPointF sceneCenterPos = (sceneSrcPoint + sceneDstPoint) /2;
            //setCenterPoint(parent->mapFromScene(sceneCenterPos));

            currentParentLine = parentLine;

            updateLines();
            //Calculate the new lines.
        }
    }


}


void EdgeItem::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "description"){
        updateLabel(data.toString());
    }
}

void EdgeItem::zoomChanged(qreal zoom)
{
    zoom = zoom * CIRCLE_RADIUS;
    if(zoom >= CIRCLE_RADIUS){
        setRenderState(RS_FULL);
    }else if(zoom >= (CIRCLE_RADIUS / 2)){
        setRenderState(RS_REDUCED);
    }else{
        setRenderState(RS_MINIMAL);
    }

    //Call base class
    GraphMLItem::zoomChanged(zoom);
}


void EdgeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    VIEW_STATE viewState = getViewState();
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);
    if(!isPointInCircle(event->pos())){
        return;
    }
    switch (event->button()) {
    case Qt::LeftButton:{
        switch(viewState){
        case VS_NONE:
            //Goto VS_Selected
        case VS_SELECTED:
            //Enter Selected Mode.
            handleSelection(true, controlPressed);
            break;
        }
        break;
    }
    case Qt::MiddleButton:{
        GraphMLItem_SetCentered(this);
        break;
    }
    default:
        return;
        break;
    }

    emit edgeItem_eventFromItem();
}

void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(isDataEditable("description")){
        textItem->setEditMode(true);
    }
}

void EdgeItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isHovered()){
        emit GraphMLItem_Hovered(getID(), true);
    }

}

void EdgeItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if(!isHovered()){
        emit GraphMLItem_Hovered(getID(), true);
    }
}

void EdgeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if(isHovered()){
        emit GraphMLItem_Hovered(getID(), false);
    }
}

void EdgeItem::updateBrushes()
{
    QPen Pen = pen;
    QBrush Brush = brush;
    QBrush TailBrush = tailBrush;
    QBrush HeadBrush = headBrush;

    //Setup Brushes.
    if(isSelected()){
        Brush = selectedBrush;
        Pen = selectedPen;
        TailBrush = selectedTailBrush;
        HeadBrush = selectedHeadBrush;
    }else{
        if(isHovered()){
            //Pen.setColor(Qt::black);
            Pen.setColor(Pen.color().lighter(180));
        }
    }



    //Update the current brush/pen
    currentBrush = Brush;
    currentPen = Pen;

    foreach(QGraphicsLineItem* line, lineSegments){
        line->setPen(currentPen);
    }

    if(arrowHeadItem){
        arrowHeadItem->setPen(currentPen);
        arrowHeadItem->setBrush(HeadBrush);
    }
    if(arrowTailItem){
        arrowTailItem->setPen(currentPen);
        arrowTailItem->setBrush(TailBrush);
    }
}





bool EdgeItem::isPointInCircle(QPointF position)
{
    if(fabs(position.x()) < CIRCLE_RADIUS && fabs(position.y()) < CIRCLE_RADIUS){
        return true;
    }
    return false;
}




void EdgeItem::updateLabel(QString labelText)
{
    if(labelText == ""){
        labelText = "ID: " + QString::number(getEntityAdapter()->getID());
    }

    if(!textItem){
        textItem = new EditableTextItem(this);
        textItem->setAlignment(Qt::AlignCenter);

        connect(textItem, SIGNAL(textUpdated(QString)),this, SLOT(labelUpdated(QString)));

        textItem->setEditable(isDataEditable("label"));


        textItem->setTextWidth((2* TEXT_LENGTH) - CIRCLE_RADIUS);

        QFont font("Arial");
        font.setPixelSize(CIRCLE_RADIUS + 2);
        textItem->setFont(font);


        qreal labelX = - textItem->boundingRect().width()/2;
        qreal labelY =  -(textItem->boundingRect().height())/2;
        textItem->setPos(labelX,labelY);
        textItem->setVisible(false);
    }

    if(textItem){
        textItem->setPlainText(labelText);
    }
}

void EdgeItem::setupBrushes()
{
    QColor color;
    QColor selectedColor;

    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::gray);
    pen.setColor(pen.color().darker());

    if(getEdgeAdapter()){
        Edge::EDGE_CLASS edgeClass = Edge::EC_NONE;
        if(getEdgeAdapter()){
            edgeClass = getEdgeAdapter()->getEdgeClass();
        }
        if(edgeClass == Edge::EC_DATA){
            pen.setColor(Qt::red);
        }else if(edgeClass == Edge::EC_WORKFLOW){
            pen.setColor(Qt::yellow);
        }else if(edgeClass == Edge::EC_ASSEMBLY){
            pen.setColor(Qt::green);
        }
    }


    color = QColor(100,100,100);

    selectedColor = Qt::blue;

    brush = QBrush(color);
    selectedBrush = QBrush(color);

    pen.setWidth(EDGE_WIDTH);

    selectedPen.setColor(selectedColor);
    selectedPen.setWidth(EDGE_WIDTH);

    headBrush = QBrush(color);
    selectedHeadBrush = QBrush(selectedColor);
    tailBrush = QBrush(color);
    selectedTailBrush = QBrush(selectedColor);

    if(source->getNodeKind() == "OutEventPortInstance" || source->getNodeKind() == "OutEventPortDelegate"){
        tailBrush = QBrush(QColor(0,200,0));
        selectedTailBrush = QBrush(tailBrush.color().lighter());
    }
    if(destination->getNodeKind() == "OutEventPortInstance" || destination->getNodeKind() == "OutEventPortDelegate"){
        headBrush = QBrush(QColor(0,200,0));
        selectedHeadBrush = QBrush(headBrush.color().lighter());
    }

    if(source->getNodeKind() == "InEventPortInstance" || source->getNodeKind() == "InEventPortDelegate"){
        tailBrush = QBrush(QColor(200,0,0));
        selectedTailBrush = QBrush(tailBrush.color().lighter());
    }

    if(destination->getNodeKind() == "InEventPortInstance" || destination->getNodeKind() == "InEventPortDelegate"){
        headBrush = QBrush(QColor(200,0,0));
        selectedHeadBrush = QBrush(headBrush.color().lighter());    }
}

void EdgeItem::updateLines()
{
    if(!(visibleSource && visibleDestination)){
        return;
    }

    QRectF sRect = visibleSource->minimumRect();
    QRectF dRect = visibleDestination->minimumRect();

    sRect.setWidth(visibleSource->boundingRect().width());
    dRect.setWidth(visibleDestination->boundingRect().width());


    QPointF srcTL = visibleSource->mapToScene(sRect.topLeft());
    QPointF srcBR = visibleSource->mapToScene(sRect.bottomRight());

    QPointF dstTL = visibleDestination->mapToScene(dRect.topLeft());
    QPointF dstBR = visibleDestination->mapToScene(dRect.bottomRight());

    QRectF srcRect(srcTL, srcBR);
    QRectF dstRect(dstTL, dstBR);
    //QRectF srcRect = visibleSource->sceneBoundingRect();
    //QRectF dstRect = visibleDestination->sceneBoundingRect();

    QPointF sceneCenter = (srcRect.center() + dstRect.center())/2;



    LINE_SIDE srcSide = LEFT;
    LINE_SIDE dstSide = LEFT;
    LINE_DIRECTION srcDir = DOWN;
    LINE_DIRECTION dstDir = DOWN;

    //Work out if the Source's Center Point is on the left or the right of the EdgeCenterPoint
    if(srcRect.center().x() <= sceneCenter.x()){
        srcSide = RIGHT;
    }

    //Work out if the Destination's Center Point is on the left or the right of the EdgeCenterPoint
    if(dstRect.center().x() <= sceneCenter.x()){
        dstSide = RIGHT;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(srcRect.center().y() <= sceneCenter.y()){
        srcDir = UP;
    }

    //Work out if the Source's Center Point is above or below the EdgeCenterPoint
    if(dstRect.center().y() <= sceneCenter.y()){
        dstDir = UP;
    }


    bool removeSrcEdge = visibleSource->getIndexOfEdgeItem(getID(), srcSide == RIGHT) == -1;
    bool removeDstEdge = visibleDestination->getIndexOfEdgeItem(getID(), dstSide == RIGHT) == -1;

    if(removeSrcEdge){
        visibleSource->removeVisibleParentForEdgeItem(getID());
        visibleSource->setVisibleParentForEdgeItem(getID(), srcSide == RIGHT);
        //return;
    }

    if(removeDstEdge){
        visibleDestination->removeVisibleParentForEdgeItem(getID());
        visibleDestination->setVisibleParentForEdgeItem(getID(), dstSide == RIGHT);
        //return;
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
    int srcEdgeCount = visibleSource->getNumberOfEdgeItems(srcSide == RIGHT);
    int srcIndex = visibleSource->getIndexOfEdgeItem(getID(), srcSide == RIGHT);




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
    int dstEdgeCount = visibleDestination->getNumberOfEdgeItems(dstSide == RIGHT);
    int dstIndex = visibleDestination->getIndexOfEdgeItem(getID(), dstSide == RIGHT);


    qreal dstYOffset = (EDGE_GAP_RATIO * dstRect.height()) + (((dstIndex + 1.0) / (dstEdgeCount + 1.0)) * (EDGE_SPACE_RATIO * dstRect.height()));
    edgeDst.setY(dstRect.top() + dstYOffset);


    //Calculate Arrow Heads
    qreal arrowHeight = ARROW_HEIGHT;

    QPointF arrowTailTL = edgeSrc -  QPointF(0, (arrowHeight / 2));
    QPointF arrowTailMR = edgeSrc +  (srcArrowSideFlip * QPointF(arrowHeight, 0));
    QPointF arrowTailBL = edgeSrc +  QPointF(0, (arrowHeight / 2));
    QPolygonF arrowTail;
    arrowTail.append(arrowTailTL);
    arrowTail.append(arrowTailMR);
    arrowTail.append(arrowTailBL);
    arrowTail.append(arrowTailTL);



    QPointF xOffset = QPointF((-dstArrowSideFlip) * arrowHeight, 0);
    QPointF arrowHeadTL = edgeDst + xOffset -  QPointF(0, (arrowHeight / 2));
    QPointF arrowHeadMR = edgeDst + xOffset + (dstArrowSideFlip * QPointF(arrowHeight, 0));
    QPointF arrowHeadBL = edgeDst + xOffset + QPointF(0, (arrowHeight / 2));

    QPolygonF arrowHead;
    arrowHead.append(arrowHeadTL);
    arrowHead.append(arrowHeadMR);
    arrowHead.append(arrowHeadBL);
    arrowHead.append(arrowHeadTL);



    QPointF srcStart = arrowTailMR + xOffset + QPointF((srcArrowSideFlip) * ARROW_HEIGHT, 0);
    QPointF dstEnd = arrowHeadMR + QPointF((-dstArrowSideFlip) * ARROW_HEIGHT, 0);


    //Set the center point of the circle
    QPointF center = parent->mapFromScene((srcStart + dstEnd) / 2);
    setCenterPos(center);








    arrowHeadMR = mapFromScene(arrowHeadMR + xOffset);
    arrowTailMR = mapFromScene(arrowTailMR);



    qreal srcArrowLength = abs((-CIRCLE_RADIUS - arrowTailMR.x())/2);
    qreal dstArrowLength = abs((CIRCLE_RADIUS - arrowHeadMR.x())/2);
    qreal srcArrowOffset = (srcRect.height() /2) - srcYOffset;
    qreal dstArrowOffset = (dstRect.height() /2) - dstYOffset;


    if(srcArrowOffset < (srcRect.height()/2) && srcDir == UP){
        srcArrowLength += srcArrowOffset;
    }else{
        srcArrowLength -= srcArrowOffset;
    }

    if(dstArrowOffset < (dstRect.height()/2) && dstDir == UP){
        dstArrowLength += dstArrowOffset;
    }else{
        dstArrowLength -= dstArrowOffset;
    }

    srcArrowLength = qMax(0.0, srcArrowLength);
    dstArrowLength = qMax(0.0, dstArrowLength);



    QPointF arrowTailEnd = arrowTailMR + QPointF((srcArrowSideFlip) * srcArrowLength, 0);
    QPointF arrowHeadEnd = arrowHeadMR + QPointF((-dstArrowSideFlip) * dstArrowLength, 0);

    QPointF headPoint = QPointF(arrowTailEnd.x(), 0);
    QPointF tailPoint = QPointF(arrowHeadEnd.x(), 0);

    QPointF cirleModifier = QPointF(CIRCLE_RADIUS,0);
    QPointF circleLeftPoint = - cirleModifier;
    QPointF circleRightPoint = cirleModifier;




    lineSegments[0]->setLine(QLineF(arrowTailMR, arrowTailEnd));

    lineSegments[1]->setLine(QLineF(arrowTailEnd, headPoint));
    lineSegments[2]->setLine(QLineF(headPoint, circleLeftPoint));
    lineSegments[3]->setLine(QLineF(circleRightPoint, tailPoint));
    lineSegments[4]->setLine(QLineF(tailPoint, arrowHeadEnd));
    lineSegments[5]->setLine(QLineF(arrowHeadEnd, arrowHeadMR));

    arrowHead = mapFromScene(arrowHead);
    arrowTail = mapFromScene(arrowTail);
    arrowHeadItem->setPolygon(arrowHead);
    arrowTailItem->setPolygon(arrowTail);




    //update();
}
