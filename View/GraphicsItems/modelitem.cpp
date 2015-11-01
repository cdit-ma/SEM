#include "modelitem.h"
#include "../nodeview.h"
#include <QPainter>
#include <QDebug>

ModelItem::ModelItem(Node *node, bool IN_SUBVIEW):  GraphMLItem(node, GraphMLItem::MODEL_ITEM)
{
    qCritical() << "MODEL ITEM";
    setInSubView(IN_SUBVIEW);
    //Cache on Graphics card! May Artifact.
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);

    modelCircleColor = Qt::gray;
    topRightColor = QColor(254,184,126);
    topLeftColor = QColor(110,210,210);
    bottomRightColor = QColor(110,170,220);
    bottomLeftColor = QColor(255,160,160);

    width = MODEL_WIDTH;
    height = MODEL_HEIGHT;
    setupInputItems();

    setupGraphMLDataConnections();
    updateFromGraphMLData();
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

QRectF ModelItem::topInputRect() const
{
    QRectF rect = QRectF();
    rect.setWidth(width / 2);
    rect.setHeight(height / 8);
    rect.moveTop(-rect.height()/2);
    rect.moveRight(-width /4);

    //rect.moveTopLeft(QPointF(-rect.width()/2, -rect.height()));
    return rect;
}

QRectF ModelItem::bottomInputRect() const
{
    QRectF rect = QRectF();
    rect.setWidth(width / 2);
    rect.setHeight(height / 8);
    rect.moveTop(-rect.height()/2);

    rect.moveLeft(width /4);
    //rect.moveLeft(QPointF(-rect.width()/2, 0));
    return rect;
}

void ModelItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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

    //Setup the Pen
    QPen pen = this->pen;

    painter->setPen(pen);

    if(isSelected()){
        pen = this->selectedPen;
    }

    painter->setPen(pen);
    painter->setBrush(modelCircleColor);
    //Draw the center circle
    painter->drawEllipse(quadrant);

    painter->drawRect(bottomInputRect());
}

void ModelItem::graphMLDataChanged(GraphMLData *data)
{
    if(getGraphML() && data){
        QString keyName = data->getKeyName();
        QString value = data->getValue();

        qCritical() << "SETTING: " << keyName << " : " << value;
        if(keyName == "label"){

            if(topInputItem){
                topInputItem->setValue(value);
            }
        }else if(keyName == "middleware"){
            if(bottomInputItem){
                bottomInputItem->setValue(value);
            }
        }
    }
}

void ModelItem::dataEditModeRequested()
{
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if (inputItem){
        QString dataKey = "label";
        QString dataValue = inputItem->getValue();

        if(inputItem == bottomInputItem){
            dataKey = "middleware";
        }

        if (isDataEditable(dataKey)){
            if(inputItem == topInputItem){
                topInputItem->setEditMode(true);
            }else{
                QPointF botLeft = inputItem->sceneBoundingRect().bottomLeft();
                QPointF botRight = inputItem->sceneBoundingRect().bottomRight();
                QLineF botLine = QLineF(botLeft,botRight);
                getNodeView()->showDropDown(this, botLine, dataKey, dataValue);
            }
        }
    }
}

void ModelItem::dataChanged(QString dataValue)
{
    InputItem* inputItem = qobject_cast<InputItem*>(QObject::sender());
    if (inputItem){

        QString keyValue = "label";

        if(inputItem == bottomInputItem){
            keyValue = "middleware";
        }

        if(!getGraphML()->getData(keyValue)->isProtected()){
            emit GraphMLItem_TriggerAction("Set New Data Value");
            emit GraphMLItem_SetGraphMLData(getID(), keyValue, dataValue);
        }
    }
}

void ModelItem::setupGraphMLDataConnections()
{
    //Don't care about X,Y,W,H etc
    connectToGraphMLData("label");
    connectToGraphMLData("middleware");
}

void ModelItem::setupInputItems()
{
    qreal fontSize = LABEL_RATIO * height;

    topInputItem = new InputItem(this, "asd", false);
    bottomInputItem = new InputItem(this, "asd", true);

    //Setup Alignment
    bottomInputItem->setAlignment(Qt::AlignCenter);
    topInputItem->setAlignment(Qt::AlignCenter);

    QFont textFont;
    textFont.setPixelSize(fontSize);
    bottomInputItem->setFont(textFont);
    textFont.setBold(true);
    topInputItem->setFont(textFont);

    //Connect to signals.
    connect(topInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(dataEditModeRequested()));
    connect(bottomInputItem, SIGNAL(InputItem_EditModeRequested()), this, SLOT(dataEditModeRequested()));
    connect(topInputItem, SIGNAL(InputItem_ValueChanged(QString)), this, SLOT(dataChanged(QString)));
    connect(bottomInputItem, SIGNAL(InputItem_ValueChanged(QString)), this, SLOT(dataChanged(QString)));

    topInputItem->updatePosSize(topInputRect());
    bottomInputItem->updatePosSize(bottomInputRect());
}

void ModelItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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

