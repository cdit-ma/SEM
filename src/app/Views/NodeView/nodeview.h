#ifndef NODEVIEW_H
#define NODEVIEW_H

#include <QGraphicsView>

#include "../../Controllers/ViewController/viewcontroller.h"

#include "SceneItems/entityitem.h"
#include "SceneItems/Node/nodeitem.h"
#include "SceneItems/Edge/edgeitem.h"
#include "SceneItems/arrowline.h"

#include <QStateMachine>
#include <QStaticText>

class NodeView : public QGraphicsView
{
    Q_OBJECT

public:
    NodeView(QWidget *parent = nullptr);
    ~NodeView();

    void setViewController(ViewController* viewController);
    void translate(QPointF point);
    void scale(qreal sx, qreal sy);

    void setContainedViewAspect(VIEW_ASPECT aspect);
    void setContainedNodeViewItem(NodeViewItem* item);
    ViewItem* getContainedViewItem();

    QColor getBackgroundColor();
    SelectionHandler* getSelectionHandler();

    void alignHorizontal();
    void alignVertical();

    void centerSelection();

    void update_minimap();

signals:
    void trans_inactive();
    void trans_InActive2Moving();
    void trans_Moving2InActive();

    void trans_InActive2RubberbandMode();
    void trans_RubberbandMode2InActive();

    void trans_RubberbandMode2RubberbandMode_Selecting();
    void trans_RubberbandMode_Selecting2RubberbandMode();

    void trans_InActive2Connecting();
    void trans_Connecting2InActive();

    void edgeToolbarRequested(QPoint screenPos, EDGE_KIND kind, EDGE_DIRECTION direction);
    
    void toolbarRequested(QPoint screenPos, QPointF itemPos);
    void viewport_changed(QRectF viewportSceneRect, double zoom_factor);
    void scenerect_changed(QRectF sceneRect);
    void viewFocussed(NodeView* view, bool focussed);

    void triggerAction(QString);
    void setData(int, QString, QVariant);
    void removeData(int, QString);
    void editData(int, QString);

    void itemSelectionChanged(ViewItem* item, bool selected);

public slots:
    void selectAll();
    void clearSelection();

    void minimap_Pan(QPointF delta);
    void minimap_Zoom(int delta);

    void zoomIn(){
        minimap_Zoom(1);
    }

    void zoomOut(){
        minimap_Zoom(-1);
    }

    void AllFitToScreen(bool if_active);
    void FitToScreen();

    void receiveMouseMove(QMouseEvent* event);

private slots:
    void viewItem_LabelChanged(QString label);
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);

    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool isActive);
    void themeChanged();
    
    void node_ConnectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void node_ConnectEdgeMode(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void node_AddMenu(QPointF scene_pos, int index);
    
    void item_EditData(ViewItem* item, QString keyName);
    void item_RemoveData(ViewItem* item, QString keyName);
    void item_Selected(ViewItem* item, bool append);
    void item_ActiveSelected(ViewItem* item);

    void item_SetExpanded(EntityItem* item, bool expand);
    void item_SetCentered(EntityItem* item);

    void item_MoveSelection(QPointF delta);

    void centerItem(int ID);
    void centerOnItem(int ID);
    void highlightItem(int ID, bool highlighted);

    void selectAndCenterConnections(const QVector<ViewItem*>& items);
    void centerOnItemIDs(const QList<int>& ids);
    void selectItemIDs(const QList<int>& ids);

    void activeViewDockChanged(ViewDockWidget* dw);
    void state_Moving_Entered();
    void state_Moving_Exited();

    void state_RubberbandMode_Entered();
    void state_RubberbandMode_Exited();

    void state_RubberbandMode_Selecting_Entered();
    void state_RubberbandMode_Selecting_Exited();

    void state_Connecting_Exited();

    void state_Default_Entered();

    void constructNode(int parent_id, NODE_KIND kind);
    void actionFinished();

protected:
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

    void wheelEvent(QWheelEvent* event);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void drawForeground(QPainter *painter, const QRectF &rect);
    void drawBackground(QPainter *painter, const QRectF &rect);

    void paintEvent(QPaintEvent *event);
    bool event(QEvent *event);

    void resizeEvent(QResizeEvent *);

private:
    void fitToScreen();
    void themeItem(EntityItem* item);

    void showItem(EntityItem* item);
    void setupConnections(EntityItem* item);

    void centerOnItems(QList<EntityItem*> items);
    void centerOnItem(EntityItem* item);
    void centerRect(QRectF rectScene);
    void centerView(QPointF scenePos);

    QRectF getSceneBoundingRectOfItems(QList<EntityItem*> items);
    QRectF viewportSceneRect();

    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    QList<ViewItem*> getTopLevelViewItems() const;
    QList<EntityItem*> getTopLevelEntityItems() const;
    QList<EntityItem*> getSelectedItems() const;

    NodeItem* getParentNodeItem(NodeViewItem* item);

    EntityItem* getEntityItem(int ID) const;
    EntityItem* getEntityItem(ViewItem* item) const;

    void zoom(int delta, QPoint anchor_screen_pos = QPoint());
    void cappedScale(qreal scale);

    QPointF getScenePosOfPoint(QPoint pos = QPoint());

    void selectItemsInRubberband();
    void _selectAll();
    void _clearSelection();
    qreal distance(QPoint p1, QPoint p2);

    void ShiftOrderInParent(NodeItem* item, int key_pressed);
    
    void setupStateMachine();

    void topLevelItemMoved();

    EntityItem* getEntityAtPos(QPointF scenePos);

    /************************************************************************/

    QList<int> topLevelGUIItemIDs;
    QHash<int, EntityItem*> guiItems;

    ViewController* viewController = nullptr;
    SelectionHandler* selectionHandler = nullptr;

    QRectF currentSceneRect;
    QPoint pan_lastPos;
    QPointF pan_lastScenePos;
    qreal pan_distance;

    QPoint rubberband_lastPos;
    QRubberBand* rubberband = nullptr;

    bool is_active = false;
    bool isPanning = false;
    bool isAspectView = false;

    bool center_on_construct_ = false;
    bool select_on_construct_ = false;
    int constructed_node_id_ = -1;
    NODE_KIND clicked_node_kind_ = NODE_KIND::NONE;

    ViewportAnchor zoom_anchor_ = ViewportAnchor::NoAnchor;

    QPointF viewportCenter_Scene;
    
    QList<double> render_times;

    QTransform old_transform;
    VIEW_ASPECT containedAspect = VIEW_ASPECT::NONE;
    NodeViewItem* containedNodeViewItem = nullptr;

    QFont background_font;
    QColor background_text_color;
    QColor background_color;

    QColor body_color;
    QColor alt_body_color;
    QColor text_color;
    QColor alt_text_color;
    QColor header_color;
    QColor highlight_color;
    QColor highlight_text_color;
    QPen default_pen;

    QStaticText background_text;
    QRectF background_text_rect;

    QStateMachine* viewStateMachine = nullptr;
    QState* state_InActive = nullptr;

    QState* state_Active = nullptr;
    QState* state_Active_Moving = nullptr;
    QState* state_Active_RubberbandMode = nullptr;
    QState* state_Active_RubberbandMode_Selecting = nullptr;
    QState* state_Active_Connecting = nullptr;

    ArrowLine* connect_line = nullptr;
};

#endif // NODEVIEW_H
