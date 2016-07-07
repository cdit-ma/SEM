#include "defaultnodeitem.h"

#include "nodeitemnew.h"
#include <QPainter>
#include <QDebug>

DefaultNodeItem::DefaultNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMargin(QMarginsF(10, 25, 10, 10));
    setMinimumWidth(72);
    setMinimumHeight(72);

}

DefaultNodeItem::~DefaultNodeItem()
{

}

QRectF DefaultNodeItem::headerRect() const
{
    QRectF rect = currentRect();
    rect.setHeight(getMinimumHeight());
    return rect;
}

QRectF DefaultNodeItem::bodyRect() const
{
    QRectF rect = currentRect();
    rect.setHeight(getHeight() - getMinimumHeight());
    rect.moveBottomLeft(currentRect().bottomLeft());
    return rect;
}

void DefaultNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    QColor grey = Qt::gray;
    grey.setAlpha(20);

    painter->setPen(Qt::NoPen);
    painter->setBrush(grey);
    painter->drawRect(boundingRect());

    painter->setPen(getPen(lod));

    painter->setBrush(Qt::gray);
    painter->drawRect(bodyRect());
    painter->setBrush(Qt::white);
    painter->drawRect(headerRect());
    painter->setBrush(Qt::NoBrush);

    QPen pen;
    pen.setColor(Qt::yellow);
    painter->setPen(pen);
    painter->drawRect(childrenBoundingRect());



    if(state > RS_BLOCK){
        QPair<QString, QString> icon = getIconPath();
        paintPixmap(painter, lod, ER_MAIN_ICON, icon.first, icon.second);
    }


    painter->setPen(Qt::NoPen);
    painter->setBrush(Qt::green);
    painter->drawEllipse(contractedRect().center(), 5, 5);

    NodeItemNew::paint(painter, option, widget);
}

QRectF DefaultNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    if(rect == ER_MAIN_ICON){
        return contractedRect();
    }
    return QRectF();
}

void DefaultNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
}

QRectF DefaultNodeItem::gridRect() const
{
    return bodyRect();
}

QRectF DefaultNodeItem::getResizeRect(RECT_VERTEX vert)
{
    return NodeItemNew::getResizeRect(vert);
}

