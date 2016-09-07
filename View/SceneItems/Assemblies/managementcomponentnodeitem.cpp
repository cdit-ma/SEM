#include "managementcomponentnodeitem.h"

ManagementComponentNodeItem::ManagementComponentNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::HARDWARE_ITEM)
{
    setMoveEnabled(true);
    setExpandEnabled(false);
    setResizeEnabled(false);

    qreal size = DEFAULT_SIZE/2.0;
    setMinimumWidth(size*2.1);
    setMinimumHeight(size);

    setExpandedWidth(size*2.1);
    setExpandedHeight(size);
    setExpanded(false);

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


        painter->save();

        //Paint label
        renderText(painter, lod, labelRect(), getData("label").toString(), 10);

        //Paint expanded right poly
        painter->setBrush(getBodyColor());
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(getRightPoly());

        painter->setPen(Qt::black);
        paintPixmap(painter, lod, ER_DEPLOYED, "Actions", "Computer");

        painter->restore();

        painter->setBrush(getBodyColor().darker(120));
        painter->drawPolygon(getLeftPoly());

        paintPixmap(painter, lod, ER_SECONDARY_ICON, "Actions", getData("type").toString());
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
        path.addPolygon(getRightPoly());

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
   case ER_MAIN_ICON:
       return mainIconRect();
   case ER_SECONDARY_ICON:
       return subIconRect();
   case ER_DEPLOYED:
       return secondSubIconRect();
   default:
       return NodeItemNew::getElementRect(rect);
   }
}

QRectF ManagementComponentNodeItem::mainIconRect() const
{
    return QRectF(getMarginOffset(), iconSize());
}

QRectF ManagementComponentNodeItem::mainRect() const
{
    QRectF mainRect(0, 0, getExpandedWidth(), getExpandedHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;
}

QRectF ManagementComponentNodeItem::subIconRect() const
{
    QRectF returnRect(rightRect());
    returnRect.setSize(QSize(16,16));
    returnRect.setTopLeft(rightRect().topLeft());

    //move one icon width left + 1 (padding) and 1 down (padding)
    returnRect.translate(QPointF(17,1));

    return returnRect;
}

QRectF ManagementComponentNodeItem::secondSubIconRect() const
{
    QRectF returnRect(subIconRect());
    returnRect.translate(subIconRect().width()+2, 0);
    returnRect.setSize(smallIconSize());

    return returnRect;
}

QRectF ManagementComponentNodeItem::thirdSubIconRect() const
{
    QRectF returnRect(secondSubIconRect());
    returnRect.translate(0, secondSubIconRect().height());

    return returnRect;
}

QRectF ManagementComponentNodeItem::rightRect() const
{
    QPointF topLeft = mainIconRect().center();
    QPointF bottomRight = mainRect().bottomRight() - QPointF(0,2);
    return QRectF(topLeft, bottomRight);
}

QRectF ManagementComponentNodeItem::labelRect() const
{
    QPointF topLeft = mainIconRect().topRight();
    QPointF bottomRight = getRightPoly().at(1);
    return QRectF(topLeft, bottomRight);
}

QPolygonF ManagementComponentNodeItem::getRightPoly() const
{
    return rightPolyExpanded;
}

QPolygonF ManagementComponentNodeItem::getLeftPoly() const
{
    return leftPoly;
}

void ManagementComponentNodeItem::setupPolys()
{
    if(leftPoly.isEmpty()){
        QRectF icon_rect = mainIconRect();
        icon_rect.setSize(QSizeF(36,36));
        icon_rect.moveCenter(icon_rect.center() - QPointF(2,2));
        leftPoly.push_back(QPointF((icon_rect.left()+icon_rect.width()/2), icon_rect.top()));
        leftPoly.push_back(QPointF(icon_rect.right(), icon_rect.top()+icon_rect.height()/2));
        leftPoly.push_back(QPointF(icon_rect.left()+icon_rect.width()/2, icon_rect.bottom()));
        leftPoly.push_back(QPointF(icon_rect.left(), icon_rect.top()+icon_rect.height()/2));
    }

    if(rightPolyExpanded.isEmpty()){
        rightPolyExpanded.push_back(mainIconRect().center());
        rightPolyExpanded.push_back(rightRect().topRight());
        rightPolyExpanded.push_back(rightRect().bottomRight() - QPointF(rightRect().height(), 0));
        rightPolyExpanded.push_back(getLeftPoly().at(2).toPoint());
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
