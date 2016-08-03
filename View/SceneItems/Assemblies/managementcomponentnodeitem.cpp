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

    setupPolys();

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

        if(isLogger()){
            painter->save();
            painter->setBrush(getBodyColor().darker(110));
            painter->drawRect(subIconRect());
            painter->restore();
        }

        if(isExpanded()){
            painter->save();
            mainTextFont.setPixelSize(labelRect().height());
            painter->setPen(Qt::black);
            painter->drawText(labelRect(), Qt::AlignCenter, getData("label").toString());
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);

            painter->drawPolygon(getRightPoly());
            painter->setBrush(Qt::NoBrush);
            painter->setPen(Qt::black);
            painter->drawText(nameTextRect(), Qt::AlignCenter, getData("kind").toString());
            painter->restore();
        }
        painter->setBrush(getBodyColor().darker(110));
        painter->drawPolygon(getLeftPoly());
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
        path.addPolygon(getLeftPoly());
        path.addRect(subIconRect());
        if(isExpanded()){
            path.addPolygon(getRightPoly());
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
       return subIconRect();
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

QRectF ManagementComponentNodeItem::subIconRect() const
{
    QPointF topLeft = mainIconRect().topLeft() + QPointF(getMinimumHeight()/10, getMinimumHeight()/10);
    QPointF bottomRight = mainIconRect().bottomRight() - QPointF(getMinimumHeight()/10, getMinimumHeight()/10);

    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::nameTextRect() const
{
    QPointF topLeft = rightRect().topLeft();
    QPointF bottomRight = getRightPoly().at(2);
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

QPolygonF ManagementComponentNodeItem::getRightPoly() const
{
    return rightPoly;
}

QPolygonF ManagementComponentNodeItem::getLeftPoly() const
{
    return leftPoly;
}

void ManagementComponentNodeItem::setupPolys()
{
    if(leftPoly.isEmpty()){
        QRectF icon_rect = mainIconRect();
        leftPoly.push_back(QPointF((icon_rect.left()+getMinimumWidth()/2), icon_rect.top()));
        leftPoly.push_back(QPointF(icon_rect.right(), icon_rect.top()+getMinimumHeight()/2));
        leftPoly.push_back(QPointF(icon_rect.left()+getMinimumWidth()/2, icon_rect.bottom()));
        leftPoly.push_back(QPointF(icon_rect.left(), icon_rect.top()+getMinimumHeight()/2));
    }

    if(rightPoly.isEmpty()){
        rightPoly.push_back(mainIconRect().center());
        rightPoly.push_back(rightRect().topRight());
        rightPoly.push_back(QPointF(rightRect().bottomRight().x()-rightRect().height(), rightRect().bottomRight().y()));
        rightPoly.push_back(getLeftPoly().at(2).toPoint());
    }
}

bool ManagementComponentNodeItem::isLogger()
{
    return getData("label").toString() == "DDS_LOGGING_SERVER";
}

bool ManagementComponentNodeItem::isDeployed()
{
    return getData("kind").toString().startsWith("Out");
}
