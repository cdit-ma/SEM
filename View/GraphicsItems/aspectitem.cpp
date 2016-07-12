#include "aspectitem.h"
#include "../nodeview.h"

#define MODEL_WIDTH 72
#define MODEL_HEIGHT 72

#define WATERMARK_LABEL_RATIO (1.0 / 6.0)

#define MARGIN_RATIO (1.0 / 18.0) //NORMAL

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

AspectItem::AspectItem(NodeAdapter *node, GraphMLItem *parent, VIEW_ASPECT aspect) : NodeItem(node, parent, GraphMLItem::ASPECT_ITEM)
{
    //Set View Aspect.
    //setViewAspect(VA_NONE);
    setViewAspect(aspect);

    backgroundColor = GET_ASPECT_COLOR(aspect);
    textColor = backgroundColor.darker(110);

    aspectPos = GET_ASPECT_POS(aspect);
    aspectLabel = GET_ASPECT_NAME(aspect).toUpper();


    width = GRID_SIZE * GRID_COUNT * ASPECT_COL_COUNT;
    height = GRID_SIZE * GRID_COUNT * ASPECT_ROW_COUNT;

    minimumWidth = width;
    minimumHeight = height;

    textFont.setBold(true);
    textFont.setPixelSize(minimumHeight * WATERMARK_LABEL_RATIO);

    mouseOverResize = NO_RESIZE;

    forcePos(getAspectPos());

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    connect(this, SIGNAL(GraphMLItem_SizeChanged()), this, SLOT(sizeChanged()));

    listenForData("width");
    listenForData("height");
    updateFromData();
    themeChanged();
}



QRectF AspectItem::boundingRect() const
{
    QRectF rect;
    qreal itemMargin = getItemMargin() * 2;
    rect.setWidth(width + itemMargin);
    rect.setHeight(height + itemMargin);
    return rect;
}

void AspectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);

    QPen borderPen = getCurrentPen();
    if(!isSelected()){
        borderPen = Qt::NoPen;
    }else{
        borderPen.setWidthF(2 * borderPen.widthF());
    }

    painter->setPen(borderPen);
    painter->setBrush(backgroundColor);

    //Double the width to save adjusting rect.
    painter->drawRect(boundingRect());

    painter->setPen(textColor);
    painter->setFont(textFont);
    painter->drawText(boundingRect(), Qt::AlignHCenter | Qt::AlignBottom, aspectLabel);

    //New Code
    if(drawGridLines()){
        painter->setPen(Qt::gray);
        QPen linePen = painter->pen();

        painter->setPen(Qt::NoPen);
        painter->setBrush(QColor(0,0,255,90));
        foreach(QRectF rect, getChildrenGridOutlines()){
            painter->drawRect(adjustRectForPen(rect, linePen));
        }

        linePen.setStyle(Qt::SolidLine);
        linePen.setWidthF(.5);
        painter->setPen(linePen);
        painter->drawLines(getGridLines());
    }
}


qreal AspectItem::getWidth() const
{
    return width;
}

qreal AspectItem::getHeight() const
{
    return height;
}

void AspectItem::setPos(const QPointF)
{
    forcePos(getAspectPos());
}

void AspectItem::setWidth(qreal w)
{
    qreal minWidth = childrenBoundingRect().right();
    minWidth = qMax(minWidth, minimumWidth);
    w = qMax(w, minWidth);

    if(w == width){
        return;
    }

    prepareGeometryChange();
    width = w;

    emit GraphMLItem_SizeChanged();
}

void AspectItem::setHeight(qreal h)
{
    qreal minHeight = childrenBoundingRect().bottom();
    minHeight = qMax(minHeight, minimumHeight);
    h = qMax(h, minHeight);

    if(h == height){
        return;
    }

    prepareGeometryChange();

    height = h;
    emit GraphMLItem_SizeChanged();
}

void AspectItem::forcePos(const QPointF pos)
{
    QGraphicsObject::setPos(pos);
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
        //Temporary for WorkerDefinitions
        return QPointF(-itemMargin - totalW, itemMargin);
        //return QPointF(0,0);
    }
}

void AspectItem::dataChanged(QString keyName, QVariant data)
{

    if((keyName == "width" || keyName == "height")){
        qreal valueD = data.toDouble();
        //If data is related to the size of the EntityItem
        if(keyName == "width"){
            setWidth(valueD);
        }else if(keyName == "height"){
            setHeight(valueD);
        }

        //Check if the Width or Height has changed.
        if(keyName == "width" && width != valueD){
            emit dataChanged(keyName, width);
        }
        if(keyName == "height" && height != valueD){
            emit dataChanged(keyName, height);
        }
    }
}

void AspectItem::themeChanged()
{
    backgroundColor = Theme::theme()->getAspectBackgroundColor(getViewAspect());
    textColor = backgroundColor.darker(110);
    update();
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
    VIEW_STATE viewState = getViewState();
    //Set the mouse down type to the type which matches the position.
    mouseOverResize = resizeEntered(event->pos());
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch(event->button()){
    case Qt::LeftButton:
        switch(viewState){
        case VS_NONE:
            //Goto VS_Selected
        case VS_SELECTED:
            //Enter Selected Mode.
            if(getNodeView()){
                getNodeView()->setStateSelected();
            }
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
    VIEW_STATE viewState = getViewState();


    //Only if left button is down.
    if(event->buttons() & Qt::LeftButton){
        QPointF deltaPos = (event->scenePos() - previousScenePosition);

        switch (viewState){
        case VS_SELECTED:
            if(mouseOverResize != NO_RESIZE){
                getNodeView()->setStateResizing();
            }
            break;
        case VS_RESIZING:

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
    VIEW_STATE viewState = getViewState();
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    //Only if left button is down.
    switch(event->button()){
    case Qt::LeftButton:{
        switch (viewState){
        case VS_RESIZING:
            emit NodeItem_ResizeFinished(getID());
            break;
        default:
            break;
        }
        break;
    }
    case Qt::RightButton:{
        switch (viewState){
        case VS_PAN:
            //IGNORE
        case VS_PANNING:
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
        case VS_NONE:
        case VS_SELECTED:
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
            // added this to re-center aspect when double-clicked
            GraphMLItem_SetCentered(this);
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

