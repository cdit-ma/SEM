#include "compactnodeitem.h"

CompactNodeItem::CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem):BasicNodeItem(viewItem, parentItem)
{
    setMoveEnabled(false);
    setExpandEnabled(false);
    setResizeEnabled(false);

    addRequiredData("index");
    addRequiredData("row");
    addRequiredData("column");

    setPrimaryTextKey("label");
    header_margins = QMarginsF(2,2,2,2);
    
    int s = getGridSize() / 2;
    setMargin(QMarginsF(s,s,s,s));


    auto text = getTextItem(EntityRect::PRIMARY_TEXT);
    text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
}

QRectF CompactNodeItem::getElementRect(EntityRect rect) const{
    switch(rect){
    case EntityRect::MAIN_ICON:
        return iconRect_Top();
    case EntityRect::SECONDARY_ICON:
        return iconRect_Bottom();
    case EntityRect::PRIMARY_TEXT:
        return textRect_Top();
    case EntityRect::SECONDARY_TEXT:
        return textRect_Bottom();
    default:
        break;
    }
    return NodeItem::getElementRect(rect);
}

void CompactNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    BasicNodeItem::paint(painter, option, widget);
    /*painter->setBrush(QColor(255,0,0,120));
    painter->drawRect(textRect_Top());
    painter->setBrush(QColor(0,0,255,120));
    painter->drawRect(textRect_Bottom());*/
}

QRectF CompactNodeItem::innerHeaderRect() const
{
    return headerRect().marginsRemoved(header_margins);
}

QRectF CompactNodeItem::iconRect_Top() const{
    auto rect = topRect();
    rect.setWidth(rect.height());
    return rect;
}
QRectF CompactNodeItem::iconRect_Bottom() const{
    auto rect = bottomRect();

    rect.setWidth(rect.height() / 2);
    rect.setHeight(rect.width());
    rect.moveCenter(rect.center() + QPointF(0, rect.height() / 2));
    return rect;
}

QRectF CompactNodeItem::topRect() const{
    auto rect = innerHeaderRect();
    rect.setHeight(rect.height() * (4.0 / 7.0));
    return rect;
}

QRectF CompactNodeItem::bottomRect() const{
    const auto& header_rect = innerHeaderRect();
    QRectF rect(header_rect);
    rect.setHeight(header_rect.height() * (3.0 / 7.0));
    rect.moveBottom(header_rect.bottom());
    return rect;
}

QRectF CompactNodeItem::textRect_Top() const{
    const auto& icon_rect = iconRect_Top();
    const auto& top_rect = topRect();

    QRectF rect(top_rect);
    rect.setLeft(icon_rect.right() + 2);
    return rect;
}

QRectF CompactNodeItem::textRect_Bottom() const{
    const auto& icon_rect = iconRect_Bottom();
    const auto& bottom_rect = bottomRect();

    QRectF rect(bottom_rect);
    rect.setLeft(icon_rect.right() + 2);
    return rect;
}