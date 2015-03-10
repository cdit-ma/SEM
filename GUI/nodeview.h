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
    void view_SetClipboardBuffer(QString);
    void view_UndoListChanged(QStringList);
    void view_RedoListChanged(QStringList);
    void view_ProjectNameChanged(QString);
    void view_ExportProject();
    void view_ExportedProject(QString data);
    void view_ImportProjects(QStringList data);

    void view_Copy(QStringList IDs);
    void view_Cut(QStringList IDs);
    void view_Paste(Node* parent, QString xmlData);
    void view_Delete(QStringList IDs);
    void view_Duplicate(QStringList IDs);
    void view_Undo();
    void view_Redo();

    void view_SetGraphMLData(GraphML*, QString, QString);
    void view_ConstructGraphMLData(GraphML*, QString);
    void view_DestructGraphMLData(GraphML*, QString);

    void view_SetAttributeModel(AttributeTableModel* model);

    void view_ViewportRectChanged(QRectF);
    void view_SceneRectChanged(QRectF);

    void view_AspectsChanged(QStringList aspects);
    void view_GUIAspectChanged(QStringList aspects);

    void view_StatusChanged(QString status);

    //SIGNALS for the Controller
    void view_TriggerAction(QString action);
    void view_ConstructNode(Node* parent, QString nodeKind, QPointF position);
    void view_ConstructEdge(Node* source, Node* destination);
    void view_ConstructConnectedComponents(Node* parent, Node* connectedNode, QString nodeKind, QPointF position);
    void view_ConstructComponentInstance(Node* parent, Node* definition, QPointF position);
    void view_ClearHistoryStates();


 // new signals for docks
	void setGoToMenuActions(QString action, bool);
    void view_enableDocks(bool enable);
    void view_nodeConstructed(NodeItem* nodeItem);
    void view_nodeSelected(Node* node);
    void view_nodeDestructed();
    void view_edgeConstructed();
    void view_edgeDestructed();

public slots:
    void constructEventPortDelegate(Node *assm, Node *eventPortInstance);
    void constructNode(QString nodeKind, int sender);
    void setDefaultAspects();
    void setEnabled(bool);

    void showDialogMessage(MESSAGE_TYPE type, QString message, GraphML* item = 0);


    void view_SelectModel();


    void duplicate();
    void copy();
    void cut();
    void paste(QString xmlData);
    void selectAll();

    void appendToSelection(GraphMLItem* item);
    void moveSelection(QPointF delta);
    void clearSelection();


    void view_ClearHistory();
    void toolbarClosed();
    void sortAspects();
    void resetModel();


    void setAutoCenterViewAspects(bool center);
    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);
    void constructGUIItem(GraphML* item);
    void destructGUIItem(QString ID);


    //void removeNodeItem(NodeItem* item);
    void centerItem(GraphMLItem* item);
    void clearView();

    void setRubberBandMode(bool On);


    void showToolbar(QPoint position);
    void sortModel();

    void view_SelectGraphML(GraphML* graphML, bool setSelected=true);
    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(GraphML* graphML);

    void view_SortNode(Node* node);
    void view_SetOpacity(GraphML* graphML, qreal opacity);


    void constructNewView(Node* centeredOn);
    

    void sortEntireModel();
    void sortNode(Node* node, Node* topMostNode = 0);

    void fitToScreen();


    //void centerAspects();
    void centerAspects();


    void goToDefinition(Node* node);
    void goToImplementation(Node* node);
    void goToInstance(Node *node);

    void deleteSelection();
    //void view_ConstructNode(QString nodeKind);
    void constructEdge(Node* src, Node* dst);
    void constructComponentInstance(Node *assm, Node *defn, int sender);

    void componentInstanceConstructed(Node* node);

    void fitInSceneRect(GraphMLItem *item);

    void setAspects(QStringList aspects);

private:

    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);


    void setGraphMLItemAsSelected(GraphMLItem* item);

    NewController* getController();
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    bool isSubView();
    bool isMainView();
    bool isAspectVisible(QString aspect);
    void addAspect(QString aspect);
    void removeAspect(QString aspect);

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

    bool isItemsAncestorSelected(GraphMLItem* selectedItem);
    void unsetItemsDescendants(GraphMLItem* selectedItem);

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

    void showAllAspects();


   
    ToolbarWidget* toolbar;


    bool IS_SUB_VIEW;

    QList<NodeView*> subViews;

    bool AUTO_CENTER_ASPECTS;

    bool toolbarJustClosed;



    //Selection Lists
    QStringList selectedIDs;
    QStringList defaultAspects;
protected:
    bool viewportEvent(QEvent *);

};

#endif // NODEVIEW_H
