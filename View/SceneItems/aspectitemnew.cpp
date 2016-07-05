#include "aspectitemnew.h"
#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>
AspectItemNew::AspectItemNew(NodeViewItem *viewItem, NodeItemNew *parentItem, VIEW_ASPECT aspect):NodeItemNew(viewItem, parentItem, NodeItemNew::ASPECT_ITEM)
{
    setAspect(aspect);
    setExpanded(true);
    setMinimumWidth(600);
    setMinimumHeight(400);

    setMargin(QMarginsF(10,10,10,10));
    setPadding(QMarginsF(10,10,10,10));


    setGridEnabled(true);

    setDefaultPen(Qt::NoPen);

    //Force
    QGraphicsObject::setPos(getAspectPos());

    aspectLabel = GET_ASPECT_NAME(aspect).toUpper();
    backgroundColor = GET_ASPECT_COLOR(aspect).darker(150);

    mainTextColor = backgroundColor.darker(110);
    mainTextFont.setPixelSize(70);
    mainTextFont.setBold(true);


    connect(this, SIGNAL(sizeChanged(QSizeF)), this, SLOT(resetPos()));
}

QRectF AspectItemNew::getElementRect(EntityItemNew::ELEMENT_RECT rect)
{
    switch(rect){
        case ER_MAIN_LABEL:
            return getMainTextRect();
        default:
            return QRectF();
    }
}

void AspectItemNew::resetPos()
{
    //Force Position
    setPos(getAspectPos());
}

QPointF AspectItemNew::getAspectPos()
{
    VIEW_ASPECT_POS aspectPos = GET_ASPECT_POS(getAspect());
    qreal aspectOffset = 10;

    qreal width = boundingRect().width();
    qreal height = boundingRect().height();

    switch(aspectPos){
    case VAP_TOPLEFT:
        return QPointF(-aspectOffset - width, -aspectOffset - height);
    case VAP_TOPRIGHT:
        return QPointF(aspectOffset, -aspectOffset - height);
    case VAP_BOTTOMRIGHT:
        return QPointF(aspectOffset, aspectOffset);
    case VAP_BOTTOMLEFT:
        return QPointF(-aspectOffset - width, aspectOffset);
    default:
        return QPointF(0,0);
    }
}

QRectF AspectItemNew::getMainTextRect() const
{
    QRectF rect;

    QMarginsF padding = getPadding();


    qreal width = getWidth() - (padding.left() + padding.right());
    qreal height = mainTextFont.pixelSize();

    QPointF rectCenter = expandedRect().center();
    rectCenter.setY(expandedRect().bottom() - padding.bottom() - (height / 2));

    rect.setWidth(width);
    rect.setHeight(height);
    rect.moveCenter(rectCenter);
    return rect;
}

void AspectItemNew::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());

    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    painter->setPen(getPen(lod));
    painter->setBrush(backgroundColor);
    painter->drawRect(expandedRect());





    painter->setPen(mainTextColor);
    painter->setFont(mainTextFont);
    painter->drawText(getMainTextRect(), Qt::AlignCenter, aspectLabel);

    NodeItemNew::paint(painter, option, widget);
}

void AspectItemNew::setPos(const QPointF &pos)
{
    Q_UNUSED(pos);
    //Call base class.
    NodeItemNew::setPos(getAspectPos());
}

QRectF AspectItemNew::gridRect() const
{
    QRectF rect = currentRect();
    QMarginsF padding = getPadding();

    rect.adjust(padding.left(), padding.top(), -padding.right(), -padding.bottom());
    return rect;
}

