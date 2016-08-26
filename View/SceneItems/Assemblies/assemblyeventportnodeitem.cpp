#include "assemblyeventportnodeitem.h"
#include <QDebug>

AssemblyEventPortNodeItem::AssemblyEventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem)
{
    qreal height = DEFAULT_SIZE  * (4.0/6.0);
    qreal width = DEFAULT_SIZE * (4.0/6.0);

    setMinimumWidth(width);
    setMinimumHeight(height);
    setExpandedWidth(width);
    setExpandedHeight(height);

    setExpanded(true);

    mainTextFont.setPointSize(4);
}

void AssemblyEventPortNodeItem::setRightJustified(bool isRight)
{
    ContainerElementNodeItem::setRightJustified(isRight);
    update();
}

QRectF AssemblyEventPortNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MAIN_ICON:{
        return iconRect();
    }
    case ER_MAIN_LABEL:{
        return textRect();
    }
    case ER_QOS:{
        return bottomSubIconRect();//.marginsRemoved(QMargins(2,2,2,2));
    }
    case ER_DEPLOYED:{
        return topSubIconRect();//.marginsRemoved(QMargins(2,2,2,2));
    }
    default:
        break;
    }

    return ContainerElementNodeItem::getElementRect(rect);
}

void AssemblyEventPortNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    //painter->setClipRect(option->exposedRect);


    if(isExpanded()){
        painter->setBrush(getBodyColor());
        painter->setPen(Qt::NoPen);
        painter->drawRect(currentRect());
    }

    if(state > RS_BLOCK){
        painter->setBrush(getBodyColor().darker(110));
        painter->drawRect(textRect());
        painter->setFont(mainTextFont);
        painter->setPen(Qt::black);


        paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");
        paintPixmap(painter, lod, ER_QOS, "Actions", "Global");
        painter->drawText(getElementRect(ER_MAIN_LABEL), Qt::AlignCenter|Qt::TextWrapAnywhere, getData("label").toString());
    }

    ContainerElementNodeItem::paint(painter, option, widget);


}

QRectF AssemblyEventPortNodeItem::iconRect() const
{
    QRectF r;
    r.setWidth(getMinimumWidth() * .75);
    r.setHeight(getMinimumHeight() * .75);

    if(isRightJustified()){
        r.moveBottomRight(currentRect().bottomRight());
    }else{
        r.moveBottomLeft(currentRect().bottomLeft());
    }
    return r;
}

QRectF AssemblyEventPortNodeItem::textRect() const
{
    QRectF r = currentRect();
    r.setHeight(getMinimumHeight() / 4);

    r.moveTopLeft(currentRect().topLeft());
    return r;
}

QRectF AssemblyEventPortNodeItem::subIconRect() const
{
    return QRectF(0, 0, 8, 8);
}

QRectF AssemblyEventPortNodeItem::topSubIconRect() const
{
    QRectF r = subIconRect();
    if(isRightJustified()){
        r.moveTopRight(iconRect().topLeft() + QPointF(2,0));
    }else{
        r.moveTopLeft(iconRect().topRight() - QPointF(2,0));
    }
    return r;
}

QRectF AssemblyEventPortNodeItem::midSubIconRect() const
{
    QRectF r = topSubIconRect();
    r.moveTopLeft(topSubIconRect().bottomLeft());
    return r;
}

QRectF AssemblyEventPortNodeItem::bottomSubIconRect() const
{
    QRectF r = midSubIconRect();
    r.moveTopLeft(midSubIconRect().bottomLeft());
    qCritical() << r;
    return r;
}
