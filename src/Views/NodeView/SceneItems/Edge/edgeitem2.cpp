#include "edgeitem2.h"
#include "../Node/nodeitem.h"
#include <QDebug>

EdgeItem2::EdgeItem2(EdgeViewItem *edgeViewItem, NodeItem *parent, NodeItem *source, NodeItem *destination):EntityItem(edgeViewItem, parent, EDGE)
{
    _isCentered = true;
    this->vSrc = 0;
    this->vDst = 0;

    this->src = source;
    this->dst = destination;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(false);

    //Bind all the source's ancestors all the way to the shared parent.
    NodeItem* pSrc = src;
    while(pSrc && pSrc != parent){
        connect(pSrc, &EntityItem::visibleChanged, this, &EdgeItem2::srcParentVisibilityChanged);
        connect(pSrc, &EntityItem::positionChanged, this, &EdgeItem2::srcParentMoved);
        pSrc = pSrc->getParentNodeItem();
    }

    //Bind all the destinations's ancestors all the way to the shared parent.
    NodeItem* pDst = dst;
    while(pDst && pDst != parent){
        connect(pDst, &EntityItem::visibleChanged, this, &EdgeItem2::dstParentVisibilityChanged);
        connect(pDst, &EntityItem::positionChanged, this, &EdgeItem2::dstParentMoved);
        pDst = pDst->getParentNodeItem();
    }

    srcParentVisibilityChanged();
    dstParentVisibilityChanged();

    setZValue(Edge::EC_UNDEFINED - edgeViewItem->getEdgeKind());

    //When ever the center point changes we should update the curves.
    connect(this, &EntityItem::positionChanged, this, &EdgeItem2::centerMoved);

    _centerPoint = (vSrc->sceneBoundingRect().center() + vDst->sceneBoundingRect().center()) / 2;

    //Listen to the X/Y data
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();

}

EdgeItem2::~EdgeItem2()
{

}

void EdgeItem2::setPos(const QPointF &pos)
{
    if(_centerPoint != pos){
        _centerPoint = pos;
        emit positionChanged();
        update();
    }
}

QPointF EdgeItem2::getPos() const
{
    return _centerPoint;
}

QRectF EdgeItem2::boundingRect() const
{
    QRectF r;
    if(vDst && vSrc){
        r = r.united(vSrc->sceneBoundingRect());
        r = r.united(sceneCenterCircleRect());
        r = r.united(vDst->sceneBoundingRect());
    }
    return r;
}

QPointF EdgeItem2::getSceneEdgeTermination(bool left) const
{
    return getCenterCircleTermination(left, _centerPoint);
}

QRectF EdgeItem2::currentRect() const
{
    QRectF r = translatedCenterCircleRect();
    //r = r.marginsAdded(margins);
    return r;
}

void EdgeItem2::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor color(255,0,0,100);
    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());

    painter->setBrush(Qt::green);
    painter->drawEllipse(translatedCenterCircleRect());

    painter->setPen(Qt::black);
    painter->drawText(boundingRect(), text);

    QLineF srcLine(mapFromScene(src1), mapFromScene(src2));
    QLineF dstLine(mapFromScene(dst1), mapFromScene(dst2));
    painter->drawLine(srcLine);
    painter->drawLine(dstLine);
}

void EdgeItem2::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isCentered() && event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetCenter();
    }
}

void EdgeItem2::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        setCentered(false);
    }
    EntityItem::dataChanged(keyName, data);
}

void EdgeItem2::dataRemoved(QString keyName)
{
    if(keyName == "x" || keyName == "y"){
        setCentered(!(hasData("x") && hasData("y")));
    }
}

QPointF EdgeItem2::getCenterCircleTermination(bool left, QPointF center) const
{
    QRectF r = translatedCenterCircleRect(center);
    QPointF point = r.center();
    point.rx() += left ? -10 : 10;
    return mapToScene(point);
}

QRectF EdgeItem2::centerCircleRect() const
{
    QRectF  r;
    r.setSize(QSizeF(20,20));
    return r;
}

QRectF EdgeItem2::translatedCenterCircleRect(QPointF center) const
{
    if(center.isNull()){
        center = _centerPoint;
    }
    QRectF r = centerCircleRect();
    r.moveCenter(center);
    return r;
}

QRectF EdgeItem2::sceneCenterCircleRect() const
{
    return mapToScene(translatedCenterCircleRect()).boundingRect();
}

