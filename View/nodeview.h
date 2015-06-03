#ifndef NODEVIEW_H//
#define NODEVIEW_H

#include "../Controller/controller.h"
#include "GraphicsItems/nodeitem.h"
#include "Dock/dockscrollarea.h"

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
    enum ALIGN{NONE, HORIZONTAL, VERTICAL};
    NodeView(bool subView = false, QWidget *parent = 0);
    ~NodeView();

    //Set Controller
    void setController(NewController* controller);
    void disconnectController();

    //Get the Selected Node.
    Node* getSelectedNode();
    NodeItem* getSelectedNodeItem();

    void setParentNodeView(NodeView *n);
    void removeSubView(NodeView* subView);

    QList<GraphMLItem *> search(QString searchString, GraphMLItem::GUI_KIND kind);

    // this is used by the parts dock
    QStringList getConstructableNodeKinds();

    void appendToSelection(Node* node);

    QPointF getPreviousViewCenterPoint();
    void updateViewCenterPoint();
    void recenterView();

    QStringList getAllAspects();
    void viewDeploymentAspect();

protected:
    //Mouse Handling Methods
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    void mouseDoubleClickEvent(QMouseEvent *event);

    //Keyboard Handling Methods
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    bool viewportEvent(QEvent *);

signals:
    void view_ModelSizeChanged();
    void view_NodeDeleted(QString childID, QString parentID="");
    void view_EdgeDeleted(QString srcID, QString dstID);

    void view_SetClipboardBuffer(QString);
    void view_UndoListChanged(QStringList);
    void view_RedoListChanged(QStringList);
    void view_ProjectNameChanged(QString);
    void view_ExportProject();
    void view_ExportedProject(QString data);
    void view_ImportProjects(QStringList data);

    void view_ExportedSnippet(QString parentName, QString snippetXMLData);
    void view_ExportSnippet(QStringList selection);
    void view_ImportedSnippet(QStringList selection, QString fileName, QString fileData);

    void view_ImportSnippet(QString nodeKind);


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

    // signals for the docks
    void view_nodeConstructed(NodeItem* nodeItem);
    void view_nodeDestructed(NodeItem* nodeItem);
    void view_nodeSelected();
    void view_edgeConstructed();
    void view_edgeDestructed();

    // signals for MEDEA menu
    void view_updateMenuActionEnabled(QString action, bool enable);
    void view_showWindowToolbar();

    void view_toggleGridLines(bool on);

    void view_updateProgressStatus(int value, QString status);
    void view_displayNotification(QString notification, int seqNum = 0, int totalNum = 1);

    void view_nodeItemLockMenuClosed(NodeItem* nodeItem);

public slots:
    void loadJenkinsNodes(QString fileData);
    void exportSnippet();
    void importSnippet(QString fileName, QString fileData);
    void minimapPressed(QMouseEvent* event);

    void minimapReleased(QMouseEvent* event);
    void minimapMoved(QMouseEvent* event);

    void scrollEvent(int delta);
    void triggerAction(QString action);
    void minimapPan(QPointF delta);
    void alignSelectionHorizontally();
    void alignSelectionVertically();

    void snapSelectionToGrid();
    void snapChildrenToGrid();

    void setDefaultAspects();
    void setEnabled(bool);

    void showDialogMessage(MESSAGE_TYPE type, QString title, QString message, GraphML* item = 0, bool centralizeItem = false);

    void view_ClearHistory();
    void clearView();

    void resetModel();
    void clearModel();
    void selectModel();
    void sortModel();


    void duplicate();
    void copy();
    void cut();
    void paste(QString xmlData);
    void selectAll();

    void undo();
    void redo();

    void appendToSelection(GraphMLItem* item);
    void removeFromSelection(GraphMLItem* item);
    void moveSelection(QPointF delta);
    void resizeSelection(QSizeF delta);
    void moveFinished();
    void resizeFinished();
    void clearSelection(bool updateTable = true, bool updateDocks = true);

    void toggleGridLines(bool gridOn);
    void autoCenterAspects(bool center);
    void selectNodeOnConstruction(bool select);
    void showManagementComponents(bool show);
    void toggleZoomAnchor(bool underMouse);

    void setRubberBandMode(bool On);
    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);

    void constructGUIItem(GraphML* item);
    void destructGUIItem(QString ID);

    void showToolbar(QPoint position = QPoint());
    void toolbarClosed();

    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(GraphML* graphML);

    void sortEntireModel();
    void sortNode(Node* node, Node* topMostNode = 0);

    void centerAspect(QString aspect);
    void setAspects(QStringList aspects, bool centerViewAspects = true);
    void fitToScreen(QList<NodeItem*> itemsToCenter = QList<NodeItem*>(), double padding = 0, bool addToMap = true);

    void centerOnItem(GraphMLItem* item = 0);
    void centerItem(GraphMLItem* item);

    void goToDefinition(Node* node = 0);
    void goToImplementation(Node* node = 0);
    void goToInstance(Node *instance = 0);

    void deleteSelection();
    void constructNode(QString nodeKind, int sender);
    void constructEdge(Node* src, Node* dst, bool triggerAction=true);
    void constructConnectedNode(Node *parentNode, Node *node, QString kind, int sender);
    void constructNewView(Node* centeredOn = 0);
    void showConnectedNodes();

    void componentInstanceConstructed(Node* node);

    void destructEdge(Edge* edge, bool addAction=true);

    void editableItemHasFocus(bool hasFocus);

    void selectAndCenter(GraphMLItem* item = 0, QString ID = "");

    void showNodeItemLockMenu(NodeItem* nodeItem);
    void nodeItemLockMenuClosed(NodeItem *nodeItem);

    void keepSelectionFullyVisible(GraphMLItem* item, bool sizeChanged = false);
    void moveViewBack();
    void moveViewForward();

