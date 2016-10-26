#include "edgeitem.h"
#include "../Node/nodeitem.h"

#include <cmath>

#define ARROW_SIZE 4.0
EdgeItem::EdgeItem(EdgeViewItem* edgeViewItem, NodeItem * parent, NodeItem* source, NodeItem* destination):EntityItem(edgeViewItem, parent, EntityItem::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->currentSrcItem = 0;
    this->destinationItem = destination;
    this->currentDstItem = 0;

    this->margins = QMarginsF(10,10,10,10);
    _hasPosition = false;

    setMoveEnabled(true);
    setSelectionEnabled(true);
    setExpandEnabled(false);


    QPen pen;
    pen.setWidthF(1);
    pen.setCapStyle(Qt::FlatCap);


    //Add the edge to the parent
    if(parent){
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

    //Bind all the source's ancestors all the way to the shared parent.
    NodeItem* srcParent = sourceItem;
    while(srcParent && srcParent != parent){
        connect(srcParent, &EntityItem::visibleChanged, this, &EdgeItem::sourceParentVisibilityChanged);
        connect(srcParent, &EntityItem::positionChanged, this, &EdgeItem::sourceParentMoved);
        srcParent = srcParent->getParentNodeItem();
    }

    //Bind all the destinations's ancestors all the way to the shared parent.
    NodeItem* dstParent = destinationItem;
    while(dstParent && dstParent != parent){
        connect(dstParent, &EntityItem::visibleChanged, this, &EdgeItem::destinationParentVisibilityChanged);
        connect(dstParent, &EntityItem::positionChanged, this, &EdgeItem::destinationParentMoved);
        dstParent = dstParent->getParentNodeItem();
    }

    //When ever the center point changes we should update the curves.
    connect(this, SIGNAL(positionChanged()), this, SLOT(centerPointMoved()));

    sourceParentVisibilityChanged();
    destinationParentVisibilityChanged();

    setZValue(Edge::EC_UNDEFINED - edgeViewItem->getEdgeKind());
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
    if(itemPos != pos){
        itemPos = pos;
        emit positionChanged();
    }
}

QPointF EdgeItem::getPos() const
{
    return itemPos;
}

QRectF EdgeItem::sourceIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(sourceIconCircle().center());
    return r;
}

QRectF EdgeItem::sourceIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));
    r.moveCenter(translatedCenterRect().center());
    r.moveRight(translatedCenterRect().left());
    return r;

}

QRectF EdgeItem::centerIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize() * 2);
    r.moveCenter(translatedCenterRect().center());
    return r;
}


QRectF EdgeItem::destinationIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveCenter(destinationIconCircle().center());
    return r;
}

QRectF EdgeItem::destinationIconCircle() const
{
    QRectF  r;
    r.setSize(QSizeF(10,10));
    r.moveCenter(translatedCenterRect().center());
    r.moveLeft(translatedCenterRect().right());
    return r;
}


QRectF EdgeItem::centerRect() const
{
    QRectF  r;
    if(hasSetPosition()){
        r.setSize(QSizeF(20,20));
    }else{
        r.setSize(QSizeF(16,16));
    }
    return r;
}

QRectF EdgeItem::translatedCenterRect() const
{
    QRectF r = centerRect();
    r.moveCenter(itemPos + QPointF(10,10));
    return r;
}

QPainterPath EdgeItem::trianglePath(QPointF startPoint, bool pointRight) const
{
    QPainterPath path;

    int x = pointRight ? ARROW_SIZE: -ARROW_SIZE;
    QPointF peakPoint = startPoint + QPointF(x,0);
    QPointF topBasePoint = startPoint - QPointF(0, ARROW_SIZE/2);
    QPointF botBasePoint = startPoint + QPointF(0, ARROW_SIZE/2);

    path.moveTo(peakPoint);
    path.lineTo(topBasePoint);
    path.lineTo(botBasePoint);
    path.lineTo(peakPoint);
    return path;
}

NodeItem *EdgeItem::getVisibleSource() const
{
    if(currentSrcItem){
        return currentSrcItem;
    }
    return sourceItem;
}

NodeItem *EdgeItem::getVisibleDestination() const
{
    if(currentDstItem){
        return currentDstItem;
    }
    return destinationItem;
}

NodeItem *EdgeItem::getParentNodeItem() const
{
    EntityItem* parent = getParent();
    if(parent && parent->isNodeItem()){
        return (NodeItem*) parent;
    }
    return 0;
}

NodeItem *EdgeItem::getSourceItem() const
{
    return sourceItem;
}

NodeItem *EdgeItem::getDestinationItem() const
{
    return destinationItem;
}

void EdgeItem::sourceParentMoved()
{
    recalcSrcCurve(true);
}

void EdgeItem::destinationParentMoved()
{
    recalcDstCurve(true);
}

