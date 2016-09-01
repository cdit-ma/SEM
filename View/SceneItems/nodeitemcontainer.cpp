#include "nodeitemcontainer.h"
#include <QDebug>

NodeItemContainer::NodeItemContainer(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    rightIcon = false;
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(true);

    height = DEFAULT_SIZE / 2.0;
    width = DEFAULT_SIZE / 2.0;

    setMinimumHeight(height);
    setMinimumWidth(width*5);

    setExpandedWidth(width*4);
    setExpandedHeight(height);

    setMargin(QMarginsF(height/2,2,2,2));
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
            painter->drawRect(outerKindRect());
        }
        painter->restore();

        painter->setPen(Qt::black);

        painter->drawText(innerKindRect(), Qt::AlignVCenter|Qt::AlignCenter, getData("kind").toString());
        painter->drawText(innerLabelRect(),Qt::AlignVCenter|Qt::AlignCenter, getData("label").toString());

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
    case ER_PRIMARY_TEXT:
        return innerLabelRect();
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

void NodeItemContainer::setRightIcon(bool isRight)
{
    rightIcon = isRight;
    update();
}

QRectF NodeItemContainer::mainRect() const
{
    QRectF mainRect(0,0,getWidth(), getExpandedHeight()-headerRect().height());
    mainRect.moveTopLeft(headerRect().bottomLeft());
    return mainRect;
}

QRectF NodeItemContainer::headerRect() const
{
    QRectF rect(0,0,getWidth(), getMinimumHeight());
    rect.moveTopLeft(getMarginOffset());
    return rect;
}

QRectF NodeItemContainer::headerTextRect() const
{
    QRectF rect(headerRect());
    if(rightIcon){
        rect.setRight(headerRect().right() - headerRect().height());
    }else{
        rect.setLeft(headerRect().left() + headerRect().height());
    }
    return rect;
}

QRectF NodeItemContainer::outerIconRect() const
{
    QRectF rect(headerRect());
    if(rightIcon){
        rect.setLeft(rect.right()-rect.height());
    }else{
        rect.setRight(rect.left()+rect.height());
    }
    return rect;
}

QRectF NodeItemContainer::innerIconRect() const
{
    qreal iconSize = (5.0/6.0) * outerIconRect().height();
    QRectF rect(0,0,iconSize, iconSize);
    rect.moveCenter(outerIconRect().center());
    return rect;
}

QRectF NodeItemContainer::innerLabelRect() const
{
    QRectF rect(outerLabelRect());
    rect.setRight(outerLabelRect().right()-outerLabelRect().height());
    return rect;
}

QRectF NodeItemContainer::outerLabelRect() const
{
    QRectF rect(headerTextRect());
    rect.setHeight(headerTextRect().height()/2.0);
    return rect;
}

QRectF NodeItemContainer::innerKindRect() const
{
    QRectF rect(outerKindRect());
    rect.setRight(rect.right()-rect.height());
    return rect;
}

QRectF NodeItemContainer::outerKindRect() const
{
    QRectF rect(headerTextRect());
    rect.setHeight(headerTextRect().height()/2.0);
    rect.moveBottomLeft(headerTextRect().bottomLeft());
    return rect;
}

QRectF NodeItemContainer::deployedRect() const
{
    QRectF rect(headerTextRect());
    rect.setHeight(rect.height()/2.0);
    rect.setWidth(rect.height());
    rect.moveTopRight(headerTextRect().topRight());
    QPointF center = rect.center();
    rect.setHeight(rect.height() * 5.0/6.0);
    rect.setWidth(rect.width() * 5.0/6.0);
    rect.moveCenter(center);
    return rect;
}

QRectF NodeItemContainer::qosRect() const
{
    QRectF rect(headerTextRect());
    rect.setHeight(rect.height()/2.0);
    rect.setWidth(rect.height());
    rect.moveBottomRight(headerTextRect().bottomRight());
    QPointF center = rect.center();
    rect.setHeight(rect.height() * 5.0/6.0);
    rect.setWidth(rect.width() * 5.0/6.0);
    rect.moveCenter(center);
    return rect;
}

QRectF NodeItemContainer::expandStateRect() const
{
    QRectF rect;
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
    /*
    if(keyName == "isExpanded"){
        update();
        expanded = getData("isExpanded").toBool();
    }*/
    NodeItemNew::dataChanged(keyName, data);
}

QPointF NodeItemContainer::getCenterOffset() const
{
    QPointF point(headerRect().topLeft());
    QRectF r = headerRect();
    point = point + QPointF(r.height()/2.0, r.height()/2.0);

    return point;
}

