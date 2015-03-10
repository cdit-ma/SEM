#ifndef NODEVIEW_H//
#define NODEVIEW_H

#include "../Controller/newcontroller.h"
#include "nodeitem.h"
#include "dockscrollarea.h"

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QRubberBand>


class ToolbarWidget;

class NodeView : public QGraphicsView
{
    friend class DockScrollArea;
    friend class ToolbarWidget;
    friend class NewController;
    Q_OBJECT

public:
    NodeView(bool subView = false, QWidget *parent = 0);
    ~NodeView();

    //Set Controller
    void setController(NewController* controller);
    void disconnectController();

    //Get the Selected Node.
    Node* getSelectedNode();

    void setParentNodeView(NodeView *n);
    void removeSubView(NodeView* subView);

    // this is used by the parts dock
    QStringList getConstructableNodeKinds();


protected:
    //Mouse Handling Methods
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    //Keyboard Handling Methods
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

signals:
    //SIGNALS for other View Elements
    void view_SetSelectedAttributeModel(AttributeTableModel* model);
    void viewportRectangleChanged(QRectF);
    void updateViewAspects(QStringList aspects);

    //SIGNALS for the Controller
    void triggerAction(QString action);

    void deletePressed(bool isDown);
    void controlPressed(bool isDown);
    void escapePressed(bool isDown);
    void shiftPressed(bool isDown);

    void copy();
    void cut();
    void paste();

    void undo();
    void redo();

    void unselect();
    void selectAll();


    void sortModel();
    void sortDeployment();

    //void centerNode(QString nodeLabel);

    void constructNode(QString nodeKind, QPointF relativePosition);
    void constructEdge(Node* src, Node* dst);

    void constructComponentInstance(Node* assm, Node* defn, QPointF center);
    void constructEventPortDelegate(Node *assm, Node *eventPortInstance, QPointF center);

    void constructConnectedComponents(Node* parent, Node* connectedNode, QString nodeKind, QPointF relativePosition);

    void turnOnViewAspect(QString aspect);

    void setGoToMenuActions(QString action, bool);

    void sceneRectChanged(QRectF newRect);

    void view_ClearHistoryStates();

    // new signals for docks
    void view_enableDocks(bool enable);
    void view_nodeConstructed(NodeItem* nodeItem);
    void view_nodeSelected(Node* node);
    void view_nodeDestructed();
    void view_edgeConstructed();
    void view_edgeDestructed();

public slots:
    void view_ClearHistory();
    void toolbarClosed();
    void forceSortViewAspects();
    void resetModel();

    void setAutoCenterViewAspects(bool center);
    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);
    void view_ConstructGraphMLGUI(GraphML* item);
    void printErrorText(GraphML* graphml, QString text);
    //void removeNodeItem(NodeItem* item);
    void centreItem(GraphMLItem* item);
    void clearView();

    void setRubberBandMode(bool On);
    void setViewAspects(QStringList aspects);

    void showToolbar(QPoint position);

    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void view_DestructGraphMLGUI(QString ID);

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);


    void sortEntireModel();
    void sortNode(Node* node, Node* topMostNode = 0);

    void fitToScreen();

    void clearSelection();

    void view_sortModel();
    void view_centerViewAspects();

    void goToDefinition(Node* node);
    void goToImplementation(Node* node);
    void goToInstance(Node* node);

    void view_deleteSelectedNode();
    void view_constructNewView(Node* centeredOn);

    void view_constructNode(QString nodeKind, int sender);
    void view_constructEdge(Node* src, Node* dst);

    void view_constructComponentInstance(Node *assm, Node *defn, int sender);
    void view_constructEventPortDelegate(Node *assm, Node *eventPortInstance);

    void componentInstanceConstructed(Node* node);

    void updateSceneRect(NodeItem *item);

private:
    NewController* getController();
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    bool isSubView();

    QStringList getAdoptableNodeList(Node* node=0);
    QList<Node*> getConnectableNodes(Node* node=0);

    QList<Node*> getFiles();
    QList<Node*> getComponents();

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    void nodeConstructed_signalUpdates(NodeItem* nodeItem);
    void nodeSelected_signalUpdates(Node *node);
    void nodeDestructed_signalUpdates();
    void edgeConstructed_signalUpdates(Node* src);

    Node* hasDefinition(Node* node);
    Node* hasImplementation(Node* node);

    NodeItem* getNodeItemFromNode(Node* node);
    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    NodeEdge* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromHash(QString ID);

    NewController* controller;

    QString centralizedItemID;
    bool CENTRALIZED_ON_ITEM;

    QHash<QString, GraphMLItem*> guiItems;

    NodeView* parentNodeView;

    QStringList allAspects;

    QPointF menuPosition;

    QStringList currentAspects;
    QRubberBand* rubberBand;
    QString NodeType;
    qreal totalScaleFactor;

    bool RUBBERBAND_MODE;
    bool drawingRubberBand;
    QPoint rubberBandOrigin;
    bool once;

    QRectF getVisibleRect();
    void adjustSceneRect(QRectF rectToCenter);
    void centerRect(QRectF rect, float extraspace = 0);

    bool CONTROL_DOWN;
    bool SHIFT_DOWN;



    QList<NodeItem*> getNodeItemsList();

    void showAllViewAspects();


    ToolbarWidget* toolbar;


    bool IS_SUB_VIEW;

    QList<NodeView*> subViews;

    bool autoCenterOn;

    bool toolbarJustClosed;


protected:
    bool viewportEvent(QEvent *);

};

#endif // NODEVIEW_H
