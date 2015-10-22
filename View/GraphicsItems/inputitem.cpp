#include "inputitem.h"
#include <QComboBox>
#include <QGraphicsScene>
#include <QPainter>
#include <QLineEdit>
#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#define TEXT_PADDING 2

InputItem::InputItem(GraphMLItem *parent, QString initialValue, bool isCombo):QGraphicsObject(parent)
{
    setupLayout(initialValue);
    isComboBox = isCombo;
    textItem->setPlainText(initialValue);

    if(!isComboBox){
        connect(textItem, SIGNAL(textUpdated(QString)), this, SLOT(textValueChanged(QString)));
    }
}

void InputItem::setCenterAligned(bool center)
{
    textItem->setCenterAligned(center);
}

void InputItem::setHandleMouse(bool on)
{
    handleMouse = on;
}

void InputItem::setFont(QFont font)
{
    textItem->setFont(font);
}

QString InputItem::getValue()
{
    return currentValue;
}


QRectF InputItem::boundingRect() const
{
    return QRectF(0 ,0,width, height);
}

QRectF InputItem::arrowRect() const
{
    QRectF iconRect = QRectF(0, 0, height, height);
    iconRect.moveTopRight(QPointF(width, 0));
    return iconRect;
}

bool InputItem::isInEditMode()
{
    return inEditMode;
}

void InputItem::setWidth(qreal w)
{
    if(width != w){
        prepareGeometryChange();
        width = w;
        updateTextSize();
    }
}

void InputItem::setHeight(qreal h)
{
    if(height != h){
        prepareGeometryChange();
        height = h;

        //Update position of the textBox.
        textItem->setPos(TEXT_PADDING, (height - textItem->boundingRect().height())/2);
        updateTextSize();
    }
}

qreal InputItem::getWidth()
{
    return width;
}

qreal InputItem::getHeight()
{
    return height;
}

void InputItem::textValueChanged(QString newValue)
{
    if(newValue != currentValue){
        emit InputItem_ValueChanged(newValue);
        currentValue = newValue;
    }
    if(isInEditMode()){
        setEditMode(false);
    }
}

void InputItem::comboBoxClosed()
{
    //Set edit mode to false.
    setEditMode(false);
}

void InputItem::setValue(QString newValue)
{
    if(newValue != currentValue){
        textItem->setPlainText(newValue);
        currentValue = newValue;
    }
}

void InputItem::setEditMode(bool editable)
{
    if(editable != inEditMode){
        inEditMode = editable;

        if(!isComboBox){
            //Set the textItem as editable.
            textItem->setEditMode(editable);
        }
    }
}

void InputItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(handleMouse && lastPressed.elapsed()  < 250){
        wasDoubleClicked = true;

    }else{
        wasDoubleClicked = false;
        QGraphicsObject::mousePressEvent(event);
    }
    lastPressed.restart();
}

void InputItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(wasDoubleClicked){
        emit InputItem_EditModeRequested();
        wasDoubleClicked = false;
        lastPressed.restart();
    }else{
        QGraphicsObject::mouseReleaseEvent(event);
    }
}

void InputItem::setupLayout(QString initialValue)
{
    width = 0;
    height = 0;
    inEditMode = false;

    handleMouse = false;
    lastPressed.start();

    textItem = new EditableTextItem(this);
    textItem->setParent(this);
    textItem->setPos(TEXT_PADDING, 0);
    textItem->setPlainText(initialValue);
}



void InputItem::updateTextSize()
{
    //Update Text width so it doesn't colide with the maximize button.
    qreal textWidth = width;
    if(isComboBox){
        //Offset by the width of the icon.
        textWidth -= height;
    }

    textItem->setTextWidth(textWidth);
}

void InputItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isComboBox){
        QImage image(":/Actions/Arrow_Down.png");
        QPixmap imageData = QPixmap::fromImage(image);

        //painter->setBrush(QColor(120,120,120,120));
        painter->setPen(Qt::NoPen);
        painter->setPen(Qt::gray);
        painter->drawRect(arrowRect());
        painter->drawPixmap(arrowRect().toAlignedRect(), imageData);
    }
}
