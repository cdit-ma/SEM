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
    friend class ToolbarWidget;
    friend class NewController;
    Q_OBJECT
public:
    NodeView(bool subView = false, QWidget *parent = 0);
    ~NodeView();

    void setDock(DockScrollArea* dock);

    //Set Controller
    void setController(NewController* controller);
    void disconnectController();

    //Get the Selected Node.
    Node* getSelectedNode();

    void setParentNodeView(NodeView *n);
    void removeSubView(NodeView* subView);


    QStringList getAdoptableNodeList(Node* node=0);
    QList<Node*> getConnectableNodes(Node* node=0);


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
    void componentNodeMade(QString type, NodeItem* nodeItem);
    void hardwareNodeMade(QString type, NodeItem* nodeItem);
    void updateDockButtons(QString dockButton);
    void updateDockContainer(QString container);

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

    void centerNode(QString nodeLabel);

    void getAdoptableNodesList(Node* node);
    void getLegalNodesList(Node* src);
    void getComponentDefinitions(Node* node);

    void updateDockAdoptableNodesList(Node* node);
    void updateMenuList(QString action, QStringList* nodeKinds, QList<Node*>* nodes);

    void constructNode(QString nodeKind, QPointF relativePosition);
    void constructEdge(Node* src, Node* dst);


    void constructConnectedComponents(Node* parent, Node* connectedNode, QString nodeKind, QPointF relativePosition);
    void constructComponentInstance(Node* assm, Node* defn, QPointF center);


    void turnOnViewAspect(QString aspect);
    void setGoToMenuActions(QString action, bool);


    void view_ClearHistoryStates();
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

    void view_DockConstructNode(Node* node, QString kind);
    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void view_DestructGraphMLGUI(QString ID);

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);


    void constructNewView(Node* centeredOn);
    

    void sortEntireModel();
    void sortNode(Node* node, Node* topMostNode = 0);

    void fitToScreen();
    void centreNode(Node* node);

    void clearSelection();

    void updateDockButtons(Node* node);
    void view_updateDockContainer(QString dockContainer);

    void view_sortModel();
    void view_centerViewAspects();

    void setGoToToolbarButtons(QString action, Node *node);

    void goToDefinition(Node* node, bool show = true);
    void goToImplementation(Node* node, bool show = true);

    void view_deleteSelectedNode();
    void view_ConstructNode(QString nodeKind);
    void view_ConstructEdge(Node* src, Node* dst);
    void view_ConstructComponentInstance(Node *assm, Node *defn, int sender);

    void updateToolbarMenuList(QString action, Node* node);
    void updateToolbarAdoptableNodesList(QStringList nodeKinds);
    void updateToolbarLegalNodesList(QList<Node*>* nodeList);
    void updateToolbarDefinitionsList(QList<Node*>* nodeList);

    void componentInstanceConstructed(Node* node);

private:
    NewController* getController();
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    bool isSubView();
    QList<Node*>* getFiles();

    void updateDocks();

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    void nodeConstructed_signalUpdates(NodeItem* nodeItem);
    void nodeSelected_signalUpdates(Node *node);
    void edgeConstructed_signalUpdates(Node* src);

    Node* hasDefinition(Node* node);
    Node* hasImplementation(Node* node);

    NodeItem* getNodeItemFromNode(Node* node);
    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    NodeEdge* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromHash(QString ID);

    NewController* controller;

    QHash<QString, GraphMLItem*> guiItems;

    NodeView* parentNodeView;

    QStringList allAspects;

    QPointF menuPosition;

    QStringList currentAspects;
    //QPoint origin;
    QRubberBand* rubberBand;
    QString NodeType;
    qreal totalScaleFactor;

    bool RUBBERBAND_MODE;
    bool drawingRubberBand;
    QPoint rubberBandOrigin;
    bool once;


    QRectF getVisibleRect();
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;



    QList<NodeItem*> getNodeItemsList();

    void showAllViewAspects();



    QList<Node*>* files;
    ToolbarWidget* toolbar;


    DockScrollArea* dock;



    bool IS_SUB_VIEW;

    QList<NodeView*> subViews;

    bool autoCenterOn;

    bool toolbarJustClosed;
protected:
    bool viewportEvent(QEvent *);

};

#endif // NODEVIEW_H
