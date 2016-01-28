#include "statusitem.h"
#include <QPainter>
#include <QDebug>
#include <QFontMetrics>
#include <QGraphicsSceneMouseEvent>

#define RADIUS 10
#define VERT_PADDING 4
#define HORIZ_PADDING 4
StatusItem::StatusItem(GraphMLItem *item):InputItem(item, "", false)
{
    setHeight(RADIUS * 2);
    setWidth(RADIUS * 2);
    setValue("0");
    editMode = false;
    wasTextEditing = false;

    setFlag(QGraphicsItem::ItemIsFocusable);
    setTextColor(Qt::white);
    connect(this, SIGNAL(InputItem_HasFocus(bool)), SLOT(textFocussed(bool)));
    setCursor(Qt::PointingHandCursor);
}

QRectF StatusItem::boundingRect() const
{
    QRectF rect = InputItem::boundingRect();
    rect.setWidth(rect.width() + rect.height()/2);
    return rect;
}

QRectF StatusItem::topRect() const
{
    QRectF rect;
    rect.setWidth(boundingRect().height()/2);
    rect.setHeight(rect.width());
    rect.moveTopRight(boundingRect().topRight());
    return rect;
}

QRectF StatusItem::botRect() const
{
    QRectF rect;
    rect.setWidth(boundingRect().height()/2);
    rect.setHeight(rect.width());
    rect.moveBottomRight(boundingRect().bottomRight());
    return rect;
}

void StatusItem::incrementDecrementNumber(bool inc)
{
    QString value = getValue();
    bool okay = false;
    qreal numValue = value.toDouble(&okay);
    if(okay){
        int delta = 1;
        if(!inc){
            delta *= -1;
        }
        numValue += delta;
        QString newValue = QString::number(numValue);
        setValue(newValue);
        emit InputItem_ValueChanged(newValue);
    }
}

void StatusItem::setNumberMode(bool num)
{
    numberMode = num;
}


void StatusItem::setFont(QFont font)
{
    font.setBold(true);
    InputItem::setFont(font);
    //Update font size.
    QFontMetrics fm(font);
    int newHeight = fm.height();

    setHeight(newHeight + (VERT_PADDING*2));
}

void StatusItem::setWidth(qreal width)
{
    InputItem::setWidth(width);
    updatePosition();
}

void StatusItem::setHeight(qreal height)
{
    InputItem::setHeight(height);
    updatePosition();
}

void StatusItem::updatePosition()
{
    QRectF rect = InputItem::boundingRect();
    qreal radius = getHeight()/2;
    QPointF newTopRight = topRight + QPointF(radius, -radius);
    rect.moveTopRight(newTopRight);
    updatePosSize(rect);

}

void StatusItem::setCircleCenter(QPointF pos)
{
    topRight = pos;
    updatePosition();
}

void StatusItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    QRectF rect = InputItem::boundingRect();
    if(editMode){
        rect = boundingRect();
    }
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0,150,150));
    painter->drawRoundedRect(rect,getHeight()/2,getHeight()/2);

    if(editMode){
        QRectF sideRect = botRect().united(topRect());
        painter->setClipping(true);
        painter->setClipRect(sideRect);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::NoPen);
        painter->drawRect(sideRect);

        //Setup Pen for side Rect.
        QPen pen;
        pen.setWidthF(1);
        pen.setColor(Qt::gray);
        pen.setJoinStyle(Qt::MiterJoin);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        //Offset rectangle to draw inside.
        QPointF penOffset(pen.widthF()/2, pen.widthF()/2);
        sideRect.setTopLeft(sideRect.topLeft() + penOffset);
        sideRect.setBottomRight(sideRect.bottomRight() - penOffset);

        painter->drawRect(sideRect);



        painter->drawLine(botRect().topLeft(), botRect().topRight());

        QImage image(":/Actions/Arrow_Down.png");
        QPixmap imageData = QPixmap::fromImage(image);

        painter->drawPixmap(botRect().toAlignedRect(), imageData);

        QImage image2(":/Actions/Arrow_Up.png");
        imageData = QPixmap::fromImage(image2);

        painter->drawPixmap(topRect().toAlignedRect(), imageData);



    }
}

void StatusItem::setValue(QString value)
{
    //Work out new size.
    QFontMetrics fm(getFont());
    int newWidth = fm.width(value) + (HORIZ_PADDING*2) ;

    newWidth = qMax((qreal)newWidth, getHeight());

    InputItem::setWidth(newWidth);
    InputItem::setValue(value);
    updatePosition();
}

void StatusItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->pos();

    if(editMode){
        if(topRect().contains(pos)){
            incrementDecrementNumber(true);
        }else if(botRect().contains(pos)){
            incrementDecrementNumber(false);
        }else if(InputItem::boundingRect().contains(pos)){
            InputItem::setEditMode(true);
        }
    }else{
        if (wasTextEditing && boundingRect().contains(pos)) {
            setEditMode(true);
            wasTextEditing = false;
            return;
        }
        if(InputItem::boundingRect().contains(pos)){
            emit statusItem_EditModeRequested();
        }else{
            event->setAccepted(false);
        }
    }
}

void StatusItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    InputItem::mouseReleaseEvent(event);
}


void StatusItem::setEditMode(bool edit)
{
    if(editMode != edit){
        editMode = edit;
        update();

        if(getTextItem()){
            if(editMode){
                getTextItem()->setCursor(Qt::IBeamCursor);
            }else{
                getTextItem()->unsetCursor();
            }
        }
    }

}

void StatusItem::textFocussed(bool focus)
{
    //Enforce edit mode to match the text editmode
    setEditMode(focus);
    if (focus) {
        wasTextEditing = true;
    }
}


qreal StatusItem::widthOffset()
{
    return getHeight()/2;
}

void StatusItem::focusOutEvent(QFocusEvent *)
{
    setEditMode(false);
}


