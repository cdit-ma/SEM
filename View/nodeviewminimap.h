#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);

signals:
    void minimap_Panned(QPointF delta);
    void minimap_Scrolled(int delta);

    void minimap_Pressed(QMouseEvent* event);
    void minimap_Moved(QMouseEvent* event);
    void minimap_Released(QMouseEvent* event);


public slots:
    void viewportRectChanged(QRectF viewport);

private:
    QRectF viewport;
    QPointF lastEventPos;
    bool viewportContainsPoint(QPointF localPos);
    bool isPanning;
    QPointF previousScenePosition;
    // QGraphicsView interface
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
};

#endif // NODEVIEWMINIMAP_H
