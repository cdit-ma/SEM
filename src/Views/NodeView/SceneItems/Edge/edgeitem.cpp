#include "edgeitem.h"
#include "../Node/nodeitem.h"
#include <QDebug>

EdgeItem::EdgeItem(EdgeViewItem *edgeViewItem, NodeItem *parent, NodeItem *source, NodeItem *destination):EntityItem(edgeViewItem, parent, EDGE)
{
    margins = QMarginsF(10,10,10,10);

    src = source;
    dst = destination;
    vSrc = 0;
    vDst = 0;

    _isCentered = true;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(false);

    //Bind all the source's ancestors all the way to the shared parent.
    NodeItem* pSrc = src;
    while(pSrc && pSrc != parent){
        connect(pSrc, &EntityItem::visibleChanged, this, &EdgeItem::srcParentVisibilityChanged);
        connect(pSrc, &EntityItem::positionChanged, this, &EdgeItem::srcParentMoved);
        pSrc = pSrc->getParentNodeItem();
    }

    //Bind all the destinations's ancestors all the way to the shared parent.
    NodeItem* pDst = dst;
    while(pDst && pDst != parent){
        connect(pDst, &EntityItem::visibleChanged, this, &EdgeItem::dstParentVisibilityChanged);
        connect(pDst, &EntityItem::positionChanged, this, &EdgeItem::dstParentMoved);
        pDst = pDst->getParentNodeItem();
    }

    QPen pen;
    pen.setWidthF(1);
    pen.setCapStyle(Qt::FlatCap);


    //Add the edge to the parent
    if(getParent()){
        parent->addChildEdge(this);
        pen.setColor(parent->getBaseBodyColor().darker(300));
    }

    switch(edgeViewItem->getEdgeKind()){
        case Edge::EC_DATA:
            pen.setStyle(Qt::DotLine);
        break;
        case Edge::EC_ASSEMBLY:
            pen.setStyle(Qt::DashLine);
        break;
    default:
        pen.setStyle(Qt::SolidLine);
    }

    //Set the Pen.
    setDefaultPen(pen);


    srcParentVisibilityChanged();
    dstParentVisibilityChanged();

    setZValue(Edge::EC_UNDEFINED - edgeViewItem->getEdgeKind());

    //When ever the center point changes we should update the curves.
    connect(this, &EntityItem::positionChanged, this, &EdgeItem::centerMoved);

    //Listen to the X/Y data
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();
}

EdgeItem::~EdgeItem()
{
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildEdge(getID());
    }

    unsetParent();
}

void EdgeItem::setPos(const QPointF &pos)
{
    if(_centerPoint != pos){
        _centerPoint = pos;
        emit positionChanged();
    }
}

QPointF EdgeItem::getPos() const
{
    return _centerPoint;
}

QRectF EdgeItem::boundingRect() const
{
    QRectF r = currentRect();
    r = r.united(srcIconCircle());
    r = r.united(dstIconCircle());
    r = r.united(sourceArrow.controlPointRect());
    r = r.united(sourceCurve.controlPointRect());
    r = r.united(destinationArrow.controlPointRect());
    r = r.united(destinationCurve.controlPointRect());
    return r;
}

QPointF EdgeItem::getSceneEdgeTermination(bool left) const
{
    return getCenterCircleTermination(left, _centerPoint);
}

