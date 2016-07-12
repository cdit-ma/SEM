#include "aspectitemnew.h"
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
AspectItemNew::AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect):NodeItemNew(viewItem, parentItem, NodeItemNew::ASPECT_ITEM)
{
    //Set the Aspect
    setAspect(aspect);

    setExpanded(true);
    setMoveEnabled(false);

    setMinimumWidth(600);
    setMinimumHeight(400);
    setDefaultPen(Qt::NoPen);

    //Setup Margins/Padding
    setMargin(QMarginsF(16,16,16,16));
    setBodyPadding(QMarginsF(10,10,10,10));

    setupBrushes();

    //Get the Aspects Position
    aspectVertex = GET_ASPECT_VERTEX(aspect);
    //Get the Label of the Aspect
    aspectLabel = GET_ASPECT_NAME(aspect).toUpper();

    connect(this, SIGNAL(sizeChanged(QSizeF)), this, SLOT(resetPos()));
}

QRectF AspectItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    switch(rect){
        case ER_MAIN_LABEL:
            return getMainTextRect();
        case ER_EXPANDCONTRACT:
            return getCircleRect();
        default:
            return QRectF();
    }
}

void AspectItemNew::resetPos()
{
    //Force Position
    setPos(getAspectPos());
}

QPointF AspectItemNew::getAspectPos()
{
    qreal aspectOffset = 0;

    qreal width = boundingRect().width();
    qreal height = boundingRect().height();

    switch(aspectVertex){
    case RV_TOPLEFT:
        return QPointF(-aspectOffset - width, -aspectOffset - height);
    case RV_TOPRIGHT:
        return QPointF(aspectOffset, -aspectOffset - height);
    case RV_BOTTOMRIGHT:
        return QPointF(aspectOffset, aspectOffset);
    case RV_BOTTOMLEFT:
        return QPointF(-aspectOffset - width, aspectOffset);
    default:
        return QPointF(0,0);
    }
}

QRectF AspectItemNew::getMainTextRect() const
{
    QMarginsF padding = getBodyPadding();

    qreal width = getWidth() - (padding.left() + padding.right());
    qreal height = mainTextFont.pixelSize();
    QPointF topLeft = expandedRect().bottomLeft() + QPointF(padding.left(), -height);

    return QRectF(topLeft, QSizeF(width, height));
}

void AspectItemNew::setupBrushes()
{
    backgroundColor = GET_ASPECT_COLOR(getAspect());
    setBodyColor(backgroundColor);
    mainTextColor = backgroundColor.darker(110);

    mainTextFont.setPixelSize(70);
    mainTextFont.setBold(true);
}

QRectF AspectItemNew::currentRect() const
{
    if(isExpanded()){
        return NodeItemNew::currentRect();
    }else{
        return QRectF();
    }
}

void AspectItemNew::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    if (getCircleRect().contains(event->pos())) {
        handleHover(true);
    }else{
        NodeItemNew::hoverMoveEvent(event);
    }
}

void AspectItemNew::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool controlDown = event->modifiers().testFlag(Qt::ControlModifier);
    if(getCircleRect().contains(event->pos())){
        handleSelection(true, controlDown);
    }else{
        NodeItemNew::mousePressEvent(event);
    }
}

QRectF AspectItemNew::getResizeRect(RECT_VERTEX vert) const
{
    QRectF rect;

    if(aspectVertex == RV_TOPLEFT){
        if(!(vert == aspectVertex || vert == RV_LEFT || vert == RV_TOP)){
            return rect;
        }
    }else if(aspectVertex == RV_TOPRIGHT){
        if(!(vert == aspectVertex || vert == RV_TOP || vert == RV_RIGHT)){
            return rect;
        }
    }else if(aspectVertex == RV_BOTTOMRIGHT){
        if(!(vert == aspectVertex || vert == RV_BOTTOM || vert == RV_RIGHT)){
            return rect;
        }
    }else if(aspectVertex == RV_BOTTOMLEFT){
        if(!(vert == aspectVertex || vert == RV_BOTTOM || vert == RV_LEFT)){
            return rect;
        }
    }else{
        return rect;
    }

    return NodeItemNew::getResizeRect(vert);
}

QRectF AspectItemNew::getCircleRect()
{
    QRectF circleRect;
    circleRect.setSize(QSizeF(72, 72));


    QPointF centerPoint;
    switch(aspectVertex){
    case RV_TOPLEFT:
        centerPoint = boundingRect().bottomRight();
        break;
    case RV_TOPRIGHT:
        centerPoint = boundingRect().bottomLeft();
        break;
    case RV_BOTTOMRIGHT:
        centerPoint = boundingRect().topLeft();
        break;
    case RV_BOTTOMLEFT:
        centerPoint = boundingRect().topRight();
        break;
    default:
        break;
    }
    circleRect.moveCenter(centerPoint);
    return circleRect;
}

void AspectItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    if(isExpanded()){
        painter->setPen(Qt::NoPen);
        painter->setBrush(backgroundColor);
        painter->drawRect(expandedRect());

        painter->setFont(mainTextFont);
        painter->setPen(mainTextColor);
        painter->drawText(getMainTextRect(), Qt::AlignCenter, aspectLabel);

        painter->setPen(Qt::NoPen);
        painter->setBrush(backgroundColor);
    }

    NodeItemNew::paint(painter, option, widget);


    painter->setPen(getPen());
    painter->setBrush(backgroundColor);
    painter->drawEllipse(getCircleRect());
}

void AspectItemNew::setPos(const QPointF &pos)
{
    Q_UNUSED(pos);
    NodeItemNew::setPos(getAspectPos());
}
