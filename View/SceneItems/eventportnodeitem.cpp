#include "eventportnodeitem.h"
#include <QDebug>

EventPortNodeItem::EventPortNodeItem(NodeViewItem *viewItem, NodeItemNew *parentItem)
    :NodeItemNew(viewItem, parentItem, NodeItemNew::DEFAULT_ITEM)
{
    setMoveEnabled(true);
    setResizeEnabled(false);
    setExpandEnabled(false);

    height = DEFAULT_SIZE / 2.0;
    width = DEFAULT_SIZE * 1.5;

    mainTextFont.setPixelSize(7);

    setMinimumWidth(width);
    setMinimumHeight(height);

    setExpandedWidth(width);
    setExpandedHeight(height);
    setExpanded(true);

    setMargin(QMarginsF(10,10,10,10));
    setBodyPadding(QMarginsF(3,3,3,3));

    addRequiredData("x");
    addRequiredData("y");

    reloadRequiredData();
    iconRight = isOutEventPort();
    initPolys();
}

void EventPortNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    RENDER_STATE state = getRenderState(lod);
    painter->setFont(mainTextFont);

    if(state > RS_BLOCK) {
        painter->setClipRect(option->exposedRect);
        painter->save();

        //Draw port poly
        painter->setPen(Qt::NoPen);
        painter->setBrush(getBodyColor());
        painter->drawPolygon(mainIconPoly());

        //Draw sub-icon polys
        painter->save();
        painter->setBrush(getBodyColor().darker(110));
        painter->drawPolygon(subIconPoly());
        painter->drawPolygon(topIconPoly());
        painter->restore();

        //Draw label rect
        painter->drawRect(labelBGRect());

        //Draw text
        painter->setPen(Qt::black);
        painter->drawText(labelRect(), Qt::AlignCenter, getData("label").toString());

        painter->restore();
    }

    //Paint icons.
    //TODO: update to lock and other sub-icons.
    if(state > RS_BLOCK){
        QPair<QString, QString> icon = getIconPath();
        paintPixmap(painter, lod, ER_SECONDARY_ICON, icon.first, icon.second);
        paintPixmap(painter, lod, ER_LOCKED_STATE, icon.first, icon.second);
    }

    //Parent class paint
    NodeItemNew::paint(painter, option, widget);
}

QRectF EventPortNodeItem::getElementRect(EntityItemNew::ELEMENT_RECT rect) const
{
   switch(rect){
   case ER_MAIN_ICON:
       return mainIconRect();
   case ER_SECONDARY_ICON:
       return subIconRect();
   case ER_LOCKED_STATE:
       return lockedRect();
   default:
       return NodeItemNew::getElementRect(rect);
   }
}

QPainterPath EventPortNodeItem::getElementPath(EntityItemNew::ELEMENT_RECT rect) const
{
    switch(rect){
    case ER_SELECTION:
        QPainterPath path;
        path.setFillRule(Qt::WindingFill);
        path.addPolygon(mainIconPoly());
        path.addPolygon(subIconPoly());
        path.addPolygon(topIconPoly());
        path.addRect(labelRect());
        return path.simplified();
    }

    return NodeItemNew::getElementPath(rect);
}

QRectF EventPortNodeItem::mainIconRect() const
{
    QRectF rect(mainRect());
    if(iconRight){
        rect.setTopLeft(QPointF(mainRect().right()-width/3, mainRect().top()));
        rect.setBottomRight(mainRect().bottomRight());
    }else{
        rect.setTopLeft(mainRect().topLeft());
        rect.setBottomRight(QPointF(mainRect().left()+width/3, mainRect().bottom()));
    }
    return rect;
}

bool EventPortNodeItem::isOutEventPort()
{
    return getData("kind").toString().startsWith("Out");
}

bool EventPortNodeItem::isInEventPort()
{
    return getData("kind").toString().endsWith("In");
}

bool EventPortNodeItem::isDelegate()
{
    return getData("kind").toString().endsWith("Delegate");
}

QRectF EventPortNodeItem::mainRect() const
{
    QRectF mainRect(0, 0, getMinimumWidth(), getMinimumHeight());
    mainRect.moveTopLeft(getMarginOffset());
    return mainRect;
}

