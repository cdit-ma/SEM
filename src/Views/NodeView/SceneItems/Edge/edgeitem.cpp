#include "edgeitem.h"
#include "../Node/nodeitem.h"
#include <QDebug>

#define ARROW_SIZE 4
EdgeItem::EdgeItem(EdgeViewItem *edgeViewItem, NodeItem *parent, NodeItem *source, NodeItem *destination):EntityItem(edgeViewItem, parent, EDGE)
{
    //Set the margins
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
        connect(pSrc, &EntityItem::visibleChanged, this, &EdgeItem::srcAncestorVisibilityChanged);
        connect(pSrc, &EntityItem::positionChanged, this, &EdgeItem::updateEdge);
        pSrc = pSrc->getParentNodeItem();
    }

    //Bind all the destinations's ancestors all the way to the shared parent.
    NodeItem* pDst = dst;
    while(pDst && pDst != parent){
        connect(pDst, &EntityItem::visibleChanged, this, &EdgeItem::dstAncestorVisibilityChanged);
        connect(pDst, &EntityItem::positionChanged, this, &EdgeItem::updateEdge);
        pDst = pDst->getParentNodeItem();
    }

    //Set the Pen
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


    srcAncestorVisibilityChanged();
    dstAncestorVisibilityChanged();

    setDefaultZValue(Edge::EC_UNDEFINED - edgeViewItem->getEdgeKind());

    //When ever the center point changes we should update the curves.
    connect(this, &EntityItem::positionChanged, this, &EdgeItem::updateEdge);

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

/**
 * @brief EdgeItem::setPos Sets the center point of the EdgeItem, This overrides the standard EntityItem function.
 * @param pos - The new center position of the EdgeItem (In parent NodeItem coordinates)
 */
void EdgeItem::setPos(const QPointF &pos)
{
    if(_centerPoint != pos){
        _centerPoint = pos;
        emit positionChanged();
    }
}

/**
 * @brief EdgeItem::getPos Returns the current center point of the EdgeItem (In parent NodeItem coordinates)
 * @return - The new Center Position
 */
QPointF EdgeItem::getPos() const
{
    return _centerPoint;
}

/**
 * @brief EdgeItem::boundingRect Gets the bounding rect of the EdgeItem, combines the bounding rects of the curves and arrow heads.
 * @return
 */
QRectF EdgeItem::boundingRect() const
{
    QRectF r = currentRect();
    r = r.united(srcIconCircle());
    r = r.united(dstIconCircle());
    r = r.united(srcArrow.controlPointRect());
    r = r.united(srcCurve.controlPointRect());
    r = r.united(dstArrow.controlPointRect());
    r = r.united(dstCurve.controlPointRect());
    //Add Margins to stop edges getting cropped
    r = r.marginsAdded(margins);
    return r;
}

/**
 * @brief EdgeItem::getSceneEdgeTermination Gets the position in which the edge line would join the left/right side of the center point.
 * @param left Left or Right side
 * @return
 */
QPointF EdgeItem::getSceneEdgeTermination(bool left) const
{
    return getCenterCircleTermination(left, _centerPoint);
}

