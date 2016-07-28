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
    void toolbarRequested(QPointF screenPos);
    void viewportChanged(QRectF rect, qreal zoom);
    void viewFocussed(NodeViewNew* view, bool focussed);

private slots:
    void viewItem_LabelChanged(QString label);
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
private slots:
    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool isActive);
    void selectionHandler_SelectAll();

    void themeChanged();

private slots:
    void fitToScreen();

    void item_Selected(ViewItem* item, bool append);
    void item_ActiveSelected(ViewItem* item);

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
    void selectItemsInRubberband();
    void _selectAll();
    void setState(VIEW_STATE state);
    void transition();
    qreal distance(QPoint p1, QPoint p2);
private:

    EntityItemNew* getEntityAtPos(QPointF scenePos);
    QList<int> topLevelGUIItemIDs;
    QHash<int, EntityItemNew*> guiItems;

    ViewController* viewController;
    SelectionHandler* selectionHandler;

    QPoint pan_lastPos;
    QPointF pan_lastScenePos;
    qreal pan_distance;

    QPoint rubberband_lastPos;
    QRubberBand* rubberband;


    QPointF viewportCenter_Scene;


    bool isAspectView;
    VIEW_ASPECT containedAspect;
    NodeViewItem* containedNodeViewItem;

    VIEW_STATE viewState;

    QColor backgroundColor;
    QString backgroundText;
    QFont backgroundFont;
    QColor backgroundFontColor;

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
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
