#include "modelitem.h"
#include "../nodeview.h"
#include <QPainter>
#include <QDebug>

ModelItem::ModelItem(NodeAdapter *node, NodeView *view):  GraphMLItem(node, 0, GraphMLItem::MODEL_ITEM)
{

    modelCircleColor = Qt::gray;

    // setup quadrant colours based on the aspects' positions
    foreach (VIEW_ASPECT aspect, GET_VIEW_ASPECTS()) {
        VIEW_ASPECT_POS aspectPos = GET_ASPECT_POS(aspect);
        QColor aspectColor = GET_ASPECT_COLOR(aspect);
        switch (aspectPos) {
        case VAP_TOPLEFT:
            topLeftColor = aspectColor;
            break;
        case VAP_TOPRIGHT:
            topRightColor = aspectColor;
            break;
        case VAP_BOTTOMLEFT:
            bottomLeftColor = aspectColor;
            break;
        case VAP_BOTTOMRIGHT:
            bottomRightColor = aspectColor;
            break;
        default:
            break;
        }
    }

    width = MODEL_WIDTH;
    height = MODEL_HEIGHT;


    setupInputItems();


    rectColor = QColor(235,235,235);


    setFlag(QGraphicsItem::ItemIsFocusable);

    connect(Theme::theme(), SIGNAL(theme_Changed()), this, SLOT(themeChanged()));
    setupDataConnections();
    updateFromData();

    setNodeView(view);
    themeChanged();

}

ModelItem::~ModelItem()
{
    delete middlewareItem;
}

void ModelItem::adjustPos(QPointF delta)
{
    QPointF currentPos = pos();
    currentPos += delta;
    //hasSelectionMoved = true;
    setPos(currentPos);
}

void ModelItem::setPos(const QPointF &pos)
{
    QGraphicsObject::setPos(pos);
    emit GraphMLItem_PositionChanged();
}

void ModelItem::setPos(qreal x, qreal y)
{
    setPos(QPointF(x,y));
}

QRectF ModelItem::boundingRect() const
{
    qreal left  = - (width / 2.0);
    qreal right = + (width / 2.0);
    qreal top   = - (height / 2.0);
    qreal bot   = + (height / 2.0);
    return QRectF(QPointF(left, top), QPointF(right, bot));
}

QRectF ModelItem::centerInputRect() const
{
    QRectF rect = QRectF();
    rect.setWidth(width);
    rect.setHeight(height / 5);
    rect.moveCenter(QPointF(0,0));
    return rect;
}

QList<VIEW_ASPECT> ModelItem::getVisibleAspects()
{
    QList<VIEW_ASPECT> list;
    foreach(GraphMLItem* child, getChildren()){
        if(child->isVisible() && child->isAspectItem()){
            list << ((AspectItem*)child)->getViewAspect();
        }
    }
    return list;
}

void ModelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if(inSubView()){
        //Don't paint in subview
        return;
    }
    //Set Clip Rectangle
    painter->setClipRect(option->exposedRect);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);

    painter->setPen(Qt::NoPen);

    if(true){
        //CLIP ON CIRCLE
        QPainterPath clipPath;
        clipPath.addEllipse(boundingRect());
        painter->setClipping(true);
        painter->setClipPath(clipPath);
    }

    QPointF origin(0,0);

    QRectF quadrant(0, 0, width / 2, height / 2);

    //Paint Bottom Right
    painter->setBrush(bottomRightColor);
    painter->drawRect(quadrant);

    //Paint Top Right
    quadrant.moveBottomLeft(origin);
    painter->setBrush(topRightColor);
    painter->drawRect(quadrant);

    //Paint Top Left
    quadrant.moveBottomRight(origin);
    painter->setBrush(topLeftColor);
    painter->drawRect(quadrant);

    //Paint Bottom Left
    quadrant.moveTopRight(origin);
    painter->setBrush(bottomLeftColor);
    painter->drawRect(quadrant);

    quadrant.moveCenter(origin);


    if(getRenderState() >= RS_REDUCED){
        //Draw the intersection lines.
        painter->setPen(Qt::gray);
        painter->drawLine(QPointF(0, -height / 2), QPointF(0, height / 2));
        painter->drawLine(QPointF(-width / 2, 0), QPointF(width / 2, 0));
    }

    if(true){
        //CLIP ON CIRCLE
        QPainterPath painterPath;
        painterPath.setFillRule(Qt::WindingFill);
        painterPath.addEllipse(quadrant);
        painterPath.addRect(centerInputRect());

        painter->setPen(getCurrentPen());
        painter->setBrush(modelCircleColor);
        painter->drawPath(painterPath.simplified());

        painter->setPen(Qt::NoPen);
        painter->setBrush(rectColor);

        QRectF rect = centerInputRect();
        qreal offset = getCurrentPen().widthF()/2;
        rect.adjust(offset,offset,-offset,-offset);
        painter->drawRect(rect);
    }
}

