#include "basicnodeitem.h"
#include <QDebug>
BasicNodeItem::BasicNodeItem(NodeViewItem *viewItem, NodeItem *parentItem) :NodeItem(viewItem, parentItem, CONTAINER_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);
    _isSortOrdered = false;

    parentContainer = qobject_cast<BasicNodeItem*>(parentItem);

    headerMargin = QMarginsF(2,2,2,2);
    int s = getGridSize();
    setBodyPadding(QMarginsF(s,s,s,s));
    setMargin(QMarginsF(s,s,s,s));

    qreal height = DEFAULT_SIZE / 2.0;
    qreal width = DEFAULT_SIZE / 2.0;

    setMinimumHeight(height);
    setMinimumWidth(width*2);

    setExpandedWidth(width*2);
    setExpandedHeight(height);

   

    setPrimaryTextKey("label");

    bool require_x_y = true;
    if(parentContainer && parentContainer->isSortOrdered()){
        setMoveEnabled(false);
        connect(this, &NodeItem::indexChanged, [=](){parentContainer->childPosChanged(this);});

        require_x_y = false;
    }

    if(require_x_y){
        addRequiredData("x");
        addRequiredData("y");
    }else{
        addRequiredData("index");
        addRequiredData("row");
        addRequiredData("row_subgroup");

    }



    if(viewItem->getNodeKind() == NODE_KIND::MEMBER){
        addRequiredData("key");
    }
    reloadRequiredData();
}

bool BasicNodeItem::isSortOrdered() const
{
    return _isSortOrdered;
}

void BasicNodeItem::setSortOrdered(bool ordered)
{
    _isSortOrdered = ordered;
}

QRectF BasicNodeItem::bodyRect() const
{
    QRectF rect = currentRect();
    rect.setTop(rect.top() + getMinimumHeight());
    return rect;
}

QPointF BasicNodeItem::getElementPosition(BasicNodeItem *child)
{
    return child->getPos();
}


BasicNodeItem *BasicNodeItem::getParentContainer() const
{
    return parentContainer;
}


void BasicNodeItem::setPos(const QPointF &p)
{
    QPointF pos = p;
    if(getParentContainer() && getParentContainer()->isSortOrdered()){
        if(!isMoving()){
            //Force it's position if it isn't moving!
            pos = getParentContainer()->getElementPosition(this);
        }
    }
    NodeItem::setPos(pos);
}

QPointF BasicNodeItem::getNearestGridPoint(QPointF newPos)
{
    if(getParentContainer() && getParentContainer()->isSortOrdered()){
        return getParentContainer()->getElementPosition(this);
    }
    return NodeItem::getNearestGridPoint(newPos);
}

QRectF BasicNodeItem::getElementRect(EntityItem::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_EXPANDCONTRACT:
        return iconRect();
    case ER_EXPANDED_STATE:
        return expandStateRect();
    case ER_MAIN_ICON:
        return iconRect();
    case ER_MAIN_ICON_OVERLAY:
    case ER_NOTIFICATION:
        return iconOverlayRect();
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
        return lockedRect();
    case ER_EDGE_KIND_ICON:
        return edgeKindRect();
    case ER_CONNECT_ICON:
        return deployedRect();
    case ER_CONNECT:
        return connectRect();
    case ER_TERTIARY_ICON:
        return tertiaryIconRect();
    case ER_CONNECT_TARGET:
        return connectTargetRect();
    case ER_CONNECT_SOURCE:
        return connectSourceRect();
    default:
        break;
    }
    return NodeItem::getElementRect(rect);
}

