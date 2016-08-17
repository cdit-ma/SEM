#include "edgeitemnew.h"
#include "nodeitemnew.h"
#include <QDebug>

EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew * parent, NodeItemNew* source, NodeItemNew* destination, KIND edge_kind):EntityItemNew(edgeViewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->destinationItem = destination;
    this->edge_kind = edge_kind;
    this->parentItem = parent;
    manuallySet = false;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(true);

    if(parent){
        parent->addChildEdge(this);
    }

    this->addRequiredData("x");
    this->addRequiredData("y");

    connect(sourceItem, SIGNAL(scenePosChanged()), this, SLOT(sourceMoved()));
    connect(destinationItem, SIGNAL(scenePosChanged()), this, SLOT(destinationMoved()));
    connect(this, SIGNAL(positionChanged()), this, SLOT(centerMoved()));

    resetPosition();
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

    QPointF centerSceneSrcPos = mapToScene(currentRect().center()) - QPointF(currentRect().width() /2,0);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());

    if(centerSceneSrcPos.x() < srcScenePos.x()){
        qreal delta = 2 * (srcScenePos.x() - centerSceneSrcPos.x());
        srcControlPoint1.rx() += delta;
        srcControlPoint2.rx() -= delta;
    }

    QPainterPath srcCurve;
    srcCurve.moveTo(mapFromScene(srcScenePos));
    srcCurve.cubicTo(mapFromScene(srcControlPoint1), mapFromScene(srcControlPoint2), mapFromScene(centerSceneSrcPos));

    prepareGeometryChange();

    sourceCurve = srcCurve;
    if(reset && shouldReset()){
        resetPosition();
    }
}

void EdgeItemNew::recalcDstCurve(bool reset)
{
    QPointF centerSceneDstPos = mapToScene(currentRect().center()) + QPointF(currentRect().width() /2,0);
    QPointF dstScenePos = getDestinationPos();

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    if(dstScenePos.x() < centerSceneDstPos.x()){
        qreal delta = 2 * (centerSceneDstPos.x() - dstScenePos.x());
        dstControlPoint1.rx() += delta;
        dstControlPoint2.rx() -= delta;
    }

    QPainterPath dstCurve;
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


    if(manuallySet){
        painter->setBrush(parentItem->getBodyColor().darker(140));
    }else{
        painter->setBrush(pen.color());
    }

    painter->drawRect(currentRect());

    if(state > RS_BLOCK){
       // paintPixmap(painter, lod, centerRect(), parentItem->getIconPath());
        paintPixmap(painter, lod, leftRect(), sourceItem->getIconPath());
        paintPixmap(painter, lod, rightRect(), destinationItem->getIconPath());
    }
}

QRectF EdgeItemNew::currentRect() const
{
    QRectF rect = QRectF(0,0,24,12);
    return rect;
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
        QPointF adjustedPos = pos() + delta;
        QPointF minPos = parentItem->gridRect().topLeft();

        //Minimize on the minimum position this item can go.
        if(adjustedPos.x() < minPos.x()){
            adjustedPos.rx() = minPos.x();
        }
        if(adjustedPos.y() < minPos.y()){
            adjustedPos.ry() = minPos.y();
        }

        //Offset by the pos() to get the restricted delta.
        delta = adjustedPos - pos();
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
}

void EdgeItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetPosition();
    }
}

QRectF EdgeItemNew::boundingRect() const
{
    QRectF r = currentRect();
    r = r.united(sourceCurve.boundingRect());
    r = r.united(destinationCurve.boundingRect());

    return r;
}

QRectF EdgeItemNew::translatedRect() const
{
    QRectF r = currentRect();
    r.translate(pos());
    return r;
}

QPointF EdgeItemNew::getSourcePos() const
{
    return sourceItem->sceneBoundingRect().center() + QPointF(sourceItem->boundingRect().width() /2 - sourceItem->getMargin().right(),0);
}

QPointF EdgeItemNew::getDestinationPos() const
{
    return destinationItem->sceneBoundingRect().center() - QPointF(destinationItem->boundingRect().width() /2 - sourceItem->getMargin().left(),0);
}

QPointF EdgeItemNew::getCenterOffset() const
{
    return currentRect().center();
}

void EdgeItemNew::resetPosition()
{
    QPointF requiredPos = (getSourcePos() + getDestinationPos()) / 2;


    QPointF newPos = parentItem->mapFromScene(requiredPos);
    prepareGeometryChange();
    setCenter(newPos);
    manuallySet = false;
}

bool EdgeItemNew::shouldReset()
{
    return !manuallySet;
    //return !manuallySet;
}

QRectF EdgeItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{

    return EntityItemNew::getElementRect(rect);
}