void ModelItem::setInSubView(bool inSubview)
{
    if(middlewareItem){
        middlewareItem->setVisible(!inSubview);
    }
    GraphMLItem::setInSubView(inSubview);
}

void ModelItem::dataChanged(QString keyName, QVariant data)
{
    if(keyName == "middleware"){
        if(middlewareItem){
            middlewareItem->setValue(data.toString());
        }
    }
}

void ModelItem::dataEditModeRequested()
{
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if (inputItem){
        QString dataKey = "middleware";
        QString dataValue = inputItem->getValue();

        if (isDataEditable(dataKey)){
            QPointF botLeft = inputItem->sceneBoundingRect().bottomLeft();
            QPointF botRight = inputItem->sceneBoundingRect().bottomRight();
            QLineF botLine = QLineF(botLeft,botRight);
            getNodeView()->showDropDown(this, botLine, dataKey, dataValue);
        }
    }
}

void ModelItem::dataChanged(QString dataValue)
{
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if (inputItem){

        QString keyValue = "middleware";

        if(!getEntityAdapter()->isDataProtected(keyValue)){
            emit GraphMLItem_TriggerAction("Set New Data Value");
            emit GraphMLItem_SetData(getID(), keyValue, dataValue);
        }
    }
}


/**
 * @brief ModelItem::themeChanged
 * @param theme
 */
void ModelItem::themeChanged()
{
    modelCircleColor = Theme::theme()->getBackgroundColor();

    // setup quadrant colours based on the aspects' positions
    foreach (VIEW_ASPECT aspect, GET_VIEW_ASPECTS()) {
        VIEW_ASPECT_POS aspectPos = GET_ASPECT_POS(aspect);

        QColor aspectColor = Theme::theme()->getAspectBackgroundColor(aspect);
        switch (aspectPos) {
        case VAP_TOPLEFT:
            topLeftColor = aspectColor;
            break;
        case VAP_TOPRIGHT:
            topRightColor = aspectColor;
            break;
        case VAP_BOTTOMLEFT:
            bottomLeftColor = aspectColor;
            break;
        case VAP_BOTTOMRIGHT:
            bottomRightColor = aspectColor;
            break;
        default:
            break;
        }
    }
    update();
}


void ModelItem::setupDataConnections()
{
    //Don't care about X,Y,W,H etc
    listenForData("label");
    listenForData("middleware");
}

void ModelItem::setupInputItems()
{
    middlewareItem = 0;
    return;
    qreal fontSize = LABEL_RATIO * height;

    middlewareItem = new InputItem(this, "MIDDLEWARE", true);
    middlewareItem->setTextColor(Qt::black);
    //Setup Alignment
    middlewareItem->setAlignment(Qt::AlignCenter);

    QFont textFont;
    textFont.setPixelSize(fontSize);
    textFont.setBold(true);
    middlewareItem->setFont(textFont);

    //Connect to signals.
    connect(middlewareItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(dataEditModeRequested()));
    connect(middlewareItem, SIGNAL(InputItem_ValueChanged(QString)), this, SLOT(dataChanged(QString)));

    middlewareItem->updatePosSize(centerInputRect());
}

void ModelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    VIEW_STATE viewState = getViewState();
    //Set the mouse down type to the type which matches the position.
    bool controlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch(event->button()){
    case Qt::LeftButton:
        switch(viewState){
        case VS_NONE:
            //Goto VS_Selected
        case VS_SELECTED:
            //Enter Selected Mode.
            getNodeView()->setStateSelected();
            handleSelection(true, controlPressed);
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }
}

