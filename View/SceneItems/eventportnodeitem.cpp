#include "eventportnodeitem.h"

EventPortNodeItem::EventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMoveEnabled(false);
    setResizeEnabled(false);
    setExpandEnabled(false);

    height = DEFAULT_SIZE/2.0;
    width = DEFAULT_SIZE;

    setMinimumWidth(width);
    setMinimumHeight(height);

    setExpanded(true);

    setDefaultPen(Qt::NoPen);
    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    addRequiredData("x");
    addRequiredData("y");

}

void EventPortNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state > RS_BLOCK) {
        painter->setClipRect(option->exposedRect);
        painter->setPen(Qt::NoPen);
        painter->save();

        mainTextFont.setPixelSize(labelRect().height());
        painter->setPen(Qt::black);
        painter->drawText(labelRect(), Qt::AlignCenter, getData("label").toString());
        painter->setBrush(getBodyColor());
        painter->drawRect(mainRect());

        painter->setPen(Qt::NoPen);
    }
    painter->restore();
    NodeItemNew::paint(painter, option, widget);
}

QPainterPath EventPortNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    return NodeItemNew::getElementPath(rect);
}

bool EventPortNodeItem::isOutEventPort()
{
    return getData("kind").toString().startsWith("Out");
}

bool EventPortNodeItem::isInEventPort()
{
    return getData("kind").toString().endsWith("In");
}

bool EventPortNodeItem::isDelegate()
{
    return getData("kind").toString().endsWith("Delegate");
}

QRectF EventPortNodeItem::mainRect()
{
    QRectF mainRect(0, 0, getMinimumWidth(), getMinimumHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;

}

QRectF EventPortNodeItem::labelRect()
{
    return QRectF(mainRect().topLeft(), mainRect().bottomRight());
}
