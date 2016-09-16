#include "hardwarenodeitem.h"
#include <QDebug>
HardwareNodeItem::HardwareNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem):ContainerElementNodeItem(viewItem, parentItem)
{
    setMoveEnabled(true);
    setExpandEnabled(false);
    setResizeEnabled(false);


    qreal size = DEFAULT_SIZE / 2.0;
    setMinimumWidth(3*size);
    setMinimumHeight(size);

    setExpandedWidth(3 * size);
    setExpandedHeight(size);
    setExpanded(true);


    setDefaultPen(Qt::NoPen);
    setBodyPadding(QMarginsF(3, 3, 3, 3));
    setMargin(QMarginsF(5, 5, 5, 5));
    cornerRadius = 5;
    textHeight = size / 2.0;

    mainTextFont.setPixelSize(size / 3.5);

    //setupBrushes();

    addRequiredData("x");
    addRequiredData("y");

    setPrimaryTextKey("label");
    setSecondaryTextKey("ip_address");

    reloadRequiredData();
}

QPainterPath HardwareNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:{
        //Add in the Circle Rect
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addRoundedRect(mainIconRect(), cornerRadius, cornerRadius);
        path.addRoundedRect(rightRect(), cornerRadius, cornerRadius);

        return path.simplified();
    }
    default:
        break;
    }
    return NodeItemNew::getElementPath(rect);
}

QRectF HardwareNodeItem::mainIconRect() const
{
    qreal iconSize = getMinimumHeight();
    return QRectF(getMarginOffset(), QSizeF(iconSize, iconSize));
}

QRectF HardwareNodeItem::mainRect() const
{
    QRectF mainRect(0, 0, getExpandedWidth(), getExpandedHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;
}

QRectF HardwareNodeItem::ipIconRect() const
{
    QRectF rightBR = rightRect();

    QPointF topLeft = rightBR.topLeft() + QPointF(cornerRadius * 2, 0 );
    QPointF bottomRight = topLeft + QPointF(rightBR.height(), rightBR.height());
    return QRectF(topLeft, bottomRight);
}

QRectF HardwareNodeItem::ipTextRect() const
{
    QPointF topLeft = ipIconRect().topRight();
    QPointF bottomRight = rightRect().bottomRight();
    return QRectF(topLeft, bottomRight);
}

QRectF HardwareNodeItem::rightRect() const
{
    QPointF topLeft = mainIconRect().topRight() + QPointF(-cornerRadius* 2, (getMinimumHeight() / 2));
    QPointF bottomRight = mainRect().bottomRight();
    return QRectF(topLeft, bottomRight);
}

QRectF HardwareNodeItem::labelRect() const
{
    QPointF topLeft = mainIconRect().topRight() + QPointF(2,0);
    QPointF bottomRight = mainRect().topRight() + QPointF(0, textHeight);
    return QRectF(topLeft, bottomRight);
}

void HardwareNodeItem::setupBrushes()
{
    QColor bodyColor = QColor(233,234,237).lighter(110);

    NodeItemNew* parentNodeItem = getParentNodeItem();
    if(parentNodeItem){
        bodyColor = parentNodeItem->getBaseBodyColor().darker(110);
    }
    setBaseBodyColor(bodyColor);
}

void HardwareNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    painter->setFont(mainTextFont);
    //Set Clip Rectangle
    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->setPen(Qt::NoPen);
        painter->setBrush(getBodyColor());

        painter->drawRoundedRect(rightRect(),cornerRadius, cornerRadius);

        painter->setBrush(getBodyColor().darker(120));
        painter->drawRoundedRect(mainIconRect(), cornerRadius, cornerRadius);

        paintPixmap(painter, lod, ER_SECONDARY_ICON, "Data", "ip_address");
    }
    //Call Base class
    NodeItemNew::paint(painter, option, widget);
}

QRectF HardwareNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_MAIN_ICON:
        return mainIconRect();
    case ER_PRIMARY_TEXT:
        return labelRect();
    case ER_SECONDARY_ICON:
        return ipIconRect();
    case ER_SECONDARY_TEXT:
        return ipTextRect();
    default:
        return NodeItemNew::getElementRect(rect);
    }

}
