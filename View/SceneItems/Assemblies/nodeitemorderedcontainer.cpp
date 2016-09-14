#include "nodeitemorderedcontainer.h"
#include <QDebug>
ContainerNodeItem::ContainerNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem){

    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);
    _isSortOrdered = false;

    headerMargin = QMarginsF(2,2,2,2);
    setBodyPadding(QMarginsF(10,10,10,10));
    setMargin(QMarginsF(10,10,10,10));

    qreal height = DEFAULT_SIZE / 2.0;
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

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
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
    case ER_SECONDARY_ICON:
        return bottomIconRect();
    case ER_PRIMARY_TEXT:
        return topTextRect();
    case ER_SECONDARY_TEXT:
        return bottomTextRect();
    case ER_DEPLOYED:
        return deployedRect();
    case ER_QOS:
        return qosRect();
    case ER_LOCKED_STATE:
        return deployedRect();
    case ER_EDGE_KIND_ICON:
        return edgeKindRect();
    case ER_CONNECT_ICON:
        return deployedRect();
    case ER_CONNECT:
        return connectRect();
        break;
    default:
        break;
    }
    return ContainerElementNodeItem::getElementRect(rect);
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
            painter->drawRect(bottomTextOutlineRect());
        }

        if(getData("key").toBool()){
            paintPixmap(painter, lod, iconOverlayRect(), "Actions", "Key");
        }

        painter->restore();

        painter->setPen(Qt::black);
        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
}

QRectF ContainerNodeItem::headerRect() const
{
    QRectF rect(getMarginOffset(),QSize(getWidth(), getMinimumHeight()));
    return rect;
}

QRectF ContainerNodeItem::connectRect() const
{
    QRectF r = headerRect();
    r.setLeft(r.right() - (smallIconSize().width() * 2));
    return r;
}

QRectF ContainerNodeItem::edgeKindRect() const
{
    QRectF r2 = connectRect();
    QRectF r;
    r.setSize(QSizeF(r2.width(), r2.width()));
    r.moveCenter(r2.center());
    return r;

}

QRectF ContainerNodeItem::innerHeaderRect() const
{
    return headerRect().marginsRemoved(headerMargin);
}

QRectF ContainerNodeItem::headerTextRect() const
{
    QRectF rect(innerHeaderRect());
    if(isRightJustified()){
        rect.setRight((rect.right() - iconRect().width()) -2);
    }else{
        rect.setLeft(rect.left() + iconRect().width() + 2);
    }
    return rect;
}

QRectF ContainerNodeItem::iconRect() const
{
    QRectF rect = innerHeaderRect();
    rect.setWidth(rect.height());
    if(isRightJustified()){
        rect.moveTopRight(innerHeaderRect().topRight());
    }else{
        rect.moveTopLeft(innerHeaderRect().topLeft());
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
    rect.setWidth(rect.width());
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
        //Offset
        qreal yOffset = (bottomRect().height() - rect.height()) / 2;
        rect.moveTopLeft(bottomRect().topLeft() + QPointF(0, yOffset));
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
    QRectF cr = currentRect().marginsRemoved(headerMargin);
    rect.moveBottomRight(cr.bottomRight());
    return rect;
}
