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
    setClickable(false);
    setEditable(false);
}

void EditableTextItem::setClickable(bool clickable)
{
    //setFlag(ItemIsFocusable, clickable);
    //setFlag(ItemIsSelectable, clickable);
}


void EditableTextItem::setEditable(bool editable)
{
    if(editable && textInteractionFlags() == Qt::NoTextInteraction){
        setTextInteractionFlags(Qt::TextEditorInteraction);
        setFocus(Qt::MouseFocusReason); // this gives the item keyboard focus
        setSelected(true);
        previousValue = getStringValue();


    }
    if(!editable && textInteractionFlags() == Qt::TextEditorInteraction){
        setTextInteractionFlags(Qt::NoTextInteraction);
        QTextCursor c = textCursor();
        c.clearSelection();
        this->setTextCursor(c);
        clearFocus();
    }
}

bool EditableTextItem::isEditable()
{
    return textInteractionFlags() == Qt::TextEditorInteraction;
}

void EditableTextItem::forceMousePress(QGraphicsSceneMouseEvent *event)
{
    this->setSelected(true);
    this->setFocus(Qt::MouseFocusReason);
    QGraphicsTextItem::mousePressEvent(event);
    QTextCursor c = textCursor();
    c.select(QTextCursor::Document);
    setTextCursor(c);
}


void EditableTextItem::focusOutEvent(QFocusEvent *event)
{
    QString newValue = getStringValue();
    if(newValue != previousValue){
        //qCritical() << newValue;
        emit textUpdated(newValue);
    }
    setEditable(false);
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
        focusOutEvent(0);
        return;
    }else if(event->key() == Qt::Key_Escape){
        setHtml("<center>" + previousValue + "</center>");
        focusOutEvent(0);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);
}