/**
 * @brief EdgeItem::currentRect Gets the currentRect for the EdgeItem, this represents the position of the Circle which represents the center point of the edge.
 * @return
 */
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
        painter->strokePath(srcCurve, pen);
        painter->strokePath(dstCurve, pen);

        painter->setPen(Qt::NoPen);
        painter->setBrush(pen.color());

        //Paint the arrow Heads
        painter->drawPath(srcArrow);
        painter->drawPath(dstArrow);
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
            //Selection Area is the Center Circle and Arrow Heads
            QPainterPath path = getElementPath(ER_MOVE);
            if(isSelected()){
                path.addEllipse(srcIconCircle());
                path.addEllipse(dstIconCircle());
            }
            path.addPath(srcArrow);
            path.addPath(dstArrow);
            return path;
        }
        case ER_MOVE:{
            //Move Area is the Center Circle
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
    //If we aren't
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

/**
 * @brief EdgeItem::calculateArrowHead Calculates a path which represents an ArrowHead, Pointing at a provided position, in a direction.
 * @param endPoint The Tip of the arrow head
 * @param pointLeft Whether the arrow header points facing left/right
 * @return
 */
QPainterPath EdgeItem::calculateArrowHead(QPointF endPoint, bool pointLeft) const
{
    QPainterPath path;

    int x = pointLeft ? -ARROW_SIZE: ARROW_SIZE;
    // |=>
    // <=|
    QPointF startPoint = endPoint - QPointF(x, 0);
    QPointF topPoint = startPoint + QPointF(x, -ARROW_SIZE/2);
    QPointF bottomPoint = startPoint + QPointF(x, ARROW_SIZE/2);

    path.moveTo(startPoint);
    path.lineTo(topPoint);
    path.lineTo(bottomPoint);
    path.lineTo(startPoint);
    return path;
}

/**
 * @brief EdgeItem::srcIconRect Gets the Source Icon Rect.
 * @return
 */
QRectF EdgeItem::srcIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(srcIconCircle().center());
    return r;
}

/**
 * @brief EdgeItem::centerIconRect Gets the Icon Rect for the Edge
 * @return
 */
QRectF EdgeItem::centerIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize() * 2);
    r.moveCenter(translatedCenterCircleRect().center());
    return r;
}

/**
 * @brief EdgeItem::dstIconRect Gets the Destination Icon Rect
 * @return
 */
QRectF EdgeItem::dstIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(dstIconCircle().center());
    return r;
}

/**
 * @brief EdgeItem::srcIconCircle Gets the Source Icon Circle
 * This swaps side depending on the whether the Source Edge enters the left of the Center
 * @return
 */
QRectF EdgeItem::srcIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));

    QRectF cR = translatedCenterCircleRect();
    r.moveCenter(cR.center());

    if(srcCurveEntersCenterLeft){
        r.moveRight(cR.left());
    }else{
        r.moveLeft(cR.right());
    }
    return r;
}

/**
 * @brief EdgeItem::dstIconCircle Gets the Destination Icon Circle
 * This swaps side depending on the whether the Destination Edge enters the left of the Center
 * @return
 */
QRectF EdgeItem::dstIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));
    QRectF cR = translatedCenterCircleRect();
    r.moveCenter(cR.center());
    if(!srcCurveEntersCenterLeft){
        r.moveRight(cR.left());
    }else{
        r.moveLeft(cR.right());
    }
    return r;
}

/**
 * @brief EdgeItem::getCenterCircleTermination Gets the left or right point of the center circle, given a particular center point.
 * @param left Get the left or right side
 * @param center The defined center point to use (If undefined, will use the current Center)
 * @return
 */
QPointF EdgeItem::getCenterCircleTermination(bool left, QPointF center) const
{
    QRectF r = translatedCenterCircleRect(center);
    QPointF point = r.center();
    point.rx() = left ? r.left() : r.right();
    return mapToScene(point);
}

/**
 * @brief EdgeItem::centerCircleRect Gets the center circle bounding rectangle (Non translated)
 * @return
 */
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

/**
 * @brief EdgeItem::translatedCenterCircleRect Gets the center circle rectangle, translated to it's actual position (In Parent Coordinates)
 * @param center
 * @return
 */
QRectF EdgeItem::translatedCenterCircleRect(QPointF center) const
{
    if(center.isNull()){
        center = _centerPoint;
    }
    QRectF r = centerCircleRect();
    r.moveCenter(center);
    return r;
}

/**
* @brief EdgeItem::sceneCenterCircleRect Returns the scene coordinate rectangle for the center circle
* @return
*/
QRectF EdgeItem::sceneCenterCircleRect() const
{
    return mapToScene(translatedCenterCircleRect()).boundingRect();
}


/**
 * @brief EdgeItem::srcAncestorVisibilityChanged Called when a Source ancestor's visibility changes. Sets the Visible Parent as the lowest visible parent of source
 */
void EdgeItem::srcAncestorVisibilityChanged()
{
    NodeItem* newSrc = getFirstVisibleParent(src);

    if(newSrc != vSrc){
        if(vSrc){
            //Disconnect old visible Src
            disconnect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
        }
        vSrc = newSrc;
        if(vSrc){
            connect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
            updateEdge();
        }
    }
}

