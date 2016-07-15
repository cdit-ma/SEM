#include "modelitemnew.h"
#include <QDebug>
ModelItemNew::ModelItemNew(NodeViewItem *viewItem):NodeItemNew(viewItem, 0, NodeItemNew::MODEL_ITEM)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    setResizeEnabled(false);
    setExpanded(true);

    qreal size = DEFAULT_SIZE /2;
    setMinimumWidth(size);
    setMinimumHeight(size);
    setExpandedHeight(size);
    setExpandedWidth(size);

    setDefaultPen(QPen(Qt::darkGray));
    backgroundColor = Qt::gray;
    setMargin(QMarginsF(size, size, size, size));
}

QPainterPath ModelItemNew::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:{
        //Add in the Circle Rect
        QPainterPath path;
        path.addEllipse(currentRect());
        return path;
    }
    default:
        break;
    }
    return NodeItemNew::getElementPath(rect);
}

void ModelItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor);
    painter->drawEllipse(currentRect());
    painter->restore();

    //Call Base class
    NodeItemNew::paint(painter, option, widget);
}

QRectF ModelItemNew::boundingRect() const
{
    QRectF br = NodeItemNew::boundingRect();
    br.moveCenter(QPointF(0,0));
    return br;
}
