#include "nodeitemorderedcontainer.h"

ContainerNodeItem::ContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem){



    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);
    _isSortOrdered = false;

    headerMargin = QMarginsF(2,2,2,2);
    setBodyPadding(QMarginsF(10,10,10,10));

    qreal height = iconSize().height() + headerMargin.top() + headerMargin.bottom();
    qreal width = DEFAULT_SIZE / 2.0;

    setMinimumHeight(height);
    setMinimumWidth(width*2);

    setExpandedWidth(width*2);
    setExpandedHeight(height);

    QPen pen;
    pen.setColor(QColor(50,50,50));
    pen.setWidthF(.5);
    setDefaultPen(pen);

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

QPointF ContainerNodeItem::getElementPosition(ContainerElementNodeItem *child)
{
    return child->getPos();
}

QPoint ContainerNodeItem::getElementIndex(ContainerElementNodeItem *child)
{
    return child->getIndexPosition();
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
            painter->setBrush(getBaseBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawRect(currentRect());
            painter->restore();
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(getBaseBodyColor().darker(110));
        painter->drawRect(headerRect());
        painter->save();

        if(gotSecondaryTextKey() && !isDataProtected(getSecondaryTextKey())){
            painter->setBrush(Qt::white);
            painter->drawRect(bottomTextOutlineRect());
        }

        painter->restore();

        painter->setPen(Qt::black);

        //iconOverlayRect

        //paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Expand");
        //paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");
        //paintPixmap(painter, lod, ER_QOS, "Actions", "Global");

        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
    if(getData("key").toBool()){
        paintPixmap(painter, lod, iconOverlayRect(), "Actions", "Key");
    }
    paintPixmap(painter, lod, bottomIconRect(), "Data", "type");

  //  painter->setBrush(Qt::red);
  //  painter->drawRect(topTextRect());
  //  painter->setBrush(Qt::blue);
  //  painter->drawRect(bottomRect());


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
        rect.setRight(rect.right() - iconRect().width() -2);
    }else{
        rect.setLeft(rect.left() + iconRect().width() + 2);
    }
    return rect;
}

QRectF ContainerNodeItem::iconRect() const
{
    QRectF rect;
    rect.setSize(iconSize() + QSize(2,2));
   if(isRightJustified()){
        rect.moveTopRight(headerRect().topRight() + QPointF(1,1));
    }else{
        rect.moveTopLeft(headerRect().topLeft() + QPointF(1,1));
    }
   return rect;
}

QRectF ContainerNodeItem::iconOverlayRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveCenter(iconRect().center());
    return rect;
}

QRectF ContainerNodeItem::topTextRect() const
{
    QRectF rect(headerTextRect());
    if(gotSecondaryTextKey()){
        rect.setHeight(20);
    }
    rect.setWidth(rect.width() /*- smallIconSize().width()*/);
    //rect.setLeft(rect.left());
    return rect;
}

QRectF ContainerNodeItem::bottomTextRect() const
{
    QRectF rect;
    if(bottomTextOutlineRect().isValid()){
        rect = bottomTextOutlineRect();
        rect.adjust(1,1,-1,-1);
    }
    return rect;
}

QRectF ContainerNodeItem::bottomTextOutlineRect() const
{
    QRectF rect;
    if(bottomRect().isValid()){
        rect = bottomRect();
        rect.setLeft(rect.left() + 10);
    }
    return rect;

}

QRectF ContainerNodeItem::bottomIconRect() const
{
    QRectF rect;
    if(bottomRect().isValid()){
        rect.setSize(smallIconSize());
        rect.moveTopLeft(bottomRect().topLeft());// + QPointF(1,1));
    }
    return rect;

}

QRectF ContainerNodeItem::bottomRect() const
{
    if(!gotSecondaryTextKey()){
        return QRectF();
    }
    QRectF rect(headerTextRect());
    rect.setTop(rect.top() + 24);
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
