#ifndef STATUSITEM_H
#define STATUSITEM_H
#include "inputitem.h"


class GraphMLItem;
class StatusItem : public InputItem
{
    Q_OBJECT
    // QGraphicsItem interface
public:
    StatusItem(GraphMLItem* item);
    QRectF boundingRect() const;
    QRectF topRect() const;
    QRectF botRect() const;

    QPointF getCircleCenter();
    void setBackgroundColor(QColor color);



    void incrementDecrementNumber(bool inc);
    void setNumberMode(bool num);
    void setFont(QFont font);
    void setWidth(qreal width);
    void setHeight(qreal height);
    void updatePosition();
    void setCircleCenter(QPointF pos);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
public slots:
    void setValue(QString value="");
    void setEditMode(bool editMode);
    void textFocussed(bool focus);
signals:
    void statusItem_EditModeRequested();
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void focusOutEvent(QFocusEvent *event);


private:
    void _setEditMode(bool editMode);
    qreal widthOffset();
    QPointF topRight;
    QColor backgroundColor;

    bool editMode;

    bool numberMode;

    bool wasTextEditing;

};

#endif // STATUSITEM_H
