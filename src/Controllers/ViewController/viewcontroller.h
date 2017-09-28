#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "../ActionController/actioncontroller.h"
#include "../SelectionController/selectioncontroller.h"
#include "../SelectionController/selectionhandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"

#include "../ExecutionManager/executionmanager.h"
#include "viewitem.h"
#include "nodeviewitem.h"
#include "edgeviewitem.h"

#include "../../ModelController/kinds.h"
#include "../../ModelController/nodekinds.h"
#include "../../ModelController/edgekinds.h"
#include <QTimer>

enum class MODEL_SEVERITY;
class NotificationManager;
class ModelController;
class NodeView;
class JenkinsManager;
class ContextMenu;

class ViewController : public QObject
{
    Q_OBJECT

public:
    ViewController();
    ~ViewController();


    static QList<ViewItem*> ToViewItemList(QList<NodeViewItem*> &items);
    static QList<ViewItem*> ToViewItemList(QList<EdgeViewItem*> &items);

    void connectModelController(ModelController* c);

    bool isWelcomeScreenShowing();
    ContextMenu* getContextMenu();

    JenkinsManager* getJenkinsManager();
    ExecutionManager* getExecutionManager();
    SelectionController* getSelectionController();
    ActionController* getActionController();

    QList<ViewItem*> getWorkerFunctions();
    QList<ViewItem*> getConstructableNodeDefinitions(NODE_KIND node_kind, EDGE_KIND edge_kind);
    QList<ViewItem*> getValidEdges(EDGE_KIND kind);

    QStringList _getSearchSuggestions();

    QMap<QString, ViewItem*> getSearchResults(QString query, QList<ViewItem*> view_items = {});
    QList<ViewItem*> filterList(QString query, QList<ViewItem*> view_items);
    QList<ViewItem*> filterList(QString query, QList<NodeViewItem*> view_items){
        return filterList(query, ViewController::ToViewItemList(view_items));
    }
    QList<ViewItem*> filterList(QString query, QList<EdgeViewItem*> view_items){
        return filterList(query, ViewController::ToViewItemList(view_items));
    }


    QHash<EDGE_DIRECTION, ViewItem*> getValidEdges2(EDGE_KIND kind);

    ViewDockWidget* constructViewDockWidget(QString label="");
    QSet<NODE_KIND> getAdoptableNodeKinds();
    QList<NodeViewItem*> getNodeKindItems();
    QList<EdgeViewItem*> getEdgeKindItems();
    NodeViewItem* getNodeItem(NODE_KIND kind);

    QList<ViewItem*> getViewItemParents(QList<ViewItem*> items);


    ModelController* getModelController();

    QList<EDGE_KIND> getValidEdgeKindsForSelection();

    QList<EDGE_KIND> getExistingEdgeKindsForSelection();
    QList<ViewItem*> getExistingEdgeEndPointsForSelection(EDGE_KIND kind);

    QList<QVariant> getValidValuesForKey(int ID, QString keyName);
    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isControllerReady();

    bool canUndo();
    bool canRedo();

    QVector<ViewItem*> getOrderedSelection(QList<int> selection);

    void setController(ModelController* c);


    bool isNodeAncestor(int ID, int ID2);
    VIEW_ASPECT getNodeViewAspect(int ID);
    QStringList getEntityKeys(int ID);
    QVariant getEntityDataValue(int ID, QString key_name);
    bool isNodeOfType(int ID, NODE_TYPE type);
    int getNodeParentID(int ID);
signals:
    //TO OTHER VIEWS SIGNALS

    void vc_showWelcomeScreen(bool);
    void vc_JenkinsReady(bool);
    void vc_JavaReady(bool);
    void vc_ReReady(bool);
    void vc_controllerReady(bool);
    void vc_ProjectLoaded(bool);
    void vc_viewItemConstructed(ViewItem* viewItem);
    void vc_viewItemDestructing(int ID, ViewItem* item);
    void vc_showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void vc_gotSearchSuggestions(QStringList suggestions);

    void vc_ActionFinished();

    void vc_editTableCell(int ID, QString keyName);

    void vc_projectClosed();
    
    void vc_SetupModelController(QString file_path);

