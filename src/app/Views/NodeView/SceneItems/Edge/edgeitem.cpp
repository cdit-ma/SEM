#include "edgeitem.h"
#include "../Node/nodeitem.h"

#define ARROW_SIZE 4

EdgeItem::EdgeItem(EdgeViewItem *edgeViewItem, NodeItem *parent, NodeItem *source, NodeItem *destination)
	: EntityItem(edgeViewItem, parent, EDGE)
{
    //Set the margins
    margins = QMarginsF(10,10,10,10);

    src = source;
    dst = destination;

    setMoveEnabled(true);
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

    //Add the edge to the parent
    if(getParent()){
        parent->addChildEdge(this);
    }
    
    QPen pen(Qt::black);
    pen.setWidthF(.5);
    pen.setCapStyle(Qt::FlatCap);
    
    kind = edgeViewItem->getEdgeKind();
    switch (kind) {
        case EDGE_KIND::DATA:
            pen.setStyle(Qt::DotLine);
            break;
        case EDGE_KIND::ASSEMBLY:
            pen.setStyle(Qt::DashLine);
            break;
        default:
            break;
    }
    
    //Set the Pen.
    setDefaultPen(pen);

    srcAncestorVisibilityChanged();
    dstAncestorVisibilityChanged();

    auto z_order = (int)EDGE_KIND::NONE - (int)edgeViewItem->getEdgeKind();
    setDefaultZValue(z_order);

    //When ever the center point changes we should update the curves.
    connect(this, &EntityItem::positionChanged, this, &EdgeItem::updateEdge);

    //Listen to the X/Y data
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();

    addHoverFunction(EntityRect::MOVE, std::bind(&EdgeItem::moveHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::SECONDARY_ICON, std::bind(&EdgeItem::sourceIconHover, this, std::placeholders::_1, std::placeholders::_2));
    addHoverFunction(EntityRect::TERTIARY_ICON, std::bind(&EdgeItem::targetIconHover, this, std::placeholders::_1, std::placeholders::_2));
    
    connect(Theme::theme(), &Theme::theme_Changed, this, &EdgeItem::updateEdgeOpacity);
    updateEdgeOpacity();
}

void EdgeItem::updateEdgeOpacity()
{
    auto new_opacity = Theme::theme()->getInactiveEdgeOpacity();
    if(inactive_opacity_ != new_opacity){
        inactive_opacity_ = new_opacity;
        update();
    }
}

EdgeItem::~EdgeItem()
{
    disconnect(this);
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildEdge(this);
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
    QRectF r = edgeRect();
    r |= srcCurve.controlPointRect();
    r |= dstCurve.controlPointRect();
    return r;
}

QRectF EdgeItem::edgeRect() const
{
    QRectF r = translatedCenterCircleRect();
    r |= srcIconCircle();
    r |= dstIconCircle();
    r |= srcArrow.controlPointRect();
    r |= dstArrow.controlPointRect();
    r += margins;
    return r;
}

QPointF EdgeItem::getSceneEdgeTermination(EDGE_DIRECTION direction, EDGE_KIND) const
{
    return getCenterCircleTermination(direction == EDGE_DIRECTION::SOURCE, _centerPoint);
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
    Q_UNUSED(widget);
    
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    
    bool full_render = vSrc->isHovered() || vDst->isHovered() || isSelected() || isHovered();
    full_render |= vSrc->isSelected() || vDst->isSelected();
    
    painter->setClipRect(option->exposedRect);
    painter->setOpacity(full_render ? 1 : inactive_opacity_);
    painter->setBrush(Qt::blue);

    if (!isSelected()) {
        painter->save();
        painter->setCompositionMode(QPainter::CompositionMode_Plus);
    }

    QPen pen = getPen();

    if (state > RENDER_STATE::BLOCK) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);

        auto inactive_pen = pen;
        inactive_pen.setWidthF(pen.widthF() / 4.0);
        
        auto src_pen = (full_render) ? pen : inactive_pen;
        auto dst_pen = (full_render) ? pen : inactive_pen;

        //Stroke the Bezier Curbe
        painter->strokePath(srcCurve, src_pen);
        painter->strokePath(dstCurve, dst_pen);
        
        painter->setPen(Qt::NoPen);
        painter->setBrush(src_pen.color());
        painter->drawPath(srcArrow);

        painter->setBrush(dst_pen.color());
        painter->drawPath(dstArrow);
    }

    if (!isSelected()) {
        painter->restore();
    }
    
    if (state > RENDER_STATE::BLOCK) {
        pen.setStyle(Qt::SolidLine);
        painter->setPen(pen);
        painter->setBrush(getBodyColor());
        painter->drawEllipse(translatedCenterCircleRect());
    }

    auto ICON = EntityRect::MAIN_ICON;
    if (isIconVisible(ICON)) {
        paintPixmap(painter, lod, ICON, getIcon(ICON));
    }
    
    if (state > RENDER_STATE::BLOCK && isSelected()) {
        {
            ICON = EntityRect::SECONDARY_ICON;
            auto is_hovered = isHovered(ICON);
            const auto brush_color = is_hovered ? getHighlightColor() : getBodyColor();
            const auto icon_color = is_hovered ? getHighlightTextColor() : QColor();
            painter->setBrush(brush_color);
            painter->setPen(pen);
            painter->drawEllipse(srcIconCircle());
            paintPixmap(painter, lod, srcIconRect(), src->getIcon(EntityRect::MAIN_ICON), icon_color);
        }
        {
            ICON = EntityRect::TERTIARY_ICON;
            auto is_hovered = isHovered(ICON);
            const auto brush_color = is_hovered ? getHighlightColor() : getBodyColor();
            const auto icon_color = is_hovered ? getHighlightTextColor() : QColor();
            painter->setBrush(brush_color);
            painter->setPen(pen);
            painter->setPen(pen);
            painter->drawEllipse(dstIconCircle());
            paintPixmap(painter, lod, dstIconRect(), dst->getIcon(EntityRect::MAIN_ICON), icon_color);
        }
    }
}

