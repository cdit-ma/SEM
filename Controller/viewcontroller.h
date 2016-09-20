#ifndef VIEWCONTROLLER_H
#define VIEWCONTROLLER_H

#include "actioncontroller.h"
#include "toolbarcontroller.h"
#include "../Widgets/New/selectioncontroller.h"
#include "selectionhandler.h"

#include "../View/viewitem.h"
#include "../View/nodeviewitem.h"
#include "../View/edgeviewitem.h"
#include "../View/Validate/validatedialog.h"
#include "../Widgets/New/medeadockwidget.h"


class NewController;
class ToolbarWidgetNew;
class NodeViewNew;
class ViewController : public QObject
{
    Q_OBJECT
public:
    ViewController();
    ~ViewController();

    QStringList getNodeKinds();
    SelectionController* getSelectionController();
    ActionController* getActionController();
    ToolActionController* getToolbarController();

    QList<ViewItem*> getWorkerFunctions();
    QList<ViewItem*> getConstructableNodeDefinitions(QString kind);
    QList<ViewItem*> getValidEdges(Edge::EDGE_KIND kind);

    QStringList _getSearchSuggestions();

    QMap<QString, ViewItem*> getSearchResults(QString result);



    QStringList getAdoptableNodeKinds();

    QList<Edge::EDGE_KIND> getValidEdgeKindsForSelection();

    QList<Edge::EDGE_KIND> getExistingEdgeKindsForSelection();
    QList<ViewItem*> getExistingEdgeEndPointsForSelection(Edge::EDGE_KIND kind);

    QStringList getValidValuesForKey(int ID, QString keyName);
    void setDefaultIcon(ViewItem* viewItem);
    ViewItem* getModel();
    bool isModelReady();
    bool isControllerReady();

    bool canUndo();
    bool canRedo();
    bool canExportSnippet();
    bool canImportSnippet();

    QVector<ViewItem*> getOrderedSelection(QList<int> selection);

    void setController(NewController* c);
signals:
    //TO OTHER VIEWS SIGNALS

    void vc_JenkinsReady(bool);
    void vc_controllerReady(bool);
    void vc_viewItemConstructed(ViewItem* viewItem);
    void vc_viewItemDestructing(int ID, ViewItem* item);
    void vc_showToolbar(QPoint globalPos, QPointF itemPos = QPointF());
    void vc_gotSearchSuggestions(QStringList suggestions);

    void vc_editTableCell(int ID, QString keyName);

    void mc_showProgress(bool, QString);
    void mc_progressChanged(int);




    void mc_modelReady(bool);
    void mc_projectModified(bool);
    void mc_undoRedoUpdated();
    void vc_actionFinished();


    //TO CONTROLLER SIGNALS

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

    void vc_executeJenkinsJob(QString filePath);

    void vc_constructNode(int parentID, QString kind, QPointF pos = QPointF());
    void vc_constructEdge(QList<int> sourceIDs, int dstID, Edge::EDGE_KIND edgeKind = Edge::EC_UNDEFINED);
    void vc_destructEdges(QList<int> sourceIDs, int dstID, Edge::EDGE_KIND edgeKind = Edge::EC_UNDEFINED);

    void vc_constructConnectedNode(int parentID, QString nodeKind, int dstID, Edge::EDGE_KIND edgeKind = Edge::EC_UNDEFINED, QPointF pos=QPointF());
    void vc_constructWorkerProcess(int parentID, int dstID, QPointF point);


    void vc_importProjects(QStringList fileData);
    void vc_openProject(QString fileName, QString filePath);

    void vc_projectSaved(QString filePath);
    void vc_projectPathChanged(QString);

    void vc_showNotification(NOTIFICATION_TYPE type, QString title, QString description, QString iconPath="", QString iconName="", int ID=-1);

    void vc_centerItem(int ID);
    void vc_selectAndCenterConnectedEntities(ViewItem* item);

    void vc_fitToScreen();

    void vc_addProjectToRecentProjects(QString filePath);

    void vc_getCodeForComponent(QString graphmlPath, QString componentName);
    void vc_validateModel(QString graphmlPath, QString reportPath);
    void vc_launchLocalDeployment(QString graphmlPath);

