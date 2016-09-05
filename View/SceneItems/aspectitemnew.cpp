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

    setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    connect(this, SIGNAL(sizeChanged(QSizeF)), this, SLOT(resetPos()));



    addRequiredData("width");
    addRequiredData("height");
    reloadRequiredData();
}

QRectF AspectItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
        case ER_PRIMARY_TEXT:
            return getMainTextRect();
        case ER_EXPANDCONTRACT:
            return getCircleRect();
        case ER_EXPANDED_STATE:
            return getExpandStateRect();
        default:
            //Call the default
            return NodeItemNew::getElementRect(rect);
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
    setBaseBodyColor(backgroundColor);
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

QRectF AspectItemNew::getExpandStateRect() const
{
     QRectF quadrantRect = getCircleRect().intersected(boundingRect());
     QRectF rect;
     qreal iconSize = SMALL_ICON_RATIO * DEFAULT_SIZE;
     rect.setSize(QSizeF(iconSize, iconSize));
     rect.moveCenter(quadrantRect.center());
     return rect;
}

int AspectItemNew::getExpandArrowRotation() const
{
    //Image starts Facing Bottom-Right. which is 135
    //However we want it to be at right angles to the edge.
    int imageOffset = 135 - 90;
    return (imageOffset + getVertexAngle(aspectVertex)) % 360;
}


QPainterPath AspectItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    QPainterPath path = NodeItemNew::getElementPath(rect);
    path.setFillRule(Qt::WindingFill);

    switch(rect){
    case ER_SELECTION:
        //Add in the Circle Rect
        path.addEllipse(getCircleRect());
        break;
    default:
        break;
    }
    return path.simplified();
}

QRectF AspectItemNew::getCircleRect() const
{
    QRectF circleRect;

    QSizeF size(DEFAULT_SIZE, DEFAULT_SIZE);
    if(isExpanded()){
        size *= 1.2;
    }

    circleRect.setSize(size);

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
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

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


    //Paint the Circle Segment
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawEllipse(getCircleRect());

    {
        QRectF arrowRect = getElementRect(ER_EXPANDED_STATE);
        painter->save();
        painter->translate(arrowRect.center());
        painter->rotate(getExpandArrowRotation());
        painter->translate(-(arrowRect.width() / 2), - (arrowRect.height() / 2));
        if(isExpanded()){
           // paintPixmap(painter, lod, arrowRect.translated(-arrowRect.topLeft()), "Actions", "Contract");
        }else{
            paintPixmap(painter, lod, arrowRect.translated(-arrowRect.topLeft()), "Actions", "Expand");
        }
        painter->restore();
    }

    //Paint the grid and stuff.
    NodeItemNew::paint(painter, option, widget);

}

void AspectItemNew::setPos(const QPointF &pos)
{
    NodeItemNew::setPos(getAspectPos());
}

QRectF AspectItemNew::viewRect() const
{
    QRectF r = NodeItemNew::viewRect();
    if(!isExpanded()){
        r = r.intersected(getCircleRect());
    }
    return r;
}
