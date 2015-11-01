#include "aspectitem.h"
#include "../nodeview.h"
#define ASPECT_ROW_COUNT 6
#define ASPECT_COL_COUNT 4

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

//#define MARGIN_RATIO 0.10 //NORMAL
#define MARGIN_RATIO (1.0 / 18.0) //NORMAL
//#define ICON_RATIO 0.80 //LARGE


#define ICON_RATIO (4.5 / 6.0) //LARGE
//#define ICON_RATIO (5.0 / 6.0) //LARGE
//#define ICON_RATIO (3.0 / 4.0) //LARGE
#define SMALL_ICON_RATIO ((1.0 / 6.0))
#define TOP_LABEL_RATIO (1.0 / 6.0)
#define RIGHT_LABEL_RATIO (1.5 / 6.0)
#define BOTTOM_LABEL_RATIO (1.0 / 9.0)
#define LABEL_RATIO (1 - ICON_RATIO)



#define SELECTED_LINE_WIDTH 3
#define ITEM_SIZE 72
#define ASPECT_COL_COUNT 6
#define ASPECT_ROW_COUNT 4

#define ACTUAL_ITEM_SIZE (ITEM_SIZE + ((MARGIN_RATIO * 2) * ITEM_SIZE))
#define GRID_COUNT 2
#define GRID_PADDING_RATIO .25

#define GRID_PADDING_SIZE (GRID_PADDING_RATIO * ACTUAL_ITEM_SIZE)
#define GRID_SIZE ((ACTUAL_ITEM_SIZE + GRID_PADDING_SIZE) / GRID_COUNT)

#define SNAP_PERCENTAGE .5
#include <QDebug>


#define ALL 0
#define CONNECTED 1
#define UNCONNECTED 2

#define THEME_LIGHT 0
#define THEME_DARK 1

#define THEME_DARK_NEUTRAL 10
#define THEME_DARK_COLOURED 11

AspectItem::AspectItem(Node *node, GraphMLItem *parent, ASPECT_POS pos) : NodeItem(node,parent, GraphMLItem::ASPECT_ITEM)
{
    QString nodeKind = node->getNodeKind();

    if(nodeKind.startsWith("Interface")){
        backgroundColor = QColor(110,210,210);
        aspectLabel = "INTERFACES";
        aspectKind = "Definitions";
    }else if(nodeKind.startsWith("Behaviour")){
        backgroundColor = QColor(254,184,126);
        aspectLabel = "BEHAVIOUR";
        aspectKind = "Workload";
    }else if(nodeKind.startsWith("Hardware")){
        backgroundColor = QColor(110,170,220);
        aspectLabel = "HARDWARE";
        aspectKind = "Hardware";
    }else if(nodeKind.startsWith("Assembly")){
        backgroundColor = QColor(255,160,160);
        aspectLabel = "ASSEMBLIES";
        aspectKind = "Assembly";
    }

    textColor = backgroundColor.darker(110);

    width = GRID_SIZE * GRID_COUNT * ASPECT_COL_COUNT;
    height = GRID_SIZE * GRID_COUNT * ASPECT_ROW_COUNT;
    minimumWidth = width;
    minimumHeight = height;

    textFont.setBold(true);
    textFont.setPointSize(minimumHeight * (LABEL_RATIO /2));

    aspectPos = pos;
    setPos(getAspectPos());

    connect(this, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(sizeChanged()));
}



QRectF AspectItem::boundingRect() const
{
    QRectF rect;
    qreal itemMargin = getItemMargin() * 2;
    rect.setWidth(width + itemMargin);
    rect.setHeight(height + itemMargin);
    return rect;
}

void AspectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);




    if(isSelected()){
        //Setup the Pen
        QPen pen = painter->pen();

        //pen = this->selectedPen;
        //pen.setWidthF(selectedPenWidth);


        if(isHovered()){
            if(pen.color() == Qt::black){
                pen.setColor(QColor(120,120,120));
            }else{
                pen.setColor(pen.color().lighter());
            }
        }

        //Trace the boundary
        painter->setPen(pen);
    }else{
        painter->setPen(Qt::NoPen);
    }

    painter->setBrush(backgroundColor);
    painter->drawRect(boundingRect());

    painter->setPen(textColor);
    painter->setFont(textFont);
    painter->drawText(boundingRect(), Qt::AlignHCenter | Qt::AlignBottom, aspectLabel);



    //New Code
    if(drawGridLines()){
        painter->setPen(Qt::gray);
        QPen linePen = painter->pen();

        linePen.setStyle(Qt::DotLine);
        painter->setPen(linePen);
        painter->drawLines(getGridLines());

        linePen.setColor(Qt::darkGray);
        linePen.setWidthF(2);

        painter->setBrush(Qt::NoBrush);
        painter->setPen(linePen);
        painter->drawRects(getChildrenGridOutlines());
    }
}


qreal AspectItem::getWidth()
{
    return width;
}

qreal AspectItem::getHeight()
{
    return height;
}

void AspectItem::setPos(const QPointF pos)
{
    QGraphicsObject::setPos(pos);
}

void AspectItem::setWidth(qreal w)
{
    if(w < minimumWidth){
        w = minimumWidth;
    }

    if(w == width){
        return;
    }

    prepareGeometryChange();

    width = w;
    emit GraphMLItem_SizeChanged();
}

void AspectItem::setHeight(qreal h)
{
    if(h < minimumHeight){
        h = minimumHeight;
    }

    if(h == height){
        return;
    }

    prepareGeometryChange();

    height = h;
    emit GraphMLItem_SizeChanged();
}

QPointF AspectItem::getAspectPos()
{
    qreal itemMargin = 0;
    if(getParent()){
        itemMargin = getParent()->boundingRect().width() / 4;
    }

    qreal totalW = boundingRect().width();
    qreal totalH = boundingRect().height();

    switch(aspectPos){
    case AP_TOPLEFT:
        return QPointF(-itemMargin - totalW, -itemMargin - totalH);
    case AP_TOPRIGHT:
        return QPointF(itemMargin, -itemMargin - totalH);
    case AP_BOTRIGHT:
        return QPointF(itemMargin, itemMargin);
    case AP_BOTLEFT:
        return QPointF(-itemMargin - totalW, itemMargin);
    default:
        return QPointF(0,0);
    }
}

void AspectItem::graphMLDataChanged(GraphMLData *data)
{

}

void AspectItem::aspectsChanged(QStringList aspects)
{
    if (aspects.contains(aspectKind)) {
        setVisible(true);
    } else {
        setVisible(false);
    }
}

void AspectItem::sizeChanged()
{
    setPos(getAspectPos());
}

QRectF AspectItem::gridRect() const
{
   QRectF rect = boundingRect();
   rect.adjust(getItemMargin(), getItemMargin(), -getItemMargin() * 2, -getItemMargin()* 2);
   return rect;
}

void AspectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    //Set the mouse down type to the type which matches the position.
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch(event->button()){
    case Qt::LeftButton:
        switch(viewState){
        case NodeView::VS_NONE:
            //Goto VS_Selected
        case NodeView::VS_SELECTED:
            //Enter Selected Mode.
            getNodeView()->setStateSelected();
            handleSelection(true, controlPressed);
            break;
        }
        break;
    }
}

void AspectItem::childPositionChanged()
{
    childUpdated();
}

void AspectItem::childSizeChanged()
{
    childUpdated();
}

void AspectItem::childUpdated()
{
    QRectF childRect = childrenBoundingRect();

    //Maximize on the current size in the Model and the minimum child rectangle
    if(childRect.right() > getWidth()){
        setWidth(childRect.right());
    }

    if(childRect.bottom() > getHeight()){
        setHeight(childRect.bottom());
    }
}

qreal AspectItem::getItemMargin() const
{
    return MARGIN_RATIO * ITEM_SIZE * 4;
}