QPainterPath EdgeItem::getElementPath(EntityRect rect) const
{
    switch (rect) {
        case EntityRect::SHAPE: {
            //Selection Area is the Center Circle and Arrow Heads
            QPainterPath path = getElementPath(EntityRect::MOVE);
            if (isSelected()) {
                path.addEllipse(srcIconCircle());
                path.addEllipse(dstIconCircle());
            } else {
                path.addPath(srcArrow);
                path.addPath(dstArrow);
            }
            return path;
        }
        default:
            break;
    }
    return EntityItem::getElementPath(rect);
}

QRectF EdgeItem::getElementRect(EntityRect rect) const
{
    switch(rect){
        case EntityRect::MOVE:
        case EntityRect::MAIN_ICON:
            return centerIconRect();
        case EntityRect::SECONDARY_ICON:
            return srcIconCircle();
        case EntityRect::TERTIARY_ICON:
            return dstIconCircle();
        default:
            break;
    }
    //Just call base class.
    return EntityItem::getElementRect(rect);
}

void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        const auto& pos = event->pos();
        if(!isCentered() && translatedCenterCircleRect().contains(pos)){
            resetCenter();
        }else if(isSelected() && srcIconRect().contains(pos)){
            vSrc->setHighlighted(false);
            emit req_centerItem(src);
        }else if(isSelected() && dstIconRect().contains(pos)){
            vDst->setHighlighted(false);
            emit req_centerItem(dst);
        }
    }
}

void EdgeItem::dataChanged(const QString& key_name, const QVariant& data)
{
    if(isDataRequired(key_name)){
        if(key_name == "x" || key_name == "y"){
            setCentered(false);
        }
        EntityItem::dataChanged(key_name, data);
    }
}

