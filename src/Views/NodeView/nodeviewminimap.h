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
    void setBackgroundColor(QColor color);
    void setScene(QGraphicsScene *scene);
    bool isPanning();

signals:
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);

public slots:
    void sceneRectChanged(QRectF sceneRect);


    void viewportRectChanged(QRectF viewportRect, qreal zoom);

private:
    QRectF zoomIcon() const;
    QRectF zoomText() const;
    QRectF infoBox() const;
    void centerView();
    void setMinimapPanning(bool pan);

    bool viewportContainsPoint(QPointF localPos);

    QPointF previousScenePos;
    QRectF viewportRect;
    QString zoomPercent;
    bool _isPanning;
    QPixmap zoomPixmap;
    bool drawRect;
    QColor backgroundColor;
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
