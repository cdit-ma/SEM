#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

/**
 * @brief The NodeViewMinimap class
 */
class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);
    void setEnabled(bool enabled);
    void setScene(QGraphicsScene *scene);
    bool isPanning();

signals:
    void minimap_Panning(bool panning);
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);

public slots:
    void viewportRectChanged(QRectF viewportRect);

private:
    void centerView();
    void setMinimapPanning(bool pan);

    bool viewportContainsPoint(QPointF localPos);

    QPointF previousScenePos;
    QRectF viewportRect;

    bool _isPanning;
    bool drawRect;
protected:
    void drawForeground(QPainter *painter, const QRectF &rect);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent *);
};

#endif // NODEVIEWMINIMAP_H
