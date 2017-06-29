#ifndef NODEVIEWMINIMAP_H
#define NODEVIEWMINIMAP_H

#include <QWidget>
#include <QGraphicsScene>

/**
 * @brief The NodeViewMinimap class
 */
class NodeViewMinimap : public QWidget
{
    Q_OBJECT
public:
    explicit NodeViewMinimap(QObject *parent = 0);
    void setBackgroundColor(QColor color);
    void setScene(QGraphicsScene *scene);
signals:
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);
    void minimap_CenterView();
public slots:
    void viewport_changed(QRectF viewportRect, double zoom_factor);
private:
    void updateScene();
    bool isPanning() const;
    QPointF getScenePos(QPoint mouse_pos);
    void updateViewport();

    QRect renderArea() const;
    QRect zoomIcon() const;
    QRect zoomText() const;
    QRect infoBox() const;


    void setMinimapPanning(bool pan);

    bool viewportContainsPoint(QPoint pos);

    QGraphicsScene* scene = 0;
    
    QRect translated_viewport_rect;
    
    QRectF scene_rect;
    QRectF viewport_rect;
    
    QString zoom_str;

    QPointF previous_pos;

    bool is_panning = false;
    
    QPixmap zoom_icon;
    
    QColor background_color = Qt::darkGray;

    QPixmap pixmap;

    QFont zoom_font;

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void resizeEvent(QResizeEvent *);
};

#endif // NODEVIEWMINIMAP_H
