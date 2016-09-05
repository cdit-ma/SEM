#include "nodeitemsquare.h"

NodeItemSquare::NodeItemSquare(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem){



    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);
    _isSortOrdered = false;

    headerMargin = QMarginsF(2,2,2,2);

    qreal height = iconSize().height() + headerMargin.top() + headerMargin.bottom();
    qreal width = DEFAULT_SIZE /2;

    setMinimumHeight(52);
    setMinimumWidth(52);

    setExpandedWidth(52);
    setExpandedHeight(52);

    setMargin(QMarginsF(6,20,6,6));

    setBodyPadding(QMarginsF(2,2,2,2));

    setPrimaryTextKey("label");
    //setSecondaryTextKey("sortOrder");
    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();

}

bool NodeItemSquare::isSortOrdered() const
{
    return _isSortOrdered;
}

void NodeItemSquare::setSortOrdered(bool ordered)
{
    _isSortOrdered = ordered;
}

QRectF NodeItemSquare::bodyRect() const
{
    QRectF rect = currentRect();
    rect.setTop(rect.top() + getMinimumHeight());
    return rect;
}

QRectF NodeItemSquare::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
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

void NodeItemSquare::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
            painter->drawRect(bottomTextRect());
        }

        painter->restore();

        painter->setPen(Qt::black);

        //paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Expand");

        paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Square");
        paintPixmap(painter, lod, ER_QOS, "Actions", "Square");

        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
}

QRectF NodeItemSquare::headerRect() const
{
    QRectF rect(getMarginOffset(),QSize(getWidth(), getMinimumHeight()));
    return rect;
}

QRectF NodeItemSquare::innerHeaderRect() const
{
    return headerRect().marginsRemoved(headerMargin);
}

QRectF NodeItemSquare::headerTextRect() const
{
    QRectF rect(innerHeaderRect());
    if(isRightJustified()){
        rect.setRight(rect.right() - iconRect().width());
    }else{
        rect.setLeft(rect.left() + iconRect().width());
    }
    return rect;
}

QRectF NodeItemSquare::iconRect() const
{
    QRectF rect;
    rect.setSize(iconSize());
    rect.moveTopLeft(headerRect().topLeft() + QPointF(getBodyPadding().left() + 8, getBodyPadding().top() + 8));
    return rect;
}

QRectF NodeItemSquare::topTextRect() const
{
    QRectF rect(0,0, getWidth() + getMargin().left() + getMargin().right(), 16);
    return rect;
}

QRectF NodeItemSquare::bottomTextRect() const
{
    QRectF rect;
    rect.setWidth(getWidth() - 4);
    rect.setHeight(8);
    rect.moveBottomRight(headerRect().bottomRight() - QPointF(2,2));
    return rect;
}

QRectF NodeItemSquare::deployedRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveBottomRight(iconRect().topLeft());
    return rect;
}

QRectF NodeItemSquare::qosRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveBottomLeft(iconRect().topRight());
    return rect;

}

QRectF NodeItemSquare::expandStateRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize() / 2);
    rect.moveBottomRight(currentRect().bottomRight());
    return rect;
}
