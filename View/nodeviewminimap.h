#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);
signals:
    void minimap_Panning(QPointF delta);
    void minimap_Panned();
    void minimap_Pan();
    void minimap_Scrolled(int delta);


public slots:
    void viewportRectChanged(QRectF viewport);
    void centerView();

private:
    void setupLayout();
    QPointF previousScenePos;
    QRectF viewport;
    bool viewportContainsPoint(QPointF localPos);
    bool isPanning;

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
