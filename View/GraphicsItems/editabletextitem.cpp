#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QCursor>


EditableTextItem::EditableTextItem(QGraphicsItem *parent, int truncationLength) :
    QGraphicsTextItem(parent)
{
    fullValue = "";
    previousValue = "";
    setFlag(ItemIsFocusable, false);
    setFlag(ItemIsSelectable, false);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptHoverEvents(true);
    this->truncationLength = truncationLength;
}

void EditableTextItem::setEditMode(bool editMode)
{
    if(editMode){
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus(Qt::MouseFocusReason);

        //Construct a fake mouse press event.
        QGraphicsSceneMouseEvent* fakePress = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
        fakePress->setScenePos(scenePos());
        fakePress->setPos(pos());
        fakePress->setButtons(Qt::LeftButton);
        fakePress->setButton(Qt::LeftButton);
        fakePress->setModifiers(Qt::NoModifier);
        mouseDoubleClickEvent(fakePress);

        QGraphicsTextItem::setPlainText(fullValue);
        //setPlainText(fullValue);
        //Select All Text.
        QTextCursor c = textCursor();
        c.select(QTextCursor::Document);
        setTextCursor(c);

        //previousValue = getStringValue();
    }else{
        setTextInteractionFlags(Qt::NoTextInteraction);

        QString currentValue = toPlainText();
        qCritical() << currentValue;
        if(fullValue != previousFullValue){
            qCritical() <<" UPDATING WITH: "<< fullValue;
            textUpdated(fullValue);
            setText(fullValue);
            //setPlainText(fullValue);
        }

        //Clear Selection.
        QTextCursor c = textCursor();
        c.clearSelection();
        setTextCursor(c);
        clearFocus();
    }
}

void EditableTextItem::setText(QString newText)
{
     if(newText.length() > truncationLength){
         newText.truncate(truncationLength);
         newText += "...";
     }

     QGraphicsTextItem::setPlainText(newText);



}





void EditableTextItem::focusOutEvent(QFocusEvent *event)
{
    setEditMode(false);
}

void EditableTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    this->setCursor(Qt::IBeamCursor);
}




QString EditableTextItem::getStringValue()
{
    QString value = toPlainText().simplified();
    value.replace(QString('\n'), QString(""));
    value.replace(QString('\r'), QString(""));
    value.replace(QString(' '), QString(""));
    return value;
}


void EditableTextItem::keyPressEvent(QKeyEvent *event)
{
    //Check for Enter pressing!
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        setEditMode(false);
        return;
    }else if(event->key() == Qt::Key_Escape){
        //setPlainText(previousValue);
        setEditMode(false);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);
}