void BasicNodeItem::paintBackground(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    painter->setClipRect(option->exposedRect);

    if(state >= RENDER_STATE::BLOCK){
        painter->setPen(Qt::NoPen);

        //Paint the Body
        if(isExpanded()){
            painter->setBrush(getBodyColor());
            painter->drawRect(currentRect());
        }

        painter->setBrush(getHeaderColor());
        painter->drawRect(headerRect());

        if(state > RENDER_STATE::BLOCK){
            //Paint the White Background for the text
            if(gotSecondaryTextKey() && !isDataProtected(getSecondaryTextKey())){
                painter->setBrush(getBodyColor());
                painter->drawRect(bottomTextOutlineRect());
            }
        }

    }
}

void BasicNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    paintBackground(painter, option, widget);
    NodeItem::paint(painter, option, widget);
}

QRectF BasicNodeItem::headerRect() const
{
    QRectF rect(getMarginOffset(),QSize(getWidth(), getMinimumHeight()));
    return rect;
}

QRectF BasicNodeItem::connectRect() const
{
    QRectF r = headerRect();
    r.setLeft(r.right() - (smallIconSize().width() * 2));
    return r;
}

QRectF BasicNodeItem::connectSourceRect() const
{
    QRectF r = headerRect();
    r.setRight(r.left() + (smallIconSize().width() * 2));
    return r;
}

QRectF BasicNodeItem::connectTargetRect() const
{
    QRectF r = headerRect();
    r.setLeft(r.right() - (smallIconSize().width() * 2));
    return r;
}

QRectF BasicNodeItem::edgeKindRect() const
{
    QRectF r2 = connectRect();
    QRectF r;
    r.setSize(QSizeF(r2.width(), r2.width()));
    r.moveCenter(r2.center());
    return r;

}

QRectF BasicNodeItem::innerHeaderRect() const
{
    return headerRect().marginsRemoved(headerMargin);
}

QRectF BasicNodeItem::headerTextRect() const
{
    QRectF rect(innerHeaderRect());
    if(isRightJustified()){
        rect.setRight((rect.right() - iconRect().width()) -2);
    }else{
        rect.setLeft(rect.left() + iconRect().width() + 2);
    }
    return rect;
}

QRectF BasicNodeItem::tertiaryIconRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveTopLeft(iconRect().topRight());
    return rect;
}

QRectF BasicNodeItem::iconRect() const
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

QRectF BasicNodeItem::iconOverlayRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize() * 2);
    rect.moveCenter(iconRect().center());
    return rect;
}

QRectF BasicNodeItem::topTextRect() const
{
    QRectF rect(headerTextRect());
    if(gotSecondaryTextKey()){
        rect.setHeight(20);
    }
    rect.setWidth(rect.width());
    return rect;
}

QRectF BasicNodeItem::bottomTextRect() const
{
    QRectF rect;
    if(bottomTextOutlineRect().isValid()){
        rect = bottomTextOutlineRect();
        rect.adjust(1,1,-1,-1);
    }
    return rect;
}

QRectF BasicNodeItem::bottomTextOutlineRect() const
{
    QRectF rect;
    if(bottomRect().isValid()){
        rect = bottomRect();
        rect.setLeft(rect.left() + 10);
    }
    return rect;

}

QRectF BasicNodeItem::bottomIconRect() const
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

QRectF BasicNodeItem::bottomRect() const
{
    if(!gotSecondaryTextKey()){
        return QRectF();
    }
    QRectF rect(headerTextRect());
    rect.setTop(rect.top() + 24);
    return rect;
}

QRectF BasicNodeItem::deployedRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveTopRight(headerTextRect().topRight());
    return rect;
}

QRectF BasicNodeItem::lockedRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveTopLeft(innerHeaderRect().topLeft());
    return rect;
}

QRectF BasicNodeItem::qosRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize());
    rect.moveBottomRight(headerTextRect().bottomRight());
    return rect;

}

QRectF BasicNodeItem::expandStateRect() const
{
    QRectF rect;
    rect.setSize(smallIconSize() / 2);
    QRectF cr = currentRect().marginsRemoved(headerMargin);
    rect.moveBottomRight(cr.bottomRight());
    return rect;
}