void EdgeItem2::srcParentVisibilityChanged()
{
    NodeItem* newSrc = getFirstVisibleParent(src);

    if(newSrc != vSrc){
        if(vSrc){
            //Disconnect old visible Src
            disconnect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem2::srcParentMoved);
        }
        vSrc = newSrc;
        if(vSrc){
            connect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem2::srcParentMoved);
            recalcSrcCurve();
        }
        qCritical() << "Current Source is: " << vSrc;
    }
}

void EdgeItem2::dstParentVisibilityChanged()
{
    NodeItem* newDst = getFirstVisibleParent(dst);

    if(newDst != vDst){
        if(vDst){
            //Disconnect old visible Dst
            disconnect(vDst, &EntityItem::sizeChanged, this, &EdgeItem2::dstParentMoved);
        }
        vDst = newDst;
        if(vDst){
            connect(vDst, &EntityItem::sizeChanged, this, &EdgeItem2::dstParentMoved);
            recalcDstCurve();
        }
        qCritical() << "Current Destination is: " << vDst;
    }
}

void EdgeItem2::srcParentMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem2::dstParentMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem2::centerMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem2::recalcSrcCurve()
{
}

void EdgeItem2::recalcDstCurve()
{
}

void EdgeItem2::recalculateEdgeDirections()
{
    if(isCentered()){
        resetCenter();
    }
    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF srcRight = vSrc->getSceneEdgeTermination(false);

    QPointF ctrLeft = getSceneEdgeTermination(true);
    QPointF ctrRight = getSceneEdgeTermination(false);

    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    QPointF dstRight = vDst->getSceneEdgeTermination(false);

    src1 = srcExitsLeft() ? srcLeft : srcRight;
    src2 = !srcLeftOfDst() ? ctrRight : ctrLeft;
    dst1 = dstExitsLeft() ? dstLeft : dstRight;
    dst2 = !srcLeftOfDst() ? ctrLeft : ctrRight;

    update();
}

void EdgeItem2::resetCenter()
{
    QPointF srcC = vSrc->sceneBoundingRect().center();
    QPointF dstC = vDst->sceneBoundingRect().center();

    //Get the scene center of the 2 ends points to spoof the location of the center.
    QPointF center = (srcC + dstC) / 2;

    bool srcLeft = srcExitsLeft(center);
    bool dstLeft = dstExitsLeft(center);

    srcC = vSrc->getSceneEdgeTermination(srcLeft);
    dstC = vDst->getSceneEdgeTermination(dstLeft);

    //Calculate new Center position
    center = (srcC + dstC) / 2;

    setPos(center);

    if(!_isCentered){
        emit req_triggerAction("Resetting Edge Position");
        removeData("x");
        removeData("y");
    }
}

bool EdgeItem2::srcExitsLeft(QPointF center) const{

    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF srcRight = vSrc->getSceneEdgeTermination(false);
    QPointF ctrLeft = getCenterCircleTermination(true, center);
    QPointF ctrRight = getCenterCircleTermination(false, center);

    bool exitsLeft = ctrLeft.x() <= srcRight.x();

    if(ctrRight.x() > srcLeft.x() && ctrLeft.x() < srcRight.x()){
        //If the center is inside the left/right of the src, check if we are on the left of the dst
        exitsLeft = srcLeftOfDst();
    }
    return exitsLeft;
}

bool EdgeItem2::dstExitsLeft(QPointF center) const{
    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    QPointF dstRight = vDst->getSceneEdgeTermination(false);
    QPointF ctrLeft = getCenterCircleTermination(true, center);
    QPointF ctrRight = getCenterCircleTermination(false, center);

    bool exitsLeft = ctrLeft.x() <= dstRight.x();

    if(ctrRight.x() > dstLeft.x() && ctrLeft.x() < dstRight.x()){
        //If the center is inside the left/right of the dst, check if we are on the left of the src
        exitsLeft = !srcLeftOfDst();
    }

    return exitsLeft;

}

bool EdgeItem2::srcLeftOfDst() const
{
    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    return srcRight.x() <= dstLeft.x();
}

NodeItem *EdgeItem2::getFirstVisibleParent(NodeItem *item)
{

    while(item && item->isHidden()){
        item = item->getParentNodeItem();
    }
    return item;
}

bool EdgeItem2::isCentered() const
{
    return _isCentered;
}

void EdgeItem2::setCentered(bool centered)
{
    if(_isCentered != centered){
        _isCentered = centered;
        if(_isCentered){
            resetCenter();
        }
    }
}
