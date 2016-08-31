#include "edgeitemnew.h"
#include "nodeitemnew.h"
#include <QDebug>

#define ARROW_SIZE 3.0
EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew * parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind):EntityItemNew(edgeViewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->destinationItem = destination;
    this->edge_kind = edge_kind;
    this->parentItem = parent;
    this->vSrcItem = 0;
    this->vDstItem = 0;
    _hasPosition = false;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(true);

    if(parent){
        parent->addChildEdge(this);
    }

    addRequiredData("x");
    addRequiredData("y");

    QPen pen;
//    /pen.setColor(QColor(50,50,50));
	if(parent){
		pen.setColor(parent->getBodyColor().darker(300));
	}
    pen.setWidthF(.5);

    if(edgeViewItem->getEdgeKind() == Edge::EC_DATA){
        pen.setStyle(Qt::DashLine);
        pen.setCapStyle(Qt::FlatCap);
    }
    setDefaultPen(pen);



    connect(this, SIGNAL(positionChanged()), this, SLOT(centerMoved()));


    NodeItemNew* sParent = sourceItem;
    while(sParent && sParent != parent){
        connect(sParent, SIGNAL(positionChanged()), this, SLOT(sourceMoved()));
        connect(sParent, SIGNAL(visibleChanged()), this, SLOT(sourceHidden()));
        sParent = sParent->getParentNodeItem();
    }

    NodeItemNew* dParent = destinationItem;
    while(dParent && dParent != parent){
        connect(dParent, SIGNAL(positionChanged()), this, SLOT(destinationMoved()));
        connect(dParent, SIGNAL(visibleChanged()), this, SLOT(destinationHidden()));
        dParent = dParent->getParentNodeItem();
    }
    sourceHidden();
    destinationHidden();
    reloadRequiredData();
}

EdgeItemNew::~EdgeItemNew()
{
    if(getParentItem()){
        getParentItem()->removeChildEdge(getID());
    }
    unsetParent();
}

EdgeItemNew::KIND EdgeItemNew::getEdgeItemKind()
{
    return edge_kind;
}

void EdgeItemNew::setPos(const QPointF &pos)
{
    //Don't do life!
    if(itemPos != pos){
        itemPos = pos;
        update();
        emit positionChanged();
    }
}

QPointF EdgeItemNew::getPos() const
{
    return itemPos;
}

QRectF EdgeItemNew::smallRect() const
{
    return QRectF(QPointF(0,0), smallIconSize());
}

QRectF EdgeItemNew::leftRect() const
{
    QRectF  r = smallRect();
    r.moveBottomLeft(rectangleRect().bottomLeft());
    return r;
}

QRectF EdgeItemNew::rightRect() const
{
    QRectF  r = smallRect();
    r.moveBottomRight(rectangleRect().bottomRight());
    return r;
}


QRectF EdgeItemNew::handleRect() const
{
    return QRectF(0,4,16,8);
}

QRectF EdgeItemNew::centerRect() const
{
    return QRectF(0, 0, 16, 16);
}

QPolygonF EdgeItemNew::sourceArrowHead() const
{
    return triangle(mapFromScene(getSceneEdgeTermination(true)));
}

QPolygonF EdgeItemNew::destinationArrowHead() const
{
    return triangle(mapFromScene(getDestinationPos()) - QPointF(ARROW_SIZE, 0));
}

QPolygonF EdgeItemNew::triangle(QPointF startPoint) const
{
    QPolygonF triangle;
    triangle.append(QPointF(ARROW_SIZE,0));
    triangle.append(QPointF(0, ARROW_SIZE/2));
    triangle.append(QPointF(0, -ARROW_SIZE/2));
    triangle.append(QPointF(ARROW_SIZE,0));

    triangle.translate(startPoint);
    return triangle;
}

NodeItemNew *EdgeItemNew::getVisibleSource() const
{
    if(vSrcItem){
        return vSrcItem;
    }
    return sourceItem;
}

NodeItemNew *EdgeItemNew::getVisibleDestination() const
{
    if(vDstItem){
        return vDstItem;
    }
    return destinationItem;
}

NodeItemNew *EdgeItemNew::getParentItem()
{
    return (NodeItemNew*)getParent();
}

NodeItemNew *EdgeItemNew::getSourceItem()
{
    return sourceItem;
}

NodeItemNew *EdgeItemNew::getDestinationItem()
{
    return destinationItem;
}

void EdgeItemNew::sourceMoved()
{
    recalcSrcCurve(true);
}

void EdgeItemNew::destinationMoved()
{
    recalcDstCurve(true);

}

void EdgeItemNew::centerMoved()
{
    recalcSrcCurve(false);
    recalcDstCurve(true);
}