void EdgeItem::dataRemoved(const QString& key_name)
{
    if(isDataRequired(key_name)){
        if(key_name == "x" || key_name == "y"){
            auto has_x_and_y = hasData("x") && hasData("y");
            if(!has_x_and_y){
                setCentered(true);
            }
        }
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
    QPointF topPoint = startPoint + QPointF(x, -ARROW_SIZE / 2);
    QPointF bottomPoint = startPoint + QPointF(x, ARROW_SIZE / 2);

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
    QRectF rect(QPointF(),smallIconSize());
    rect.moveCenter(srcIconCircle().center());
    return rect;
}

/**
 * @brief EdgeItem::centerIconRect Gets the Icon Rect for the Edge
 * @return
 */
QRectF EdgeItem::centerIconRect() const
{
    QRectF rect(QPointF(), smallIconSize());
    rect.moveCenter(translatedCenterCircleRect().center());
    return rect;
}

/**
 * @brief EdgeItem::dstIconRect Gets the Destination Icon Rect
 * @return
 */
QRectF EdgeItem::dstIconRect() const
{
    QRectF rect(QPointF(),smallIconSize());
    rect.moveCenter(dstIconCircle().center());
    return rect;
}

/**
 * @brief EdgeItem::srcIconCircle Gets the Source Icon Circle
 * This swaps side depending on the whether the Source Edge enters the left of the Center
 * @return
 */
QRectF EdgeItem::srcIconCircle() const
{
    QRectF r;
    if(isSelected()){
        r.setSize(QSizeF(10,10));

        QRectF cR = translatedCenterCircleRect();
        r.moveCenter(cR.center());

        if(srcCurveEntersCenterLeft){
            r.moveRight(cR.left());
        }else{
            r.moveLeft(cR.right());
        }
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
    QRectF r;
    if(isSelected()){
        r.setSize(QSizeF(10,10));

        QRectF cR = translatedCenterCircleRect();
        r.moveCenter(cR.center());

        if(!srcCurveEntersCenterLeft){
            r.moveRight(cR.left());
        }else{
            r.moveLeft(cR.right());
        }
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
    QRectF r;
    if(_isCentered){
        r.setSize(QSizeF(8, 8));
    }else{
        r.setSize(QSizeF(10, 10));
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
 * @brief EdgeItem::srcAncestorVisibilityChanged Called when a Source ancestor's visibility changes. Sets the Visible Parent as the lowest visible parent of source
 */
void EdgeItem::srcAncestorVisibilityChanged()
{
    NodeItem* newSrc = getFirstVisibleParent(src);
    if(newSrc != vSrc){
        if(vSrc){
            //Disconnect old visible Src
            disconnect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
            disconnect(vSrc, &EntityItem::hoveredChanged, this, &EdgeItem::repaint);
            disconnect(vSrc, &EntityItem::selectionChanged, this, &EdgeItem::repaint);
        }
        vSrc = newSrc;
        if(vSrc){
            connect(vSrc, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
            connect(vSrc, &EntityItem::hoveredChanged, this, &EdgeItem::repaint);
            connect(vSrc, &EntityItem::selectionChanged, this, &EdgeItem::repaint);
            updateEdge();
        }
    }
}

void EdgeItem::repaint()
{
    update();
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
            disconnect(vDst, &EntityItem::hoveredChanged, this, &EdgeItem::repaint);
            disconnect(vDst, &EntityItem::selectionChanged, this, &EdgeItem::repaint);
        }
        vDst = newDst;
        if(vDst){
            connect(vDst, &EntityItem::sizeChanged, this, &EdgeItem::updateEdge);
            connect(vDst, &EntityItem::hoveredChanged, this, &EdgeItem::repaint);
            connect(vDst, &EntityItem::selectionChanged, this, &EdgeItem::repaint);
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
    
    QPainterPath curve;
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
    QPointF ctrLeft = getSceneEdgeTermination(EDGE_DIRECTION::SOURCE, kind);
    QPointF ctrRight = getSceneEdgeTermination(EDGE_DIRECTION::TARGET, kind);

    /*
     * Map of the position points.
     * [(SRC) sP1] --> [sP2 (CEN) dP1] --> [dP2 (DST)]
     */

    //Get the entry/exit points into the source/destination based off the above variables.
    QPointF sP1 = vSrc->getSceneEdgeTermination(EDGE_DIRECTION::SOURCE, kind);
    QPointF dP2 = vDst->getSceneEdgeTermination(EDGE_DIRECTION::TARGET, kind);

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
    dP2.rx() -= ARROW_SIZE;
    sP2.rx() += useOption1 ? -ARROW_SIZE : ARROW_SIZE;
    
    //Update the curves
    updateSrcCurve(sP1, sP2, false, useOption1);
    //Flipping the locations of dP1/dP2 will mean the line will start being drawn at then termination points, which will make the line look more uniform with the source.
    updateDstCurve(dP2, dP1, true, !useOption1);
}

/**
 * @brief EdgeItem::resetCenter Calculates the best center point for the EdgeItem to use, and recalculates the Source/Destination curves.
 */
void EdgeItem::resetCenter()
{
    if(vSrc && vDst){
        auto srcCenter = vSrc->getSceneEdgeTermination(EDGE_DIRECTION::SOURCE, kind);
        auto dstCenter = vDst->getSceneEdgeTermination(EDGE_DIRECTION::TARGET, kind);
        
        //Calculate new Center position
        auto center = (srcCenter + dstCenter) / 2;
        auto scene_center = mapFromScene(center);
        setPos(scene_center);

        if(!_isCentered){
            emit req_triggerAction("Resetting Edge Position");
            removeData("x");
            removeData("y");
        }
    }
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
 * @brief EdgeItem::setMoveFinished Called when the EdgeItem finishes moving.
 */
bool EdgeItem::setMoveFinished()
{
    if(!isCentered()){
        setPos(getNearestGridPoint(QPointF()));
    }
    return EntityItem::setMoveFinished();
}

void EdgeItem::sourceIconHover(bool hovered, const QPointF& pos)
{
    if(hovered){
        AddTooltip("Double-click to center on edge's source");
    }
    vSrc->setHighlighted(hovered);
}

void EdgeItem::targetIconHover(bool hovered, const QPointF& pos)
{
    if(hovered){
        AddTooltip("Double-click to center on edge's target");
    }
    vDst->setHighlighted(hovered);
}

void EdgeItem::moveHover(bool hovered, const QPointF& pos)
{
    if(hovered){
        QString tooltip = "Click and drag to move edge";
        if(!isCentered()){
            tooltip += "\nDouble-click to reset edge";
        }
        AddTooltip(tooltip);
        AddCursor(Qt::OpenHandCursor);
    }
}