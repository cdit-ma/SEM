#include "edgeitemnew.h"
#include "nodeitemnew.h"
#include <QDebug>

#define ARROW_SIZE 4.0
EdgeItemNew::EdgeItemNew(EdgeViewItem* edgeViewItem, NodeItemNew * parent, NodeItemNew* source, NodeItemNew* destination):EntityItemNew(edgeViewItem, parent, EntityItemNew::EDGE)
{
    this->edgeViewItem = edgeViewItem;
    this->sourceItem = source;
    this->currentSrcItem = 0;
    this->destinationItem = destination;
    this->currentDstItem = 0;

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
    NodeItemNew* srcParent = sourceItem;
    while(srcParent && srcParent != parent){
        connect(srcParent, &EntityItemNew::visibleChanged, this, &EdgeItemNew::sourceParentVisibilityChanged);
        connect(srcParent, &EntityItemNew::positionChanged, this, &EdgeItemNew::sourceParentMoved);
        srcParent = srcParent->getParentNodeItem();
    }

    //Bind all the destinations's ancestors all the way to the shared parent.
    NodeItemNew* dstParent = destinationItem;
    while(dstParent && dstParent != parent){
        connect(dstParent, &EntityItemNew::visibleChanged, this, &EdgeItemNew::destinationParentVisibilityChanged);
        connect(dstParent, &EntityItemNew::positionChanged, this, &EdgeItemNew::destinationParentMoved);
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

EdgeItemNew::~EdgeItemNew()
{
    if(getParentNodeItem()){
        getParentNodeItem()->removeChildEdge(getID());
    }

    unsetParent();
}

void EdgeItemNew::setPos(const QPointF &pos)
{
    if(itemPos != pos){
        itemPos = pos;
        emit positionChanged();
    }
}

QPointF EdgeItemNew::getPos() const
{
    return itemPos;
}

QRectF EdgeItemNew::sourceIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveTopLeft(translatedIconsRect().topLeft() + QPointF(1,1));
    return r;
}

QRectF EdgeItemNew::destinationIconRect() const
{
    QRectF  r;
    r.setSize(smallIconSize());
    r.moveTopRight(translatedIconsRect().topRight() + QPointF(-1,1));
    return r;
}


QRectF EdgeItemNew::itemRect() const
{
    QRectF r;
    //Should be 16x16
    r.setSize(QSizeF(DEFAULT_SIZE /4, DEFAULT_SIZE / 4));//smallIconSize() * 2);
    return r;
}

QPolygonF EdgeItemNew::sourceArrowHead() const
{
    return triangle(mapFromScene(getSceneEdgeTermination(true)));
}

QPolygonF EdgeItemNew::destinationArrowHead() const
{
    return triangle(mapFromScene(getDestinationPos()), destinationEntersLeft());
}

QPolygonF EdgeItemNew::triangle(QPointF startPoint, bool pointRight) const
{
    QPolygonF triangle;
    int x = pointRight ? ARROW_SIZE: -ARROW_SIZE;

    triangle.append(QPointF(x,0));

    QPointF peakPoint = startPoint + QPointF(x,0);
    QPointF topBasePoint = startPoint - QPointF(0, ARROW_SIZE/2);
    QPointF botBasePoint = startPoint + QPointF(0, ARROW_SIZE/2);

    triangle.append(peakPoint);
    triangle.append(topBasePoint);
    triangle.append(botBasePoint);
    triangle.append(peakPoint);
    return triangle;
}

NodeItemNew *EdgeItemNew::getVisibleSource() const
{
    if(currentSrcItem){
        return currentSrcItem;
    }
    return sourceItem;
}

NodeItemNew *EdgeItemNew::getVisibleDestination() const
{
    if(currentDstItem){
        return currentDstItem;
    }
    return destinationItem;
}

NodeItemNew *EdgeItemNew::getParentNodeItem() const
{
    EntityItemNew* parent = getParent();
    if(parent && parent->isNodeItem()){
        return (NodeItemNew*) parent;
    }
    return 0;
}

NodeItemNew *EdgeItemNew::getSourceItem()
{
    return sourceItem;
}

NodeItemNew *EdgeItemNew::getDestinationItem()
{
    return destinationItem;
}

void EdgeItemNew::sourceParentMoved()
{
    recalcSrcCurve(true);
}

void EdgeItemNew::destinationParentMoved()
{
    recalcDstCurve(true);
}

void EdgeItemNew::centerPointMoved()
{
    recalcSrcCurve(false);
    recalcDstCurve(true);
}

void EdgeItemNew::recalcSrcCurve(bool reset)
{
    QPointF srcScenePos = getSourcePos();
    bool srcLeft = sourceExitsLeft();

    QPointF centerSceneSrcPos = getSceneEdgeTermination(true);

    QPointF srcControlPoint1(centerSceneSrcPos.x(), srcScenePos.y());
    QPointF srcControlPoint2(srcScenePos.x(), centerSceneSrcPos.y());

    qreal offset = abs(srcScenePos.y() - centerSceneSrcPos.y());

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
        sourceCurve = srcCurve;
        if(reset && !hasSetPosition()){
            resetPosition();
        }
    }
}

void EdgeItemNew::recalcDstCurve(bool reset)
{
    QPointF dstScenePos = getDestinationPos();
    bool dstLeft = destinationEntersLeft();

    QPointF centerSceneDstPos = getSceneEdgeTermination(false);

    QPointF dstControlPoint1(dstScenePos.x(), centerSceneDstPos.y());
    QPointF dstControlPoint2(centerSceneDstPos.x(), dstScenePos.y());

    qreal offset = abs(dstScenePos.y() - centerSceneDstPos.y());

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
        destinationCurve = dstCurve;
        if(reset && !hasSetPosition()){
            resetPosition();
        }
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
    //EntityItemNew::paint(painter, option, widget);

    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    QPen pen = getPen();

    if(state > RS_BLOCK){
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->strokePath(sourceCurve, pen);
        painter->strokePath(destinationCurve, pen);

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
            painter->drawRect(translatedIconsRect());
        }
    }

    paintPixmap(painter, lod, ER_MAIN_ICON, sourceItem->getIconPath());
    paintPixmap(painter, lod, ER_SECONDARY_ICON, destinationItem->getIconPath());
}

