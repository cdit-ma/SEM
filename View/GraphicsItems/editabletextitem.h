#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0);
    void setEditMode(bool editMode = true);
    void setPlainText(const QString &text);
    void setTextWidth(qreal width);

signals:
    void textUpdated(QString data);

    void editableItem_hasFocus(bool hasFocus);

    // QGraphicsItem interface
protected:
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);


private:
    QString getTruncatedText(const QString text );
    QString getStringValue();

    QString currentFullValue;
    QString currentTruncValue;
    bool inEditingMode;

protected:
     void keyPressEvent(QKeyEvent *event);


};

#endif // EDITABLETEXTITEM_H