void EdgeItemNew::recalcSrcCurve(bool reset)
{
    QPointF srcScenePos = getVisibleSource()->getSceneEdgeTermination(true);

    QPointF srcRightPoint = getVisibleSource()->getSceneEdgeTermination(false);


    bool srcLeft = (srcRightPoint.x() > (getSceneCenter().x() - 8));
    bool centerLeft = true;

    if(!srcLeft){
        srcScenePos = srcRightPoint;
    }

    QPointF centerSceneSrcPos = getSceneEdgeTermination(centerLeft);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());


    qreal offset = abs(srcScenePos.y() - centerSceneSrcPos.y());

    //Both into left
    if(centerLeft == srcLeft){
        srcControlPoint1.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
        srcControlPoint2.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
    }else{
        //Different
        srcControlPoint1.rx() = ((srcScenePos.x() + centerSceneSrcPos.x()) / 2);
        srcControlPoint2.rx() = ((srcScenePos.x() + centerSceneSrcPos.x()) / 2);
    }


    /*
    if(srcLeft){
        srcControlPoint1.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
    }else{
        srcControlPoint1.rx() = qMax(srcScenePos.x(), centerSceneSrcPos.x()) + offset;
    }

    if(centerLeft){
        srcControlPoint2.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
    }else{
        srcControlPoint2.rx() = qMax(srcScenePos.x(), centerSceneSrcPos.x()) + offset;
    }*/

    /*
    if(srcLeft){
        x = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
    }else{
        x = qMax(srcScenePos.x(), centerSceneSrcPos.x()) + offset;
    }

    srcControlPoint1.rx() = x;
    srcControlPoint2.rx() = x;
*/
    QPainterPath srcCurve;
    srcCurve.moveTo(mapFromScene(centerSceneSrcPos));
    srcCurve.cubicTo(mapFromScene(srcControlPoint2), mapFromScene(srcControlPoint1), mapFromScene(srcScenePos));

    prepareGeometryChange();

    sourceCurve = srcCurve;
    if(reset && !hasSetPosition()){
        resetPosition();
    }
}

void EdgeItemNew::recalcDstCurve(bool reset)
{
    QPointF dstScenePos = getVisibleDestination()->getSceneEdgeTermination(true);

    QPointF dstRightPos = getVisibleDestination()->getSceneEdgeTermination(false);


    bool dstLeft = (dstScenePos.x() > (getSceneCenter().x() + 8));
    bool centerLeft = false;

    if(!dstLeft){
        dstScenePos = dstRightPos;
    }

    QPointF centerSceneDstPos = getSceneEdgeTermination(centerLeft);

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());


    qreal offset = abs(dstScenePos.y() - centerSceneDstPos.y());

    //Both into right
    if(dstLeft == centerLeft){
        dstControlPoint1.rx() = qMax(dstScenePos.x(), centerSceneDstPos.x()) + offset;
        dstControlPoint2.rx() = qMax(dstScenePos.x(), centerSceneDstPos.x()) +  offset;
    }else{
        //Different
        dstControlPoint1.rx() = ((dstScenePos.x() + centerSceneDstPos.x()) / 2);
        dstControlPoint2.rx() = ((dstScenePos.x() + centerSceneDstPos.x()) / 2);
    }

    QPainterPath dstCurve;
    //dstCurve.moveTo(mapFromScene(dstScenePos));
    //dstCurve.cubicTo(mapFromScene(dstControlPoint2), mapFromScene(dstControlPoint1), mapFromScene(centerSceneDstPos));
    dstCurve.moveTo(mapFromScene(centerSceneDstPos));
    dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));


    prepareGeometryChange();

    destinationCurve = dstCurve;

    if(reset && !hasSetPosition()){
        resetPosition();
    }
}

void EdgeItemNew::setManuallyPositioned(bool value)
{
    if(_hasPosition != value){
       _hasPosition = value;
       if(!_hasPosition){
           resetPosition();
       }
       update();
    }
}

bool EdgeItemNew::hasSetPosition() const
{
    return _hasPosition || isMoving();
}

void EdgeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Call Base paint
    EntityItemNew::paint(painter, option, widget);

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    QPen pen = getPen();



    if(state > RS_BLOCK){
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->strokePath(sourceCurve, QPen(Qt::red));
        painter->strokePath(destinationCurve, QPen(Qt::blue));

        painter->setPen(Qt::NoPen);
        painter->setBrush(pen.color());
        painter->drawPolygon(sourceArrowHead());
        painter->drawPolygon(destinationArrowHead());
    }


    painter->setBrush(getBodyColor());

    if(state > RS_BLOCK){
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        if(hasSetPosition()){
            painter->drawEllipse(currentRect());
        }else{
            painter->drawRect(rectangleRect());
        }
    }

    paintPixmap(painter, lod, leftRect(), sourceItem->getIconPath());
    paintPixmap(painter, lod, rightRect(), destinationItem->getIconPath());

    if(state > RS_BLOCK){
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->strokePath(sourceCurve, QPen(Qt::red));
        painter->strokePath(destinationCurve, QPen(Qt::blue));
    }
}

QRectF EdgeItemNew::currentRect() const
{
    QRectF r = centerRect();
    r.moveTopLeft(itemPos);
    return r;
}

QRectF EdgeItemNew::rectangleRect() const
{
    QRectF r = handleRect();
    r.moveTopLeft(itemPos + QPointF(0,4));
    return r;
}

