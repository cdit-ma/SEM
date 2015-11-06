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
    alignment = Qt::AlignLeft;

    currentFullValue = "";
    currentTruncValue = "";
    maxLength = maximumLength;

    inEditingMode = false;

    setFlag(ItemIsFocusable, false);
    setFlag(ItemIsSelectable, false);

    setTextInteractionFlags(Qt::NoTextInteraction);

    this->setAcceptHoverEvents(false);


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
        QGraphicsTextItem::setPlainText(currentFullValue);

        //Select the entire TextItem field
        setAlignment(alignment);
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
            emit textUpdated(currentValue);
        }else{
             //Set the text as the current Truncated text.
            QGraphicsTextItem::setPlainText(currentTruncValue);
        }

        //Clear Selection.
        QTextCursor c = textCursor();
        c.clearSelection();
        setTextCursor(c);
        clearFocus();
        setAlignment(alignment);
    }
}

bool EditableTextItem::isInEditMode()
{
    return inEditingMode;
}

bool EditableTextItem::isTruncated()
{
    return toPlainText() != currentFullValue;
}

void EditableTextItem::setFontSize(qreal fontSize)
{
    QFont textFont = this->font();
    textFont.setPointSizeF(fontSize);
    setFont(textFont);
}

void EditableTextItem::setPlainText(const QString &text)
{
    if(currentFullValue != text){
        currentFullValue = text;
        currentTruncValue = getTruncatedText(text);
    }

    QGraphicsTextItem::setPlainText(currentTruncValue);
    setAlignment(alignment);
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
            setAlignment(alignment);
        }
    }else{
        setVisible(false);
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

    QString fullText = text;


    //Have to truncate.
    //
    bool firstInsert = true;
    bool isEven = text.size() % 2;
    if(fm.width(fullText) > textWidth){
        int centerIndex = fullText.size() / 2;
        QString truncChars = "...";

        if(isEven){
            truncChars += ".";
        }

        fullText = fullText.insert(centerIndex, truncChars);

        while(fm.width(fullText) >= textWidth){
            if(fullText.size() <= 3){
                //Only got ... left
                break;
            }
            centerIndex = fullText.size() / 2;

            int lowerIndex = centerIndex - 2;
            int upperIndex = centerIndex + 2;

            //Remove upperIndex first, to not move lower index.
            fullText = fullText.remove(upperIndex, 1);
            fullText = fullText.remove(lowerIndex, 1);
        }
        return fullText;
    }else{
        return fullText;
    }

    return fullText;
}

QRectF EditableTextItem::boundingRect() const
{
    return QGraphicsTextItem::boundingRect();
}



void EditableTextItem::setAlignment(Qt::Alignment align)
{
    alignment = align;

    QTextBlockFormat format;
    format.setAlignment(align);
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    setTextCursor(cursor);
}




QString EditableTextItem::getStringValue()
{
    QString value = toPlainText().simplified();
    value.replace(QString('\n'), QString(""));
    value.replace(QString('\r'), QString(""));
    value.replace(QString(' '), QString(""));
    return value;
}

QString EditableTextItem::getFullValue() const
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
