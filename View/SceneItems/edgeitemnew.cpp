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

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(true);

    if(parent){
        parent->addChildEdge(this);
    }

    bezierCurve1 = new QGraphicsPathItem(this);
    bezierCurve2 = new QGraphicsPathItem(this);

    QPen curvePen;
    curvePen.setCosmetic(true);
    curvePen.setWidth(2);
    curvePen.setCapStyle(Qt::SquareCap);

    curvePen.setColor(Qt::green);
    bezierCurve1->setPen(curvePen);
    curvePen.setColor(Qt::red);
    bezierCurve2->setPen(curvePen);

    //bezierCurve1->hide();
    //bezierCurve2->hide();

    QPointF srcScenePos = sourceItem->sceneBoundingRect().center() + QPointF(sourceItem->boundingRect().width() /2 - sourceItem->getMargin().right(),0);
    QPointF dstScenePos = destinationItem->sceneBoundingRect().center() - QPointF(destinationItem->boundingRect().width() /2 - sourceItem->getMargin().left(),0);

    QPointF itemPos = mapFromScene((srcScenePos + dstScenePos)/2);
    this->setPos(itemPos);

    connect(sourceItem, SIGNAL(scenePosChanged()), this, SLOT(sourceMoved()));
    connect(destinationItem, SIGNAL(scenePosChanged()), this, SLOT(destinationMoved()));
    connect(this, SIGNAL(positionChanged()), this, SLOT(centerMoved()));
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
    return QRectF(0,0, boundingRect().height(), boundingRect().height());
}

QRectF EdgeItemNew::centerRect() const
{
    QRectF  r = smallRect();
    r.moveCenter(boundingRect().center());
    return r;
}

QRectF EdgeItemNew::leftRect() const
{
    QRectF  r = smallRect();
    r.moveBottomLeft(boundingRect().bottomLeft());
    return r;
}

QRectF EdgeItemNew::rightRect() const
{
    QRectF  r = smallRect();
    r.moveBottomRight(boundingRect().bottomRight());
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
    QPointF srcScenePos = sourceItem->sceneBoundingRect().center() + QPointF(sourceItem->boundingRect().width() /2 - sourceItem->getMargin().right(),0);

    QPointF centerSceneSrcPos = mapToScene(currentRect().center()) - QPointF(boundingRect().width() /2,0);

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

    bezierCurve1->setPath(srcCurve);
}

void EdgeItemNew::destinationMoved()
{
    QPointF centerSceneDstPos = mapToScene(currentRect().center()) + QPointF(boundingRect().width() /2,0);
    QPointF dstScenePos = destinationItem->sceneBoundingRect().center() - QPointF(destinationItem->boundingRect().width() /2 - sourceItem->getMargin().left(),0);

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    if(dstScenePos.x() < centerSceneDstPos.x()){
        qreal delta = 2 * (centerSceneDstPos.x() - dstScenePos.x());
        dstControlPoint1.rx() += delta;
        dstControlPoint1.rx() -= delta;
    }

    QPainterPath dstCurve;
    dstCurve.moveTo(mapFromScene(centerSceneDstPos));
    dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));

    bezierCurve2->setPath(dstCurve);
}

void EdgeItemNew::centerMoved()
{
    sourceMoved();
    destinationMoved();
}

void EdgeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    painter->setPen(Qt::NoPen);
    painter->setBrush(parentItem->getBodyColor().darker(140));
    painter->drawRect(boundingRect());




    if(state > RS_BLOCK){
        paintPixmap(painter, lod, centerRect(), parentItem->getIconPath());
        paintPixmap(painter, lod, leftRect(), sourceItem->getIconPath());
        paintPixmap(painter, lod, rightRect(), destinationItem->getIconPath());
    }

    //painter->setPen(Qt::black);
    //painter->strokePath(bezierCurve1->path(), QPen(Qt::black));
    //painter->strokePath(bezierCurve2->path(), QPen(Qt::black));
}

QRectF EdgeItemNew::currentRect() const
{
    return QRectF(0,0,36,12);
}

void EdgeItemNew::dataChanged(QString keyName, QVariant data)
{
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
}

QRectF EdgeItemNew::boundingRect() const
{
    return currentRect();
}

QRectF EdgeItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{

    return EntityItemNew::getElementRect(rect);
}
