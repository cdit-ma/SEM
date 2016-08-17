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

    bezierCurve1->setPen(curvePen);
    bezierCurve2->setPen(curvePen);

    QPointF srcScenePos = sourceItem->sceneBoundingRect().center() + QPointF(sourceItem->boundingRect().width() /2 - sourceItem->getMargin().right(),0);
    QPointF dstScenePos = destinationItem->sceneBoundingRect().center() - QPointF(destinationItem->boundingRect().width() /2 - sourceItem->getMargin().left(),0);

    QPointF itemPos = mapFromScene((srcScenePos + dstScenePos)/2);
    this->setPos(itemPos);

    connect(sourceItem, SIGNAL(scenePosChanged()), this, SLOT(sourceMoved()));
    connect(destinationItem, SIGNAL(scenePosChanged()), this, SLOT(destinationMoved()));
    connect(this, SIGNAL(scenePosChanged()), this, SLOT(destinationMoved()));
}

EdgeItemNew::~EdgeItemNew()
{
    qCritical() << "EDGE DED";
    if(getParentItem()){
        getParentItem()->removeChildEdge(getID());
    }
    unsetParent();
    qCritical() << "2EDGE DED";
}

EdgeItemNew::KIND EdgeItemNew::getEdgeItemKind()
{
    return edge_kind;
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
    updatePosition();
}

void EdgeItemNew::destinationMoved()
{
    updatePosition();
}

void EdgeItemNew::updatePosition()
{
    QPointF srcScenePos = sourceItem->sceneBoundingRect().center() + QPointF(sourceItem->boundingRect().width() /2 - sourceItem->getMargin().right(),0);
    QPointF dstScenePos = destinationItem->sceneBoundingRect().center() - QPointF(destinationItem->boundingRect().width() /2 - sourceItem->getMargin().left(),0);
    QPointF centerSceneSrcPos = mapToScene(currentRect().center()) - QPointF(boundingRect().width() /2,0);
    QPointF centerSceneDstPos = mapToScene(currentRect().center()) + QPointF(boundingRect().width() /2,0);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    qreal divisor = sqrt(fabs(centerSceneDstPos.y() - dstScenePos.y()));
    qreal multiplicant = 0;//50 / (divisor <= 0 ? 1 : divisor);


    if(dstScenePos.x() < centerSceneDstPos.x()){
        qCritical() << multiplicant;
        dstControlPoint1.rx() += (2 + multiplicant) * (centerSceneDstPos.x() - dstScenePos.x());
        dstControlPoint2.rx() -= (2 + multiplicant) * (centerSceneDstPos.x() - dstScenePos.x());
    }



    QPainterPath srcCurve;
    srcCurve.moveTo(mapFromScene(srcScenePos));
    srcCurve.cubicTo(mapFromScene(srcControlPoint1), mapFromScene(srcControlPoint2), mapFromScene(centerSceneSrcPos));

    QPainterPath dstCurve;
    dstCurve.moveTo(mapFromScene(centerSceneDstPos));
    dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));


    //dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));


    bezierCurve1->setPath(srcCurve);
    bezierCurve2->setPath(dstCurve);

}

void EdgeItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    painter->setPen(Qt::NoPen);

    painter->setBrush(parentItem->getBodyColor().darker(140));
    painter->drawRect(boundingRect());
}

QRectF EdgeItemNew::currentRect() const
{
    return QRectF(0,0,18,6);
}

void EdgeItemNew::dataChanged(QString keyName, QVariant data)
{
}

QRectF EdgeItemNew::boundingRect() const
{
    return currentRect();
}

QRectF EdgeItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{

    return EntityItemNew::getElementRect(rect);
}
