#include "attributenodeitem.h"

AttributeNodeItem::AttributeNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{

    setMoveEnabled(true);
    setResizeEnabled(false);
    setExpandEnabled(false);

    width = DEFAULT_SIZE * 1.5;
    height = DEFAULT_SIZE / 2;

    setMinimumHeight(height);
    setMinimumWidth(width);

    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    addRequiredData("x");
    addRequiredData("y");

    reloadRequiredData();
}

QRectF AttributeNodeItem::mainIconRect() const
{
    QRectF rect;
    rect.setHeight(height);
    rect.setWidth(width/3);
    return rect;
}

void AttributeNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setFont(mainTextFont);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->save();

        painter->setBrush(getBodyColor());
        painter->drawRect(mainIconRect());
    }
}

QRectF AttributeNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MAIN_ICON:
        return mainIconRect();
    }
}

QPainterPath AttributeNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    QPainterPath path;
    path.addRect(mainIconRect());
    return path;
}

