#include "managementcomponentnodeitem.h"

ManagementComponentNodeItem::ManagementComponentNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::HARDWARE_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(false);

    qreal size = DEFAULT_SIZE/2.0;
    setMinimumWidth(size);
    setMinimumHeight(size);

    setExpandedWidth(size*5);
    setExpandedHeight(size);
    setExpanded(true);

    setDefaultPen(Qt::NoPen);
    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    textHeight = size / 2.0;

    addRequiredData("x");
    addRequiredData("y");
    reloadRequiredData();
}

void ManagementComponentNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->setPen(Qt::NoPen);
        if(isExpanded()){
            painter->save();
            mainTextFont.setPixelSize(labelRect().height());
            painter->setPen(Qt::black);
            painter->drawText(labelRect(), Qt::AlignCenter, getData("kind").toString());
            painter->setBrush(Qt::white);
            painter->setPen(Qt::NoPen);
            painter->drawPolygon(rightPoly());
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::black);
            painter->drawText(nameTextRect(), Qt::AlignCenter, getData("label").toString());
            painter->restore();
        }


        painter->setBrush(Qt::white);
        painter->drawPolygon(leftPoly());
    }


    painter->save();
    painter->restore();
    QPair<QString, QString> icon = getIconPath();
    paintPixmap(painter, lod, ER_MAIN_ICON, icon.first, icon.second);


    NodeItemNew::paint(painter, option, widget);
}

QPainterPath ManagementComponentNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:{
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addPolygon(leftPoly());
        if(isExpanded()){
            path.addPolygon(rightPoly());
        }
        return path.simplified();
    }
    default:
        break;
    }


    return NodeItemNew::getElementPath(rect);
}

QRectF ManagementComponentNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
   switch(rect){
   case ER_EXPANDCONTRACT:
       return mainIconRect();
   case ER_MAIN_ICON:
       return mainIconRect();
   case ER_SECONDARY_ICON:
       return loggerIconRect();
   case ER_SECONDARY_TEXT:
       return nameTextRect();
   default:
       return NodeItemNew::getElementRect(rect);
   }
}

QRectF ManagementComponentNodeItem::mainIconRect() const
{
    qreal iconSize = getMinimumHeight();
    return QRectF(getMarginOffset(), QSizeF(iconSize, iconSize));
}

QRectF ManagementComponentNodeItem::mainRect() const
{
    QRectF mainRect(0, 0, getExpandedWidth(), getExpandedHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;
}

QRectF ManagementComponentNodeItem::loggerIconRect() const
{
    QPointF topLeft = rightRect().topLeft();
    QPointF bottomRight = rightRect().bottomLeft() + QPointF(rightRect().height(), 0);

    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::nameTextRect() const
{
    QPointF topLeft = loggerIconRect().topRight();
    QPointF bottomRight = rightPoly().at(2);
    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::rightRect() const
{
    QPointF topLeft = mainIconRect().topRight() + QPointF(0, (getMinimumHeight() / 2));
    QPointF bottomRight = mainRect().bottomRight();
    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::labelRect() const
{
    QPointF topLeft = mainIconRect().topRight();
    QPointF bottomRight = mainRect().topRight() + QPointF(0, textHeight);
    return QRectF(topLeft, bottomRight);
}

QPolygonF ManagementComponentNodeItem::rightPoly() const
{
    QVector<QPoint> points;
    points.push_back(rightRect().topLeft().toPoint());
    points.push_back(rightRect().topRight().toPoint());
    points.push_back(QPoint(rightRect().bottomRight().x()-rightRect().height(), rightRect().bottomRight().y()));
    points.push_back(leftPoly().at(2).toPoint());
    return QPolygonF(points);
}

QPolygonF ManagementComponentNodeItem::leftPoly() const
{
    QVector<QPoint> points;
    points.push_back(QPoint((mainIconRect().left()+getMinimumWidth()/2), mainIconRect().top()));
    points.push_back(QPoint(mainIconRect().right(), mainIconRect().top()+getMinimumHeight()/2));
    points.push_back(QPoint(mainIconRect().left()+getMinimumWidth()/2, mainIconRect().bottom()));
    points.push_back(QPoint(mainIconRect().left(), mainIconRect().top()+getMinimumHeight()/2));
    return QPolygonF(points);
}
