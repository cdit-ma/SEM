#ifndef NODEVIEW_H
#define NODEVIEW_H

#include "../../Controllers/ViewController/viewcontroller.h"

#include "SceneItems/entityitem.h"
#include "SceneItems/Node/nodeitem.h"
#include "SceneItems/Edge/edgeitem.h"
#include "SceneItems/arrowline.h"

#include <QGraphicsView>
#include <QStateMachine>
#include <QStaticText>

class NodeView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit NodeView(ViewController& view_controller, QWidget* parent = nullptr);
    ~NodeView() final;

    void setContainedViewAspect(VIEW_ASPECT aspect);
    void setContainedNodeViewItem(NodeViewItem* item);
    ViewItem* getContainedViewItem();

    QColor getBackgroundColor() const;
    SelectionHandler& getSelectionHandler() const;

    void alignHorizontal();
    void alignVertical();
    void centerSelection();

    void update_minimap();

signals:
    // Transition Signals
    void leftMousePressed();
    void rightMousePressed();
    void mouseReleased();
    void ctrlShiftPressed();
    void itemMoveTriggered();
    void connectModeTriggered();

    void toolbarRequested(QPoint screenPos, QPointF itemPos);

    void viewport_changed(QRectF viewport_scene_rect, double zoom_factor);
    void scenerect_changed(QRectF scene_rect);

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

    void zoomIn() { minimap_Zoom(1); }
    void zoomOut() { minimap_Zoom(-1); }

    void fitViewToScreen(bool if_active);
    void fitToScreen();

    void receiveMouseMove(QMouseEvent* event);

private slots:
    void viewItem_LabelChanged(const QString& label);

    void viewItem_Constructed(ViewItem* view_item);
    void viewItem_Destructed(int ID, ViewItem* view_item);

    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool is_active_);
    void themeChanged();

    void node_ConnectEdgeMenu(QPointF scene_pos, EDGE_KIND kind, EDGE_DIRECTION direction);
    void node_AddMenu(QPointF scene_pos, int index);
	
	void item_EditData(ViewItem* item, const QString& key_name);
	void item_RemoveData(ViewItem* item, const QString& key_name);

    void item_Selected(ViewItem* item, bool append);
    void item_ActiveSelected(ViewItem* item);
    void item_SetExpanded(EntityItem* item, bool expand);

    void centerOnItem(int ID);
    void highlightItem(int ID, bool highlighted);

    void selectAndCenterConnections(const QVector<ViewItem*>& items);
    void centerOnItemIDs(const QList<int>& ids);
    void selectItemIDs(const QList<int>& ids);

    void activeViewDockChanged(ViewDockWidget* dw);

    void addNodeTriggered(int parent_id, NODE_KIND kind);
    void actionFinished();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void wheelEvent(QWheelEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void drawForeground(QPainter* painter, const QRectF &rect) override;
    void drawBackground(QPainter* painter, const QRectF &rect) override;

    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;

    void resizeEvent(QResizeEvent *event) override;

private:
    void connectViewController();
    void setupStateMachine();

    void themeItem(EntityItem* item);

    void showItem(EntityItem* item);
    void setupItemConnections(EntityItem* item);

    void centerOnItems(const QList<EntityItem *> &items);
    void centerOnItemInternal(EntityItem* item);
    void centerRect(QRectF rect_scene);
    void centerViewOn(QPointF scene_pos);

    void zoom(int delta, QPoint anchor_screen_pos = QPoint());
    void cappedScale(qreal scale);
    void translate(QPointF point);

    void shiftOrderInParent(NodeItem* item, int key_pressed);
    void topLevelItemMoved();

    qreal distance(QPoint p1, QPoint p2);

    QRectF getSceneBoundingRectOfItems(const QList<EntityItem *> &items);
    QRectF viewportSceneRect();

    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    QList<ViewItem*> getTopLevelViewItems() const;
    QList<EntityItem*> getTopLevelEntityItems() const;
    QList<EntityItem*> getSelectedItems() const;

    NodeItem* getParentNodeItem(NodeViewItem* item);

    EntityItem* getEntityItem(int ID) const;
    EntityItem* getEntityItem(ViewItem* item) const;
    EntityItem* getEntityAtPos(QPointF scenePos) const;

    // State Functions
    void setMovingModeOn();
    void setMovingModeOff();
    void moveSelection(QPointF delta);

    void setRubberbandModeOn();
    void setRubberbandModeOff();
    void selectItemsInRubberband();

    void setConnectingModeOn(QPointF scene_pos, EDGE_KIND edge_kind, EDGE_DIRECTION direction);
    void connectUsingConnectLine();

    /*
     * MEMBER VARIABLES
     */

    QList<int> top_level_gui_item_ids_;
    QHash<int, EntityItem*> gui_items_;

    ViewController& view_controller_;
    SelectionHandler* selection_handler_ = nullptr;

    bool is_active_ = false;
    bool is_aspect_view_ = false;

    bool center_on_construct_ = false;
    bool select_on_construct_ = false;

    int constructed_node_id_ = -1;
    NODE_KIND clicked_node_kind_ = NODE_KIND::NONE;

    VIEW_ASPECT contained_aspect_ = VIEW_ASPECT::NONE;
    NodeViewItem* contained_node_view_item_ = nullptr;

    ViewportAnchor zoom_anchor_ = ViewportAnchor::NoAnchor;

    QRectF current_scene_rect_;

    qreal pan_distance_ = 0;
    QPoint event_last_pos_;
    QPointF event_last_scene_pos_;

    QRubberBand* rubberband_ = nullptr;
    QTransform old_transform_;

    QFont background_font_;
    QColor background_text_color_;
    QColor background_color_;

    QColor body_color_;
    QColor alt_body_color_;
    QColor text_color_;
    QColor alt_text_color_;
    QColor header_color_;
    QColor highlight_color_;
    QColor highlight_text_color_;
    QPen default_pen_;

    QStaticText background_text_;
    QRectF background_text_rect_;

    // View input/event state machine
    QStateMachine* state_machine_ = nullptr;
    QState* state_idle_ = nullptr;
    QState* state_panning_mode_ = nullptr;
    QState* state_moving_mode_ = nullptr;
    QState* state_connecting_mode_ = nullptr;
    QState* state_rubberband_keys_pressed_ = nullptr;
    QState* state_rubberband_mode_ = nullptr;

    ArrowLine* connect_line_ = nullptr;

    const QPointF null_point_ = QPointF();
};

#endif // NODEVIEW_H
