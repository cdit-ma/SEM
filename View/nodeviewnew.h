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
    NodeViewNew(QWidget *parent = 0);
    ~NodeViewNew();
    void setViewController(ViewController* viewController);
    void translate(QPointF point);
    void scale(qreal sx, qreal sy);

    void setContainedViewAspect(VIEW_ASPECT aspect);
    void setContainedNodeViewItem(NodeViewItem* item);

    QColor getBackgroundColor();
    QRectF getViewportRect();
    void viewportChanged();
    SelectionHandler* getSelectionHandler();
    void fitToScreen();
    void centerSelection();
signals:
    void toolbarRequested(QPoint screenPos, QPointF itemPos);
    void viewportChanged(QRectF rect, qreal zoom);
    void viewFocussed(NodeViewNew* view, bool focussed);

    void triggerAction(QString);
    void setData(int, QString, QVariant);
    void removeData(int, QString);

private slots:
    void viewItem_LabelChanged(QString label);
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
private slots:
    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool isActive);
    void selectAll();

    void clearSelection();

    void themeChanged();

private slots:

    void item_RemoveData(ViewItem* item, QString keyName);
    void item_Selected(ViewItem* item, bool append);
    void item_ActiveSelected(ViewItem* item);

    void item_SetExpanded(EntityItemNew* item, bool expand);
    void item_SetCentered(EntityItemNew* item);
    void item_AdjustingPos(bool adjusting);

    void item_AdjustPos(QPointF delta);
    void item_Resizing(bool resizing);
    void item_ResizeFinished(NodeItemNew* item, RECT_VERTEX vertex);
    void item_Resize(NodeItemNew *item, QSizeF delta, RECT_VERTEX vert);

    void minimap_Panning(bool panning);
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);
private:
    void setupConnections(EntityItemNew* item);

    void centerOnItems(QList<EntityItemNew*> items);
    void centerRect(QRectF rectScene);
    void centerView(QPointF scenePos);

    QRectF viewportRect();
    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    QList<ViewItem*> getTopLevelViewItems();
    QList<EntityItemNew*> getTopLevelEntityItems();

    QList<EntityItemNew*> getSelectedItems();


    NodeItemNew* getParentNodeItem(NodeViewItem* item);

    EntityItemNew* getEntityItem(int ID);
    EntityItemNew* getEntityItem(ViewItem* item);

    void zoom(int delta, QPoint anchorScreenPos = QPoint());

    QPointF getScenePosOfPoint(QPoint pos = QPoint());
private:
    void selectItemsInRubberband();
    void _selectAll();
    void _clearSelection();
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
    bool isBackgroundSelected;
    VIEW_ASPECT containedAspect;
    NodeViewItem* containedNodeViewItem;

    VIEW_STATE viewState;

    QColor backgroundColor;
    QString backgroundText;
    QFont backgroundFont;
    QColor backgroundFontColor;
    QColor selectedBackgroundFontColor;

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
