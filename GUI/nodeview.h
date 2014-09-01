#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include "nodeitem.h"
#include <QPointF>
class NodeView : public QGraphicsView
{
    Q_OBJECT
public:
    NodeView(QWidget *parent = 0);

signals:
    virtual void updateZoom(qreal zoom);
    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void shiftPressed(bool isDown);


    void copy();
    void cut();
    void paste();

    void unselect();
    void selectAll();

public slots:
    void addNodeItem(NodeItem* item);
    void removeNodeItem(NodeItem* item);
    void addEdgeItem(NodeEdge* edge);
    void centreItem(NodeItem* item);
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);


private:
    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    qreal totalScaleFactor;
};

#endif // NODEVIEW_H
