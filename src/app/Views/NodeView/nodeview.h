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
    NodeView(QWidget *parent = 0);
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
    void viewport_changed(QRectF viewportRect, double zoom_factor);
    void scenerect_changed(QRectF sceneRect);
    void viewFocussed(NodeView* view, bool focussed);

    void triggerAction(QString);
    void setData(int, QString, QVariant);
    void removeData(int, QString);
    void editData(int, QString);

private slots:
    void viewItem_LabelChanged(QString label);
    void viewItem_Constructed(ViewItem* viewItem);
    void viewItem_Destructed(int ID, ViewItem* viewItem);
private slots:
    void selectionHandler_ItemSelectionChanged(ViewItem* item, bool selected);
    void selectionHandler_ItemActiveSelectionChanged(ViewItem* item, bool isActive);
    void themeChanged();

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
    
private slots:
    
    
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
    
    void centerConnections(ViewItem *item);
    void highlightItem(int ID, bool highlighted);
private:
    void fitToScreen();
    void themeItem(EntityItem* item);

    QColor body_color;
    QColor alt_body_color;
    QColor text_color;
    QColor alt_text_color;
    QColor header_color;
    QColor highlight_color;
    QColor highlight_text_color;
    QPen default_pen;


    void showItem(EntityItem* item);
    void setupConnections(EntityItem* item);

    void centerOnItems(QList<EntityItem*> items);
    QRectF getSceneBoundingRectOfItems(QList<EntityItem*> items);
    void centerRect(QRectF rectScene);
    void centerView(QPointF scenePos);

    QRectF viewportRect();
    void nodeViewItem_Constructed(NodeViewItem* item);
    void edgeViewItem_Constructed(EdgeViewItem* item);

    QList<ViewItem*> getTopLevelViewItems() const;
    QList<EntityItem*> getTopLevelEntityItems() const;
    QList<EntityItem*> getSelectedItems() const;


    NodeItem* getParentNodeItem(NodeViewItem* item);

    EntityItem* getEntityItem(int ID) const;
    EntityItem* getEntityItem(ViewItem* item) const;

    void zoom(int delta, QPoint anchorScreenPos = QPoint());

    QPointF getScenePosOfPoint(QPoint pos = QPoint());
private:
    void selectItemsInRubberband();
    void _selectAll();
    void _clearSelection();
    qreal distance(QPoint p1, QPoint p2);
private:
    void ShiftOrderInParent(NodeItem* item, int key_pressed);
    
    void setupStateMachine();

    void topLevelItemMoved();

    EntityItem* getEntityAtPos(QPointF scenePos);
    QList<int> topLevelGUIItemIDs;
    QHash<int, EntityItem*> guiItems;

    ViewController* viewController = 0;
    SelectionHandler* selectionHandler = 0;

    QRectF currentSceneRect;
    QPoint pan_lastPos;
    QPointF pan_lastScenePos;
    qreal pan_distance;

    QPoint rubberband_lastPos;

    QRubberBand* rubberband = 0;

    bool is_active = false;
    bool isPanning = false;
    bool isAspectView = false;
    QPointF viewportCenter_Scene;
    
    QList<double> render_times;

    QTransform old_transform;
    VIEW_ASPECT containedAspect = VIEW_ASPECT::NONE;
    NodeViewItem* containedNodeViewItem = 0;


    QFont background_font;
    QColor background_text_color;
    QColor background_color;

    QStaticText background_text;
    QRectF background_text_rect;


    QStateMachine* viewStateMachine = 0;
    QState* state_InActive = 0;

    QState* state_Active = 0;
    QState* state_Active_Moving = 0;
    QState* state_Active_RubberbandMode = 0;
    QState* state_Active_RubberbandMode_Selecting = 0;
    QState* state_Active_Connecting = 0;


    ArrowLine* connect_line = 0;

private slots:
    void activeViewDockChanged(ViewDockWidget* dw);
    void state_Moving_Entered();
    void state_Moving_Exited();

    void state_RubberbandMode_Entered();
    void state_RubberbandMode_Exited();

    void state_RubberbandMode_Selecting_Entered();
    void state_RubberbandMode_Selecting_Exited();

    void state_Connecting_Exited();

    void state_Default_Entered();




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

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *);
};

#endif // NODEVIEW_H
