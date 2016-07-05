#include "defaultnodeitem.h"

#include "nodeitemnew.h"
#include <QPainter>
#include <QDebug>

DefaultNodeItem::DefaultNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    qCritical() << "DefaultNodeItem()";
    setMargin(QMarginsF(10, 25, 10, 10));
    setMinimumWidth(72);
    setMinimumHeight(72);
}

DefaultNodeItem::~DefaultNodeItem()
{

}

void DefaultNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    QColor grey = Qt::gray;
    grey.setAlpha(20);

    painter->setPen(Qt::NoPen);
    painter->setBrush(grey);
    painter->drawRect(boundingRect());

    painter->setPen(getPen(lod));
    painter->setBrush(Qt::white);
    painter->drawRect(currentRect());


    QPen pen;
    pen.setColor(Qt::yellow);
    painter->setPen(pen);
    painter->drawRect(childrenBoundingRect());

    pen.setColor(Qt::green);
    painter->setPen(pen);
    QPointF centerPoint = contractedRect().center();

    painter->drawEllipse(centerPoint, 10, 10);

    NodeItemNew::paint(painter, option, widget);
}

QRectF DefaultNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    return QRectF();
}

void DefaultNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
}

QRectF DefaultNodeItem::gridRect() const
{
    return expandedRect();
}

