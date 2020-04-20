#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "viewitem.h"
#include "nodeviewitem.h"
#include "edgeviewitem.h"
#include "../ActionController/actioncontroller.h"
#include "../SelectionController/selectioncontroller.h"
#include "../SelectionController/selectionhandler.h"
#include "../NotificationManager/notificationmanager.h"
#include "../ExecutionManager/executionmanager.h"
#include "../JenkinsManager/jenkinsmanager.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Controllers/AggregationProxy/aggregationproxy.h"
#include "../../Widgets/Charts/Data/Events/event.h"
#include "../../../modelcontroller/kinds.h"
#include "../../../modelcontroller/nodekinds.h"
#include "../../../modelcontroller/edgekinds.h"
#include "../../../modelcontroller/dataupdate.h"
#include "../../../modelcontroller/viewcontrollerint.h"

#undef ERROR
#include <QTimer>

enum class MODEL_SEVERITY;
class NotificationManager;
class ModelController;
class NodeView;
class JenkinsManager;
class ContextMenu;
class DefaultDockWidget;
class CodeBrowser;
class JobMonitor;

class ViewController : public ViewControllerInterface
{
	Q_OBJECT

public:
    ViewController();
    ~ViewController() final;

    DefaultDockWidget* constructDockWidget(const QString& label, const QString& icon_path, const QString& icon_name, QWidget* widget, BaseWindow* window = nullptr);

    static QList<ViewItem*> ToViewItemList(const QList<NodeViewItem*>& node_items);
    static QList<ViewItem*> ToViewItemList(const QList<EdgeViewItem*>& edge_items);

    bool isWelcomeScreenShowing();
    ContextMenu* getContextMenu();

    SelectionController* getSelectionController();
    ActionController* getActionController();

    QList<ViewItem*> getConstructableNodeDefinitions(NODE_KIND node_kind, EDGE_KIND edge_kind);

    QList<ViewItem*> getNodesInstances(int ID) const;
    QList<ViewItem*> getViewItems(const QList<int>& IDs);

    QStringList _getSearchSuggestions();
    QStringList GetIDs();

    QHash<QString, ViewItem*> getSearchResults(const QString& query, QList<ViewItem*> view_items = {});
    QList<ViewItem*> filterList(const QString& query, const QList<ViewItem*>& view_items);
    QList<ViewItem*> filterList(const QString& query, const QList<NodeViewItem*>& node_items){
        return filterList(query, ViewController::ToViewItemList(node_items));
    }
    QList<ViewItem*> filterList(const QString& query, const QList<EdgeViewItem*>& edge_items){
        return filterList(query, ViewController::ToViewItemList(edge_items));
    }

    QHash<EDGE_DIRECTION, ViewItem*> getValidEdges(EDGE_KIND kind);
    QMultiMap<EDGE_DIRECTION, ViewItem*> getExistingEndPointsOfSelection(EDGE_KIND kind);

    ViewDockWidget* constructViewDockWidget(const QString& title, QWidget* parent);

    QList<NodeViewItem*> getNodeKindItems();
    QList<EdgeViewItem*> getEdgeKindItems();

    QList<ViewItem*> getViewItemParents(const QList<ViewItem*>& items);

    int getNodeDefinitionID(int ID) const;
    QList<int> getNodeInstanceIDs(int ID) const;

    void QueryRunningExperiments();
    
    ModelController* getModelController();

    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > getValidEdgeKinds(const QList<int>& ids);
    
    QSet<NODE_KIND> getValidNodeKinds();
    QSet<EDGE_KIND> getCurrentEdgeKinds();

    static QSet<NODE_KIND> getValidChartNodeKinds();
    QSet<MEDEA::ChartDataKind> getValidChartDataKindsForSelection();

    QList<QVariant> getValidValuesForKey(int ID, const QString& keyName);
    static void SetDefaultIcon(ViewItem& viewItem);
    
    ViewItem* getModel();
    bool isControllerReady();

    bool canUndo();
    bool canRedo();

    void RequestJenkinsNodes();
    void RequestJenkinsBuildJob();
    void RequestJenkinsBuildJobName(const QString& job_name);
    void ShowJenkinsBuildDialog(const QString& job_name, QList<Jenkins_Job_Parameter> paramaters);
    void ShowJenkinsBuildDialog(const QStringList& jobs);

    bool isNodeAncestor(int ID, int ID2);

    VIEW_ASPECT getNodeViewAspect(int ID);

    void constructEdges(int id, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction);

    void HighlightItems(const QList<int> &ids);

signals:
    void GotJava(bool);
    void GotRe(bool);
    void GotRegen(bool);
    void GotJenkins(bool);

    void vc_controllerReady(bool);
    void vc_viewItemConstructed(ViewItem* viewItem);
    void vc_viewItemDestructing(int ID, ViewItem* item);
    void vc_editTableCell(int ID, QString keyName);
    void vc_addProjectToRecentProjects(QString filePath);
    void vc_removeProjectFromRecentProjects(QString filePath);

    void vc_showWelcomeScreen(bool);
    void vc_showToolbar(QPoint globalPos, QPointF itemPos = QPointF());