/**
 * @brief EdgeItem::dstAncestorVisibilityChanged Called when a Destination ancestor's visibility changes. Sets the Visible Parent as the lowest visible parent of destination
 */
void EdgeItem::dstAncestorVisibilityChanged()
{
    NodeItem* newDst = getFirstVisibleParent(dst);

    if(newDst != vDst){
        if(vDst){
            //Disconnect old visible Dst
            disconnect(vDst, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
        }
        vDst = newDst;
        if(vDst){
            connect(vDst, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
            updateEdge();
        }
    }
}


/**
 * @brief EdgeItem::updateSrcCurve Updates the curve between the Visible Source and the Center Point
 * @param srcP The source end point
 * @param ctrP The center end point
 * @param srcP_Left Whether the source end point is the source's left or right end point
 * @param ctrP_Left Whether the center end point is the center's left or right end point
 */
void EdgeItem::updateSrcCurve(QPointF srcP, QPointF ctrP, bool srcP_Left, bool ctrP_Left)
{
    QPainterPath newCurve = calculateBezierCurve(srcP, ctrP, srcP_Left, ctrP_Left);

    if(newCurve != srcCurve){
        //Update the curve
        srcCurve = newCurve;
        //Calculate new arrow head, at the end of the curve, facing the correct direction
        srcArrow = calculateArrowHead(mapFromScene(ctrP), ctrP_Left);
        srcCurveEntersCenterLeft = ctrP_Left;
        prepareGeometryChange();
    }
}

/**
 * @brief EdgeItem::calculateBezierCurve Calculates a bezier curve based on the provided points.
 * @param P1 The first point in the curve
 * @param P2 The second point in the curve
 * @param P1_Left Whether the first point is on the left
 * @param P2_Left Whether the second point is on the left
 * @return
 */
QPainterPath EdgeItem::calculateBezierCurve(QPointF P1, QPointF P2, bool P1_Left, bool P2_Left) const
{
    QPainterPath curve;

    //Calculate the control points for the bezier curve, By using the swapping the X/Y coordinates from the start end points of the curve.
    QPointF ctrlP1 = QPointF(P2.x(), P1.y());
    QPointF ctrlP2 = QPointF(P1.x(), P2.y());

    //If both ends exit the same side as each other, make our control points make a singularly curved loop
    if(P1_Left == P2_Left){
        //Calculate the vertical offset between our two end points.
        qreal yOffset = fabs(P1.y() - P2.y());

        if(P1_Left){
            //Both exiting Left, so we should make the curve grow to the left of the smaller end point
            ctrlP1.rx() = qMin(P1.x(), P2.x()) - yOffset;
        }else{
            //Both exiting Right, so we should make the curve grow to the right of the bigger end point.
            ctrlP1.rx() = qMax(P1.x(), P2.x()) + yOffset;
        }
        //The X value for both points should be the same
        ctrlP2.rx() = ctrlP1.x();
    }else{
        //Both ends are on different sides, so make the control points halve the slope of the curve.
        ctrlP1.rx() = ((P1.x() + P2.x()) / 2);
        ctrlP2.rx() = ctrlP1.x();
    }

    //Move to the first point.
    curve.moveTo(mapFromScene(P1));
    //Setup our cubic bezier curve.
    curve.cubicTo(mapFromScene(ctrlP1), mapFromScene(ctrlP2), mapFromScene(P2));
    return curve;
}

/**
 * @brief EdgeItem::updateDstCurve Updates the curve between the Visible Source and the Center Point
 * @param srcP The destination end point
 * @param ctrP The center end point
 * @param srcP_Left Whether the source end point is the source's left or right end point
 * @param ctrP_Left Whether the center end point is the center's left or right end point
 */
void EdgeItem::updateDstCurve(QPointF dstP, QPointF ctrP, bool dstP_Left, bool ctrP_Left)
{
    //Calculate the dst curve backwards, so that the dashed pen will appear to feed from the center point.
    QPainterPath newCurve = calculateBezierCurve(dstP, ctrP, dstP_Left, ctrP_Left);

    if(newCurve != dstCurve){
        //Update the curve
        dstCurve = newCurve;
        //Calculate new arrow head, at the end of the curve, facing the correct direction
        dstArrow = calculateArrowHead(mapFromScene(dstP), dstP_Left);
        prepareGeometryChange();
    }
}

/**
 * @brief EdgeItem::recalculateEdgeDirections Recalculates the sides in which the edge should use and be routed. Attempts to minimize the total length of the line.
 */
void EdgeItem::updateEdge()
{
    if(!(vSrc && vDst)){
        return;
    }
    //If we are meant to be centered, reset the center position.
    if(isCentered()){
        resetCenter();
    }

    //Get the Left/Right positions for the Center Point
    QPointF ctrLeft = getSceneEdgeTermination(true);
    QPointF ctrRight = getSceneEdgeTermination(false);

    /*
     * Map of the position points.
     * [(SRC) sP1] --> [sP2 (CEN) dP1] --> [dP2 (DST)]
     * */

    //Check if given the current Center, the source/destination should exit the left side
    bool useSrcLeft = srcExitsLeft();
    bool useDstLeft = dstExitsLeft();

    //Get the entry/exit points into the source/destinatino based off the above variables.
    QPointF sP1 = vSrc->getSceneEdgeTermination(useSrcLeft);
    QPointF dP2 = vDst->getSceneEdgeTermination(useDstLeft);



    //Calculate the lines which represents the 2 options

    //Option # 1 - Source -> centerLeft centerRight - Destination
    QLineF option1_1(sP1, ctrLeft);
    QLineF option1_2(ctrRight, dP2);
    //Option # 2 - Source -> centerRight centerLeft - Destination
    QLineF option2_1(sP1, ctrRight);
    QLineF option2_2(ctrLeft, dP2);

    //Calculate the sum of the lengths
    qreal option1_length = option1_1.length() + option1_2.length();
    qreal option2_length = option2_1.length() + option2_2.length();

    //Determine if option1 is shorter than option2
    bool useOption1 = option1_length <= option2_length;

    //Determine the location of the points based the shorter option
    QPointF sP2 = useOption1 ? ctrLeft : ctrRight;
    //Allow for the Arrow head at the Source end.
    QPointF dP1 = useOption1 ? ctrRight : ctrLeft;

    //Allow for the Arrow head at the ends of the source/destination.
    dP2.rx() += useDstLeft ? -ARROW_SIZE : ARROW_SIZE;
    sP2.rx() += useOption1 ? -ARROW_SIZE : ARROW_SIZE;

    //Update the curves
    updateSrcCurve(sP1, sP2, useSrcLeft, useOption1);
    //Flipping the locations of dP1/dP2 will mean the line will start being drawn at then termination points, which will make the line look more uniform with the source.
    updateDstCurve(dP2, dP1, useDstLeft, !useOption1);
}

/**
 * @brief EdgeItem::resetCenter Calculates the best center point for the EdgeItem to use, and recalculates the Source/Destination curves.
 */
void EdgeItem::resetCenter()
{
    if(vSrc && vDst){
        QPointF srcCenter = vSrc->sceneBoundingRect().center();
        QPointF dstCenter = vDst->sceneBoundingRect().center();

        //Get the scene center of the 2 ends points to spoof the location of the center.
        QPointF center = (srcCenter + dstCenter) / 2;
        //Get the center coordinate in local coordinates
        center = mapFromScene(center);

        //Determine if, given the spoofed center, we should use the left or right or
        bool useSrcLeft = srcExitsLeft(center);
        bool useDstLeft = dstExitsLeft(center);

        //Get the positions
        srcCenter = vSrc->getSceneEdgeTermination(useSrcLeft);
        dstCenter = vDst->getSceneEdgeTermination(useDstLeft);

        //Calculate new Center position
        center = (srcCenter + dstCenter) / 2;

        setPos(mapFromScene(center));

        if(!_isCentered){
            emit req_triggerAction("Resetting Edge Position");
            removeData("x");
            removeData("y");
        }
    }
}

/**
 * @brief EdgeItem::srcExitsLeft Returns whether or not the Edge should leave the Source on the Left or right side.
 * @param center - The Center position of the EdgeItem (if unset, the stored center point will be used)
 * @return True for left, False for right
 */
bool EdgeItem::srcExitsLeft(QPointF center) const{

    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF srcRight = vSrc->getSceneEdgeTermination(false);
    //Allow for the arrow head
    QPointF ctrLeft = getCenterCircleTermination(true, center) - QPointF(ARROW_SIZE, 0);
    QPointF ctrRight = getCenterCircleTermination(false, center) + QPointF(ARROW_SIZE, 0);;

    bool exitsLeft = ctrLeft.x() <= srcRight.x();

    if(ctrRight.x() > srcLeft.x() && ctrLeft.x() < srcRight.x()){
        //If the center is inside the left/right of the src, check if we are on the left of the dst
        exitsLeft = srcLeftOfDst();
    }
    return exitsLeft;
}

/**
 * @brief EdgeItem::dstExitsLeft Returns whether or not the Edge should leave the Destination on the Left or right side.
 * @param center - The Center position of the EdgeItem (if unset, the stored center point will be used)
 * @return True for left, False for right
 */
bool EdgeItem::dstExitsLeft(QPointF center) const{
    //Allow for the arrow head
    QPointF dstLeft = vDst->getSceneEdgeTermination(true) - QPointF(ARROW_SIZE, 0);
    QPointF dstRight = vDst->getSceneEdgeTermination(false) + QPointF(ARROW_SIZE, 0);
    QPointF ctrLeft = getCenterCircleTermination(true, center);
    QPointF ctrRight = getCenterCircleTermination(false, center);

    bool exitsLeft = ctrLeft.x() <= dstRight.x();

    if(ctrRight.x() > dstLeft.x() && ctrLeft.x() < dstRight.x()){
        //If the center is inside the left/right of the dst, check if we are on the left of the src
        exitsLeft = !srcLeftOfDst();
    }

    return exitsLeft;

}

/**
 * @brief EdgeItem::srcLeftOfDst Determines if the Source is on the left of the Destination
 * @return True for src left of destination, false for src right of destination
 */
bool EdgeItem::srcLeftOfDst() const
{
    QPointF srcLeft = vSrc->getSceneEdgeTermination(true);
    QPointF dstLeft = vDst->getSceneEdgeTermination(true);
    return srcLeft.x() <= dstLeft.x();
}

/**
 * @brief EdgeItem::getFirstVisibleParent Gets the first visible parent, starting at the actual NodeItem the EdgeItem is connected to.
 * @param item the item to start checking from.
 * @return The first visible parent, or Null
 */
NodeItem *EdgeItem::getFirstVisibleParent(NodeItem *item)
{

    while(item && item->isHidden()){
        item = item->getParentNodeItem();
    }
    return item;
}

void EdgeItem::updateZValue(bool childSelected, bool childActive)
{
    EntityItem::updateZValue(childSelected, childActive);

    if(vSrc && vDst){
        vSrc->updateZValue(isSelected(), isActiveSelected());
        vDst->updateZValue(isSelected(), isActiveSelected());
    }

}

/**
 * @brief EdgeItem::isCentered Returns whether or not the EdgeItem is auto-centered or manually positioned.
 * @return
 */
bool EdgeItem::isCentered() const
{
    return _isCentered && !isMoving();
}

/**
 * @brief EdgeItem::setCentered Sets whether or not the EdgeItem is auto-centered or manually positioned.
 * @param centered
 */
void EdgeItem::setCentered(bool centered)
{
    if(_isCentered != centered){
        _isCentered = centered;
        //Size of circe has changed
        updateEdge();
    }
}

/**
 * @brief EdgeItem::setMoveStarted Called when the EdgeItem starts moving.
 */
void EdgeItem::setMoveStarted()
{
    NodeItem* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, true);
    }
    EntityItem::setMoveStarted();
}

/**
 * @brief EdgeItem::setMoveFinished Called when the EdgeItem finishes moving.
 */
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