QRectF EdgeItem::currentRect() const
{
    QRectF r = translatedCenterCircleRect();
    r = r.marginsAdded(margins);
    return r;
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    painter->setClipRect(option->exposedRect);

    QPen pen = getPen();

    if(state > RS_BLOCK){
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        //Paint the bezier curves
        painter->strokePath(sourceCurve, pen);
        painter->strokePath(destinationCurve, pen);

        painter->setPen(Qt::NoPen);
        painter->setBrush(pen.color());

        //Paint the arrow Heads
        painter->drawPath(sourceArrow);
        painter->drawPath(destinationArrow);
    }

    if(state > RS_BLOCK){
        painter->setBrush(getBodyColor());
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);

        QPainterPath path;
        path.setFillRule(Qt::WindingFill);

        //Draw the center Circle/Rectangle
        path.addEllipse(translatedCenterCircleRect());

        if(isSelected()){
            //If we are selected add the circles to the left/right
            path.addEllipse(srcIconCircle());
            path.addEllipse(dstIconCircle());
        }
        path = path.simplified();

        //Stroke the path
        painter->drawPath(path);
    }
    //Draw the icons.

    if(state == RS_BLOCK){
        painter->setClipPath(getElementPath(ER_SELECTION));
    }
    paintPixmap(painter, lod, ER_EDGE_KIND_ICON, getIconPath());
    if(state > RS_BLOCK && isSelected()){
        paintPixmap(painter, lod, ER_MAIN_ICON, src->getIconPath());
        paintPixmap(painter, lod, ER_SECONDARY_ICON, dst->getIconPath());
    }
}

QPainterPath EdgeItem::getElementPath(EntityItem::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_SELECTION:{
            QPainterPath path = getElementPath(ER_MOVE);
            path.addPath(sourceArrow);
            path.addPath(destinationArrow);
            return path;
        }
        case ER_MOVE:{
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            path.addEllipse(translatedCenterCircleRect());
            return path;
        }
    default:
        break;
    }
    return EntityItem::getElementPath(rect);
}


QRectF EdgeItem::getElementRect(EntityItem::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_EDGE_KIND_ICON:{
            return centerIconRect();
        }
        case ER_MAIN_ICON:{
            return srcIconRect();
        }
        case ER_SECONDARY_ICON:{
            return dstIconRect();
        }
        default:
        break;
    }
    //Just call base class.
    return EntityItem::getElementRect(rect);
}


void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(!isCentered() && event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetCenter();
    }
}

void EdgeItem::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        setCentered(false);
    }
    EntityItem::dataChanged(keyName, data);
}

void EdgeItem::dataRemoved(QString keyName)
{
    if(keyName == "x" || keyName == "y"){
        setCentered(!(hasData("x") && hasData("y")));
    }
}

QPainterPath EdgeItem::trianglePath(QPointF endPoint, bool pointRight) const
{
    QPainterPath path;

    int arrow = 4;
    int x = pointRight ? -arrow: arrow;
    // |=>
    // <=|
    QPointF topBasePoint = endPoint + QPointF(x, -arrow/2);
    QPointF botBasePoint = endPoint + QPointF(x, arrow/2);

    path.moveTo(endPoint);
    path.lineTo(topBasePoint);
    path.lineTo(botBasePoint);
    path.lineTo(endPoint);
    return path;
}

QRectF EdgeItem::srcIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(srcIconCircle().center());
    return r;
}

QRectF EdgeItem::centerIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize() * 2);
    r.moveCenter(translatedCenterCircleRect().center());
    return r;
}

QRectF EdgeItem::dstIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(dstIconCircle().center());
    return r;
}

QRectF EdgeItem::srcIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));
    QRectF cR = translatedCenterCircleRect();
    r.moveCenter(cR.center());
    if(srcCurveP2Left){
        r.moveRight(cR.left());
    }else{
        r.moveLeft(cR.right());
    }
    return r;
}

QRectF EdgeItem::dstIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));
    QRectF cR = translatedCenterCircleRect();
    r.moveCenter(cR.center());
    if(!srcCurveP2Left){
        r.moveRight(cR.left());
    }else{
        r.moveLeft(cR.right());
    }
    return r;
}

QPointF EdgeItem::getCenterCircleTermination(bool left, QPointF center) const
{
    QRectF r = translatedCenterCircleRect(center);
    QPointF point = r.center();
    point.rx() += left ? -10 : 10;
    return mapToScene(point);
}

