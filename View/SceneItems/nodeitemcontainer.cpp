#include "nodeitemcontainer.h"
#include <QDebug>

NodeItemContainer::NodeItemContainer(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);

    height = DEFAULT_SIZE / 2.0;
    width = DEFAULT_SIZE / 2.0;

    setMinimumHeight(height);
    setMinimumWidth(width*5);

    setExpandedWidth(width*4);
    setExpandedHeight(height);

    setExpanded(false);

    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(0,0,0,0));

    addRequiredData("x");
    addRequiredData("y");

    reloadRequiredData();
}

QRectF NodeItemContainer::bodyRect() const
{
    return mainRect();
}

void NodeItemContainer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setFont(mainTextFont);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->save();
        if(isExpanded()){
            painter->save();
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawRect(mainRect());
            painter->restore();
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(getBodyColor().darker(110));
        painter->drawRect(headerRect());
        painter->save();

        if(!getData("locked").toBool()){
            painter->setBrush(Qt::white);
            painter->drawRect(labelBGRect());
        }
        painter->restore();

        painter->setPen(Qt::black);

        painter->drawText(kindLabelRect(), Qt::AlignVCenter|Qt::AlignCenter, getData("kind").toString());
        painter->drawText(labelRect(),Qt::AlignVCenter|Qt::AlignCenter, getData("label").toString());

        paintPixmap(painter, lod, ER_EXPANDED_STATE, "Actions", "Expand");
        paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");
        paintPixmap(painter, lod, ER_QOS, "Actions", "Global");

        painter->restore();

    }
    NodeItemNew::paint(painter, option, widget);
}

QRectF NodeItemContainer::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_EXPANDCONTRACT:
        return outerIconRect();
    case ER_EXPANDED_STATE:
        return expandStateRect();
    case ER_SELECTION:
        return mainRect();
    case ER_MAIN_ICON:
        return innerIconRect();
    case ER_MAIN_LABEL:
        return labelRect();
    case ER_LOCKED_STATE:
        return deployedRect();
    case ER_QOS:
        return qosRect();
    case ER_DEPLOYED:
        return deployedRect();
    default:
        return NodeItemNew::getElementRect(rect);
    }
}

QPainterPath NodeItemContainer::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    QPainterPath path;
    switch(rect){
    case ER_SELECTION:
        path.setFillRule(Qt::WindingFill);
        path.addRect(headerRect());
        if(isExpanded()){
            path.addRect(mainRect());
        }
        return path.simplified();
    default:
        return NodeItemNew::getElementPath(rect);
    }

}

QRectF NodeItemContainer::mainRect() const
{
    QRectF mainRect(0,0,getExpandedWidth(), getExpandedHeight()-headerRect().height());
    mainRect.moveTopLeft(headerRect().bottomLeft());
    return mainRect;
}

QRectF NodeItemContainer::headerRect() const
{
    QRectF rect(0,0,getWidth(), getMinimumHeight());
    rect.moveTopLeft(getMarginOffset());
    return rect;
}

QRectF NodeItemContainer::outerIconRect() const
{
    QRectF rect(headerRect());
    rect.setWidth(headerRect().height());
    return rect;
}

QRectF NodeItemContainer::innerIconRect() const
{
    qreal iconSize = (5.0/6.0) * outerIconRect().height();
    QRectF rect(0,0,iconSize, iconSize);
    rect.moveCenter(outerIconRect().center());
    return rect;
}

QRectF NodeItemContainer::labelRect() const
{
    QRectF rect(labelBGRect());
    rect.setWidth(rect.width() - getMinimumHeight()/2);
    return rect;
}

QRectF NodeItemContainer::labelBGRect() const
{
    QRectF rect(headerRect());
    rect.setWidth(getWidth() - outerIconRect().width());
    rect.setHeight(getMinimumHeight()/2);
    rect.moveBottomLeft(outerIconRect().bottomRight());
    return rect;
}

QRectF NodeItemContainer::kindLabelRect() const
{
    QRectF rect(headerRect());
    rect.setWidth(getWidth() - outerIconRect().width() - getMinimumHeight()/2);
    rect.setHeight(getMinimumHeight()/2);
    rect.moveTopLeft(outerIconRect().topRight());
    return rect;
}

QRectF NodeItemContainer::deployedRect() const
{
    QRectF rect(headerRect());
    rect.setHeight(rect.height()/2);
    rect.setWidth(rect.height());
    rect.moveTopRight(headerRect().topRight());
    QPointF center = rect.center();
    rect.setHeight(rect.height() * 5.0/6.0);
    rect.setWidth(rect.width() * 5.0/6.0);
    rect.moveCenter(center);
    return rect;
}

QRectF NodeItemContainer::qosRect() const
{
    QRectF rect(headerRect());
    rect.setHeight(rect.height()/2);
    rect.setWidth(rect.height());
    rect.moveBottomRight(headerRect().bottomRight());
    QPointF center = rect.center();
    rect.setHeight(rect.height() * 5.0/6.0);
    rect.setWidth(rect.width() * 5.0/6.0);
    rect.moveCenter(center);
    return rect;
}

QRectF NodeItemContainer::expandStateRect() const
{
    QRectF rect(mainRect());
    rect.setHeight(8);
    rect.setWidth(8);
    if(expanded){
        rect.moveBottomRight(mainRect().bottomRight());
    } else {
        rect.moveBottomRight(headerRect().bottomRight());
    }

    QPointF center = rect.center();
    rect.setSize(QSizeF(rect.height()*(4.0/6.0), rect.width()*(4.0/6.0)));
    rect.moveCenter(center);
    return rect;
}

void NodeItemContainer::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "isExpanded"){
        update();
        expanded = getData("isExpanded").toBool();
    }
    NodeItemNew::dataChanged(keyName, data);
}

