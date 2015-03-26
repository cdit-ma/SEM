#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QDebug>
#include <QKeyEvent>


EditableTextItem::EditableTextItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent)
{
    setFlag(ItemIsFocusable, false);
    setFlag(ItemIsSelectable, false);
    setTextInteractionFlags(Qt::NoTextInteraction);

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

        //Select All Text.
        QTextCursor c = textCursor();
        c.select(QTextCursor::Document);
        setTextCursor(c);
        previousValue = getStringValue();
    }else{
        setTextInteractionFlags(Qt::NoTextInteraction);

        //Update the the value of this Text Item.
        QString newValue = getStringValue();
        if(newValue != previousValue){
            textUpdated(newValue);
        }

        //Clear Selection.
        QTextCursor c = textCursor();
        c.clearSelection();
        setTextCursor(c);
        clearFocus();
    }
}




void EditableTextItem::focusOutEvent(QFocusEvent *event)
{
    setEditMode(false);
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
        setPlainText(previousValue);
        setEditMode(false);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);
}
