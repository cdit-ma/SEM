#include "inputitem.h"
#include "graphmlitem.h"
#include <QComboBox>
#include <QGraphicsScene>
#include <QPainter>
#include <QLineEdit>
#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#define TEXT_PADDING 4

InputItem::InputItem(GraphMLItem *parent, QString initialValue, bool isCombo):QGraphicsObject(parent)
{
    setupLayout(initialValue);
    isComboBox = isCombo;
    handleMouse = true;
    textItem->setPlainText(initialValue);

    setAcceptHoverEvents(false);
    backgroundBrush = Qt::NoBrush;

    if(!isComboBox){
        connect(textItem, SIGNAL(editableItem_hasFocus(bool)), this, SIGNAL(InputItem_HasFocus(bool)));
        connect(textItem, SIGNAL(textUpdated(QString)), this, SIGNAL(InputItem_ValueChanged(QString)));
    }
}

void InputItem::setAlignment(Qt::Alignment align)
{
    if(textItem){
        textItem->setAlignment(align);
    }

}

void InputItem::setBrush(QBrush brush)
{
    if(backgroundBrush != brush){
        backgroundBrush = brush;
        update();
    }
}

void InputItem::setToolTipString(QString tooltip)
{
    if(_tooltip == "" && tooltip == ""){
        setToolTip("");
    }else{
        _tooltip = tooltip;
        setToolTip(textItem->getFullValue() + "\n" + _tooltip);
    }
}

bool InputItem::isTruncated()
{
    return textItem->isTruncated();
}

void InputItem::setTextColor(QColor color)
{
    textItem->setDefaultTextColor(color);
}



void InputItem::setDropDown(bool isCombo)
{
    if(isComboBox != isCombo){
        isComboBox = isCombo;
        updateTextSize();
        update();
    }
}

void InputItem::setHandleMouse(bool on)
{
    handleMouse = on;
}

void InputItem::setFont(QFont font)
{
    textItem->setFont(font);
    updateTextSize();
}

void InputItem::updatePosSize(QRectF size)
{
    setPos(size.topLeft());
    setWidth(size.width());
    setHeight(size.height());
}

QString InputItem::getValue()
{
    return textItem->getFullValue();
}


QRectF InputItem::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

QRectF InputItem::arrowRect() const
{
    QRectF iconRect = QRectF(0, 0, height, height);
    iconRect.moveTopRight(QPointF(width, 0));
    return iconRect;
}

bool InputItem::isInEditMode()
{
    return textItem->isInEditMode();
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

EditableTextItem *InputItem::getTextItem()
{
    return textItem;
}

QFont InputItem::getFont()
{
    return textItem->font();
}

void InputItem::setValue(QString newValue)
{
    textItem->setPlainText(newValue);
    //Update Tooltip.
    setToolTipString(_tooltip);
}

void InputItem::setEditMode(bool editable)
{
    if(!isComboBox){
        //Set the textItem as editable.
        textItem->setEditMode(editable);
        textItem->setFocus(Qt::MouseFocusReason);
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

    handleMouse = false;
    lastPressed.start();

    textItem = new EditableTextItem(this);
    textItem->setParent(this);
    textItem->setPlainText(initialValue);
}



void InputItem::updateTextSize()
{
    //Update Text width so it doesn't colide with the maximize button.
    qreal textWidth = width - (2 * TEXT_PADDING);
    if(isComboBox){
        //Offset by the width of the icon.
        textWidth -= height;
    }

    textItem->setTextWidth(textWidth);
    qreal moveHeight = (height - textItem->boundingRect().height())/2;
    textItem->setPos(TEXT_PADDING, moveHeight);
}

void InputItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    qreal lod = option->levelOfDetailFromTransform(painter->worldTransform());
    qreal actualSize = lod * height;
    if(actualSize > MINIMUM_TEXT_SIZE){
        painter->setClipping(true);
        painter->setClipRect(boundingRect());


        painter->setPen(Qt::NoPen);
        painter->setBrush(backgroundBrush);
        painter->drawRect(boundingRect());

        if(isComboBox){
            QImage image(":/Actions/Arrow_Down.png");
            QPixmap imageData = QPixmap::fromImage(image);

            painter->drawPixmap(arrowRect().toAlignedRect(), imageData);

            QPen linePen;
            linePen.setColor(Qt::black);
            linePen.setWidthF(.5);
            painter->setPen(linePen);

            QLineF line;
            line.setP1(arrowRect().topLeft());
            line.setP2(arrowRect().bottomLeft());
            painter->drawLine(line);
        }
    }
}