QRectF EdgeItemNew::translatedIconsRect() const
{
    QRectF r = currentRect();
    r.setHeight(r.height()/2);
    r.moveCenter(currentRect().center());
    return r;
}

QRectF EdgeItemNew::currentRect() const
{
    QRectF r = itemRect();
    r.moveTopLeft(itemPos);
    return r;
}

void EdgeItemNew::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "x" || keyName == "y"){
        setManuallyPositioned(true);
    }
    EntityItemNew::dataChanged(keyName, data);
}

void EdgeItemNew::dataRemoved(QString keyName)
{
    if(keyName == "x" || keyName == "y"){
        setManuallyPositioned(hasData("x") && hasData("y"));
    }
}

QRectF EdgeItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
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
    return EntityItemNew::getElementRect(rect);
}

void EdgeItemNew::setMoveStarted()
{
    NodeItemNew* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, true);
    }
    EntityItemNew::setMoveStarted();
}

bool EdgeItemNew::setMoveFinished()
{
    NodeItemNew* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        parentNodeItem->setChildMoving(this, false);
    }

    if(hasSetPosition()){
        setPos(getNearestGridPoint());
    }
    return EntityItemNew::setMoveFinished();
}


void EdgeItemNew::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(hasSetPosition() && event->button() == Qt::LeftButton && currentRect().contains(event->pos())){
        resetPosition();
    }
}

QPointF EdgeItemNew::getTopLeftOffset() const
{
    return getInternalOffset();
}

QPainterPath EdgeItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_SELECTION:
        case ER_MOVE:{
         QPainterPath path;
         path.setFillRule(Qt::WindingFill);
         path.addRect(translatedIconsRect());
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


QPointF EdgeItemNew::getSourcePos(QPointF center) const
{
    NodeItemNew* item = getVisibleSource();
    if(item){
        return item->getSceneEdgeTermination(sourceExitsLeft(center));
    }
    return QPointF();
}

bool EdgeItemNew::sourceExitsLeft(QPointF center) const
{
    NodeItemNew* item = getVisibleSource();
    if(item){
        if(center.isNull()){
            center = getSceneCenter();
        }
        QPointF srcRightPos = item->getSceneEdgeTermination(false);
        return (srcRightPos.x() > (center.x() - 8));
    }
    return false;
}

bool EdgeItemNew::destinationEntersLeft(QPointF center) const
{
    NodeItemNew* item = getVisibleDestination();
    if(item){
        if(center.isNull()){
            center = getSceneCenter();
        }
        QPointF dstRightPos = item->getSceneEdgeTermination(true);
        return (dstRightPos.x() > (center.x() + 8));;
    }
    return false;
}

QPointF EdgeItemNew::getDestinationPos(QPointF center) const
{
    NodeItemNew* item = getVisibleDestination();
    if(item){
        bool entersLeft = destinationEntersLeft(center);
        QPointF offset = QPointF(entersLeft? -ARROW_SIZE:ARROW_SIZE, 0);
        return item->getSceneEdgeTermination(entersLeft) + offset;
    }
    return QPointF();
}

QPointF EdgeItemNew::getInternalOffset() const
{
    return itemRect().center();
}

QPointF EdgeItemNew::getSceneEdgeTermination(bool left) const
{
    qreal y = currentRect().center().y();
    qreal x = left ? currentRect().left() - ARROW_SIZE: currentRect().right();
    return mapToScene(x,y);
}

void EdgeItemNew::resetPosition()
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
    //setCenter(midPoint);

    bool onLeft = destinationEntersLeft(midPoint);

    //Calculate the accurate center position for edge, using our spoofed center point (Take into account the arrows position)
    QPointF requiredPos = (getSourcePos(midPoint) + getDestinationPos(midPoint) + QPointF(onLeft? ARROW_SIZE:-ARROW_SIZE, 0)) / 2;


    //Map it back from scene pos
    QPointF newCenterPos = mapFromScene(requiredPos);

    //Set it, which will in turn update the edges.
    setCenter(newCenterPos);

    //Add an undo step, and remove X/Y data.
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

QPointF EdgeItemNew::getSceneCenter() const
{
    return mapToScene(getPos() + getInternalOffset());// + getInternalOffset())) ;
    //return itemPos + getInternalOffset();
}

void EdgeItemNew::sourceParentVisibilityChanged()
{
    //Unset the size changed variable.
    if(currentSrcItem){
        disconnect(currentSrcItem, &EntityItemNew::sizeChanged, this, &EdgeItemNew::sourceParentMoved);
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
        connect(currentSrcItem, &EntityItemNew::sizeChanged, this, &EdgeItemNew::sourceParentMoved);
        //Recalculate the source curve.
        recalcSrcCurve(true);
    }
}

void EdgeItemNew::destinationParentVisibilityChanged()
{
    //Unset the size changed variable.
    if(currentDstItem){
        disconnect(currentDstItem, &EntityItemNew::sizeChanged, this, &EdgeItemNew::destinationParentMoved);
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
        connect(currentDstItem, &EntityItemNew::sizeChanged, this, &EdgeItemNew::destinationParentMoved);
        //Recalculate the destination curve.
        recalcDstCurve(true);
    }
}
