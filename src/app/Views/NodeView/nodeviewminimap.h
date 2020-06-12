#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QGraphicsView>

class ViewDockWidget;

/**
 * @brief The NodeViewMinimap class
 */
class NodeViewMinimap : public QGraphicsView
{
    Q_OBJECT
    
public:
    explicit NodeViewMinimap(QObject *parent = nullptr);
    void setBackgroundColor(QColor color);
    
    // NOTE: These are non-virtual functions
    //  QGraphicsView's implementations are called within the function
    void setEnabled(bool enabled);
    void setScene(QGraphicsScene *scene);
    
    bool isPanning();

signals:
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);
    void minimap_CenterView();

public slots:
    void sceneRectChanged(QRectF sceneRect);
    void viewportRectChanged(QRectF viewportRect, qreal zoom);
    
private slots:
    void activeViewDockWidgetChanged(ViewDockWidget *viewDock, ViewDockWidget *prevDock);

private:
    void themeChanged();
    
    QRectF zoomIcon() const;
    QRectF zoomText() const;
    QRectF infoBox() const;
    
    void centerView();
    void setMinimapPanning(bool pan);

    bool viewportContainsPoint(QPointF localPos);

    QPointF previousScenePos;
    QRectF viewportRect;
    QString zoomPercent;
    QPixmap zoomPixmap;
    QColor backgroundColor;
    
    bool _isPanning = false;
    bool drawRect = false;
    
protected:
    void drawForeground(QPainter *painter, const QRectF &rect) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent *) override;
};

#endif // NODEVIEWMINIMAP_H