#ifndef DATAFLOWGRAPHICSVIEW_H
#define DATAFLOWGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

class DataflowGraphicsView : public QGraphicsView
{

public:
    DataflowGraphicsView(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent *event);

private:
    bool panning_ = false;
    QPointF prev_pan_origin_;

};

#endif // DATAFLOWGRAPHICSVIEW_H
