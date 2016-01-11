#ifndef NODEVIEW_H
#define NODEVIEW_H

#include "../Controller/controller.h"
#include "GraphicsItems/graphmlitem.h"
#include "GraphicsItems/nodeitem.h"
#include "GraphicsItems/edgeitem.h"
#include "GraphicsItems/entityitem.h"
#include "GraphicsItems/aspectitem.h"
#include "GraphicsItems/modelitem.h"

#include "../Controller/edgeadapter.h"
#include "../Controller/nodeadapter.h"
#include "Dock/dockscrollarea.h"

#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QRubberBand>
#include <QMutex>
//#include <QSoundEffect>

#include "../enumerations.h"


class ToolbarWidget;


/**
 * @brief The NodeView class
 */
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

    void setApplicationDirectory(QString appDir);
    void setCursor(QCursor cursor);
    void unsetCursor();

    void destroySubViews();

    //Set Controller

    void setController(NewController* controller);

    void disconnectController();

    VIEW_STATE getViewState();

    qreal getCurrentZoom();

    bool hasModel();
    bool projectRequiresSaving();

    QString getProjectAsGraphML();
    QString getSelectionAsGraphMLSnippet();
    QString getProjectFileName();

    QString getImportableSnippetKind();

    void scrollContentsBy(int dx, int dy);

    //Get the Selected Node.
    //Node* getSelectedNode();
    int getSelectedNodeID();
    int getSelectedID();

    EntityItem* getSelectedEntityItem();
    NodeItem* getSelectedNodeItem();
    GraphMLItem* getSelectedGraphMLItem();

    QList<GraphMLItem*> getSelectedItems();
    QList<QPair<QString, QString> > getFunctionsList();
    //QList<EntityItem*> getSelectedEntityItems();

    QList<int> getSelectedNodeIDs();

    void appendToSelection(Node* node);

    void setParentNodeView(NodeView *n);
    void removeSubView(NodeView* subView);

    QList<GraphMLItem*> search(QString searchString, QStringList viewAspects = QStringList(), QStringList entityKinds = QStringList(), QStringList dataKeys = QStringList("label"), GraphMLItem::GUI_KIND kind = GraphMLItem::ENTITY_ITEM);
    void searchSuggestionsRequested(QString searchString, QStringList viewAspects, QStringList entityKinds, QStringList dataKeys);

    // this is used by the parts dock
    QStringList getGUIConstructableNodeKinds();
    QStringList getAllNodeKinds();


    QPointF getPreviousViewCenterPoint();
    bool managementComponentsShown();
    void updateViewCenterPoint();
    void recenterView();

    void visibleViewRectChanged(QRect rect);
    QRect getVisibleViewRect();

    void viewDeploymentAspect();


    QPixmap getImage(QString alias, QString imageName);
    EntityItem* getImplementation(int ID);
    QList<EntityItem*> getInstances(int ID);
    EntityItem* getDefinition(int ID);
    int getDefinitionID(int ID);
    int getImplementationID(int ID);

    EntityItem* getAggregate(int ID);
    EntityItem* getDeployedNode(int ID);

    bool isSubView();
    bool isMainView();
    bool isTerminating();
    bool isNodeKindDeployable(QString nodeKind);

    QString getProjectFilePath();


    void setVisible(bool visible);


    void aspectGraphicsChanged();
    void setupTheme(VIEW_THEME theme = VT_NORMAL_THEME);
    VIEW_THEME getTheme();

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


    void setReadOnlyMode(bool readOnly);

    bool viewportEvent(QEvent *);

private:
    void resetViewState();
    void sortSelection(bool recurse=false);
    void expandSelection(bool expand);
    void setupSoundEffects();

private slots:
    void controllerDestroyed();
    void settingChanged(QString groupName, QString keyName, QVariant value);
    void modelReady();


    void hardwareClusterMenuClicked(int viewMode);

    void actionFinished();

