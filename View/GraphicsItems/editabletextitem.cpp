#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QCursor>


EditableTextItem::EditableTextItem(QGraphicsItem *parent) :
    QGraphicsTextItem(parent)
{
    currentFullValue = "";
    currentTruncValue = "";

    inEditingMode = false;
    setFlag(ItemIsFocusable, false);
    setFlag(ItemIsSelectable, false);
    setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptHoverEvents(true);
}

void EditableTextItem::setEditMode(bool editMode)
{
    if(!inEditingMode && editMode){
        //Set the Editing Mode
        inEditingMode = true;
        //Set the Flag
        setTextInteractionFlags(Qt::TextEditorInteraction);
        //Gain the focus of the TextItem
        setFocus(Qt::MouseFocusReason);

        //Construct a fake mouse press event.
        QGraphicsSceneMouseEvent* fakePress = new QGraphicsSceneMouseEvent(QEvent::GraphicsSceneMouseDoubleClick);
        fakePress->setScenePos(scenePos());
        fakePress->setPos(pos());
        fakePress->setButtons(Qt::LeftButton);
        fakePress->setButton(Qt::LeftButton);
        fakePress->setModifiers(Qt::NoModifier);
        //Send the MousePress event to the DoubleClick Handler.
        mouseDoubleClickEvent(fakePress);

        //Update the textItem to have the full value instead of the truncated value.
        QGraphicsTextItem::setPlainText(currentFullValue);

        //Select the entire TextItem field
        QTextCursor c = textCursor();
        c.select(QTextCursor::Document);
        setTextCursor(c);
    }else if(inEditingMode && !editMode){
        //Set the Editing Mode
        inEditingMode = false;
        //Set the Flag
        setTextInteractionFlags(Qt::NoTextInteraction);

        //Get the current Value of the TextItem (Should be Non-Truncated value)
        QString currentValue = toPlainText();

        //Check if the value is different to the previous fullValue.
        if(currentFullValue != currentValue){
            textUpdated(currentValue);
        }else{
            //Set the text as the current Truncated text.
            QGraphicsTextItem::setPlainText(currentTruncValue);
        }

        //Clear Selection.
        QTextCursor c = textCursor();
        c.clearSelection();
        setTextCursor(c);
        clearFocus();
    }
}

void EditableTextItem::setPlainText(const QString &text)
{
    if(currentFullValue != text){
        currentFullValue = text;
        currentTruncValue = getTruncatedText(text);
        QGraphicsTextItem::setPlainText(currentTruncValue);
    }

    QGraphicsTextItem::setPlainText(currentTruncValue);

}

void EditableTextItem::setTextWidth(qreal width)
{
    QGraphicsTextItem::setTextWidth(width);

    QString newTruncValue = getTruncatedText(currentFullValue);
    if(newTruncValue != currentTruncValue){
        currentTruncValue = newTruncValue;
        QGraphicsTextItem::setPlainText(newTruncValue);
    }
}


/**
 * @brief EditableTextItem::focusInEvent
 * This is used to tell the view that the focus is on this item.
 * @param event
 */
void EditableTextItem::focusInEvent(QFocusEvent*)
{
    // if we're in editing mode, tell the view that this item has focus
    if (inEditingMode) {
        emit editableItem_hasFocus(true);
    }
}


void EditableTextItem::focusOutEvent(QFocusEvent *event)
{
    if(inEditingMode){
        //Only exit Edit mode if we are currently editing.
        setEditMode(false);

        // if we're done editing, tell the view that this item no longer has focus
        emit editableItem_hasFocus(false);
    }
}

void EditableTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::IBeamCursor);
}

QString EditableTextItem::getTruncatedText(const QString text)
{
    QFontMetrics fm(font());

    QString newText = text;
    qreal newTextWidth = fm.width(newText);
    qreal ratio = newTextWidth / QGraphicsTextItem::textWidth();

    //Magic.
    if(ratio >= .98){
        //Calculate the number of characters we can fit.
        int stringLength = (newText.size() / ratio) - 3;
        newText.truncate(stringLength);
        newText += "...";
    }
    return newText;
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
    int keyPressed = event->key();

    if(keyPressed == Qt::Key_Enter || keyPressed == Qt::Key_Return || keyPressed == Qt::Key_Escape){
        focusOutEvent(0);
        return;
    }
    if(keyPressed == Qt::Key_Delete){
        event->setAccepted(true);
        //return;
    }
    QGraphicsTextItem::keyPressEvent(event);
}
