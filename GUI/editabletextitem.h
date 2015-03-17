#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0);
    void setClickable(bool clickable);
    void setEditable(bool editable);
    bool isEditable();
    void forceMousePress(QGraphicsSceneMouseEvent* event);

signals:
    void textUpdated(QString data);



    // QGraphicsItem interface
protected:

    void focusOutEvent(QFocusEvent* event);


private:
    QString getStringValue();
     QString previousValue;

     // QGraphicsItem interface


     // QGraphicsItem interface
protected:
     void keyPressEvent(QKeyEvent *event);


};

#endif // EDITABLETEXTITEM_H
