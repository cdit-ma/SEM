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

    void constructNodeItem(QString type);


    void copy();
    void cut();
    void paste();

    void undo();
    void redo();

    void unselect();
    void selectAll();

    void updateNodeType(QString name);


public slots:
    void updateNodeTypeName(QString name);
    void addNodeItem(NodeItem* item);
    void removeNodeItem(NodeItem* item);
    void addEdgeItem(NodeEdge* edge);
    void centreItem(NodeItem* item);
    void clearView();
protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void wheelEvent(QWheelEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);


private:
    QString NodeType;
    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    qreal totalScaleFactor;
};

#endif // NODEVIEW_H
