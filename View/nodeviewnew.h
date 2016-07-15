#ifndef NODEVIEWNEW_H
#define NODEVIEWNEW_H

#include <QGraphicsView>
//#include <QObject>

#include "SceneItems/nodeitemnew.h"
#include "viewitem.h"
#include "Controller/viewcontroller.h"

class NodeViewNew : public QGraphicsView
{
    Q_OBJECT
public:
    NodeViewNew(VIEW_ASPECT aspect=VA_NONE);
    void setViewController(ViewController* viewController);
    void translate(QPointF point);
    void scale(qreal sx, qreal sy);

    QRectF getViewportRect();
    void viewportChanged();
signals:
    void viewportChanged(QRectF rect, qreal zoom);
    void viewFocussed(NodeViewNew* view, bool focussed);
private slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void selectionHandler_ItemSelected(ViewItem*item, bool selected);
private slots:
    void fitToScreen();

    void item_SetSelected(EntityItemNew* item, bool selected, bool append);
    void item_SetExpanded(EntityItemNew* item, bool expand);
    void item_SetCentered(EntityItemNew* item);
    void item_AdjustingPos(bool adjusting);
    void item_AdjustPos(QPointF delta);

    void minimap_Panning(bool panning);
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);
private:
    void centerOnItems(QList<EntityItemNew*> items);
    NodeItemNew* getModelItem();
    void centerRect(QRectF rectScene);
    void centerView(QPointF scenePos);

    QRectF viewportRect();
    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    NodeItemNew* getParentNodeItem(NodeViewItem* item);

    EntityItemNew* getEntityItem(int ID);
    EntityItemNew* getEntityItem(ViewItem* item);

    void zoom(int delta, QPoint anchorScreenPos = QPoint());

    QPointF getScenePosOfPoint(QPoint pos = QPoint());
private:
    void clearSelection();
    void selectAll();
    void setupAspect();
private:
    QHash<int, EntityItemNew*> guiItems;

    ViewController* viewController;
    SelectionHandler* selectionHandler;

    bool isPanning;
    QPoint panOrigin_Screen;
    QPointF panPrev_Scene;

    QPointF viewportCenter_Scene;

    VIEW_ASPECT aspect;
    QString aspectName;
    QColor aspectColor;
    QFont aspectFont;
    QColor aspectFontColor;

protected:
    void keyPressEvent(QKeyEvent* event);
    void wheelEvent(QWheelEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void focusInEvent(QFocusEvent* event);
    void focusOutEvent(QFocusEvent* event);

    // QGraphicsView interface
protected:
    void drawBackground(QPainter *painter, const QRectF &rect);
};

#endif // NODEVIEWNEW_H
