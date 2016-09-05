#include "defaultnodeitem.h"

#include "nodeitemnew.h"
#include <QPainter>
#include <QDebug>


#define INNER_PADDING 3

DefaultNodeItem::DefaultNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMargin(QMarginsF(10, 25, 10, 10));
    setBodyPadding(QMarginsF(10,10,10,10));
    setMinimumWidth(DEFAULT_SIZE);
    setMinimumHeight(DEFAULT_SIZE);
    setExpandedHeight(DEFAULT_SIZE);
    setExpandedWidth(DEFAULT_SIZE);

    setHeaderPadding(QMarginsF(INNER_PADDING, INNER_PADDING, INNER_PADDING, INNER_PADDING));

    setupBrushes();

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("width");
    addRequiredData("height");
    addRequiredData("isExpanded");
    reloadRequiredData();
}

DefaultNodeItem::~DefaultNodeItem()
{

}

void DefaultNodeItem::setHeaderPadding(QMarginsF padding)
{
    if(headerPadding != padding){
        headerPadding = padding;
        update();
    }
}

QMarginsF DefaultNodeItem::getHeaderPadding() const
{
    return headerPadding;
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

QRectF DefaultNodeItem::moveRect() const
{
    return headerRect();
}

QRectF DefaultNodeItem::expandStateRect() const
{
    QRectF rect;
    if(hasChildNodes()){
        qreal iconSize = (SMALL_ICON_RATIO * getMinimumHeight()) / 2;
        rect.setWidth(iconSize);
        rect.setHeight(iconSize);
        rect.moveBottomRight(currentRect().marginsRemoved(getHeaderPadding()).bottomRight());
    }
    return rect;
}

void DefaultNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);


    if(state > RS_BLOCK){
        painter->setPen(Qt::NoPen);
        painter->setBrush(getBaseBodyColor());
        painter->drawRect(bodyRect());
        painter->setBrush(getBaseBodyColor().darker(105));
        painter->drawRect(headerRect());
        painter->setBrush(Qt::NoBrush);
    }





    if(state > RS_MINIMAL){

        paintPixmap(painter, lod, ER_STATUS, "Actions", "Computer");
        paintPixmap(painter, lod, ER_SECONDARY_ICON, "Actions", "Clock");


        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(255,255,255,150));
        painter->drawRect(getElementRect(ER_MAIN_ICON_OVERLAY));
        paintPixmap(painter, lod, ER_MAIN_ICON_OVERLAY, "Actions", "Key");

        if(isExpanded()){
            paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Contract");
        }else{
            paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Expand");
        }
    }

     if(state > RS_BLOCK){
         paintPixmap(painter, lod, ER_LOCKED_STATE, "Actions", "Lock_Closed");
        QString label = getData("label").toString();

        painter->setPen(Qt::black);
        painter->drawText(getElementRect(ER_PRIMARY_TEXT), Qt::AlignVCenter | Qt::AlignLeft, label);
        painter->drawText(getElementRect(ER_SECONDARY_TEXT), Qt::AlignCenter, label);

        painter->setPen(Qt::NoPen);

        //Check for disabled?
        painter->setBrush(Qt::white);

        painter->drawRect(getElementRect(ER_SECONDARY_TEXT));
     }

    NodeItemNew::paint(painter, option, widget);
}



void DefaultNodeItem::setupBrushes()
{
    QColor bodyColor = QColor(233,234,237).lighter(110);

    NodeItemNew* parentNodeItem = getParentNodeItem();
    if(parentNodeItem && parentNodeItem->getNodeItemKind() == DEFAULT_ITEM){
        DefaultNodeItem* parent = (DefaultNodeItem*)parentNodeItem;
        bodyColor = parent->getBaseBodyColor().darker(110);
    }
    setBodyColor(bodyColor);
}

QRectF DefaultNodeItem::mainIconRect() const
{
    qreal iconSize = ICON_RATIO * getMinimumHeight();

    QRectF iconRect(0, 0, iconSize, iconSize);
    iconRect.moveCenter(contractedRect().center());
    return iconRect;
}

QRectF DefaultNodeItem::statusIcon(RECT_VERTEX vert) const
{
    qreal iconSize = SMALL_ICON_RATIO * getMinimumHeight();
    QRectF rect(0, 0, iconSize, iconSize);
    QRectF hRect = headerRect().marginsRemoved(getHeaderPadding());

    switch(vert){
    case RV_TOPLEFT:{
        rect.moveTopLeft(hRect.topLeft());
        break;
    }
    case RV_TOPRIGHT:{
        rect.moveTopRight(hRect.topRight());
        break;
    }
    case RV_BOTTOMLEFT:{
        rect.moveBottomLeft(hRect.bottomLeft());
        break;
    }
    case RV_BOTTOMRIGHT:{
        rect.moveBottomRight(hRect.bottomRight());
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
    QRectF cRect = headerRect().marginsRemoved(getHeaderPadding());
    rect.setWidth(cRect.right() - mainIconRect().right());
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

QRectF DefaultNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
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
    case ER_SECONDARY_TEXT:
        return topLabelRect();
    case ER_PRIMARY_TEXT:
        return rightLabelRect();
    case ER_EXPANDED_STATE:
        return expandStateRect();
    case ER_EXPANDCONTRACT:
        return mainIconRect();
    default:
        return NodeItemNew::getElementRect(rect);
    }
}

void DefaultNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
}


QRectF DefaultNodeItem::getResizeRect(RECT_VERTEX vert)
{
    return NodeItemNew::getResizeRect(vert);
}

