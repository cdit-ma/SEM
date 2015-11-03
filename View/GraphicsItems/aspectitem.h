#ifndef ASPECTITEM_H
#define ASPECTITEM_H
#include "nodeitem.h"
#include "entityitem.h"
#include <QFont>

class AspectItem : public NodeItem
{
    Q_OBJECT

public:
    AspectItem(Node *node, GraphMLItem *parent, VIEW_ASPECT aspect);
    QRectF boundingRect() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    qreal getWidth();
    qreal getHeight();
    void setPos(const QPointF pos);
    void setWidth(qreal width);
    void setHeight(qreal height);

private:
    RESIZE_TYPE mouseOverResize;
    QPointF previousScenePosition;
    QPointF getAspectPos();

    VIEW_ASPECT_POS aspectPos;
    QString aspectLabel;

    qreal minimumHeight;
    qreal minimumWidth;
    qreal width;
    qreal height;
    QColor backgroundColor;
    QColor textColor;
    QFont textFont;

public slots:
    void graphMLDataChanged(GraphMLData *);

    void sizeChanged();

public:
     QRectF gridRect() const;

    // QGraphicsItem interface
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);


    // NodeItem interface
public slots:
    void childPositionChanged();
    void childSizeChanged();
    void childUpdated();

public:
    qreal getItemMargin() const;

};

#endif // ASPECTITEM_H
