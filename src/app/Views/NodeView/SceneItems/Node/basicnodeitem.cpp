#include "basicnodeitem.h"
#include <QDebug>
#include "stacknodeitem.h"

BasicNodeItem::BasicNodeItem(NodeViewItem *viewItem, NodeItem *parentItem) : NodeItem(viewItem, parentItem)
{
    setMoveEnabled(true);
    setExpandEnabled(true);

    parentContainer = qobject_cast<BasicNodeItem*>(parentItem);
    parentStackContainer = qobject_cast<StackNodeItem*>(parentItem);

    header_margins = QMarginsF(4,2,4,2);
   
    

    setPrimaryTextKey(KeyName::Label);

    bool require_x_y = true;
    if(parentContainer && parentContainer->isSortOrdered()){
        setMoveEnabled(false);
        connect(this, &NodeItem::indexChanged, [=](){parentContainer->childPosChanged(this);});

        require_x_y = false;
    }

    if(require_x_y){
        addRequiredData(KeyName::X);
        addRequiredData(KeyName::Y);
    }else{
        addRequiredData(KeyName::Index);
        addRequiredData(KeyName::Row);
        addRequiredData(KeyName::Column);
    }
    reloadRequiredData();
}

bool BasicNodeItem::isSortOrdered() const
{
    return is_sorted_ordered;
}

void BasicNodeItem::setSortOrdered(bool ordered)
{
    is_sorted_ordered = ordered;
}

QPointF BasicNodeItem::getElementPosition(BasicNodeItem *child)
{
    return child->getPos();
}


BasicNodeItem *BasicNodeItem::getParentContainer() const
{
    return parentContainer;
}


StackNodeItem* BasicNodeItem::getParentStackContainer() const{
    return parentStackContainer;
}

QPointF BasicNodeItem::validateMove(QPointF delta){
    if(getParentStackContainer()){
        return delta;
    }else{
        return NodeItem::validateMove(delta);
    }
}



void BasicNodeItem::setPos(const QPointF &p)
{
    QPointF pos = p;
    if(getParentContainer() && getParentContainer()->isSortOrdered()){
        pos = getParentContainer()->getElementPosition(this);
    }
    NodeItem::setPos(pos);
}

QPointF BasicNodeItem::getNearestGridPoint(QPointF newPos)
{
    if(getParentContainer() && getParentContainer()->isSortOrdered()){
        return getParentContainer()->getElementPosition(this);
    }else{

    }
    return NodeItem::getNearestGridPoint(newPos);
}



QRectF BasicNodeItem::getElementRect(EntityItem::EntityRect rect) const
{
    switch(rect){
    case EntityRect::EXPAND_CONTRACT:
    case EntityRect::MAIN_ICON:
        return headerContent_Icon();
    case EntityRect::MAIN_ICON_OVERLAY:
        return headerContent_Icon_Overlay();
    case EntityRect::NOTIFICATION_RECT:
        return notificationRect();
    case EntityRect::SECONDARY_ICON:
        return headerContent_Data_Secondary_Icon();
    case EntityRect::PRIMARY_TEXT:
        return headerContent_Data_Primary_Text();
    case EntityRect::SECONDARY_TEXT:
        return headerContent_Data_Secondary_Text();
    case EntityRect::LOCKED_STATE_ICON:
        return headerContent_Data_Primary_Icon();
    case EntityRect::CONNECT_TARGET:
        return connectTargetRect();
    case EntityRect::CONNECT_SOURCE:
        return connectSourceRect();
    case EntityRect::HEADER:
    case EntityRect::MOVE:
        return headerRect();
    default:
        break;
    }
    return NodeItem::getElementRect(rect);
}



void BasicNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    paintBackground(painter, option, widget);
    NodeItem::paint(painter, option, widget);
}


QRectF BasicNodeItem::connectSourceRect() const
{

    QRectF r = getElementRect(EntityRect::HEADER);
    auto left = r.right();
    r.setWidth(smallIconSize().width());

    auto h_offset = r.width() / 2;
    r.moveLeft(left - h_offset);
    return r;
}

QRectF BasicNodeItem::notificationRect() const
{
    QRectF r = getElementRect(EntityRect::HEADER);
    r.setHeight(smallIconSize().width() / 2);
    auto v_offset = r.height() / 2;
    r.moveTop(r.top() - v_offset);
    return r;
}

QRectF BasicNodeItem::connectTargetRect() const
{
    QRectF r = getElementRect(EntityRect::HEADER);
    auto right = r.left();
    r.setWidth(smallIconSize().width());

    auto h_offset = r.width() / 2;
    r.moveRight(right + h_offset);
    return r;
}



QRectF BasicNodeItem::headerContent() const
{
    return headerRect().marginsRemoved(header_margins);
}


QRectF BasicNodeItem::headerContent_Data() const
{
    QRectF rect(headerContent());
    auto icon_rect = headerContent_Icon();

    auto x_padding = 2;

    if(isRightJustified()){
        rect.setRight(icon_rect.left() - x_padding);
    }else{
        rect.setLeft(icon_rect.right() + x_padding);
    }
    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Primary() const{
    QRectF rect;

    if(gotPrimaryTextKey()){
        rect = headerContent_Data();

        if(gotSecondaryTextKey()){
            rect.setHeight(rect.height() * ratio);
        }
    }

    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Secondary() const{
    auto header_content = headerContent_Data();

    QRectF rect;
    if(gotSecondaryTextKey()){
        rect = header_content;
        if(gotPrimaryTextKey()){
            rect.setTop(headerContent_Data_Primary().bottom());
        }
    }
    return rect;
}


QRectF BasicNodeItem::headerContent_Icon() const
{
    const auto header_rect = headerContent();
    QRectF rect(header_rect);
    
    //Make it a square
    rect.setWidth(rect.height());

    if(isRightJustified()){
        rect.moveTopRight(header_rect.topRight());
    }else{
        rect.moveTopLeft(header_rect.topLeft());
    }
   return rect;
}

QRectF BasicNodeItem::headerContent_Icon_Overlay() const
{
    QRectF rect;
    const auto& icon = headerContent_Icon();
    rect.setSize(icon.size() / 4.0);
    rect.moveTopRight(icon.topRight());
    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Primary_Text() const{
    QRectF rect = headerContent_Data_Primary();
    if(isReadOnly()){
        rect.setRight(headerContent_Data_Primary_Icon().left());
    }
    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Primary_Icon() const{
    const auto& header_content = headerContent_Data_Primary();
    QRectF rect;
    if(isIconVisible(EntityRect::LOCKED_STATE_ICON)){
        rect.setSize(smallIconSize() / 2);
        rect.moveTopRight(header_content.topRight() + QPointF(0, (header_content.height() - rect.height()) / 2));
    }
    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Secondary_Text() const{

    QRectF rect;
    if(gotSecondaryTextKey()){
        rect = headerContent_Data_Secondary();

        if(isIconVisible(EntityRect::SECONDARY_ICON)){
            rect.setLeft(headerContent_Data_Secondary_Icon().right() + 1);
        }
    }
    return rect;
}

QRectF BasicNodeItem::headerContent_Data_Secondary_Icon() const{
    auto header_content = headerContent_Data_Secondary();
    QRectF rect;
    if(isIconVisible(EntityRect::SECONDARY_ICON) && header_content.isValid()){
        rect.setSize(smallIconSize() * 0.75);
        rect.moveTopLeft(header_content.topLeft() + QPointF(0, (header_content.height() - rect.height()) / 2));
    }
    return rect;
}
