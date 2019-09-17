#ifndef DATAFLOWGRAPHICSVIEW_H
#define DATAFLOWGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsItem>

class DataflowGraphicsView : public QGraphicsView
{

public:
    DataflowGraphicsView(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    bool panning_ = false;
    QPointF prev_pan_origin_;

};

#endif // DATAFLOWGRAPHICSVIEW_H
