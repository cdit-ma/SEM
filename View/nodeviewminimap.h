#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);

signals:
    void viewportRectMoved(QPointF delta);

public slots:
    void viewportRectChanged(QRectF viewport);

private:
    QRectF viewport;

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
};

#endif // NODEVIEWMINIMAP_H
