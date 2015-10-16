#ifndef EDGEITEM_H
#define EDGEITEM_H

#include "nodeitem.h"
#include "../../Model/edge.h"
#include "editabletextitem.h"


class EdgeItemArrow;
class EdgeItem: public GraphMLItem
{
    Q_OBJECT
public:
    enum LINE_SIDE{LEFT,RIGHT};
    enum LINE_DIRECTION{UP, DOWN};

    EdgeItem(Edge *edge, NodeItem *parent, NodeItem* source, NodeItem* destination);
    ~EdgeItem();

    QRectF boundingRect() const;
    QRectF circleRect() const;
    void setCenterPos(QPointF pos);
    QPointF getCenterPos();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    NodeItem* getSource();
    NodeItem* getDestination();


    void setHighlighted(bool highlighted);
    void setSelected(bool selected);

signals:
    //Unsure
    void edgeItem_eventFromItem();

private slots:
    void labelUpdated(QString newLabel);
    void setVisible(bool visible);
    void updateVisibleParents();
    void updateLine();
    void graphMLDataChanged(GraphMLData * data);
    void zoomChanged(qreal zoom);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

private:
    void updateBrushes();
    bool isPointInCircle(QPointF itemPosition);




    void updateLabel(QString label);
    void updateLines();
    void setupBrushes();


    NodeItem* parent;

    //Represents the End points of the LineItem
    NodeItem* source;
    NodeItem* destination;

    //Represents the current visible end poitns of the LineItem
    NodeItem* visibleSource;
    NodeItem* visibleDestination;

    //The Text Item which is the label.
    EditableTextItem* textItem;

    //The Lines which make up the line
    QVector<QGraphicsLineItem*> lineSegments;

    EdgeItemArrow* arrowHeadItem;
    EdgeItemArrow* arrowTailItem;


    //Represents the Line in coordinates relative to the parent.
    QLineF currentParentLine;

    //Brush's used by paint to change the visible style of the EdgeItem
    QBrush brush;
    QBrush selectedBrush;

    QBrush currentBrush;
    QPen currentPen;

    QBrush tailBrush;
    QBrush selectedTailBrush;

    QBrush headBrush;
    QBrush selectedHeadBrush;

    //Pens used by paint to change the visible style of the EdgeItem
    QPen pen;
    QPen selectedPen;

};

#endif // EDGEITEM_H
