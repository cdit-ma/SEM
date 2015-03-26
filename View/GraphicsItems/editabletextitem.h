#ifndef EDITABLETEXTITEM_H
#define EDITABLETEXTITEM_H

#include <QGraphicsTextItem>

class EditableTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit EditableTextItem(QGraphicsItem *parent = 0, int truncationLength = 12);
    void setEditMode(bool editMode = true);
    void startEditMode(bool );
    void setText(QString newText);
signals:
    void textUpdated(QString data);



    // QGraphicsItem interface
protected:
    void focusOutEvent(QFocusEvent* event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);


private:
    QString getStringValue();

    QString previousFullValue;
    QString previousValue;
    QString fullValue;

    int truncationLength;
     // QGraphicsItem interface


     // QGraphicsItem interface
protected:
     void keyPressEvent(QKeyEvent *event);


};

#endif // EDITABLETEXTITEM_H