    void vc_centerOnItems(QList<int> ids);
    void vc_selectItems(QList<int> ids);
    void vc_centerItem(int ID);
    void vc_fitToScreen(bool if_active_view = false);
    void vc_highlightItem(int ID, bool highlight);
    void vc_selectAndCenterConnectedEntities(const QVector<ViewItem*>& item);

    void vc_viewItemsInChart(QVector<ViewItem*> selectedItems, QList<MEDEA::ChartDataKind> dataKinds);
    void vc_displayChartPopup();
    void vc_displayExperimentDataflow();

	void modelClosed();

protected:
    void ModelReady(bool ready) final;
    void NodeConstructed(int parent_id, int id, NODE_KIND node_kind) final;
    void EdgeConstructed(int id, EDGE_KIND edge_kind, int src_id, int dst_id) final;
    void EntityDestructed(int id, GRAPHML_KIND kind) final;
    void DataChanged(int id, DataUpdate data) final;
    void DataRemoved(int id, const QString& key_name) final;
    void NodeEdgeKindsChanged(int id) final;
    void NodeTypesChanged(int id) final;
    void AddNotification(MODEL_SEVERITY severity, const QString& title, const QString& description, int ID) final;

public slots:
    void incrementSelectedKey(const QString& key_name);
    void decrementSelectedKey(const QString& key_name);
    
    void welcomeScreenToggled(bool visible);

    void showCodeViewer(const QString& tabName, const QString& content);

    JobMonitor* getExecutionMonitor();
    void showExecutionMonitor();
    void RefreshExecutionMonitor(const QString& job_name);
    void ListJenkinsJobs();

    void jenkinsManager_GotJenkinsNodesList(const QString& graphmlData);

    void getCodeForComponent();
    void validateModel();
    void selectModel();

    void setClipboardData(const QString& data);

    void newProject();
    bool OpenProject();
    bool OpenExistingProject(QString file_path);

    void importProjects();

    void autoSaveProject();
    void saveProject();
    void saveAsProject();
    void closeProject();
    void closeMEDEA();

    void generateProjectWorkspace();
    void executeModelLocal();

    void centerSelection();
    void alignSelectionVertical();
    void alignSelectionHorizontal();
    void selectAndCenterConnectedEntities();
    void selectAndCenterInstances();
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

    void viewSelectionChart(const QList<MEDEA::ChartDataKind>& dataKinds);

    void setControllerReady(bool ready);
    void openURL(const QString& url);

private slots:
    void initializeController();
    void table_dataChanged(int ID, const QString& key, const QVariant& data);

private:
    void SetParentNode(ViewItem* parent, ViewItem* child);
    void notification_Added(QSharedPointer<NotificationObject> notification);
    void notification_Destructed(QSharedPointer<NotificationObject> notification);

    void SettingChanged(SETTINGS key, const QVariant& value);
    void AutosaveDurationChanged(int duration_minutes);
    void StartAutosaveCountdown();
    
    void StoreViewItem(ViewItem* view_item);
    QList<ViewItem*> getSearchableEntities();

    void setupEntityKindItems();

    void _showGitHubPage(const QString& relURL = "");
    void _showWebpage(const QString& URL);
    void _showWiki(ViewItem* item = nullptr);

    QString getTempFileForModel();

    void spawnSubView(ViewItem *item);
    
    void DestructViewItem(ViewItem* item);
    void ResetViewItems();

    ViewItem* getViewItem(int ID) const;
    ViewItem* getActiveSelectedItem() const;

    NodeViewItem* getNodesImpl(int ID) const;
    NodeViewItem* getNodesDefinition(int ID) const;

    NodeViewItem* getNodeViewItem(int ID) const;
    NodeViewItem* getSharedParent(NodeViewItem* node1, NodeViewItem* node2);

    NodeView* getActiveNodeView();
    void TeardownController();

    bool _newProject(const QString& file_path = "");
    bool _saveProject();
    bool _saveAsProject(const QString& file_path = "");
    bool _closeProject(bool show_welcome = false);
    void _importProjects();
    void _importProjectFiles(const QStringList& fileName);

    QHash<NODE_KIND, NodeViewItem*> nodeKindItems;
    QHash<EDGE_KIND, EdgeViewItem*> edgeKindItems;

    bool _controllerReady = false;

    ViewItem* root_item = nullptr;
    QHash<int, ViewItem*> view_items_;
    int model_id_ = -1;

    BaseDockWidget* codeViewer = nullptr;
    CodeBrowser* codeBrowser = nullptr;
    BaseDockWidget* execution_monitor = nullptr;
    JobMonitor* job_monitor = nullptr;

    SelectionController* selectionController = nullptr;
    ActionController* actionController = nullptr;
    ExecutionManager* execution_manager = nullptr;
    JenkinsManager* jenkins_manager = nullptr;

    ContextMenu* menu = nullptr;
    ModelController* controller = nullptr;

    QTimer autosave_timer_;
    bool is_autosave_enabled_ = false;
    int autosave_id_ = 0;
    
    bool showingWelcomeScreen = true;
};

#endif // VIEWCONTROLLER_H