void EdgeItem::centerPointMoved()
{
    recalcSrcCurve(false);
    recalcDstCurve(true);
}

void EdgeItem::recalcSrcCurve(bool reset)
{
    QPointF srcScenePos = getSourcePos();
    bool srcLeft = sourceExitsLeft();

    QPointF centerSceneSrcPos = getSceneEdgeTermination(true);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());

    qreal offset = fabs(srcScenePos.y() - centerSceneSrcPos.y());

    if(srcLeft){
        srcControlPoint1.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
        srcControlPoint2.rx() = qMin(srcScenePos.x(), centerSceneSrcPos.x()) - offset;
    }else{
        //Different
        srcControlPoint1.rx() = ((srcScenePos.x() + centerSceneSrcPos.x()) / 2);
        srcControlPoint2.rx() = ((srcScenePos.x() + centerSceneSrcPos.x()) / 2);
    }

    QPainterPath srcCurve;
    srcCurve.moveTo(mapFromScene(centerSceneSrcPos));
    srcCurve.cubicTo(mapFromScene(srcControlPoint2), mapFromScene(srcControlPoint1), mapFromScene(srcScenePos));




    if(sourceCurve != srcCurve){
        prepareGeometryChange();
        sourceArrow = trianglePath(mapFromScene(centerSceneSrcPos), true);
        sourceCurve = srcCurve;
        if(reset && !hasSetPosition()){
            resetPosition();
        }
    }
}

void EdgeItem::recalcDstCurve(bool reset)
{
    QPointF dstScenePos = getDestinationPos();
    bool dstLeft = destinationEntersLeft();

    QPointF centerSceneDstPos = getSceneEdgeTermination(false);

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    qreal offset = fabs(dstScenePos.y() - centerSceneDstPos.y());

    //Both into right
    if(!dstLeft){
        dstControlPoint1.rx() = qMax(dstScenePos.x(), centerSceneDstPos.x()) + offset;
        dstControlPoint2.rx() = qMax(dstScenePos.x(), centerSceneDstPos.x()) +  offset;
    }else{
        //Different
        dstControlPoint1.rx() = ((dstScenePos.x() + centerSceneDstPos.x()) / 2);
        dstControlPoint2.rx() = ((dstScenePos.x() + centerSceneDstPos.x()) / 2);
    }

    QPainterPath dstCurve;
    dstCurve.moveTo(mapFromScene(centerSceneDstPos));
    dstCurve.cubicTo(mapFromScene(dstControlPoint1), mapFromScene(dstControlPoint2), mapFromScene(dstScenePos));

    if(destinationCurve != dstCurve){
        prepareGeometryChange();
        destinationArrow = trianglePath(mapFromScene(dstScenePos), dstLeft);
        destinationCurve = dstCurve;
        if(reset && !hasSetPosition()){
            resetPosition();
        }

    }
}

void EdgeItem::setManuallyPositioned(bool value)
{
    if(_hasPosition != value){
       _hasPosition = value;
       if(!_hasPosition){
           resetPosition();
       }
       update();
    }
}

bool EdgeItem::hasSetPosition() const
{
    return _hasPosition || isMoving();
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
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
        path.addEllipse(translatedCenterRect());

        if(isSelected()){
            //If we are selected add the circles to the left/right
            path.addEllipse(sourceIconCircle());
            path.addEllipse(destinationIconCircle());
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
        paintPixmap(painter, lod, ER_MAIN_ICON, sourceItem->getIconPath());
        paintPixmap(painter, lod, ER_SECONDARY_ICON, destinationItem->getIconPath());
    }
}

QRectF EdgeItem::currentRect() const
{
    QRectF r = translatedCenterRect();
    r = r.marginsAdded(margins);
    return r;
}

void EdgeItem::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        setManuallyPositioned(true);
    }
    EntityItem::dataChanged(keyName, data);
}

void EdgeItem::dataRemoved(QString keyName)
{
    if(keyName == "x" || keyName == "y"){
        setManuallyPositioned(hasData("x") && hasData("y"));
    }
}

QRectF EdgeItem::getElementRect(EntityItem::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_EDGE_KIND_ICON:{
            return centerIconRect();
        }
        case ER_MAIN_ICON:{
            return sourceIconRect();
        }
        case ER_SECONDARY_ICON:{
            return destinationIconRect();
        }
        default:
        break;
    }
    //Just call base class.
    return EntityItem::getElementRect(rect);
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

    if(hasSetPosition()){
        setPos(getNearestGridPoint());
    }
    return EntityItem::setMoveFinished();
}


void EdgeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(hasSetPosition() && event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetPosition();
    }
}

QPointF EdgeItem::getTopLeftOffset() const
{
    return QPointF(10,10);//centerRect().center();
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
            path.addEllipse(translatedCenterRect());
            return path;
        }
    default:
        break;
    }
    return EntityItem::getElementPath(rect);

}

