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

signals:
    void minimap_Panning(QPointF delta);
    void minimap_Panned();
    void minimap_Pan();
    void minimap_Scrolled(int delta);


public slots:
    void viewportRectChanged(QRectF viewportRect);
    void centerView();
    void setVisible(bool visible);

private:
    void setupLayout();
    void fitToScreen();
    bool viewportContainsPoint(QPointF localPos);

    QPointF previousScenePos;
    QRectF viewportRect;
    bool isPanning;

protected:
    void drawForeground(QPainter *painter, const QRectF &rect);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    // QGraphicsView interface
};

#endif // NODEVIEWMINIMAP_H
