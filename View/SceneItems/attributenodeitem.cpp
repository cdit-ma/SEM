#include "attributenodeitem.h"

AttributeNodeItem::AttributeNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{

    setMoveEnabled(true);
    setResizeEnabled(false);
    setExpandEnabled(false);

    height = DEFAULT_SIZE / 2.0;
    width = DEFAULT_SIZE * 1.5;

    mainTextFont.setPixelSize(7);

    setMinimumHeight(height);
    setMinimumWidth(width);

    setExpandedWidth(width);
    setExpandedHeight(height);
    setExpanded(true);

    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    addRequiredData("x");
    addRequiredData("y");
    addRequiredData("value");

    reloadRequiredData();

    isInstance = getData("kind").toString().endsWith("Instance");

}

QRectF AttributeNodeItem::mainRect() const
{
    QRectF mainRect(0,0,getExpandedWidth(), getExpandedHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;
}

QRectF AttributeNodeItem::mainIconRect() const
{
    QRectF rect;
    rect.setHeight(height);
    rect.setWidth(height);
    rect.moveTopLeft(mainRect().topLeft());
    return rect;
}

QRectF AttributeNodeItem::bottomLabelRect() const
{
    QRectF rect;
    rect.setHeight(height/2);
    rect.setWidth(width-mainIconRect().width());
    rect.moveTopLeft(labelRect().bottomLeft());
    return rect;
}

QRectF AttributeNodeItem::labelRect() const
{
    QRectF rect;
    rect.setWidth(width-mainIconRect().width());
    rect.setHeight(height/2);
    rect.moveTopLeft(mainIconRect().topRight());
    return rect;
}

void AttributeNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setFont(mainTextFont);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->save();
        painter->setPen(Qt::NoPen);

        painter->setBrush(getBodyColor());
        painter->drawRect(mainRect());
        painter->setBrush(getBodyColor().lighter(110));
        painter->drawRect(bottomLabelRect());

        painter->setPen(Qt::black);

        if(isInstance){
            painter->drawText(bottomLabelRect(), Qt::AlignCenter, getData("value").toString());
        } else {
            painter->drawText(bottomLabelRect(), Qt::AlignCenter, getData("type").toString());
        }
        painter->drawText(labelRect(), Qt::AlignCenter, getData("label").toString());

        painter->restore();
    }

    QPair<QString, QString> icon = getIconPath();
    paintPixmap(painter, lod, ER_MAIN_ICON, icon.first, icon.second);

    NodeItemNew::paint(painter, option, widget);
}

QRectF AttributeNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MAIN_ICON:
        return mainIconRect();
    case ER_PRIMARY_TEXT:
        return bottomLabelRect();
    case ER_SECONDARY_TEXT:
        return labelRect();
    default:
        return NodeItemNew::getElementRect(rect);
    }
}

QPainterPath AttributeNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRect(mainRect());
        return path.simplified();
    }

    return NodeItemNew::getElementPath(rect);
}

void AttributeNodeItem::dataChanged(QString keyName, QVariant data)
{
    NodeItemNew::dataChanged(keyName, data);
    if(keyName == "value" || keyName == "type" || keyName == "label"){
        update();
    }
}



