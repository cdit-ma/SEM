#include "nodeitemorderedcontainer.h"

ContainerNodeItem::ContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem){



    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);
    _isSortOrdered = false;

    headerMargin = QMarginsF(2,2,2,2);

    qreal height = iconSize().height() + headerMargin.top() + headerMargin.bottom();
    qreal width = DEFAULT_SIZE / 2.0;

    setMinimumHeight(height);
    setMinimumWidth(width*5);

    setExpandedWidth(width*4);
    setExpandedHeight(height);


    setBodyPadding(QMarginsF(0,0,0,0));

    setPrimaryTextKey("label");
    //setSecondaryTextKey("sortOrder");
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();

}

bool ContainerNodeItem::isSortOrdered() const
{
    return _isSortOrdered;
}

void ContainerNodeItem::setSortOrdered(bool ordered)
{
    _isSortOrdered = ordered;
}

QRectF ContainerNodeItem::bodyRect() const
{
    QRectF rect = currentRect();
    rect.setTop(rect.top() + getMinimumHeight());
    return rect;
}

QRectF ContainerNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_EXPANDCONTRACT:
        return iconRect();
    case ER_EXPANDED_STATE:
        return expandStateRect();
    case ER_MAIN_ICON:
        return iconRect();
    case ER_PRIMARY_TEXT:
        return topTextRect();
    case ER_SECONDARY_TEXT:
        return bottomTextRect();
    case ER_DEPLOYED:
        return deployedRect();
    case ER_QOS:
        return qosRect();
    default:
        return ContainerElementNodeItem::getElementRect(rect);
    }
}

void ContainerNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->save();
        if(isExpanded()){
            painter->save();
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawRect(currentRect());
            painter->restore();
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(getBodyColor().darker(110));
        painter->drawRect(headerRect());
        painter->save();

        if(gotSecondaryTextKey() && !isDataProtected(getSecondaryTextKey())){
            painter->setBrush(Qt::white);
            painter->drawRect(bottomTextRect());
        }

        painter->restore();

        painter->setPen(Qt::black);

        //paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Expand");
        paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");
        paintPixmap(painter, lod, ER_QOS, "Actions", "Global");

        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
}

QRectF ContainerNodeItem::headerRect() const
{
    QRectF rect(getMarginOffset(),QSize(getWidth(), getMinimumHeight()));
    return rect;
}

QRectF ContainerNodeItem::innerHeaderRect() const
{
    return headerRect().marginsRemoved(headerMargin);
}

QRectF ContainerNodeItem::headerTextRect() const
{
    QRectF rect(innerHeaderRect());
    if(isRightJustified()){
        rect.setRight(rect.right() - iconRect().width());
    }else{
        rect.setLeft(rect.left() + iconRect().width());
    }
    return rect;
}

QRectF ContainerNodeItem::iconRect() const
{
    QRectF rect;
    rect.setSize(iconSize());
   if(isRightJustified()){
        rect.moveTopRight(innerHeaderRect().topRight());
    }else{
        rect.moveTopLeft(innerHeaderRect().topLeft());
    }
    return rect;
}

QRectF ContainerNodeItem::topTextRect() const
{
    QRectF rect(headerTextRect());
    rect.setHeight(rect.height() / 2.0);
    rect.setWidth(rect.width() - smallIconSize().width() - 2);
    rect.setLeft(rect.left() + 2);
    return rect;
}

QRectF ContainerNodeItem::bottomTextRect() const
{
    QRectF rect(headerTextRect());
    rect.setWidth(rect.width() - smallIconSize().width() - 2);
    rect.setTop(rect.top() + rect.height() / 2);
    rect.setLeft(rect.left() + 2);
    return rect;
}

QRectF ContainerNodeItem::deployedRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveTopRight(headerTextRect().topRight());
    return rect;
}

QRectF ContainerNodeItem::qosRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveBottomRight(headerTextRect().bottomRight());
    return rect;

}

QRectF ContainerNodeItem::expandStateRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize() / 2);
    rect.moveBottomRight(currentRect().bottomRight());
    return rect;
}