void EdgeItemNew::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        qreal value = data.toReal();

        QPointF oldCenter = getCenter();
        QPointF newCenter = oldCenter;

        if(keyName == "x"){
            newCenter.setX(value);
        }else if(keyName == "y"){
            newCenter.setY(value);
        }
        setManuallyPositioned(true);

        if(newCenter != oldCenter){
            setCenter(newCenter);
        }
    }
}

void EdgeItemNew::dataRemoved(QString keyName)
{
    if(keyName == "x" || keyName == "y"){
        setManuallyPositioned(hasData("x") && hasData("y"));
    }
}

QPointF EdgeItemNew::validateAdjustPos(QPointF delta)
{
    if(parentItem){
        QPointF adjustedPos = getPos() + delta;
        QPointF minPos = parentItem->gridRect().topLeft();

        //Minimize on the minimum position this item can go.
        if(adjustedPos.x() < minPos.x()){
            adjustedPos.rx() = minPos.x();
        }
        if(adjustedPos.y() < minPos.y()){
            adjustedPos.ry() = minPos.y();
        }

        //Offset by the pos() to get the restricted delta.
        delta = adjustedPos - getPos();
    }

    return EntityItemNew::validateAdjustPos(delta);

}

void EdgeItemNew::setMoving(bool moving)
{
    if(!moving){
        setCenter(getNearestGridPoint());
    }

    EntityItemNew::setMoving(moving);
    update();
}

void EdgeItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetPosition();
    }
}

QPainterPath EdgeItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_SELECTION:
        case ER_MOVE:{
         QPainterPath path;
         path.setFillRule(Qt::WindingFill);
         path.addRect(rectangleRect());
         if(hasSetPosition()){
            path.addEllipse(currentRect());
         }
         return path;
    }
    default:
        break;
    }
    return EntityItemNew::getElementPath(rect);

}

void EdgeItemNew::setCenter(QPointF center)
{
    setPos(center - getInternalOffset());
}

QPointF EdgeItemNew::getCenter() const
{
    return getPos() + getInternalOffset();
}

QRectF EdgeItemNew::boundingRect() const
{
    QRectF r = currentRect();
    r = r.united(destinationArrowHead().boundingRect());
    r = r.united(sourceArrowHead().boundingRect());
    r = r.united(sourceCurve.boundingRect());
    r = r.united(destinationCurve.boundingRect());
    return r;
}


QPointF EdgeItemNew::getSourcePos() const
{
    NodeItemNew* item = getVisibleSource();
    if(item){
        QPointF point = (item->getSceneEdgeTermination(true) + item->getSceneEdgeTermination(false)) /2;
        return point;
        //return item->getSceneEdgeTermination(sourceExitsLeft());
    }
    return QPointF();
}

bool EdgeItemNew::sourceExitsLeft() const
{
    NodeItemNew* item = getVisibleSource();
    if(item){
        return item->getSceneCenter().x() > getSceneCenter().x();
    }
    return false;
}

bool EdgeItemNew::destinationEntersLeft() const
{
    NodeItemNew* item = getVisibleDestination();
    if(item){
        return item->getSceneCenter().x() > getSceneCenter().x();
    }
    return true;
}

QPointF EdgeItemNew::getDestinationPos() const
{
    NodeItemNew* item = getVisibleDestination();
    if(item){
        QPointF point = (item->getSceneEdgeTermination(true) + item->getSceneEdgeTermination(false)) /2;
        return point;
        //return item->getSceneEdgeTermination(destinationEntersLeft());
    }
    return QPointF();
}

QPointF EdgeItemNew::getCenterOffset() const
{
    return mapFromParent(itemPos) + getInternalOffset();
}

QPointF EdgeItemNew::getInternalOffset() const
{
    return centerRect().center();
}

QPointF EdgeItemNew::getSceneEdgeTermination(bool left) const
{
    qreal y = currentRect().center().y();
    qreal x = left ? currentRect().left() - ARROW_SIZE: currentRect().right();
    return mapToScene(x,y);
}

void EdgeItemNew::resetPosition()
{
    QPointF requiredPos = (getSourcePos() + getDestinationPos()) / 2;

    QPointF newPos;
    if(parentItem){
        newPos = parentItem->mapFromScene(requiredPos);
    }else{
        newPos = mapFromScene(requiredPos);
    }
    setCenter(newPos);

    if(_hasPosition){
        emit req_triggerAction("Resetting Edge Position");
        removeData("x");
        removeData("y");
    }
}

bool EdgeItemNew::shouldReset()
{
    return !_hasPosition;
}

void EdgeItemNew::sourceHidden()
{
    NodeItemNew* item = sourceItem;

    vSrcItem = 0;
    while(item){
        if(item->isVisible()){
            vSrcItem = item;
            break;
        }
        item = item->getParentNodeItem();
    }
    if(vSrcItem){
        recalcSrcCurve(true);
    }
}

void EdgeItemNew::destinationHidden()
{
    NodeItemNew* item = destinationItem;

    vDstItem = 0;
    while(item){
        if(item->isVisible()){
            vDstItem = item;
            break;
        }
        item = item->getParentNodeItem();
    }
    if(vDstItem){
        recalcDstCurve(true);
    }
}
