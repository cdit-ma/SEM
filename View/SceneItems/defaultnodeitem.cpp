#include "defaultnodeitem.h"

#include "nodeitemnew.h"
#include <QPainter>
#include <QDebug>

#define ICON_RATIO (4.0 / 6.0)
#define SMALL_ICON_RATIO (1.0 / 6.0)
#define LABEL_RATIO (2.5 / 6.0)
#define INNER_PADDING 3

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



    painter->setPen(Qt::NoPen);
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
        paintPixmap(painter, lod, ER_LOCKED_STATE, "Actions", "Lock_Closed");
        paintPixmap(painter, lod, ER_STATUS, "Actions", "Locked");
        paintPixmap(painter, lod, ER_SECONDARY_ICON, "Actions", "Snippet");
        paintPixmap(painter, lod, ER_MAIN_ICON_OVERLAY, "Actions", "Key");
    }

    painter->setPen(Qt::NoPen);
    painter->setPen(Qt::red);
    painter->drawText(getElementRect(ER_MAIN_LABEL), Qt::AlignVCenter | Qt::AlignLeft, "LABEL");
    painter->drawText(getElementRect(ER_SECONDARY_LABEL), Qt::AlignCenter, "LABEL");

    painter->drawRect(getElementRect(ER_SECONDARY_TEXT));
    //painter->drawRect(getElementRect(ER_MAIN_ICON));

    NodeItemNew::paint(painter, option, widget);
}

QRectF DefaultNodeItem::mainIconRect() const
{
    qreal iconSize = ICON_RATIO * getMinimumHeight();// - (INNER_PADDING);

    QRectF iconRect(0, 0, iconSize, iconSize);
    iconRect.moveCenter(contractedRect().center());
    return iconRect;
}

QRectF DefaultNodeItem::statusIcon(RECT_VERTEX vert) const
{
    qreal iconSize = SMALL_ICON_RATIO * getMinimumHeight();
    QRectF rect(0, 0, iconSize, iconSize);

    switch(vert){
    case RV_TOPLEFT:{
        rect.moveTopLeft(currentRect().topLeft());
        rect.adjust(INNER_PADDING, INNER_PADDING, INNER_PADDING, INNER_PADDING);
        break;
    }
    case RV_TOPRIGHT:{
        rect.moveTopRight(currentRect().topRight());
        rect.adjust(-INNER_PADDING, INNER_PADDING, -INNER_PADDING, INNER_PADDING);
        break;
    }
    case RV_BOTTOMLEFT:{
        rect.moveBottomLeft(contractedRect().bottomLeft());
        rect.adjust(INNER_PADDING, -INNER_PADDING, INNER_PADDING, -INNER_PADDING);
        break;
    }
    case RV_BOTTOMRIGHT:{
        rect.moveBottomRight(contractedRect().bottomLeft() + QPointF(getWidth(), 0));
        rect.adjust(-INNER_PADDING, -INNER_PADDING, -INNER_PADDING, -INNER_PADDING);
        break;
    }
    case RV_NONE:{
        rect.moveCenter(contractedRect().center());
        break;
    }
    default:
        rect = QRectF();
        break;
    }
    return rect;
}

QRectF DefaultNodeItem::rightLabelRect() const
{
    QRectF rect;
    rect.setWidth(expandedRect().right() - mainIconRect().right());
    rect.setHeight(LABEL_RATIO * getMinimumHeight());
    QPointF topLeft = mainIconRect().topRight();
    topLeft += QPointF(0, (mainIconRect().height() / 2)) + QPointF(0, -rect.height() / 2);
    rect.moveTopLeft(topLeft);
    if(rect.isValid()){
        return rect;
    }
    return QRectF();
}

QRectF DefaultNodeItem::topLabelRect() const
{
    QRectF rect;
    rect.setWidth(getWidth());
    rect.setHeight(getMargin().top());
    rect.moveBottomLeft(contractedRect().topLeft());
    return rect;
}

QRectF DefaultNodeItem::bottomTextRect() const
{
    QRectF rect;
    rect.moveTopLeft(statusIcon(RV_BOTTOMLEFT).topRight());
    rect.setBottomRight(statusIcon(RV_BOTTOMRIGHT).bottomLeft());
    return rect;
}

QRectF DefaultNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    switch(rect){
    case ER_MAIN_ICON:
        return mainIconRect();
    case ER_LOCKED_STATE:
        return statusIcon(RV_TOPLEFT);
    case ER_STATUS:
        return statusIcon(RV_TOPRIGHT);
    case ER_SECONDARY_ICON:
        return statusIcon(RV_BOTTOMLEFT);
    case ER_MAIN_ICON_OVERLAY:
        return statusIcon(RV_NONE);
    case ER_SECONDARY_LABEL:
        return topLabelRect();
    case ER_MAIN_LABEL:
        return rightLabelRect();
    case ER_SECONDARY_TEXT:
        return bottomTextRect();
    default:
        return QRectF();
    }
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

