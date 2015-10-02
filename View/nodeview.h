#ifndef NODEVIEW_H
#define NODEVIEW_H

#include "../Controller/controller.h"
#include "GraphicsItems/nodeitem.h"
#include "Dock/dockscrollarea.h"

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QRubberBand>
#include <QMutex>


class ToolbarWidget;

class NodeView : public QGraphicsView
{
    friend class DockScrollArea;
    friend class ToolbarWidget;
    friend class NewController;
    Q_OBJECT

public:
    enum VIEW_STATE{VS_NONE, VS_SELECTED, VS_RUBBERBAND, VS_RUBBERBANDING, VS_CONNECT, VS_MOVING, VS_RESIZING, VS_CONNECTING, VS_PAN, VS_PANNING};

    enum ALIGN{NONE, HORIZONTAL, VERTICAL};
    NodeView(bool subView = false, QWidget *parent = 0);
    ~NodeView();

    void setCursor(QCursor cursor);
    void unsetCursor();

    void destroySubViews();
    //Set Controller
    void setController(NewController* controller);
    void disconnectController();

    VIEW_STATE getViewState();


    //Get the Selected Node.
    Node* getSelectedNode();
    QString getSelectedNodeID();
    NodeItem* getSelectedNodeItem();

    QList<NodeItem*> getSelectedNodeItems();
    QStringList getSelectedNodeIDs();

    void setParentNodeView(NodeView *n);
    void removeSubView(NodeView* subView);

    QList<GraphMLItem *> search(QString searchString, GraphMLItem::GUI_KIND kind);

    // this is used by the parts dock
    QStringList getGUIConstructableNodeKinds();
    QStringList getAllNodeKinds();

    void appendToSelection(Node* node);

    QPointF getPreviousViewCenterPoint();
    bool managementComponentsShown();
    void updateViewCenterPoint();
    void recenterView();

    QStringList getAllAspects();
    void viewDeploymentAspect();


    QPixmap getImage(QString alias, QString imageName);
    NodeItem* getImplementation(QString ID);
    QList<NodeItem*> getInstances(QString ID);
    NodeItem* getDefinition(QString ID);
    QString getDefinitionID(QString ID);
	QString getImplementationID(QString ID);

    NodeItem* getAggregate(QString ID);
    NodeItem* getDeployedNode(QString ID);

    bool isSubView();
    bool isTerminating();
    bool isNodeKindDeployable(QString nodeKind);


    void aspectGraphicsChanged();


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

private:
    void sortSelection();
private slots:
    void hardwareClusterMenuClicked(int viewMode);

    void actionFinished();


signals:
    void view_ClearSubViewAttributeTable();
    void view_ModelReady();
    void view_EnableDebugLogging(bool enable, QString applicationPath="");

	void view_HardwareDockEnabled(bool enabled);
    void view_OpenHardwareDock();
    void view_ModelSizeChanged();
    void view_Clear();
    void view_ProjectCleared();

    void view_SetClipboardBuffer(QString);

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
    void view_Paste(QString ID, QString xmlData);
    void view_Delete(QStringList IDs);
    void view_Replicate(QStringList IDs);
    void view_Undo();
    void view_Redo();

    void view_SetGraphMLData(QString, QString, QString);
    void view_ConstructGraphMLData(GraphML*, QString);
    void view_DestructGraphMLData(GraphML*, QString);

    void view_SetAttributeModel(AttributeTableModel* model);

    void view_ViewportRectChanged(QRectF);
    void view_ZoomChanged(qreal);
    void view_SceneRectChanged(QRectF);

    void view_AspectsChanged(QStringList aspects);
    void view_GUIAspectChanged(QStringList aspects);

    //SIGNALS for the Controller
    void view_TriggerAction(QString action);
    void view_ConstructNode(QString parentID, QString nodeKind, QPointF position);
    void view_ConstructEdge(QString srcID, QString dstID);
    void view_DestructEdge(QString srcID, QString dstID);
    void view_ConstructConnectedNode(QString parentID, QString relativeID, QString nodeKind, QPointF position);

    void view_constructDestructEdges(QStringList srcIDs, QString dstID);

    void view_ClearHistoryStates();

    void view_highlightAspectButton(QString aspect = "");

