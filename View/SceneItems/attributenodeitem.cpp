#include "attributenodeitem.h"

AttributeNodeItem::AttributeNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{

    setMoveEnabled(true);
    setResizeEnabled(false);
    setExpandEnabled(false);

    height = DEFAULT_SIZE / 4.0;
    width = DEFAULT_SIZE * 1.5;

    mainTextFont.setPixelSize(7);

    setMinimumHeight(height);
    setMinimumWidth(width);

    setExpandedWidth(width);
    setExpandedHeight(height);
    setExpanded(true);

    setMargin(QMarginsF(10,15,10,10));
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

QRectF AttributeNodeItem::labelRect() const
{
    QRectF rect;
    rect.setHeight(height);
    rect.setWidth(5*height);
    rect.moveTopLeft(mainIconRect().topRight());
    return rect;
}

QRectF AttributeNodeItem::topLabelRect() const
{
    QRectF rect;
    rect.setWidth(width);
    rect.setHeight(getMargin().top());
    rect.moveBottomLeft(mainRect().topLeft());
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

        painter->setPen(Qt::black);

        if(isInstance){
            painter->drawText(labelRect(), Qt::AlignCenter, getData("value").toString());
        } else {
            painter->drawText(labelRect(), Qt::AlignCenter, getData("type").toString());
        }
        painter->drawText(topLabelRect(), Qt::AlignCenter, getData("label").toString());

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
    case ER_MAIN_LABEL:
        return labelRect();
    case ER_SECONDARY_LABEL:
        return topLabelRect();
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



