#include "assemblyeventportnodeitem.h"
#include <QDebug>
#include <QFontDatabase>

AssemblyEventPortNodeItem::AssemblyEventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :ContainerElementNodeItem(viewItem, parentItem)
{
    headerPadding = QMargins(2,2,2,2);
    textHeight = smallIconSize().height() + headerPadding.top();
    qreal size = iconSize().width() + smallIconSize().width() + headerPadding.left() + headerPadding.right();
    setMinimumWidth(size);
    setMinimumHeight(size);;
    setExpandedWidth(size);
    setExpandedHeight(size);

    setResizeEnabled(false);


    setExpanded(true);
    mainTextFont = QFont("Verdana");
    mainTextFont.setPixelSize(20);
}

void AssemblyEventPortNodeItem::setRightJustified(bool isRight)
{
    ContainerElementNodeItem::setRightJustified(isRight);
    update();
}

QRectF AssemblyEventPortNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MAIN_ICON:{
        return iconRect();
    }
    case ER_MAIN_LABEL:{
        return textInnerRect();
    }
    case ER_QOS:{
        return bottomSubIconRect();
    }
    case ER_DEPLOYED:{
        return topSubIconRect();
    }
    default:
        break;
    }

    return ContainerElementNodeItem::getElementRect(rect);
}

void AssemblyEventPortNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setClipRect(option->exposedRect);


    if(isExpanded()){
        painter->setBrush(getBodyColor());
        painter->setPen(Qt::NoPen);
        painter->drawRect(currentRect());
    }

    if(state > RS_BLOCK){
        painter->setBrush(getBodyColor().darker(110));
        painter->drawRect(textRect());
        painter->setFont(mainTextFont);
        painter->setPen(Qt::black);


        if(state > RS_MINIMAL){
            paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");
            paintPixmap(painter, lod, ER_QOS, "Actions", "Global");
        }

        QString text = getData("label").toString();
        renderText(painter, lod, textInnerRect(), text, 8);
    }

    ContainerElementNodeItem::paint(painter, option, widget);
}

QRectF AssemblyEventPortNodeItem::iconRect() const
{
    QRectF r;
    r.setSize(iconSize());

    if(isRightJustified()){
        r.moveBottomRight(iconsRect().bottomRight());
    }else{
        r.moveBottomLeft(iconsRect().bottomLeft());
    }
    return r;
}

QRectF AssemblyEventPortNodeItem::textRect() const
{
    QRectF r;
    r.setHeight(textHeight);
    r.setWidth(getWidth());
    r.moveTopLeft(currentRect().topLeft());
    return r;
}

QRectF AssemblyEventPortNodeItem::textInnerRect() const
{
    return textRect().adjusted(1, 1, -1, -1);
}

QRectF AssemblyEventPortNodeItem::iconsRect() const
{
    QRectF r;

    r.setWidth(iconSize().width() + smallIconSize().width());
    r.setHeight(iconSize().height());
    r.moveTopLeft(currentRect().topLeft() + QPoint(headerPadding.left(), textHeight));
    return r;
}


QRectF AssemblyEventPortNodeItem::topSubIconRect() const
{
    QRectF r;
    r.setSize(smallIconSize());
    if(isRightJustified()){
        r.moveTopLeft(iconsRect().topLeft());
    }else{
        r.moveTopRight(iconsRect().topRight());
    }
    return r;
}

QRectF AssemblyEventPortNodeItem::bottomSubIconRect() const
{
    QRectF r;
    r.setSize(smallIconSize());
    if(isRightJustified()){
        r.moveBottomLeft(iconsRect().bottomLeft());
    }else{
        r.moveBottomRight(iconsRect().bottomRight());
    }
    return r;
}
