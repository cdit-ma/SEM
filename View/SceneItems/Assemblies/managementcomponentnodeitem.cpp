#include "managementcomponentnodeitem.h"

ManagementComponentNodeItem::ManagementComponentNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::HARDWARE_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(true);
    setResizeEnabled(false);

    qreal size = DEFAULT_SIZE/2.0;
    setMinimumWidth(size*2);
    setMinimumHeight(size);

    setExpandedWidth(size*5);
    setExpandedHeight(size);
    setExpanded(true);

    setDefaultPen(Qt::NoPen);
    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    mainTextFont.setPixelSize(size / 3.5);

    setupPolys();

    addRequiredData("x");
    addRequiredData("y");

    reloadRequiredData();
}

void ManagementComponentNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);


    painter->setFont(mainTextFont);

    if(state > RS_BLOCK){
        painter->setClipRect(option->exposedRect);
        painter->setPen(Qt::NoPen);

        if(isExpanded()){
            painter->save();

            //Paint label
            painter->setPen(Qt::black);
            painter->drawText(labelRect(), Qt::AlignCenter, getData("label").toString());

            //Paint expanded right poly
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawPolygon(getRightPolyExpanded());

            painter->setPen(Qt::black);
            painter->drawRect(secondSubIconRect());
            painter->drawRect(thirdSubIconRect());

            painter->restore();
        } else {
            painter->save();
            //Paint non expanded right poly
            painter->setBrush(getBodyColor());
            painter->setPen(Qt::NoPen);
            painter->drawPolygon(getRightPoly());
            painter->restore();
        }
        painter->setBrush(getBodyColor().darker(120));
        painter->drawPolygon(getLeftPoly());

        painter->setBrush(getBodyColor());
        painter->drawRect(subIconRect());
        paintPixmap(painter, lod, ER_SECONDARY_ICON, "Actions", "Database");
    }

    NodeItemNew::paint(painter, option, widget);
}

QPainterPath ManagementComponentNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:{
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addPolygon(getLeftPoly());
        if(isExpanded()){
            path.addPolygon(getRightPolyExpanded());
        } else {
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
   case ER_EXPANDED_STATE:
       return expandStateRect();
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
    QPointF topLeft = rightRect().topLeft();
    QPointF bottomRight = QPointF(rightRect().center().x(), rightRect().bottom());
    QRectF returnRect(topLeft, bottomRight);
   // returnRect.translate(-returnRect.width(),0);

    return returnRect;
}

QRectF ManagementComponentNodeItem::secondSubIconRect() const
{
    QRectF returnRect(subIconRect());
    returnRect.translate(subIconRect().width() * 2, 0);

    return returnRect;
}

QRectF ManagementComponentNodeItem::thirdSubIconRect() const
{
    QRectF returnRect(subIconRect());
    returnRect.translate(subIconRect().width() * 4, 0);

    return returnRect;
}

QRectF ManagementComponentNodeItem::rightRectExpanded() const
{
    QPointF topLeft = mainIconRect().topRight() + QPointF(0, (getMinimumHeight() / 2));;
    QPointF bottomRight = mainRect().bottomRight();
    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::rightRect() const
{
    QPointF topLeft = mainIconRect().topRight() + QPointF(0, (getMinimumHeight() / 2));;
    QPointF bottomRight = mainRect().bottomLeft() + 2*QPointF(mainRect().height(), 0);
    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::labelRect() const
{
    QPointF topLeft = mainIconRect().topRight();
    QPointF bottomRight = getRightPolyExpanded().at(1);
    return QRectF(topLeft, bottomRight);
}

QPolygonF ManagementComponentNodeItem::getRightPoly() const
{
    return rightPoly;
}

QPolygonF ManagementComponentNodeItem::getRightPolyExpanded() const
{
    return rightPolyExpanded;
}

QPolygonF ManagementComponentNodeItem::getLeftPoly() const
{
    return leftPoly;
}

QRectF ManagementComponentNodeItem::expandStateRect() const
{
    QRectF rect;

    qreal iconSize = (SMALL_ICON_RATIO * getMinimumHeight());
    rect.setWidth(iconSize);
    rect.setHeight(iconSize);
    rect.moveBottomLeft(rightPoly.at(2));
    //rect.moveCenter(subIconRect().center());
    //rect.translate(subIconRect().width(), 0);
    //rect.translate(-rect.width(),0);

    return rect;
}

void ManagementComponentNodeItem::setupPolys()
{
    if(leftPoly.isEmpty()){
        QRectF icon_rect = mainIconRect();
        leftPoly.push_back(QPointF((icon_rect.left()+icon_rect.width()/2), icon_rect.top()));
        leftPoly.push_back(QPointF(icon_rect.right(), icon_rect.top()+icon_rect.height()/2));
        leftPoly.push_back(QPointF(icon_rect.left()+icon_rect.width()/2, icon_rect.bottom()));
        leftPoly.push_back(QPointF(icon_rect.left(), icon_rect.top()+icon_rect.height()/2));
    }

    if(rightPolyExpanded.isEmpty()){
        rightPolyExpanded.push_back(mainIconRect().center());
        rightPolyExpanded.push_back(rightRectExpanded().topRight());
        rightPolyExpanded.push_back(rightRectExpanded().bottomRight() - QPointF(rightRectExpanded().height(), 0));
        rightPolyExpanded.push_back(getLeftPoly().at(2).toPoint());
    }

    if(rightPoly.isEmpty()){
        rightPoly.push_back(mainIconRect().center());
        rightPoly.push_back(rightRect().topRight());
        rightPoly.push_back(rightRect().bottomRight() - QPointF(rightRect().height(), 0));
        rightPoly.push_back(getLeftPoly().at(2));
    }
}

bool ManagementComponentNodeItem::isLogger()
{
    return getData("label").toString() == "DDS_LOGGING_SERVER";
}

bool ManagementComponentNodeItem::isDeployed()
{
    return true;
}
