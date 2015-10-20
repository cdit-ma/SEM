#include "inputitem.h"
#include <QComboBox>
#include <QGraphicsScene>
#include <QPainter>
#include <QLineEdit>
#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include "graphicscombobox.h"
#include <QDebug>
#define TEXT_PADDING 2
InputItem::InputItem(GraphMLItem* parent, QStringList defaultOptions, QString initialValue):QGraphicsObject(parent)
{
    type = IE_COMBOBOX;

    setupLayout();


    comboBox = new GraphicsComboBox();
    comboBox->addItems(defaultOptions);
    comboBox->setStyleSheet(
                                "QComboBox{border:none;color:#00000000;background-color:#00000000;padding:0px;}"
                                "QComboBox::drop-down{"
                                    "width: 0px;"
                                    "height: 0px;"
                                "}"


                                "QComboBox QAbstractItemView {"
                                "color:black;"
                                "selection-background-color: gray;"
                                "selection-color: white;"
                                "border:1px solid gray;"
                                "}"


                             );

    setWidth(100);
    setHeight(20);


    comboBoxProxy = new QGraphicsProxyWidget(this);
    comboBoxProxy->setWidget(comboBox);
    comboBoxProxy->setPos(TEXT_PADDING, 0);
    comboBoxProxy->setVisible(false);
    edititem->setPlainText(initialValue);




    //connect(comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(textValueChanged(QString)));
    connect(comboBox, SIGNAL(combobox_Closed()), this, SLOT(setEditMode()));
    connect(comboBox, SIGNAL(activated(QString)), this, SLOT(textValueChanged(QString)));

}

InputItem::InputItem(GraphMLItem *parent,bool, QString initialValue):QGraphicsObject(parent)
{
    type = IE_TEXTBOX;
    setupLayout();

    edititem->setPlainText(initialValue);
    connect(edititem, SIGNAL(textUpdated(QString)), this, SLOT(textValueChanged(QString)));
}

void InputItem::setCenterAligned(bool center)
{
    edititem->setCenterAligned(center);
}

void InputItem::setHandleMouse(bool on)
{
    handleMouse = on;
}

void InputItem::setFont(QFont font)
{
    if(type == IE_COMBOBOX){
        comboBox->setFont(font);
    }
    edititem->setFont(font);
}

QString InputItem::getValue()
{
    return currentValue;
}


QRectF InputItem::boundingRect() const
{
    return QRectF(0,0,width,height);
}

QRectF InputItem::arrowRect() const
{
    QRectF iconRect = QRectF(0,0, height,height);
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
        inputWidth = width - (2 * TEXT_PADDING);
        updateTextSize();
    }
}

void InputItem::setHeight(qreal h)
{
    if(height != h){
        prepareGeometryChange();
        height = h;
        inputHeight = height;
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

void InputItem::setValue(QString value)
{

    edititem->setPlainText(value);
}

void InputItem::setEditMode(bool editable)
{
    inEditMode = editable;

    if(type == IE_COMBOBOX){
        comboBoxProxy->setVisible(editable);

        if(editable){
            comboBox->showPopup();
        }

    }else if(type == IE_TEXTBOX){
        edititem->setEditMode(editable);
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
    }else{
        QGraphicsObject::mouseReleaseEvent(event);
    }
}

void InputItem::setupLayout()
{
    width = 0;
    height = 0;
    comboBox = 0;
    comboBoxProxy = 0;
    handleMouse = false;
    lastPressed.start();

    edititem = new EditableTextItem(this);
    edititem->setParent(this);
    edititem->setPos(TEXT_PADDING, 0);

    setAcceptHoverEvents(false);
}



void InputItem::updateTextSize()
{
    if(type == IE_COMBOBOX){
        comboBox->setFixedWidth(inputWidth);
        comboBox->setFixedHeight(inputHeight);
    }
    edititem->setTextWidth(inputWidth);
}

void InputItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //painter->setBrush(Qt::red);
    painter->drawRect(this->boundingRect());

    if(comboBox){
        painter->drawRect(comboBox->rect());
    }
     //painter->drawRect(this->boundingRect());
    //QRectF imageRect = arrowRect();


    //QImage image(":/Actions/Arrow_Down.png");
    //QPixmap imageData = QPixmap::fromImage(image);

    //painter->drawPixmap(imageRect.left(), imageRect.top(), imageRect.width(), imageRect.height(), imageData);
    //painter->drawRect(imageRect);
}
