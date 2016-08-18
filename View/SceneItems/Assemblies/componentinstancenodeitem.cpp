#include "componentinstancenodeitem.h"
#include <QDebug>

ComponentInstanceNodeItem::ComponentInstanceNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(false);

    height = DEFAULT_SIZE / 2.0;
    width = DEFAULT_SIZE * 4;

    setMinimumHeight(height);
    setMinimumWidth(width);

    setExpandedWidth(width);
    setExpandedHeight(100);

    setExpanded(true);

    setMargin(QMarginsF(10,15,10,10));
    setBodyPadding(QMarginsF(0,0,0,0));

    addRequiredData("x");
    addRequiredData("y");

    reloadRequiredData();
}

QRectF ComponentInstanceNodeItem::bodyRect() const
{
    return mainRect();
}

void ComponentInstanceNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setFont(mainTextFont);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->save();
        if(isExpanded()){
            painter->save();
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawRect(mainRect());
            painter->restore();
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(getBodyColor().darker(110));
        painter->drawRect(headerRect());

        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
}

QRectF ComponentInstanceNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_EXPANDCONTRACT:
        return headerRect();
    case ER_SELECTION:
        return mainRect();
    case ER_MAIN_ICON:
        return mainIconRect();
    default:
        return NodeItemNew::getElementRect(rect);
    }
}

QPainterPath ComponentInstanceNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    QPainterPath path;
    switch(rect){
    case ER_SELECTION:
        path.setFillRule(Qt::WindingFill);
        path.addRect(headerRect());
        if(isExpanded()){
            path.addRect(mainRect());
        }
        return path.simplified();
    default:
        return NodeItemNew::getElementPath(rect);
    }

}

QRectF ComponentInstanceNodeItem::mainRect() const
{
    QRectF mainRect(0,0,getExpandedWidth(), getExpandedHeight()-headerRect().height());
    mainRect.moveTopLeft(headerRect().bottomLeft());
    return mainRect;
}

QRectF ComponentInstanceNodeItem::headerRect() const
{
    QRectF rect(0,0,getExpandedWidth(), getMinimumHeight());
    rect.moveTopLeft(getMarginOffset());
    return rect;
}

QRectF ComponentInstanceNodeItem::mainIconRect() const
{
    QRectF rect(headerRect());
    rect.setWidth(headerRect().height());
    return rect;
}

void ComponentInstanceNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
}