    // signals for the docks
    void view_nodeSelected();
    void view_nodeConstructed(NodeItem* nodeItem);    
    void view_nodeDeleted(QString ID, QString parentID = "");
    void view_edgeConstructed();
    void view_edgeDeleted(QString srcID, QString dstID);

    // signals for MEDEA menu
    void view_updateMenuActionEnabled(QString action, bool enable);
    void view_showWindowToolbar();

    void view_toggleGridLines(bool on);

    void view_updateProgressStatus(int percent, QString action="");
    void view_displayNotification(QString notification, int seqNum = 0, int totalNum = 1);

    void view_nodeItemLockMenuClosed(NodeItem* nodeItem);
    void view_QuestionAnswered(bool answer);

public slots:

    void setStateResizing();
    void setStateMove();
    void setStateMoving();
    void setStateConnect();
    void setStateSelected();
    void request_ImportSnippet();
    void hardwareDockOpened(bool opened);
    void showQuestion(MESSAGE_TYPE type, QString title, QString message, QString ID);
    void setAttributeModel(GraphMLItem* item=0, bool tellSubView=false);
    void importProjects(QStringList xmlDataList);

    void loadJenkinsNodes(QString fileData);
    void exportSnippet();
    void exportProject();
    void importSnippet(QString fileName, QString fileData);

    void scrollEvent(int delta);
    void triggerAction(QString action);
    void minimapPan();
    void minimapPanning(QPointF delta);
    void minimapPanned();
    void minimapScrolled(int delta);
    void alignSelectionHorizontally();
    void alignSelectionVertically();

    void snapSelectionToGrid();
    void snapChildrenToGrid();

    void setDefaultAspects();
    void setEnabled(bool);

    void showMessage(MESSAGE_TYPE type, QString title, QString message, QString ID="", bool centralizeItem = false);

    void view_ClearHistory();
    void clearView();


    void resetModel(bool addAction = true);
    void clearModel();
    void selectModel();
    void sortModel();


    void replicate();
    void copy();
    void cut();
    void paste(QString xmlData);
    void selectAll();

    void undo();
    void redo();

    void appendToSelection(GraphMLItem* item, bool updateActions=true);
    void removeFromSelection(GraphMLItem* item);
    void moveSelection(QPointF delta);
    void resizeSelection(QString ID, QSizeF delta);
    void moveFinished();
    void resizeFinished(QString ID);
    void clearSelection(bool updateTable = true, bool updateDocks = true);

    void toggleGridLines(bool gridOn);
    void autoCenterAspects(bool center);
    void selectNodeOnConstruction(bool select);
    void showManagementComponents(bool show);
    void showLocalNode(bool show);
    void toggleZoomAnchor(bool underMouse);

    void setConnectModeFromToolbar(bool on, QList<NodeItem*> legalNodeItems = QList<NodeItem*>());

    void setConnectMode(bool on);
    void setRubberBandMode(bool On);
    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);

    void constructGUIItem(GraphML* item);
    void destructGUIItem(QString ID, GraphML::KIND kind);

    void showToolbar(QPoint position = QPoint());
    void toolbarClosed();

    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(QString ID);

    void sort();
    void sortEntireModel();


    void centerAspect(QString aspect);
    void setAspects(QStringList aspects, bool centerViewAspects = true);
    void fitToScreen(QList<NodeItem*> itemsToCenter = QList<NodeItem*>(), double padding = 0, bool addToMap = true);

    void centerOnItem(GraphMLItem* item = 0);
    void centerItem(GraphMLItem* item=0);
    void centralizedItemMoved();

    void centerItem(QString ID);
    void centerDefinition(QString ID = "");
    void centerImplementation(QString ID = "");
    void centerInstance(QString instanceID);


    void deleteSelection();
    void constructNode(QString nodeKind, int sender);
    void constructEdge(QString srcID, QString dstID);
    void destructEdge(QString srcID, QString dstID, bool triggerAction=true);

    void constructDestructEdges(QStringList srcIDs, QString dstID);

    void deleteFromIDs(QStringList IDs);
    void constructConnectedNode(QString parentID, QString dstID, QString kind, int sender=0);

    void constructNewView(QString nodeID);
    void constructNewView(int nodeKindToCenter=0);

    QList<NodeItem*> getNodeItemsOfKind(QString kind, QString ID="", int depth=-1);
    void showConnectedNodes();


    void editableItemHasFocus(bool hasFocus);

    void selectAndCenter(GraphMLItem* item = 0, QString ID = "");


    void keepSelectionFullyVisible(GraphMLItem* item, bool sizeChanged = false);
    void moveViewBack();
    void moveViewForward();

    void highlightDeployment(bool clear = false);

    void setEventFromEdgeItem();

    void showHardwareClusterChildrenViewMenu(NodeItem* nodeItem);
    void hardwareClusterChildrenViewMenuClosed(NodeItem* nodeItem);

    void nodeEntered(QString ID, bool enter);

    void highlightNode(QString nodeID = "");

