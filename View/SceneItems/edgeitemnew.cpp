#include "edgeitemnew.h"
#include "nodeitemnew.h"
#include <QDebug>

#define ARROW_SIZE 5
EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew * parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind):EntityItemNew(edgeViewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->destinationItem = destination;
    this->edge_kind = edge_kind;
    this->parentItem = parent;
    this->vSrcItem = 0;
    this->vDstItem = 0;
    manuallySet = false;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(true);

    if(parent){
        parent->addChildEdge(this);
    }

    this->addRequiredData("x");
    this->addRequiredData("y");

    //connect(sourceItem, SIGNAL(scenePosChanged()), this, SLOT(sourceMoved()));
    //connect(destinationItem, SIGNAL(scenePosChanged()), this, SLOT(destinationMoved()));
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
    //resetPosition();
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
    return QRectF(0,0, currentRect().height(), currentRect().height());
}

QRectF EdgeItemNew::centerRect() const
{
    QRectF  r = smallRect();
    r.moveCenter(currentRect().center());
    return r;
}

QRectF EdgeItemNew::leftRect() const
{
    QRectF  r = smallRect();
    r.moveBottomLeft(currentRect().bottomLeft());
    return r;
}

QRectF EdgeItemNew::rightRect() const
{
    QRectF  r = smallRect();
    r.moveBottomRight(currentRect().bottomRight());
    return r;
}


QRectF EdgeItemNew::handleRect() const
{
    return QRectF(0,0,24,12);
}

QPolygonF EdgeItemNew::triangle() const
{
    QPolygonF triangle;

    //Tip
    triangle.append(QPointF(ARROW_SIZE/2,0));
    triangle.append(QPointF(-ARROW_SIZE/2,ARROW_SIZE/2));
    triangle.append(QPointF(-ARROW_SIZE/2,-ARROW_SIZE/2));
    triangle.append(QPointF(ARROW_SIZE/2,0));
    return triangle;
}

NodeItemNew *EdgeItemNew::getVisibleSource()
{
    return sourceItem;

}

NodeItemNew *EdgeItemNew::getVisibleDestination()
{
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
    QPointF srcScenePos = getSourcePos();

    QPointF centerSceneSrcPos = getSceneEdgeTermination(true);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());

    if(centerSceneSrcPos.x() < srcScenePos.x()){
        qreal delta = 2 * (srcScenePos.x() - centerSceneSrcPos.x());
        srcControlPoint1.rx() += delta;
        srcControlPoint2.rx() -= delta;
    }

    QPainterPath srcCurve;
    //srcCurve.moveTo(mapFromScene(srcScenePos));
    //srcCurve.cubicTo(mapFromScene(srcControlPoint1), mapFromScene(srcControlPoint2), mapFromScene(centerSceneSrcPos));
    srcCurve.moveTo(mapFromScene(centerSceneSrcPos));
    srcCurve.cubicTo(mapFromScene(srcControlPoint2), mapFromScene(srcControlPoint1), mapFromScene(srcScenePos));

    prepareGeometryChange();

    sourceCurve = srcCurve;
    if(reset && shouldReset()){
        resetPosition();
    }
}

void EdgeItemNew::recalcDstCurve(bool reset)
{
    QPointF centerSceneDstPos = getSceneEdgeTermination(false);
    QPointF dstScenePos = getDestinationPos();

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    if(dstScenePos.x() < centerSceneDstPos.x()){
        qreal delta = 2 * (centerSceneDstPos.x() - dstScenePos.x());
        dstControlPoint1.rx() += delta;
        dstControlPoint2.rx() -= delta;
    }

    QPainterPath dstCurve;
    //dstCurve.moveTo(mapFromScene(dstScenePos));
    //dstCurve.cubicTo(mapFromScene(dstControlPoint2), mapFromScene(dstControlPoint1), mapFromScene(centerSceneDstPos));
    dstCurve.moveTo(mapFromScene(centerSceneDstPos));
    dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));


    prepareGeometryChange();

    destinationCurve = dstCurve;

    if(reset && shouldReset()){
        resetPosition();
    }
}

void EdgeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    EntityItemNew::paint(painter, option, widget);


    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    QPen pen = getPen();

    if(!sourceItem->getData("kind").toString().contains("EventPort")){
        pen.setStyle(Qt::DashLine);
        pen.setCapStyle(Qt::FlatCap);
    }
    painter->setPen(pen);

    if(state > RS_BLOCK){
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(sourceCurve);
        painter->drawPath(destinationCurve);
    }

    painter->setPen(getPen());
    {
        painter->setBrush(pen.color());
        painter->save();
        painter->translate(mapFromScene(getSceneEdgeTermination(true) - QPointF(ARROW_SIZE/2,0)));
        painter->drawPolygon(triangle());
        painter->restore();
        painter->save();
        painter->translate(mapFromScene(getDestinationPos() - QPointF(ARROW_SIZE/2,0)));
        painter->drawPolygon(triangle());
        painter->restore();
    }
    if(manuallySet){
        painter->setBrush(parentItem->getBodyColor().darker(140));
    }else{
        painter->setBrush(pen.color());
    }

    if(state > RS_BLOCK){
        painter->drawRect(currentRect());
    }

    paintPixmap(painter, lod, leftRect(), sourceItem->getIconPath());
    paintPixmap(painter, lod, rightRect(), destinationItem->getIconPath());
}

QRectF EdgeItemNew::currentRect() const
{
    QRectF r = handleRect();
    r.moveTopLeft(itemPos);
    return r;
}

void EdgeItemNew::dataChanged(QString keyName, QVariant data)
{

    if(keyName == "x" || keyName == "y"){
        double dbl = data.toDouble();
        if(dbl != -1){
            manuallySet = true;
        }
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
    EntityItemNew::setMoving(moving);

    if(!moving){
        setCenter(getNearestGridPoint());
    }
    manuallySet = true;
    update();
}

void EdgeItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetPosition();
    }
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
    r = r.united(sourceCurve.boundingRect());
    r = r.united(destinationCurve.boundingRect());

    return r;
}


QPointF EdgeItemNew::getSourcePos() const
{
    if(vSrcItem){
        return vSrcItem->getSceneEdgeTermination(false);
    }
    return sourceItem->getSceneEdgeTermination(false);
}

QPointF EdgeItemNew::getDestinationPos() const
{
    if(vDstItem){
        return vDstItem->getSceneEdgeTermination(true);
    }
    return destinationItem->getSceneEdgeTermination(true);
}

QPointF EdgeItemNew::getCenterOffset() const
{
    return mapFromParent(itemPos) + handleRect().center();
}

QPointF EdgeItemNew::getInternalOffset() const
{
    return handleRect().center();
}

QPointF EdgeItemNew::getSceneEdgeTermination(bool left) const
{
    qreal y = currentRect().center().y();
    qreal x = left ? currentRect().left(): currentRect().right();
    return mapToScene(x,y);
}

void EdgeItemNew::resetPosition()
{
    QPointF requiredPos = (getSourcePos() + getDestinationPos()) / 2;

    QPointF newPos;
    if(parentItem){
        newPos = parentItem->mapFromScene(requiredPos);
    }
    setCenter(newPos);
    manuallySet = false;
    update();
}

bool EdgeItemNew::shouldReset()
{
    return !manuallySet;
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