QRectF EventPortNodeItem::labelRect() const
{

    QRectF rect(mainRect());
    if(iconRight){
        rect.setTopLeft(QPointF(rect.left(), rect.top() + height/4));
        rect.setBottomRight(rect.bottomRight() + QPointF(-width/3, -height/4));
    }else{
        rect.setTopLeft(rect.topLeft() + QPointF(width/3, height/4));
        rect.setBottomRight(QPointF(rect.right(), rect.bottom() - height/4));
    }
    return rect;
}

QRectF EventPortNodeItem::labelBGRect() const
{
    QRectF rect(mainRect());
    if(iconRight){
        rect.setTopLeft(QPointF(rect.left(), rect.top() + height/4));
        rect.setBottomRight(rect.bottomRight() + QPointF(0, -height/4));
    }else{
        rect.setTopLeft(rect.topLeft() + QPointF(0, height/4));
        rect.setBottomRight(QPointF(rect.right(), rect.bottom() - height/4));
    }
    return rect;
}

QPolygonF EventPortNodeItem::mainIconPoly() const
{
    return iconRight ? rightIconPoly : leftIconPoly;
}

QRectF EventPortNodeItem::subIconRect() const
{
    QRectF rect;
    rect.setWidth(height/4);
    rect.setHeight(height/4);
    if(iconRight){
        rect.moveTopRight(subIconPoly().at(1));
    } else {
        rect.moveTopLeft(subIconPoly().at(0));
    }
    return rect;
}

QPolygonF EventPortNodeItem::subIconPoly() const
{
    return iconRight ? rightSubPoly : leftSubPoly;
}

QPolygonF EventPortNodeItem::topIconPoly() const
{
    return iconRight ? rightTopPoly : leftTopPoly;
}

QRectF EventPortNodeItem::lockedRect() const
{
    QRectF rect;
    rect.setWidth(height/4);
    rect.setHeight(height/4);
    if(iconRight){
        rect.moveTopLeft(topIconPoly().at(0));
    } else {
        rect.moveTopRight(topIconPoly().at(1));
    }
    return rect;
}

//Construct a bunch of polys
void EventPortNodeItem::initPolys()
{
    //Ratios ahoy
    QRectF rect(mainIconRect());
    if(leftIconPoly.isEmpty()){
        leftIconPoly.push_back(rect.topLeft());
        leftIconPoly.push_back(rect.topRight() - QPointF(height/4, 0));
        leftIconPoly.push_back(rect.topRight() + QPointF(0, height/4));
        leftIconPoly.push_back(rect.bottomRight() - QPointF(0, height/4));
        leftIconPoly.push_back(rect.bottomRight() - QPointF(height/4, 0));
        leftIconPoly.push_back(rect.bottomLeft());
    }
    if(rightIconPoly.isEmpty()){
        rightIconPoly.push_back(rect.topLeft() + QPointF(height/4,0));
        rightIconPoly.push_back(rect.topRight());
        rightIconPoly.push_back(rect.bottomRight());
        rightIconPoly.push_back(rect.bottomLeft() + QPointF(height/4, 0));
        rightIconPoly.push_back(rect.bottomLeft() + QPointF(0, -height/4));
        rightIconPoly.push_back(rect.topLeft() + QPointF(0, height/4));
    }
    if(leftSubPoly.isEmpty()){
        leftSubPoly.push_back(leftIconPoly.at(3));
        leftSubPoly.push_back(leftIconPoly.at(3) + QPointF(height/2, 0));
        leftSubPoly.push_back(leftIconPoly.at(4) + QPointF(height/2, 0));
        leftSubPoly.push_back(leftIconPoly.at(4));
    }

    if(rightSubPoly.isEmpty()){
        rightSubPoly.push_back(rightIconPoly.at(4) - QPointF(height/2, 0));
        rightSubPoly.push_back(rightIconPoly.at(4));
        rightSubPoly.push_back(rightIconPoly.at(3));
        rightSubPoly.push_back(rightIconPoly.at(3) - QPointF(height/2, 0));
    }

    if(rightTopPoly.isEmpty()){
        rightTopPoly = leftSubPoly;
        rightTopPoly.translate(-(width/3 + height/4), -3 * height/4);
    }

    if(leftTopPoly.isEmpty()){
        leftTopPoly = rightSubPoly;
        leftTopPoly.translate(width/3 + height/4, -3 * height/4);
    }
}

void EventPortNodeItem::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "label"){
        update();
    }
    NodeItemNew::dataChanged(keyName, data);
}
