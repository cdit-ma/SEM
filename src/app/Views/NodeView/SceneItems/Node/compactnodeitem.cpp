#include "compactnodeitem.h"

CompactNodeItem::CompactNodeItem(NodeViewItem* viewItem, NodeItem* parentItem) : BasicNodeItem(viewItem, parentItem)
{
    setMoveEnabled(false);
    setExpandEnabled(true);

    setContractedHeight(getContractedHeight() / 2);
    setContractedWidth(40);
    
    //setExpandedWidth(getContractedWidth());
    //setExpandedHeight(getMinimumHeight());

    addRequiredData(KeyName::Index);
    addRequiredData(KeyName::Row);
    addRequiredData(KeyName::Column);

    setPrimaryTextKey(KeyName::Label);
    getTextItem(EntityRect::PRIMARY_TEXT)->setAlignment(Qt::AlignCenter);
    getTextItem(EntityRect::SECONDARY_TEXT)->setAlignment(Qt::AlignCenter);
    getTextItem(EntityRect::TERTIARY_TEXT)->setAlignment(Qt::AlignCenter);
}

QRectF CompactNodeItem::childrenRect() const{
    return QRectF(0,0, getContractedWidth(), getContractedHeight() * 2);
}

QRectF CompactNodeItem::getElementRect(EntityRect rect) const{
    switch(rect){
    case EntityRect::EXPAND_CONTRACT:
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
    case EntityRect::TERTIARY_TEXT:
        return textRect_Tertiary();
    case EntityRect::SHAPE:
    case EntityRect::HEADER:
        return headerRect();
    default:
        break;
    }
    return BasicNodeItem::getElementRect(rect);
}

QRectF CompactNodeItem::headerRect() const{
    auto rect = BasicNodeItem::headerRect();

    if(isExpanded()){
        rect |= primaryRow() | secondaryRow() | tertiaryRow();
        rect += QMargins(0, 0, 0, 2);
    }
    return rect;
}

QRectF CompactNodeItem::primaryRow() const
{
    auto rect = BasicNodeItem::headerRect();;

    auto margin = QMarginsF(2, 2, 2, 2);
    if(isRightJustified()){
        margin.setRight(4);
    }else{
        margin.setLeft(4);
    }

    return rect - margin;
}

QRectF CompactNodeItem::textRect_Primary() const{
    const auto& icon_rect = iconRect_Primary();
    auto rect = primaryRow();

    if(isRightJustified()){
        rect.setRight(icon_rect.left());
    }else{
        rect.setLeft(icon_rect.right());
    }
    return rect;
}


QRectF CompactNodeItem::secondaryRow() const{
    QRectF rect;
    if(isExpanded() && isIconVisible(EntityRect::SECONDARY_ICON)){
        rect = primaryRow();
        rect.moveTop(rect.bottom());
        rect -= QMarginsF(0, 1, 0, 0);
    }
    return rect;
}

QRectF CompactNodeItem::tertiaryRow() const{
    QRectF rect;
    if(isExpanded() && isIconVisible(EntityRect::SECONDARY_ICON)){
        rect = primaryRow();
        const auto& union_rect = primaryRow() | secondaryRow();

        rect.moveTop(union_rect.bottom());
        rect -= QMarginsF(0, 1, 0, 0);
    }

    return rect;
}
    

QRectF CompactNodeItem::iconRect_Secondary() const{
    QRectF rect;
    if(isIconVisible(EntityRect::SECONDARY_ICON)){
        const auto& full_rect = secondaryRow();
        rect = full_rect;

        //Baby Icon
        rect.setWidth(rect.height() *.75);
        rect.setHeight(rect.width());

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
        const auto& full_rect = primaryRow();
        rect = full_rect;
        //Baby Icon
        rect.setWidth(rect.height());
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
        const auto& full_rect = tertiaryRow();
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

QRectF CompactNodeItem::textRect_Secondary() const{
    const auto& icon_rect = iconRect_Secondary();

    auto rect = secondaryRow();

    if(isRightJustified()){
        rect.setRight(icon_rect.left() - 1);
    }else{
        rect.setLeft(icon_rect.right() + 1);
    }
    return rect;
}

QRectF CompactNodeItem::textRect_Tertiary() const{
    const auto& icon_rect = iconRect_Tertiary();
    auto rect = tertiaryRow();

    if(isRightJustified()){
        rect.setRight(icon_rect.left() - 1);
    }else{
        rect.setLeft(icon_rect.right() + 1);
    }
    return rect;
}


bool CompactNodeItem::isExpandEnabled(){
    return true;
}