signals:
    void view_ProjectFileChanged(QString);
    void view_ProjectNameChanged(QString);

    void view_LoadSettings();
    void view_searchFinished(QStringList searchResult);
    void view_themeChanged(VIEW_THEME theme);

    void view_ClearSubViewAttributeTable();
    void view_ModelReady();
    void view_ModelDisconnected();
    void view_EnableDebugLogging(bool enable, QString applicationPath="");

	void view_HardwareDockEnabled(bool enabled);
    void view_OpenHardwareDock();
    void view_ModelSizeChanged();
    void view_Clear();
    void view_ProjectCleared();

    void view_SetClipboardBuffer(QString);

    void view_ExportProject();
    void view_ExportedProject(QString data);
    void view_ImportProjects(QStringList data);

    void view_OpenProject(QString fileName, QString fileData);

    void view_SaveProject(QString filePath);
    void view_SavedProject(QString filePath, QString fileData);
    void view_ProjectSaved(bool success, QString filePath);


    void view_ExportedSnippet(QString parentName, QString snippetXMLData);
    void view_ExportSnippet(QList<int> selection);
    void view_ImportedSnippet(QList<int> selection, QString fileName, QString fileData);

    void view_ImportSnippet(QString nodeKind);
    void view_ExportSnippet(QString nodeKind);

    void view_ShowCPPForComponent(QString componentName);


    void view_Copy(QList<int> IDs);
    void view_Cut(QList<int> IDs);
    void view_SetReadOnly(QList<int> IDs, bool readOnly);
    void view_Paste(int ID, QString xmlData);
    void view_Delete(QList<int> IDs);
    void view_Replicate(QList<int> IDs);
    void view_Undo();
    void view_Redo();

    void view_SetData(int, QString, QVariant);
    void view_ConstructData(GraphML*, QString);
    void view_DestructData(GraphML*, QString);

    void view_SetAttributeModel(AttributeTableModel* model);

    void view_ViewportRectChanged(QRectF);
    void view_ZoomChanged(qreal);
    void view_SceneRectChanged(QRectF);


    //SIGNALS for the Controller
    void view_TriggerAction(QString action);
    void view_ConstructWorkerProcessNode(int parentID, QString className, QString functionName, QPointF position);
    void view_ConstructNode(int parentID, QString nodeKind, QPointF position);
    void view_ConstructEdge(int srcID, int dstID, bool reverseOkay = false);
    void view_DestructEdge(int srcID, int dstID);
    void view_ConstructConnectedNode(int parentID, int relativeID, QString nodeKind, QPointF position);

    void view_constructDestructEdges(QList<int> srcIDs, int dstID);

    void view_ClearHistoryStates();

    void view_highlightAspectButton(VIEW_ASPECT aspect);

    // signals for the docks
    void view_nodeSelected();
    void view_nodeConstructed(NodeItem* EntityItem);
    void view_nodeDeleted(int ID, int parentID = -1);
    void view_edgeConstructed();
    void view_edgeDeleted(int srcID = -1, int dstID = -1);

    // signals for MEDEA menu
    void view_updateMenuActionEnabled(QString action, bool enable);
    void view_showWindowToolbar();

    void view_toggleGridLines(bool on);

    void view_toggleAspect(VIEW_ASPECT, bool);

    void view_updateProgressStatus(int percent, QString action="");
    void view_displayNotification(QString notification, int seqNum = 0, int totalNum = 1);

    void view_EntityItemLockMenuClosed(EntityItem* EntityItem);
    void view_QuestionAnswered(bool answer);

    void view_ProjectRequiresSaving(bool requiresSave);
