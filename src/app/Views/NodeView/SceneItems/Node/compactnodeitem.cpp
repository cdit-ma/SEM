#include "compactnodeitem.h"

CompactNodeItem::CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem) : BasicNodeItem(viewItem, parentItem)
{
    setMoveEnabled(false);
    setExpandEnabled(false);

    addRequiredData("index");
    addRequiredData("row");
    addRequiredData("column");

    setPrimaryTextKey("label");
    getTextItem(EntityRect::PRIMARY_TEXT)->setAlignment(Qt::AlignCenter);// | Qt::AlignVCenter);
    getTextItem(EntityRect::SECONDARY_TEXT)->setAlignment(Qt::AlignCenter);// | Qt::AlignVCenter);

    addHoverFunction(EntityRect::SECONDARY_ICON, std::bind(&CompactNodeItem::secondaryIconHover, this, std::placeholders::_1, std::placeholders::_2));
}

QRectF CompactNodeItem::getElementRect(EntityRect rect) const{
    switch(rect){
    case EntityRect::MAIN_ICON:
        return iconRect_Primary();
    case EntityRect::TERTIARY_ICON:
        return iconRect_Tertiary();
    case EntityRect::SECONDARY_ICON:
        return iconRect_Secondary();
    case EntityRect::PRIMARY_TEXT:
        return textRect_Primary();
    case EntityRect::SECONDARY_TEXT:
        return textRect_Secondary();
    default:
        break;
    }
    return BasicNodeItem::getElementRect(rect);
}


QRectF CompactNodeItem::innerHeaderRect() const
{
    auto rect = headerRect();

    auto margin = QMarginsF(2, 2, 2, 2);
    if(isRightJustified()){
        margin.setRight(4);
    }else{
        margin.setLeft(4);
    }

    return headerRect() - margin;
}

QRectF CompactNodeItem::topRect() const{
    auto rect = innerHeaderRect();
    rect.setHeight(rect.height() * (5.0 / 8.0));
    return rect;
}

QRectF CompactNodeItem::bottomRect() const{
    auto rect = innerHeaderRect();
    rect.setTop(topRect().bottom());
    return rect;
}

QRectF CompactNodeItem::textRect_Primary() const{
    const auto& icon_rect = iconRect_Primary();
    auto rect = topRect();

    if(isRightJustified()){
        rect.setRight(icon_rect.left());
    }else{
        rect.setLeft(icon_rect.right());
    }
    return rect;
}
    

QRectF CompactNodeItem::iconRect_Secondary() const{
    QRectF rect;
    if(isIconVisible(EntityRect::SECONDARY_ICON)){
        const auto& full_rect = bottomRect();
        rect = full_rect;
        rect.setWidth(rect.height());

        if(!isRightJustified()){
            rect.moveBottomLeft(full_rect.bottomLeft());
        }else{
            rect.moveBottomRight(full_rect.bottomRight());
        }
    }
    return rect;
}

QRectF CompactNodeItem::iconRect_Primary() const{
    QRectF rect;
    if(isIconVisible(EntityRect::MAIN_ICON)){
        const auto& full_rect = topRect();
        rect = full_rect;
        //Baby Icon
        rect.setWidth(rect.height() *.75);
        rect.setHeight(rect.width());

        QPointF offset;
        offset.ry() += (full_rect.height() - rect.height()) / 2.0;
       

        if(isRightJustified()){
            rect.moveTopRight(full_rect.topRight() + offset);
        }else{
            rect.moveTopLeft(full_rect.topLeft() + offset);
        }
    }
    return rect;
}

QRectF CompactNodeItem::iconRect_Tertiary() const{
    QRectF rect;
    if(isIconVisible(EntityRect::TERTIARY_ICON)){
        const auto& full_rect = bottomRect();
        rect = full_rect;
        //Baby Icon
        rect.setWidth(rect.height() / 2.0);
        rect.setHeight(rect.width());

        QPointF offset;
        const auto& main_icon = iconRect_Secondary();
        
        if(isRightJustified()){
            offset.rx() -= main_icon.width() + 1;
        }else{
            offset.rx() += main_icon.width() + 1;
        }
        offset.ry() += (full_rect.height() - rect.height()) / 2.0;

        if(isRightJustified()){
            rect.moveTopRight(full_rect.topRight() + offset);
        }else{
            rect.moveTopLeft(full_rect.topLeft() + offset);
        }
    }
    return rect;
}

QRectF CompactNodeItem::iconRect_Union() const{
    return iconRect_Secondary() | iconRect_Tertiary();
}

QRectF CompactNodeItem::textRect_Secondary() const{
    const auto& icon_rect = iconRect_Union();
    auto rect = bottomRect();

    if(isRightJustified()){
        rect.setRight(icon_rect.left() - 1);
    }else{
        rect.setLeft(icon_rect.right() + 1);
    }
    return rect;
}


void CompactNodeItem::secondaryIconHover(bool hovered, const QPointF& pos){
    if(hovered){
        AddTooltip("Aggregate: " + getData("type").toString());
    }

}