    void vc_importXMEProject(QString xmePath, QString graphmlPath);
    void vc_importXMIProject(QString XMIPath);

    void vc_answerQuestion(bool);
    void vc_exportSnippet(QList<int> IDs);
    void vc_importSnippet(QList<int> IDs, QString fileName, QString fileData);

    void vc_highlightItem(int ID, bool highlight);
public slots:
    void notificationAdded();
    void notificationsSeen();

    void projectOpened(bool success);

    void gotExportedSnippet(QString snippetData);
    void askQuestion(QString title, QString message, int ID);



    void modelValidated(QString reportPath);
    void importGraphMLFile(QString graphmlPath);
    void importGraphMLExtract(QString data);
    void showCodeViewer(QString tabName, QString content);


    void jenkinsManager_IsBusy(bool busy);
    void jenkinsManager_SettingsValidated(bool success, QString errorString);
    void jenkinsManager_GotJenkinsNodesList(QString graphmlData);


    void getCodeForComponent();
    void validateModel();
    void selectModel();
    void launchLocalDeployment();





    void actionFinished(bool success, QString gg);
    void controller_entityConstructed(int ID, ENTITY_KIND eKind, QString kind, QHash<QString, QVariant> data, QHash<QString, QVariant> properties);
    void controller_entityDestructed(int ID, ENTITY_KIND eKind, QString kind);
    void controller_dataChanged(int ID, QString key, QVariant data);
    void controller_dataRemoved(int ID, QString key);

    void controller_propertyChanged(int ID, QString property, QVariant data);
    void controller_propertyRemoved(int ID, QString property);

    void setClipboardData(QString data);

    void newProject();
    void openProject();
    void openExistingProject(QString file);
    void importProjects();
    void importXMEProject();
    void importXMIProject();
    void importSnippet();
    void exportSnippet();
    void saveProject();
    void saveAsProject();
    void closeProject();
    void closeMEDEA();

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

    void setSelectionReadOnly(bool locked);

    void constructDDSQOSProfile();
    void requestSearchSuggestions();


    void setModelReady(bool okay);
    void setControllerReady(bool ready);


private slots:
    void initializeController();
    void table_dataChanged(int ID, QString key, QVariant data);

private:
    void _showGitHubPage(QString relURL="");
    QString getTempFileForModel();
    void spawnSubView(ViewItem *item );
    bool destructViewItem(ViewItem* item);
    QList<ViewItem*> getViewItems(QList<int> IDs);
    ViewItem* getActiveSelectedItem() const;

    QList<NodeViewNew*> getNodeViewsContainingID(int ID);

    NodeViewItem* getNodeViewItem(int ID);

    NodeViewItem* getNodesImpl(int ID);
    NodeViewItem* getNodesDefinition(int ID);

    NodeViewItem* getSharedParent(NodeViewItem* node1, NodeViewItem* node2);

    NodeViewNew* getActiveNodeView();
    void _teardownProject();


    bool _newProject();
    bool _saveProject();
    bool _saveAsProject();
    bool _closeProject();
    void _importProjects();
    void _importProjectFiles(QStringList fileName);
    bool _openProject(QString filePath = "");

    QList<ViewItem*> getItemsOfKind(Node::NODE_KIND kind);
    QList<ViewItem*> getItemsOfKind(Edge::EDGE_KIND kind);
    bool _modelReady;


    bool _controllerReady;


    bool destructChildItems(ViewItem* parent);
    bool clearVisualItems();


    ViewItem* getViewItem(int ID);

    QHash<QString, int> treeLookup;
    QMultiMap<Node::NODE_KIND, int> nodeKindLookups;
    QMultiMap<Edge::EDGE_KIND, int> edgeKindLookups;

    QHash<int, ViewItem*> viewItems;
    QList<int> topLevelItems;
    ViewItem* rootItem;

    MedeaDockWidget *codeViewer;
    ValidateDialog* validationDialog;

    SelectionController* selectionController;
    ActionController* actionController;
    ToolActionController* toolbarController;

    ToolbarWidgetNew* toolbar;
    NewController* controller;

    bool showSearchSuggestions;
};

#endif // VIEWCONTROLLER_H