    void vc_undo();
    void vc_redo();
    void vc_triggerAction(QString);
    void vc_setData(int, QString, QVariant);
    void vc_removeData(int, QString);
    void vc_deleteEntities(QList<int> IDs);
    void vc_cutEntities(QList<int> IDs);
    void vc_copyEntities(QList<int> IDs);
    void vc_paste(QList<int> IDs, QString data);
    void vc_replicateEntities(QList<int> IDs);
    void vc_constructNode(int parentID, NODE_KIND nodeKind, QPointF pos = QPointF());

    void vc_constructEdges(QList<int> sourceIDs, QList<int> dstID, EDGE_KIND edgeKind);

    void vc_constructEdge(QList<int> sourceIDs, int dstID, EDGE_KIND edgeKind);
    void vc_destructEdges(QList<int> sourceIDs, int dstID, EDGE_KIND edgeKind);
    void vc_destructAllEdges(QList<int> sourceIDs, EDGE_KIND edgeKind);
    
    void vc_constructConnectedNode(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos=QPointF());
    void vc_constructWorkerProcess(int parentID, int dstID, QPointF point);
    void vc_importProjects(QStringList fileData);
    void vc_projectSaved(QString filePath);
    void vc_projectPathChanged(QString);
    void vc_answerQuestion(bool);
    
    void mc_showProgress(bool, QString);
    void mc_progressChanged(int);
    void mc_modelReady(bool);
    void mc_projectModified(bool);
    void mc_undoRedoUpdated();
    //TO CONTROLLER SIGNALS

/*
    void vc_setupModel();
    void vc_undo();
    void vc_redo();
    void vc_triggerAction(QString);
    void vc_setData(int, QString, QVariant);
    void vc_removeData(int, QString);
    void vc_deleteEntities(QList<int> IDs);
    void vc_cutEntities(QList<int> IDs);
    void vc_copyEntities(QList<int> IDs);
    void vc_paste(QList<int> IDs, QString data);
    void vc_replicateEntities(QList<int> IDs);

    

    void vc_constructNode(int parentID, NODE_KIND nodeKind, QPointF pos = QPointF());
    void vc_constructEdge(QList<int> sourceIDs, int dstID, EDGE_KIND edgeKind);
    void vc_destructEdges(QList<int> sourceIDs, int dstID, EDGE_KIND edgeKind);

    void vc_constructConnectedNode(int parentID, NODE_KIND nodeKind, int dstID, EDGE_KIND edgeKind, QPointF pos=QPointF());
    void vc_constructWorkerProcess(int parentID, int dstID, QPointF point);


    void vc_importProjects(QStringList fileData);
    void vc_openProject(QString fileName, QString filePath);

    void vc_projectSaved(QString filePath);
    void vc_projectPathChanged(QString);*/

    //void vc_newNotification(QString description, QString iconPath, QString iconName, int entityID, Notification::Severity s, Notification::Type2 t, Notification::Category c);
    //void vc_showNotification(Notification::Severity severity, QString title, QString description, QString iconPath="", QString iconName="", int ID=-1);
    //void vc_showNotification(Notification::Type type, QString title, QString description, QString iconPath="", QString iconName="", int ID=-1);
    void vc_executeJenkinsJob(QString filePath);
    void vc_centerItem(int ID);
    void vc_selectAndCenterConnectedEntities(ViewItem* item);

    void vc_fitToScreen();

    void vc_addProjectToRecentProjects(QString filePath);
    void vc_removeProjectFromRecentProjects(QString filePath);
    

    void vc_getCodeForComponent(QString graphmlPath, QString componentName);
    void vc_validateModel(QString graphmlPath, QString reportPath);
    void vc_modelValidated(QStringList report);
    void vc_launchLocalDeployment(QString graphmlPath);

    //void vc_backgroundProcess(bool inProgress, BACKGROUND_PROCESS process = BACKGROUND_PROCESS::UNKNOWN);

    void vc_importXMEProject(QString xmePath, QString graphmlPath);
    void vc_importXMIProject(QString XMIPath);

    void vc_highlightItem(int ID, bool highlight);

public slots:
    void welcomeScreenToggled(bool visible);
    void highlight(QList<int> ids);


    void modelValidated(QString reportPath);
    void importGraphMLFile(QString graphmlPath);
    void importGraphMLExtract(QString data);
    void showCodeViewer(QString tabName, QString content);


    void jenkinsManager_SettingsValidated(bool success, QString errorString);
    void GotJava(bool java, QString javaVersion);
    void GotRe(bool re, QString string);

    void jenkinsManager_GotJenkinsNodesList(QString graphmlData);