/**
 * @brief EdgeItemNew::boundingRect - Calculates the Rectangle of this edge by combining all paths together.
 * @return
 */
QRectF EdgeItem::boundingRect() const
{
    QRectF r = currentRect();
    r = r.united(sourceArrow.controlPointRect());
    r = r.united(sourceCurve.controlPointRect());
    r = r.united(destinationArrow.controlPointRect());
    r = r.united(destinationCurve.controlPointRect());
    return r;
}


QPointF EdgeItem::getSourcePos(QPointF center) const
{
    NodeItem* item = getVisibleSource();
    if(item){
        return item->getSceneEdgeTermination(sourceExitsLeft(center));
    }
    return QPointF();
}

bool EdgeItem::sourceExitsLeft(QPointF center) const
{
    NodeItem* item = getVisibleSource();
    if(item){
        if(center.isNull()){
            center = getSceneCenter();
        }
        QPointF srcRightPos = item->getSceneEdgeTermination(false);
        return (srcRightPos.x() > (center.x() - 8));
    }
    return false;
}

bool EdgeItem::destinationEntersLeft(QPointF center) const
{
    NodeItem* item = getVisibleDestination();
    if(item){
        if(center.isNull()){
            center = getSceneCenter();
        }
        QPointF dstRightPos = item->getSceneEdgeTermination(true);
        return (dstRightPos.x() > (center.x() + 8));;
    }
    return false;
}

QPointF EdgeItem::getDestinationPos(QPointF center) const
{
    NodeItem* item = getVisibleDestination();
    if(item){
        bool entersLeft = destinationEntersLeft(center);
        QPointF offset = QPointF(entersLeft? -ARROW_SIZE:ARROW_SIZE, 0);
        return item->getSceneEdgeTermination(entersLeft) + offset;
    }
    return QPointF();
}


QPointF EdgeItem::getSceneEdgeTermination(bool left) const
{
    qreal y = translatedCenterRect().center().y();
    qreal x = left ? translatedCenterRect().left() - ARROW_SIZE: translatedCenterRect().right();
    return mapToScene(x,y);
}

void EdgeItem::resetPosition()
{
    QPointF srcCenter;
    QPointF dstCenter;

    if(getVisibleSource()){
        srcCenter = getVisibleSource()->sceneBoundingRect().center();
    }

    if(getVisibleDestination()){
        dstCenter = getVisibleDestination()->sceneBoundingRect().center();
    }

    //Get the scene center of the 2 ends points to spoof the location of the center.
    QPointF midPoint = (srcCenter + dstCenter) / 2;

    bool onLeft = destinationEntersLeft(midPoint);

    //Calculate the accurate center position for edge, using our spoofed center point (Take into account the arrows position)
    QPointF requiredPos = (getSourcePos(midPoint) + getDestinationPos(midPoint) + QPointF(onLeft? ARROW_SIZE:-ARROW_SIZE, 0)) / 2;


    //Map it back from scene pos
    QPointF newCenterPos = mapFromScene(requiredPos);

    //Set it, which will in turn update the edges.
    setPos(newCenterPos - getTopLeftOffset());
    //setCenter(newCenterPos);

    //Add an undo step, and remove X/Y data.
    if(_hasPosition){
        emit req_triggerAction("Resetting Edge Position");
        removeData("x");
        removeData("y");
    }
}

QPointF EdgeItem::getSceneCenter() const
{
    return mapToScene(getPos() + getTopLeftOffset());
}

void EdgeItem::sourceParentVisibilityChanged()
{
    //Unset the size changed variable.
    if(currentSrcItem){
        disconnect(currentSrcItem, &EntityItem::sizeChanged, this, &EdgeItem::sourceParentMoved);
    }

    //Find the first visible parent from sourceItem
    currentSrcItem = sourceItem;
    while(currentSrcItem){
        if(currentSrcItem->isVisible()){
            break;
        }
        currentSrcItem = currentSrcItem->getParentNodeItem();
    }
    if(currentSrcItem){
        connect(currentSrcItem, &EntityItem::sizeChanged, this, &EdgeItem::sourceParentMoved);
        //Recalculate the source curve.
        recalcSrcCurve(true);
    }
}

void EdgeItem::destinationParentVisibilityChanged()
{
    //Unset the size changed variable.
    if(currentDstItem){
        disconnect(currentDstItem, &EntityItem::sizeChanged, this, &EdgeItem::destinationParentMoved);
    }

    //Find the first visible parent from destinationItem
    currentDstItem = destinationItem;
    while(currentDstItem){
        if(currentDstItem->isVisible()){
            break;
        }
        currentDstItem = currentDstItem->getParentNodeItem();
    }

    if(currentDstItem){
        connect(currentDstItem, &EntityItem::sizeChanged, this, &EdgeItem::destinationParentMoved);
        //Recalculate the destination curve.
        recalcDstCurve(true);
    }
}
