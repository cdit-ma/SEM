#ifndef NODEVIEWNEW_H
#define NODEVIEWNEW_H

#include <QGraphicsView>
//#include <QObject>

#include "SceneItems/nodeitemnew.h"
#include "viewitem.h"
#include "Controller/viewcontroller.h"
//#include "View/Table/attributetablemodel.h"

class NodeViewNew : public QGraphicsView
{
    Q_OBJECT
public:
    NodeViewNew();
    ~NodeViewNew();
    void setViewController(ViewController* viewController);
    void translate(QPointF point);
    void scale(qreal sx, qreal sy);

    void setContainedViewAspect(VIEW_ASPECT aspect);
    void setContainedNodeViewItem(NodeViewItem* item);

    QRectF getViewportRect();
    void viewportChanged();
    SelectionHandler* getSelectionHandler();
signals:
    void viewportChanged(QRectF rect, qreal zoom);
    void viewFocussed(NodeViewNew* view, bool focussed);

public slots:
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
private slots:
    void selectionHandler_ItemSelected(ViewItem*item, bool selected);
    void selectionHandler_ActiveSelectedItemChanged(ViewItem* item, bool isActive);

    void themeChanged();

private slots:
    void fitToScreen();

    void item_SetSelected(EntityItemNew* item, bool selected, bool append);
    void item_SetActiveSelected(EntityItemNew* item, bool active);
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

    QList<ViewItem*> getTopLevelViewItems();
    QList<EntityItemNew*> getTopLevelEntityItems();

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
    QList<int> topLevelGUIItemIDs;
    QHash<int, EntityItemNew*> guiItems;

    ViewController* viewController;
    SelectionHandler* selectionHandler;

    bool isPanning;
    QPoint panOrigin_Screen;
    QPointF panPrev_Scene;

    QPointF viewportCenter_Scene;


    VIEW_ASPECT containedAspect;
    NodeViewItem* containedNodeViewItem;

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

    void drawBackground(QPainter *painter, const QRectF &rect);

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *);
};

#endif // NODEVIEWNEW_H