public slots:
    QPointF getCenterOfScreenScenePos(QPoint mousePosition = QPoint());
    void canUndo(bool okay);
    void canRedo(bool okay);
    void dropDownChangedValue(QString value);
    void showDropDown(GraphMLItem *item, QLineF dropDownPosition, QString keyName, QString currentValue);
    void setStateResizing();
    void setStateMove();
    void setStateMoving();
    void setStateConnect();
    void setStateSelected();
    void clearState();
    void request_ImportSnippet();
    void request_ExportSnippet();

    void hardwareDockOpened(bool opened);
    void showQuestion(MESSAGE_TYPE type, QString title, QString message, int ID);
    void setAttributeModel(GraphMLItem* item=0, bool tellSubView=false);
    void importProjects(QStringList xmlDataList);

    void openProject(QString fileName, QString fileData);
    void saveProject(QString filePath);


    void loadJenkinsNodes(QString fileData);
    void importSnippet(QString fileName, QString fileData);

    void scrollEvent(int delta, QPoint mouseCenter = QPoint());//, QPointF mouseCenter);
    void triggerAction(QString action);
    void minimapPan();
    void minimapPanning(QPointF delta);
    void minimapPanned();
    void minimapScrolled(int delta);
    void alignSelectionHorizontally();
    void alignSelectionVertically();


    void setEnabled(bool);

    void showMessage(MESSAGE_TYPE type, QString title, QString message, int ID=-1, bool centralizeItem = false);

    void view_ClearHistory();


    void resetModel(bool addAction = true);
    void clearModel();
    void selectModel();


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
    void resizeSelection(int ID, QSizeF delta);
    void moveFinished();
    void resizeFinished(int ID);
    void clearSelection(bool updateTable = true, bool updateDocks = true);

    void toggleGridLines(bool gridOn);
    void autoCenterAspects(bool center);
    void selectNodeOnConstruction(bool select);
    void showManagementComponents(bool show);
    void showLocalNode(bool show);
    void toggleZoomAnchor(bool underMouse);




    void selectedInRubberBand(QPointF fromScenePoint, QPointF toScenePoint);

    void constructEntityItem(EntityAdapter* item);
    void destructEntityItem(EntityAdapter* item);
    void constructNodeItem(NodeAdapter* node);
    void constructEdgeItem(EdgeAdapter* node);
    void destructGUIItem(int ID, GraphML::GRAPHML_KIND kind);

    void showToolbar(QPoint position = QPoint());
    void toolbarClosed();

    void view_CenterGraphML(GraphML* graphML);
    void view_LockCenteredGraphML(int ID);

    void sort();
    void expand(bool expand);

    void sortEntireModel();



    void centerAspect(VIEW_ASPECT aspect);
    void toggleAspect(VIEW_ASPECT aspect, bool on);
    void fitToScreen(QList<GraphMLItem*> itemsToCenter = QList<GraphMLItem*>(), double padding = 0, bool addToMap = true);

    void centerOnItem(GraphMLItem* item = 0);
    void centerItem(GraphMLItem* item=0);
    void centralizedItemMoved();

    void centerItem(int ID);
    void centerDefinition(int ID = -1);
    void centerImplementation(int ID = -1);
    void centerInstance(int instanceID);


    void deleteSelection();

    void constructWorkerProcessNode(QString workerName, QString operationName, int sender);
    void constructNode(QString nodeKind, int sender);
    void constructEdge(int srcID, int dstID);
    void destructEdge(int srcID, int dstID, bool triggerAction=true);

    void constructDestructEdges(QList<int> srcIDs, int dstID);

    void deleteFromIDs(QList<int> IDs);
    void constructConnectedNode(int parentID, int dstID, QString kind, int sender=0);

    void constructNewView(int viewID = -1);

    void highlightOnHover(int nodeID = -1);

    QList<NodeItem *> getEntityItemsOfKind(QString kind, int ID=-1, int depth=-1);
    void showConnectedNodes();



    void editableItemHasFocus(bool hasFocus);

    void selectAndCenterItem(int ID = -1);


    void keepSelectionFullyVisible(GraphMLItem* item, bool sizeChanged = false);
    void moveViewBack();
    void moveViewForward();

    void highlightDeployment(bool clear = false);

    void setEventFromEdgeItem();

    void showHardwareClusterChildrenViewMenu(EntityItem* EntityItem);
    void hardwareClusterChildrenViewMenuClosed(EntityItem* EntityItem);

    void itemEntered(int ID, bool enter);


