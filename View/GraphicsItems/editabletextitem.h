#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0);
    void setEditMode(bool editMode = true);
    void startEditMode(bool );

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