QRectF EdgeItem::centerCircleRect() const
{
    QRectF  r;
    if(_isCentered){
        r.setSize(QSizeF(16,16));
    }else{
        r.setSize(QSizeF(20,20));
    }
    return r;
}

QRectF EdgeItem::translatedCenterCircleRect(QPointF center) const
{
    if(center.isNull()){
        center = _centerPoint;
    }
    QRectF r = centerCircleRect();
    r.moveCenter(center);
    return r;
}

QRectF EdgeItem::sceneCenterCircleRect() const
{
    return mapToScene(translatedCenterCircleRect()).boundingRect();
}

void EdgeItem::srcParentVisibilityChanged()
{
    NodeItem* newSrc = getFirstVisibleParent(src);

    if(newSrc != vSrc){
        if(vSrc){
            //Disconnect old visible Src
            disconnect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::srcParentMoved);
        }
        vSrc = newSrc;
        if(vSrc){
            connect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::srcParentMoved);
            recalculateEdgeDirections();
        }
    }
}

void EdgeItem::dstParentVisibilityChanged()
{
    NodeItem* newDst = getFirstVisibleParent(dst);

    if(newDst != vDst){
        if(vDst){
            //Disconnect old visible Dst
            disconnect(vDst, &EntityItem::sizeChanged, this, &EdgeItem::dstParentMoved);
        }
        vDst = newDst;
        if(vDst){
            connect(vDst, &EntityItem::sizeChanged, this, &EdgeItem::dstParentMoved);
            recalculateEdgeDirections();
        }
    }
}

void EdgeItem::srcParentMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem::dstParentMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem::centerMoved()
{
    recalculateEdgeDirections();
}

void EdgeItem::recalcSrcCurve()
{
    QPainterPath srcCurve;

    //Calculate the control points for the bezier curve
    QPointF srcCtrlP1 = QPointF(srcCurveP2.x(), srcCurveP1.y());
    QPointF srcCtrlP2 = QPointF(srcCurveP1.x(), srcCurveP2.y());

    //If both ends exit the same side as each other, make our control points calculate a loop.
    if(srcCurveP1Left == srcCurveP2Left){
        //Both on the left, should shrink our control points.
        qreal yOffset = fabs(srcCurveP1.y() - srcCurveP2.y());

        if(srcCurveP1Left){
            srcCtrlP1.rx() = qMin(srcCurveP1.x(), srcCurveP2.x()) - yOffset;
        }else{
            srcCtrlP1.rx() = qMax(srcCurveP1.x(), srcCurveP2.x()) + yOffset;
        }
        srcCtrlP2.rx() = srcCtrlP1.x();
    }else{
        srcCtrlP1.rx() = ((srcCurveP1.x() + srcCurveP2.x()) / 2);
        srcCtrlP2.rx() = srcCtrlP1.x();
    }

    srcCurve.moveTo(mapFromScene(srcCurveP1));
    srcCurve.cubicTo(mapFromScene(srcCtrlP1), mapFromScene(srcCtrlP2), mapFromScene(srcCurveP2));

    this->sourceCurve = srcCurve;

    sourceArrow = trianglePath(mapFromScene(srcCurveP2), srcCurveP2Left);
    prepareGeometryChange();
}

void EdgeItem::recalcDstCurve()
{
    QPainterPath dstCurve;

    //Calculate the control points for the bezier curve
    QPointF dstCtrlP1 = QPointF(dstCurveP2.x(), dstCurveP1.y());
    QPointF dstCtrlP2 = QPointF(dstCurveP1.x(), dstCurveP2.y());

    //If both ends exit the same side as each other, make our control points calculate a loop.
    if(dstCurveP1Left == dstCurveP2Left){
        //Both on the left, should shrink our control points.
        qreal yOffset = fabs(dstCurveP1.y() - dstCurveP2.y());

        if(dstCurveP1Left){
            dstCtrlP1.rx() = qMin(dstCurveP1.x(), dstCurveP2.x()) - yOffset;
        }else{
            dstCtrlP1.rx() = qMax(dstCurveP1.x(), dstCurveP2.x()) + yOffset;
        }
        dstCtrlP2.rx() = dstCtrlP1.x();
    }else{
        dstCtrlP1.rx() = ((dstCurveP1.x() + dstCurveP2.x()) / 2);
        dstCtrlP2.rx() = dstCtrlP1.x();
    }

    dstCurve.moveTo(mapFromScene(dstCurveP1));
    dstCurve.cubicTo(mapFromScene(dstCtrlP1), mapFromScene(dstCtrlP2), mapFromScene(dstCurveP2));

    this->destinationCurve = dstCurve;

    destinationArrow = trianglePath(mapFromScene(dstCurveP2), dstCurveP1Left);
    prepareGeometryChange();
}