    void getCodeForComponent();
    void validateModel();
    void selectModel();
    void launchLocalDeployment();

    void model_NodeConstructed(int parent_id, int id, NODE_KIND kind);
    void model_EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id);
    void controller_entityDestructed(int ID, GRAPHML_KIND kind);
    void controller_dataChanged(int ID, QString key, QVariant data);
    void controller_dataRemoved(int ID, QString key);

    void setClipboardData(QString data);

    void newProject();
    bool OpenProject();
    bool OpenExistingProject(QString file_path);

    void importProjects();
    void importXMEProject();
    void importXMIProject();
   
    void autoSaveProject();
    void saveProject();
    void saveAsProject();
    void closeProject();
    void closeMEDEA();


    void importIdlFile();

    void generateWorkspace();
    void executeModelLocal();

    void executeJenkinsJob();

    void fitView();
    void fitAllViews();
    void centerSelection();
    void alignSelectionVertical();
    void alignSelectionHorizontal();
    void selectAndCenterConnectedEntities();
    void centerOnID(int ID);

    void showWiki();
    void reportBug();
    void showWikiForSelectedItem();

    void centerImpl();
    void centerDefinition();

    void popupDefinition();
    void popupImpl();
    void popupSelection();
    void popupItem(int ID);

    void aboutQt();
    void aboutMEDEA();

    void cut();
    void copy();
    void paste();
    void replicate();
    void deleteSelection();
    void expandSelection();
    void contractSelection();

    void editLabel();
    void editReplicationCount();

    void constructDDSQOSProfile();
    void requestSearchSuggestions();

    void setControllerReady(bool ready);
    void openURL(QString url);

private slots:
    void ModelControllerReady(bool ready);
    void initializeController();
    void table_dataChanged(int ID, QString key, QVariant data);

    void modelNotification(MODEL_SEVERITY severity, QString description, int ID);

private:
    QList<ViewItem*> getSearchableEntities();

    void setupEntityKindItems();
    void _showGitHubPage(QString relURL="");
    void _showWebpage(QString URL);
    void _showWiki(ViewItem* item=0);
    QString getTempFileForModel();
    void spawnSubView(ViewItem *item);
    bool destructViewItem(ViewItem* item);
    QList<ViewItem*> getViewItems(QList<int> IDs);
    ViewItem* getActiveSelectedItem() const;

    QList<NodeView*> getNodeViewsContainingID(int ID);

    NodeViewItem* getNodeViewItem(int ID);

    NodeViewItem* getNodesImpl(int ID);
    NodeViewItem* getNodesDefinition(int ID);

    NodeViewItem* getSharedParent(NodeViewItem* node1, NodeViewItem* node2);

    NodeView* getActiveNodeView();
    void TeardownController();


    bool newProjectUsed;


    bool _newProject(QString file_path="");
    bool _saveProject();
    bool _saveAsProject(QString file_path = "");
    bool _closeProject(bool show_welcome=false);
    void _importProjects();
    void _importProjectFiles(QStringList fileName);
    bool _openProject(QString filePath = "");

    QList<ViewItem*> getItemsOfKind(NODE_KIND kind);
    QList<ViewItem*> getItemsOfKind(EDGE_KIND kind);


    QHash<NODE_KIND, NodeViewItem*> nodeKindItems;
    QHash<EDGE_KIND, EdgeViewItem*> edgeKindItems;

    bool _controllerReady = false;


    bool destructChildItems(ViewItem* parent);
    bool clearVisualItems();


    ViewItem* getViewItem(int ID);

    QHash<QString, int> treeLookup;
    QMultiMap<NODE_KIND, int> nodeKindLookups;
    QMultiMap<EDGE_KIND, int> edgeKindLookups;

    QHash<int, ViewItem*> viewItems;
    QList<int> topLevelItems;
    ViewItem* rootItem;

    BaseDockWidget* codeViewer = 0;
    BaseDockWidget* execution_browser = 0;

    SelectionController* selectionController;
    ActionController* actionController;
    ExecutionManager* execution_manager;
    JenkinsManager* jenkins_manager;

    ContextMenu* menu = 0;
    ModelController* controller;
    QMutex mutex;
    QTimer* autosave_timer_ = 0;
    int autosave_id_ = 0;
    bool showSearchSuggestions;
    
    bool showingWelcomeScreen = true;

};

#endif // VIEWCONTROLLER_H