private:
    bool onlyHardwareClustersSelected();
    void handleSelection(GraphMLItem* item, bool setSelected, bool controlDown);
    void setState(VIEW_STATE newState);
    void transition();
    void selectJenkinsImportedNodes();

    void ensureAspect(QString ID);

    void _deleteFromIDs(QStringList IDs);

    void updateActionsEnabledStates();
    void alignSelectionOnGrid(ALIGN alignment = NONE);
    void view_ConstructNodeGUI(Node* node);
    void view_ConstructEdgeGUI(Edge* edge);


    void setGraphMLItemSelected(GraphMLItem* item, bool setSelected);

    NewController* getController();
    void connectGraphMLItemToController(GraphMLItem* GUIItem);


    bool isMainView();

    void addAspect(QString aspect);

    void removeAspect(QString aspect);

    QStringList getAdoptableNodeList(QString ID);
    QStringList getConnectableNodes(QString ID);
    QList<NodeItem*> getConnectableNodeItems(QString ID);
    QList<NodeItem*> getConnectableNodeItems(QStringList IDs = QStringList());
    QList<NodeItem*> getNodeInstances(QString ID);
	QList<NodeItem*> getHardwareList();

    QList<Node*> getFiles();
    QList<Node*> getComponents();
    QList<Node*> getBlackBoxes();

    void storeGraphMLItemInHash(GraphMLItem* item);
    bool removeGraphMLItemFromHash(QString ID);

    bool isItemsAncestorSelected(GraphMLItem* selectedItem);
    void unsetItemsDescendants(GraphMLItem* selectedItem);

    NodeItem* getNodeItemFromNode(Node* node);
    NodeItem* getNodeItemFromID(QString ID);
    NodeItem* getNodeItemFromGraphMLItem(GraphMLItem* item);
    EdgeItem* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromHash(QString ID);


    GraphMLItem* getGraphMLItemFromScreenPos(QPoint pos);
    void nodeConstructed_signalUpdates(NodeItem* nodeItem);

    void nodeSelected_signalUpdates();
    void edgeConstructed_signalUpdates();
    void edgeDestructed_signalUpdates(Edge* edge, QString ID = "");

    void updateActionsEnabled();

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
    NodeItem* centralizedNodeItem;
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


    bool managementComponentVisible;
    bool localNodeVisible;
    bool AUTO_CENTER_ASPECTS;
    bool GRID_LINES_ON;
    bool SELECT_ON_CONSTRUCTION;

    bool toolbarDockConstruction;
    bool constructedFromImport;
    bool importFromJenkins;
    bool toolbarJustClosed;
    bool editingNodeItemLabel;

    bool pasting;


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

    QHash<QString, QPixmap> imageLookup;

    QMenu* prevLockMenuOpened;

    QString prevSelectedNodeID;
    QString highlightedID;

    QMutex viewMutex;

    bool IS_DESTRUCTING;
    bool updateDeployment;

    QPointF panningSceneOrigin;
    QPoint panningOrigin;

    QString currentTableID;

    bool hardwareDockOpen;
    bool clearingModel;
    bool eventFromEdgeItem;
    bool wasPanning;

    QGraphicsLineItem* connectLine;
    VIEW_STATE viewState;

    NodeItem* prevItemHighlighted;

    bool connectModeFromToolbarOn;
    QStringList connectFromIDs;
    QString connectToID;
    QList<NodeItem*> connectNodeItems;
};

#endif // NODEVIEW_H