void EdgeItem::recalculateEdgeDirections()
{
    if(!(vSrc && vDst)){
        return;
    }
    if(isCentered()){
        resetCenter();
    }
    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF srcRight = vSrc->getSceneEdgeTermination(false);

    QPointF ctrLeft = getSceneEdgeTermination(true);
    QPointF ctrRight = getSceneEdgeTermination(false);

    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    QPointF dstRight = vDst->getSceneEdgeTermination(false);

    /*
     * [(SRC) sP1] ------>[sP2 (CEN) dp1] --> [dp2 (DST)]
     * */

    bool useSrcLeft = srcExitsLeft();
    bool useDstLeft = dstExitsLeft();

    QPointF sP1 = useSrcLeft ? srcLeft : srcRight;
    QPointF dP2 = useDstLeft ? dstLeft : dstRight;

    QLineF srcLine1(sP1, ctrLeft);
    QLineF srcLine2(sP1, ctrRight);
    QLineF dstLine1(dP2, ctrLeft);
    QLineF dstLine2(dP2, ctrRight);
    //Source into center left
    bool src2CenterLeft = (srcLine1.length() + dstLine2.length()) < (srcLine2.length() + dstLine1.length());


    QPointF sP2 = src2CenterLeft ? ctrLeft : ctrRight;
    QPointF dP1 = src2CenterLeft ? ctrRight : ctrLeft;

    if(srcCurveP1 != sP1 || srcCurveP2 != sP2){
        srcCurveP1 = sP1;
        srcCurveP2 = sP2;
        srcCurveP1Left = useSrcLeft;
        srcCurveP2Left = src2CenterLeft;
        recalcSrcCurve();
    }

    if(dstCurveP1 != dP1 || dstCurveP2 != dP2){
        dstCurveP1 = dP1;
        dstCurveP2 = dP2;
        dstCurveP1Left = useDstLeft;
        dstCurveP2Left = !src2CenterLeft;
        recalcDstCurve();
    }


}

void EdgeItem::resetCenter()
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

    setPos(mapFromScene(center));

    if(!_isCentered){
        emit req_triggerAction("Resetting Edge Position");
        removeData("x");
        removeData("y");
    }
}

bool EdgeItem::srcExitsLeft(QPointF center) const{

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

bool EdgeItem::dstExitsLeft(QPointF center) const{
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

bool EdgeItem::srcLeftOfDst() const
{
    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    return srcLeft.x() <= dstLeft.x();
}

NodeItem *EdgeItem::getFirstVisibleParent(NodeItem *item)
{

    while(item && item->isHidden()){
        item = item->getParentNodeItem();
    }
    return item;
}

bool EdgeItem::isCentered() const
{
    return _isCentered && !isMoving();
}

void EdgeItem::setCentered(bool centered)
{
    if(_isCentered != centered){
        _isCentered = centered;
        if(_isCentered){
            resetCenter();
        }
    }
}

void EdgeItem::setMoveStarted()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, true);
    }
    EntityItem::setMoveStarted();
}

bool EdgeItem::setMoveFinished()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, false);
    }

    if(!isCentered()){
        setPos(getNearestGridPoint());
    }
    return EntityItem::setMoveFinished();
}

