#include "aspectitemnew.h"
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
AspectItemNew::AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect):NodeItemNew(viewItem, parentItem, NodeItemNew::ASPECT_ITEM)
{
    setAspect(aspect);

    setExpanded(true);
    setMoveEnabled(false);

    setMinimumWidth(600);
    setMinimumHeight(400);
    setDefaultPen(Qt::NoPen);

    //Setup Margins/Padding
    setMargin(QMarginsF(10,10,10,10));
    setPadding(QMarginsF(10,10,10,10));

    aspectVertex = GET_ASPECT_VERTEX(aspect);

    aspectLabel = GET_ASPECT_NAME(aspect).toUpper();
    backgroundColor = GET_ASPECT_COLOR(aspect);//.darker(150);
    mainTextColor = backgroundColor.darker(110);

    mainTextFont.setPixelSize(70);
    mainTextFont.setBold(false);

    connect(this, SIGNAL(sizeChanged(QSizeF)), this, SLOT(resetPos()));
}

QRectF AspectItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    switch(rect){
        case ER_MAIN_LABEL:
            return getMainTextRect();
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
    qreal aspectOffset = 10;

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
    QMarginsF padding = getPadding();

    qreal width = getWidth() - (padding.left() + padding.right());
    qreal height = mainTextFont.pixelSize();
    QPointF topLeft = expandedRect().bottomLeft() + QPointF(padding.left(), -height);

    return QRectF(topLeft, QSizeF(width, height));
}

QRectF AspectItemNew::getResizeRect(RECT_VERTEX vert)
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

void AspectItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(expandedRect());

    painter->setFont(mainTextFont);
    painter->setPen(mainTextColor);
    painter->drawText(getMainTextRect(), Qt::AlignCenter, aspectLabel);

    NodeItemNew::paint(painter, option, widget);
}

void AspectItemNew::setPos(const QPointF &pos)
{
    Q_UNUSED(pos);
    NodeItemNew::setPos(getAspectPos());
}

QRectF AspectItemNew::gridRect() const
{
    QRectF rect = currentRect();
    QMarginsF padding = getPadding();

    rect.adjust(padding.left(), padding.top(), -padding.right(), -padding.bottom());
    return rect;
}

