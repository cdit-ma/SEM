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

AspectItem::AspectItem(Node *node, GraphMLItem *parent, VIEW_ASPECT aspect) : NodeItem(node,parent, GraphMLItem::ASPECT_ITEM)
{
    //Set View Aspect.
    setViewAspect(aspect);

    backgroundColor = getAspectColor(aspect);
    textColor = backgroundColor.darker(110);

    aspectPos = getAspectPosition(aspect);
    aspectLabel = getAspectName(aspect).toUpper();


    width = GRID_SIZE * GRID_COUNT * ASPECT_COL_COUNT;
    height = GRID_SIZE * GRID_COUNT * ASPECT_ROW_COUNT;
    minimumWidth = width;
    minimumHeight = height;

    textFont.setBold(true);
    textFont.setPointSize(minimumHeight * (LABEL_RATIO /2));

    mouseOverResize = NO_RESIZE;

    setPos(getAspectPos());

    connect(this, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(sizeChanged()));


    connectToGraphMLData("width");
    connectToGraphMLData("height");

    //Set Values Direc
    updatePositionInModel(true);
    updateSizeInModel(true);
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

    QPen borderPen = getCurrentPen();
    if (!isSelected()) {
        borderPen = Qt::NoPen;
    }

    painter->setPen(borderPen);
    painter->setBrush(backgroundColor);
    painter->drawRect(adjustRectForPen(boundingRect(), borderPen));

    painter->setPen(textColor);
    painter->setFont(textFont);
    painter->drawText(boundingRect(), Qt::AlignHCenter | Qt::AlignBottom, aspectLabel);

    if (drawGridLines()) {
        painter->setPen(Qt::gray);
        QPen linePen = painter->pen();

        linePen.setStyle(Qt::DotLine);
        painter->setPen(linePen);
        painter->drawLines(getGridLines());

        linePen.setColor(Qt::darkGray);
        linePen.setWidthF(2);

        painter->setBrush(Qt::NoBrush);
        painter->setPen(linePen);
        //painter->drawRects(getChildrenGridOutlines());

        foreach (QRectF rect, getChildrenGridOutlines()) {
            painter->drawRect(adjustRectForPen(rect, linePen));
        }
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
    w = qMax(w, minimumWidth);

    if(w == width){
        return;
    }

    prepareGeometryChange();
    width = w;

    emit GraphMLItem_SizeChanged();
}

void AspectItem::setHeight(qreal h)
{
    h = qMax(h, minimumHeight);

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
    case VAP_TOPLEFT:
        return QPointF(-itemMargin - totalW, -itemMargin - totalH);
    case VAP_TOPRIGHT:
        return QPointF(itemMargin, -itemMargin - totalH);
    case VAP_BOTTOMRIGHT:
        return QPointF(itemMargin, itemMargin);
    case VAP_BOTTOMLEFT:
        return QPointF(-itemMargin - totalW, itemMargin);
    default:
        return QPointF(0,0);
    }
}

void AspectItem::graphMLDataChanged(GraphMLData *data)
{
    if(data && data->getParent() == getGraphML() && !getGraphML()->isDeleting()){
        QString keyName = data->getKeyName();
        QString value = data->getValue();
        bool isDouble = false;
        double valueD = value.toDouble(&isDouble);


        if((keyName == "width" || keyName == "height") && isDouble){
            //If data is related to the size of the EntityItem
            if(keyName == "width"){
                setWidth(valueD);
            }else if(keyName == "height"){
                setHeight(valueD);
            }

            //Check if the Width or Height has changed.
            if(keyName == "width" && width != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "width", QString::number(width));
            }
            if(keyName == "height" && height != valueD){
                emit GraphMLItem_SetGraphMLData(getID(), "height", QString::number(height));
            }
        }
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
    mouseOverResize = resizeEntered(event->pos());
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
            //Store the previous position.
            previousScenePosition = event->scenePos();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void AspectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();


    //Only if left button is down.
    if(event->buttons() & Qt::LeftButton){
        QPointF deltaPos = (event->scenePos() - previousScenePosition);

        switch (viewState){
        case NodeView::VS_SELECTED:
            if(mouseOverResize != NO_RESIZE){
                getNodeView()->setStateResizing();
            }
            break;
        case NodeView::VS_RESIZING:

            //If we are resizing horizontally, remove the vertical change.
            if(mouseOverResize == HORIZONTAL_RESIZE){
                deltaPos.setY(0);
                //If we are resizing vertically, remove the horizontal change.
            }else if(mouseOverResize == VERTICAL_RESIZE){
                deltaPos.setX(0);
            }
            emit NodeItem_ResizeSelection(getID(), QSizeF(deltaPos.x(), deltaPos.y()));
            previousScenePosition = event->scenePos();
            break;
        default:
            break;
        }
    }
}

void AspectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    NodeView::VIEW_STATE viewState = getNodeView()->getViewState();
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    //Only if left button is down.
    switch(event->button()){
    case Qt::LeftButton:{
        switch (viewState){
        case NodeView::VS_RESIZING:
            emit NodeItem_ResizeFinished(getID());
            break;
        default:
            break;
        }
        break;
    }
    case Qt::RightButton:{
        switch (viewState){
        case NodeView::VS_PAN:
            //IGNORE
        case NodeView::VS_PANNING:
            //IGNORE
            break;
        default:
            //If we haven't Panned, we need to make sure this EntityItem is selected before the toolbar opens.1
            handleSelection(true, controlPressed);
            break;
        }
        break;
    }
    case Qt::MiddleButton:{
        switch (viewState){
        case NodeView::VS_NONE:
        case NodeView::VS_SELECTED:
            if(controlPressed){
                if(inMainView()){
                    //emit GraphMLItem_TriggerAction("Sorting Node");
                    //sort();
                }
            }else{
                emit GraphMLItem_SetCentered(this);
            }

            //IGNORE
            break;
        default:
            break;
        }
        break;
    }
    default:
        break;
    }
    mouseOverResize = NO_RESIZE;
}

void AspectItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(inSubView()){
        return;
    }

    RESIZE_TYPE type = resizeEntered(event->pos());

    switch(event->button()){
    case Qt::LeftButton:{
        switch(type){
        case HORIZONTAL_RESIZE:
            //Continue
        case VERTICAL_RESIZE:
            //Continue
        case RESIZE:
            GraphMLItem_TriggerAction("Optimizes Size of AspectItem");
            resizeToOptimumSize(type);
            updateSizeInModel();
            break;
        default:
            break;
        }
    }
    default:
        break;
    }
}

void AspectItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    QString tooltip;
    QCursor cursor;
    RESIZE_TYPE resizeType =  resizeEntered(event->pos());


    switch(resizeType){
    case RESIZE:
        if(isSelected()){
            tooltip = "Click and drag to change size.\nDouble click to auto set size.";
            cursor = Qt::SizeFDiagCursor;
        }
        break;
    case HORIZONTAL_RESIZE:
        if(isSelected()){
            tooltip = "Click and drag to change width.\nDouble click to auto set width.";
            cursor = Qt::SizeHorCursor;
        }
        break;
    case VERTICAL_RESIZE:
        if(isSelected()){
            tooltip = "Click and drag to change height.\nDouble click to auto set height.";
            cursor = Qt::SizeVerCursor;
        }
        break;
    default:
        break;
    }


    setToolTip(tooltip);

    if(cursor.shape() != Qt::ArrowCursor){
        setCursor(cursor);
    }else{
        unsetCursor();
    }

    GraphMLItem::hoverMoveEvent(event);
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