private:
    AspectItem* getAspectItem(VIEW_ASPECT aspect);
    void setConnectMode(bool on);
    void setRubberBandMode(bool On);
    void setState(VIEW_STATE newState);

    void handleSelection(GraphMLItem* item, bool setSelected, bool controlDown);
    void transition();
    void selectJenkinsImportedNodes();
    void _deleteFromIDs(QList<int> IDs);
    void updateActionsEnabledStates();
    void alignSelectionOnGrid(ALIGN alignment = NONE);
    void view_ConstructEdgeGUI(Edge* edge);
    void setGraphMLItemSelected(GraphMLItem* item, bool setSelected);
    void connectGraphMLItemToController(GraphMLItem* GUIItem);
    void storeGraphMLItemInHash(GraphMLItem* item);
    void unsetItemsDescendants(GraphMLItem* selectedItem);
    void nodeConstructed_signalUpdates(NodeItem *EntityItem);
    void nodeSelected_signalUpdates();
    void edgeConstructed_signalUpdates();
    void centerRect(QRectF rect, double padding = 0, bool addToMap = true);
    void centerViewOn(QPointF center);
    void recenterView(QPointF modelPos, QRectF centeredRect, bool addToMap = false);
    void adjustModelPosition(QPointF delta);
    void adjustModelPosition(QPoint delta);
    void addToMaps(QPointF modelPos, QRectF centeredRect);
    void clearMaps(int fromKey = 0);

    void enforceItemAspectOn(int ID);
    void enforceEntityItemVisible(int ID);

    bool allowedFocus(QWidget* widget);
    bool isEditableDataDropDown(EntityItem* node);
    bool isNodeVisuallyConnectable(NodeAdapter *node);
    bool onlyHardwareClustersSelected();
    bool isItemsAncestorSelected(GraphMLItem* selectedItem);

    void removeGraphMLItemFromHash(int ID);

    NewController* getController();
    QRectF getVisibleRect();
    ModelItem* getModelItem();
    QPointF getModelScenePos();


    void setNoModelTextVisible(bool hasModel);

    int getMapSize();



    QStringList getAdoptableNodeList(int ID);
    QList<int> getConnectableNodes(int ID);
    QList<NodeItem*> getConnectableNodeItems(int ID);
    QList<NodeItem*> getConnectableNodeItems(QList<int> IDs = QList<int>());
    QList<NodeItem*> getNodeInstances(int ID);
    QList<EntityItem *> getHardwareList();
    QPair<QString, bool> getEditableDataKeyName(GraphMLItem* node);
    QPair<QString, bool> getStatusDataKeyName(GraphMLItem* node);

    QList<Node*> getFiles();
    QList<Node*> getComponents();
    QList<Node*> getBlackBoxes();


    EntityItem* getSharedEntityItemParent(EntityItem* src, EntityItem* dst);
    EntityItem* getEntityItemFromNode(Node* node);
    EntityItem* getEntityItemFromID(int ID);
    EntityItem* getEntityItemFromGraphMLItem(GraphMLItem* item);

    EdgeItem* getEdgeItemFromGraphMLItem(GraphMLItem* item);
    GraphMLItem *getGraphMLItemFromGraphML(GraphML* item);

    GraphMLItem* getGraphMLItemFromScreenPos(QPoint pos);

    GraphMLItem* getGraphMLItemFromID(int ID);
    NodeItem* getNodeItemFromID(int ID);

    QString getData(int ID, QString key);
    QList<EntityItem*> getEntityItemsList();
    QList<NodeItem*> getNodeItemsList();
    QList<EdgeItem*> getEdgeItemsList();




    NodeView* parentNodeView;
    ToolbarWidget* toolbar;
    NewController* controller;
    QRubberBand* rubberBand;
    QMenu* prevLockMenuOpened;
    QComboBox* comboBox;
    QGraphicsLineItem* connectLine;



    QMutex viewMutex;

    QStringList nonDrawnItemKinds;
    QList<NODE_CLASS> nonDrawnNodeClasses;
    QList<EDGE_CLASS> nonDrawnEdgeClasses;


    QPoint panningOrigin;
    QPoint previousPanPos;
    QPoint rubberBandOrigin;

    QPointF toolbarPosition;
    QPointF centerPoint;
    QPointF prevCenterPoint;
    //QPointF panningSceneOrigin;
    QPoint testSceneOrigin;

    int centralizedItemID;
    int prevSelectedNodeID;
    int prevHighlightedID;
    int currentTableID;

    int initialRect;
    int notificationNumber;
    int numberOfNotifications;
    int currentMapKey;

    VIEW_STATE viewState;
    VIEW_THEME currentTheme;

    bool CENTRALIZED_ON_ITEM;
    bool MINIMAP_EVENT;
    bool CONTROL_DOWN;
    bool SHIFT_DOWN;
    bool AUTO_CENTER_ASPECTS;
    bool GRID_LINES_ON;
    bool SELECT_AFTER_CONSTRUCTION;
    bool IS_SUB_VIEW;
    bool IS_DESTRUCTING;

    bool updateDeployment;
    bool localNodeVisible;
    bool managementComponentVisible;
    bool toolbarDockConstruction;
    bool constructedFromImport;
    bool importFromJenkins;
    bool toolbarJustClosed;
    bool editingEntityItemLabel;
    bool viewMovedBackForward;
    bool pasting;
    bool hardwareDockOpen;
    bool clearingModel;
    bool eventFromEdgeItem;
    bool wasPanning;

    qreal zoomCurrent;
    //Selection Lists
    QList<int> selectedIDs;
    QList<int> highlightedIDs;
    QList<NodeView*> subViews;

    QStack<QPointF> viewCenterPointStack;
    QStack<QPointF> viewModelPositions;
    QStack<QRectF> viewCenteredRectangles;

    QHash<int, QPointF> modelPositions;
    QHash<int, QRectF> centeredRects;

    QHash<int, int> definitionIDs;

    QHash<QString, QPixmap> imageLookup;
    QHash<int, GraphMLItem*> guiItems;
    QHash<int, QString> noGuiIDHash;

    QHash<int, NODE_CLASS> noGUINodeIDHash;

    QRect visibleViewRect;

    bool showSearchSuggestions;
    bool showConnectLine;
    int prevHighlightedFromToolbarID;

    bool ZOOM_UNDER_MOUSE;


    QGraphicsTextItem* backgroundText;
	//QSoundEffect* clickSound;

    QString applicationDirectory;
protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dropEvent(QDropEvent* );
};

#endif // NODEVIEW_H
