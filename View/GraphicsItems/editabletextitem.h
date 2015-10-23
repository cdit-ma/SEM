#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0, int maximumLength = 64);
    QRectF boundingRect() const;
    void setEditMode(bool editMode = true);
    bool isInEditMode();

    void setFontSize(qreal fontSize);
    void setPlainText(const QString &text);
    void setTextWidth(qreal width);
    void setCenterAligned(bool center);
    QString getStringValue();
    QString getFullValue();
    void setEditable(bool edit);



signals:
    void textUpdated(QString data);
    void editableItem_EditModeRequested();

    void editableItem_hasFocus(bool hasFocus);

    // QGraphicsItem interface
protected:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent* event);
    void keyPressEvent(QKeyEvent *event);
private:
    QString getTruncatedText(const QString text );


    bool editable;
    bool centerJustified;
    QString currentFullValue;
    QString currentTruncValue;
    bool inEditingMode;
    qreal textWidth;
    int maxLength;
    QTextDocument* doc;
};

#endif // EDITABLETEXTITEM_H
