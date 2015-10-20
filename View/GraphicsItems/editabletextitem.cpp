#include "editabletextitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QTextCursor>
#include <QDebug>
#include <QKeyEvent>
#include <QFontMetrics>
#include <QTextDocument>
#include <QCursor>


EditableTextItem::EditableTextItem(QGraphicsItem *parent, int maximumLength) :
    QGraphicsTextItem(parent)
{

    currentFullValue = "";
    currentTruncValue = "";
    maxLength = maximumLength;
    centerJustified = false;


    inEditingMode = false;

    setFlag(ItemIsFocusable, false);
    setFlag(ItemIsSelectable, false);

    setTextInteractionFlags(Qt::NoTextInteraction);

   // this->setAcceptHoverEvents(false);


    doc = this->document();

    doc->setDocumentMargin(0);
    doc->setTextWidth(this->textWidth);
    QTextOption option = doc->defaultTextOption();
    option.setWrapMode(QTextOption::WrapAnywhere);
    doc->setDefaultTextOption(option);
    setDocument(doc);
    editable = true;

}

void EditableTextItem::setEditMode(bool editMode)
{
    prepareGeometryChange();

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
        if(centerJustified){
            QGraphicsTextItem::setHtml("<center>" + currentFullValue + "</center>");
        }else{
            QGraphicsTextItem::setPlainText(currentFullValue);
        }

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

        if(currentFullValue != currentValue){
            textUpdated(currentValue);
        }else{
            if(centerJustified){
                QGraphicsTextItem::setHtml("<center>" + currentTruncValue + "</center>");
            }else{
                 //Set the text as the current Truncated text.
                QGraphicsTextItem::setPlainText(currentTruncValue);
            }
        }

        //Clear Selection.
        QTextCursor c = textCursor();
        c.clearSelection();
        setTextCursor(c);
        clearFocus();
    }
}

void EditableTextItem::setHandleMouse(bool handleMouse)
{
    this->handleMouse = handleMouse;
}

void EditableTextItem::setFontSize(qreal fontSize)
{
    QFont textFont = this->font();
    textFont.setPointSizeF(fontSize);
    setFont(textFont);
}

void EditableTextItem::setPlainText(const QString &text)
{
    //qDebug() << "setPlainText";
    if(currentFullValue != text){
        currentFullValue = text;
        currentTruncValue = getTruncatedText(text);
    }
    if(centerJustified){
        QGraphicsTextItem::setHtml("<center>" + currentTruncValue + "</center>");
    }else{
        QGraphicsTextItem::setPlainText(currentTruncValue);
    }
}

void EditableTextItem::setTextWidth(qreal width)
{
    textWidth = width;
    doc->setTextWidth(width);
    QGraphicsTextItem::setTextWidth(width);

    if(width > 15){
        setVisible(true);
        QString newTruncValue = getTruncatedText(currentFullValue);

        if(newTruncValue != currentTruncValue){
            currentTruncValue = newTruncValue;
            QGraphicsTextItem::setPlainText(newTruncValue);
        }
    }else{
        setVisible(false);
    }
}

void EditableTextItem::setCenterAligned(bool center)
{
    if(centerJustified != center){
        QGraphicsTextItem::setHtml("<center>" + currentTruncValue + "</center>");
        centerJustified = center;
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
    Q_UNUSED(event);
    if(inEditingMode){
        //Only exit Edit mode if we are currently editing.
        setEditMode(false);

        // if we're done editing, tell the view that this item no longer has focus
        emit editableItem_hasFocus(false);
    }
}


QString EditableTextItem::getTruncatedText(const QString text)
{
    QFontMetrics fm(font());

    QString newText = text;

    qreal spaceForDots = fm.width("...");
    //Allow for Margins at high zoom levels
    qreal availableWidth = textWidth - spaceForDots;

    QString truncValue;

    QString lastTruncValue;
    int counter = 0;

    while(truncValue != newText){
        if((counter + 1) * 2 > newText.size()){
            //Add middle character.
            truncValue = truncValue.insert(counter,newText.at(counter));
            break;
        }else{
            truncValue = truncValue.insert(counter, newText.at(counter));
            truncValue = truncValue.insert(truncValue.size() - counter, newText.at((newText.size() -1) - counter));
        }
        if(fm.width(truncValue) >= availableWidth){
            truncValue = lastTruncValue;
            break;
        }
        lastTruncValue = truncValue;
        counter ++;
        if(counter * 2 == newText.size()){
            break;
        }
    }

    qCritical() << truncValue;

    if(truncValue != newText){
        truncValue.insert(counter, "...");
    }

    return truncValue;
    /*
    qCritical() << truncValue;

    while(newText.size() >= 3){
        qreal truncWidth = fm.width(newText);
        if(truncWidth < availableWidth){
            break;
        }
        newText.truncate(newText.size() - 3);
        newText.append("..");
    }

    //setVisible(newText.size() > 2);

    qCritical() << availableWidth << " OLD TEXT" << text << "New Text " << newText;

    return newText;*/
}

QRectF EditableTextItem::boundingRect() const
{
    return QGraphicsTextItem::boundingRect();
}




QString EditableTextItem::getStringValue()
{
    QString value = toPlainText().simplified();
    value.replace(QString('\n'), QString(""));
    value.replace(QString('\r'), QString(""));
    value.replace(QString(' '), QString(""));
    return value;
}

QString EditableTextItem::getFullValue()
{
    return currentFullValue;
}

void EditableTextItem::setEditable(bool edit)
{
    editable = edit;
}


void EditableTextItem::keyPressEvent(QKeyEvent *event)
{
    //Check for Enter pressing!
    int keyPressed = event->key();
     bool CONTROL = event->modifiers() & Qt::ControlModifier;

    if(keyPressed == Qt::Key_Enter || keyPressed == Qt::Key_Return || keyPressed == Qt::Key_Escape){
        focusOutEvent(0);
        return;
    }
    if(keyPressed == Qt::Key_V && CONTROL){
        return;
    }
    if(keyPressed == Qt::Key_Delete){
        event->setAccepted(true);
        //return;
    }
    QGraphicsTextItem::keyPressEvent(event);

}
