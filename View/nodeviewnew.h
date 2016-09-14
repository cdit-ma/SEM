#ifndef NODEVIEWNEW_H
#define NODEVIEWNEW_H

#include <QGraphicsView>
//#include <QObject>

#include "SceneItems/nodeitemnew.h"
#include "viewitem.h"
#include "Controller/viewcontroller.h"

#include <QStateMachine>


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
    void resetMinimap();
    void viewportChanged();
    SelectionHandler* getSelectionHandler();
    void fitToScreen();
    void alignHorizontal();
    void alignVertical();

    void centerSelection();
    QList<int> getIDsInView();
signals:
    void trans_InActive2Moving();
    void trans_Moving2InActive();

    void trans_InActive2Resizing();
    void trans_Resizing2InActive();

    void trans_InActive2RubberbandMode();
    void trans_RubberbandMode2InActive();

    void trans_RubberbandMode2RubberbandMode_Selecting();
    void trans_RubberbandMode_Selecting2RubberbandMode();

    void trans_InActive2Connecting();
    void trans_Connecting2InActive();



    void sceneRectChanged(QRectF sceneRect);
    void toolbarRequested(QPoint screenPos, QPointF itemPos);
    void viewportChanged(QRectF rect, qreal zoom);
    void viewFocussed(NodeViewNew* view, bool focussed);

    void triggerAction(QString);
    void setData(int, QString, QVariant);
    void removeData(int, QString);
    void editData(int, QString);

private slots:

    void test();
    void viewItem_LabelChanged(QString label);
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

private slots:
    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool isActive);
    void selectAll();
    void itemsMoved();



    void clearSelection();

    void themeChanged();

private slots:
    void node_ConnectMode(NodeItemNew* item);
    void item_EditData(ViewItem* item, QString keyName);
    void item_RemoveData(ViewItem* item, QString keyName);
    void item_Selected(ViewItem* item, bool append);
    void item_ActiveSelected(ViewItem* item);

    void item_SetExpanded(EntityItemNew* item, bool expand);
    void item_SetCentered(EntityItemNew* item);

    void item_MoveSelection(QPointF delta);
    void item_Resize(NodeItemNew *item, QSizeF delta, RECT_VERTEX vert);

    void minimap_Panning(bool panning);
    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);

    void centerItem(int ID);
    void highlightItem(int ID, bool highlighted);
private:
    void setupConnections(EntityItemNew* item);

    void centerOnItems(QList<EntityItemNew*> items);
    QRectF getSceneBoundingRectOfItems(QList<EntityItemNew*> items);
    void centerRect(QRectF rectScene);
    void centerView(QPointF scenePos);

    QRectF viewportRect();
    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    QList<ViewItem*> getTopLevelViewItems() const;
    QList<EntityItemNew*> getTopLevelEntityItems() const;
    QList<EntityItemNew*> getSelectedItems() const;
    QList<EntityItemNew*> getOrderedSelectedItems() const;


    NodeItemNew* getParentNodeItem(NodeViewItem* item);

    EntityItemNew* getEntityItem(int ID) const;
    EntityItemNew* getEntityItem(ViewItem* item) const;
    NodeItemNew* getNodeItem(ViewItem* item) const;

    void zoom(int delta, QPoint anchorScreenPos = QPoint());

    QPointF getScenePosOfPoint(QPoint pos = QPoint());
private:
    void selectItemsInRubberband();
    void _selectAll();
    void _clearSelection();
    qreal distance(QPoint p1, QPoint p2);
private:
    void setupStateMachine();

    EntityItemNew* getEntityAtPos(QPointF scenePos);
    QList<int> topLevelGUIItemIDs;
    QHash<int, EntityItemNew*> guiItems;

    ViewController* viewController;
    SelectionHandler* selectionHandler;

    QRectF currentSceneRect;
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

    bool isPanning;

    QColor backgroundColor;
    QString backgroundText;
    QFont backgroundFont;
    QColor backgroundFontColor;
    QColor selectedBackgroundFontColor;


    QStateMachine* viewStateMachine;
    QState* state_InActive;

    QState* state_Active;
    QState* state_Active_Moving;
    QState* state_Active_Resizing;
    QState* state_Active_RubberbandMode;
    QState* state_Active_RubberbandMode_Selecting;
    QState* state_Active_Connecting;

    QGraphicsLineItem* connectLineItem;
    QLineF connectLine;

private slots:
    void state_Moving_Entered();
    void state_Moving_Exited();

    void state_Resizing_Entered();
    void state_Resizing_Exited();

    void state_RubberbandMode_Entered();
    void state_RubberbandMode_Exited();

    void state_RubberbandMode_Selecting_Entered();
    void state_RubberbandMode_Selecting_Exited();

    void state_Connecting_Entered();
    void state_Connecting_Exited();

    void state_Default_Entered();




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