private:
    void enableClipboardActions(QStringList IDs );
    void alignSelectionOnGrid(ALIGN alignment = NONE);
    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);

    void setGraphMLItemSelected(GraphMLItem* item, bool setSelected);

    NewController* getController();
    void connectGraphMLItemToController(GraphMLItem* GUIItem, GraphML* graphML);

    bool isSubView();
    bool isMainView();

    void addAspect(QString aspect);
    void removeAspect(QString aspect);

 	QStringList getAdoptableNodeList(Node* node=0);
    QList<Node*> getConnectableNodes(Node* node=0);

    QList<Node*> getFiles();
    QList<Node*> getComponents();

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    bool isItemsAncestorSelected(GraphMLItem* selectedItem);
    void unsetItemsDescendants(GraphMLItem* selectedItem);

    NodeItem* getNodeItemFromNode(Node* node);
    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    EdgeItem* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromHash(QString ID);



    void nodeConstructed_signalUpdates(NodeItem* nodeItem);
    void nodeDestructed_signalUpdates(NodeItem *nodeItem);
    void nodeSelected_signalUpdates(Node *node);
    void edgeConstructed_signalUpdates(Edge* edge);
    void edgeDestructed_signalUpdates(Edge* edge, QString ID = "");

    void updateActionsEnabled(Node* selectedNode = 0);

    Node* hasDefinition(Node* node);
    Node* hasImplementation(Node* node);

    QRectF getVisibleRect();
    void centerRect(QRectF rect, double padding = 0, bool addToMap = true, double sizeRatio = 1);

    void centerViewOn(QPointF center);
    void recenterView(QPointF modelPos, QRectF centeredRect, bool addToMap = false);

    NodeItem* getModelItem();
    QPointF getModelScenePos();

    void adjustModelPosition(QPointF delta);

    int getMapSize();
    void addToMaps(QPointF modelPos, QRectF centeredRect);
    void clearMaps(int fromKey = 0);

    QList<NodeItem*> getNodeItemsList();
    QList<EdgeItem*> getEdgeItemsList();

    bool allowedFocus(QWidget* widget);



    NewController* controller;

    QString centralizedItemID;
    bool CENTRALIZED_ON_ITEM;

    QHash<QString, GraphMLItem*> guiItems;
    //Contains QString ID and either Node/Edge for kind
    QHash<QString, QString> noGuiIDHash;

    NodeView* parentNodeView;

    QStringList allAspects;

    QPointF toolbarPosition;

    QStringList currentAspects;
    QRubberBand* rubberBand;
    QString NodeType;
    qreal totalScaleFactor;

    bool MINIMAP_EVENT;
    bool RUBBERBAND_MODE;
    bool drawingRubberBand;
    QPoint rubberBandOrigin;
    bool once;

    bool CONTROL_DOWN;
    bool SHIFT_DOWN;

    QStringList nonDrawnItemKinds;

    ToolbarWidget* toolbar;


    bool IS_SUB_VIEW;

    QList<NodeView*> subViews;

    QStack<QPointF> viewCenterPointStack;
    QStack<QPointF> viewModelPositions;
    QStack<QRectF> viewCenteredRectangles;

    bool IS_RESIZING;
    bool IS_MOVING;
    bool managementComponentVisible;
    bool AUTO_CENTER_ASPECTS;
    bool GRID_LINES_ON;
    bool SELECT_ON_CONSTRUCTION;

    bool constructedFromImport;
    bool importFromJenkins;
    bool toolbarJustClosed;
    bool editingNodeItemLabel;

    bool pasting;
    bool panning;

    //Selection Lists
    QStringList selectedIDs;
    QStringList defaultAspects;

    QPointF centerPoint;
    QPointF prevCenterPoint;

    int initialRect;
    bool viewMovedBackForward;

    int notificationNumber;
    int numberOfNotifications;

    int currentMapKey;
    QMap<int, QPointF> modelPositions;
    QMap<int, QRectF> centeredRects;

    QMenu* prevLockMenuOpened;
};

#endif // NODEVIEW_H
