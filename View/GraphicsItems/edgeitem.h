#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "nodeitem.h"
#include "../../Model/edge.h"

class EdgeItem: public GraphMLItem
{
    Q_OBJECT
public:
    enum LINE_SIDE{LEFT,RIGHT};
    enum LINE_DIRECTION{UP, DOWN};

    EdgeItem(Edge *edge, NodeItem* source, NodeItem* destination);
    ~EdgeItem();

    QRectF boundingRect() const;

    void adjustPos(QPointF delta);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();
    void setHidden(bool hidden);
public slots:
    void setSelected(bool selected);
    void setVisible(bool visible);
    void updateEdge();
    void graphMLDataChanged(GraphMLData * data);
    void graphMLDataChanged(QString name, QString key, QString value);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event );
private:
    bool isPointInCircle(QPointF itemPosition);
    void resetEdgeCenter(NodeItem* visibleSource, NodeItem* visibleDestination);
    void updateLabel();
    void updateLines();
    void setupBrushes();
    void setLineVisibility(bool visible);

    //Represents the End points of the LineItem
    NodeItem* source;
    NodeItem* destination;

    //Represents the current visible end poitns of the LineItem
    NodeItem* visibleSource;
    NodeItem* visibleDestination;

    //The Text Item which is the label.
    QGraphicsTextItem* label;

    //The Lines which make up the line
    QVector<QGraphicsLineItem*> lineSegments;

    //The Arrow Tail and Head shapes
    QPolygonF arrowTail;
    QPolygonF arrowHead;


    //State flags
    bool IS_VISIBLE;
    bool LINES_VISIBLE;
    bool IS_DELETING;
    bool IS_HIDDEN;
    bool IS_SELECTED;
    bool IS_MOVING;
    bool HAS_MOVED;
    bool CENTER_MOVED;



    //Used to keep track of the position of the EdgeItem
    QPointF previousScenePosition;
    QPointF centerDelta;

    //Brush's used by paint to change the visible style of the EdgeItem
    QBrush brush;
    QBrush tailBrush;
    QBrush selectedTailBrush;
    QBrush headBrush;
    QBrush selectedHeadBrush;
    QBrush selectedBrush;

    //Pens used by paint to change the visible style of the EdgeItem
    QPen pen;
    QPen selectedPen;

    //Stores the radius of the EdgeItem Circle
    qreal circleRadius;
};

#endif // EDGEITEM_H
