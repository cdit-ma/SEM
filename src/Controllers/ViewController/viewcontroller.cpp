#include "viewcontroller.h"

#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Widgets/DockWidgets/nodeviewdockwidget.h"
#include "../../Views/ContextToolbar/contexttoolbar.h"
#include "../../Views/NodeView/nodeview.h"
#include "../../Widgets/CodeEditor/codebrowser.h"

#include "../../Controllers/ExecutionManager/executionmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"

#include "../../ModelController/modelcontroller.h"
#include "../../ModelController/entityfactory.h"
#include "../../Utils/filehandler.h"
#include "../../Utils/IdlParser/idlparser.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QListIterator>
#include <QStringBuilder>
#include <QDesktopServices>
#include <QFile>

#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"
#define GME_FILE_EXT "GME Documents (*.xme)"
#define GME_FILE_SUFFIX ".xme"

#define XMI_FILE_EXT "UML XMI Documents (*.xml)"
#define XMI_FILE_SUFFIX ".xml"

#define IDL_FILE_EXT "IDL Documents (*.idl)"
#define IDL_FILE_SUFFIX ".idl"


ViewController::ViewController() : QObject(){
    //qRegisterMetaType<NOTIFICATION_TYPE>("NOTIFICATION_TYPE");
    controller = 0;

    codeViewer = 0;


    rootItem = new ViewItem(this);

    //Setup nodes
    setupEntityKindItems();

    //Initialize Settings
    SettingsController::initializeSettings();

    NotificationManager::construct_singleton(this);

    //notificationManager = new NotificationManager(this, this);
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);
    toolbarController = new ToolbarController(this);
    toolbar = new ContextToolbar(this);

    jenkins_manager = new JenkinsManager(this);
    execution_manager = new ExecutionManager(this);

    connect(execution_manager, &ExecutionManager::GotCodeForComponent, this, &ViewController::showCodeViewer);

    connect(this, &ViewController::vc_showToolbar, toolbar, &ContextToolbar::showToolbar);
}

void ViewController::connectModelController(ModelController* c){
    connect(controller, &ModelController::NodeConstructed, this, &ViewController::model_NodeConstructed);
    connect(controller, &ModelController::EdgeConstructed, this, &ViewController::model_EdgeConstructed);
    connect(controller, &ModelController::EntityDestructed, this, &ViewController::controller_entityDestructed);
    
    connect(controller, &ModelController::ModelReady, this, &ViewController::ModelControllerReady);
    connect(controller, &ModelController::DataChanged, this, &ViewController::controller_dataChanged);
    connect(controller, &ModelController::DataRemoved, this, &ViewController::controller_dataRemoved);
    connect(controller, &ModelController::ProjectModified, this, &ViewController::mc_projectModified);
    connect(controller, &ModelController::ProjectFileChanged, this, &ViewController::vc_projectPathChanged);

    
    connect(controller, &ModelController::ActionFinished, this, &ViewController::vc_ActionFinished);
    connect(controller, &ModelController::Notification, this, &ViewController::modelNotification);

    connect(controller, &ModelController::SetClipboardData, this, &ViewController::setClipboardData);
    connect(controller, &ModelController::UndoRedoUpdated, this, &ViewController::mc_undoRedoUpdated);
    connect(controller, &ModelController::ShowProgress, this, &ViewController::mc_showProgress);
    connect(controller, &ModelController::ProgressChanged, this, &ViewController::mc_progressChanged);
    connect(controller, &ModelController::ModelReady, this, &ViewController::vc_ProjectLoaded);


    connect(this, &ViewController::vc_SetupModelController, controller, &ModelController::SetupController);
    connect(this, &ViewController::vc_importProjects, controller, &ModelController::importProjects);
   

    connect(this, &ViewController::vc_setData, controller, &ModelController::setData);
    connect(this, &ViewController::vc_removeData, controller, &ModelController::removeData);
    connect(this, &ViewController::vc_constructNode, controller, &ModelController::constructNode);
    connect(this, &ViewController::vc_constructEdge, controller, &ModelController::constructEdge);
    connect(this, &ViewController::vc_destructEdges, controller, &ModelController::destructEdges);
    connect(this, &ViewController::vc_destructAllEdges, controller, &ModelController::destructAllEdges);
    connect(this, &ViewController::vc_constructConnectedNode, controller, &ModelController::constructConnectedNode);
    connect(this, &ViewController::vc_constructWorkerProcess, controller, &ModelController::constructWorkerProcess);
    connect(this, &ViewController::vc_projectSaved, controller, &ModelController::setProjectSaved);
    connect(this, &ViewController::vc_undo, controller, &ModelController::undo);
    connect(this, &ViewController::vc_redo, controller, &ModelController::redo);
    connect(this, &ViewController::vc_triggerAction, controller, &ModelController::triggerAction);
    connect(this, &ViewController::vc_cutEntities, controller, &ModelController::cut);
    connect(this, &ViewController::vc_copyEntities, controller, &ModelController::copy);
    connect(this, &ViewController::vc_paste, controller, &ModelController::paste);
    connect(this, &ViewController::vc_replicateEntities, controller, &ModelController::replicate);
    connect(this, &ViewController::vc_deleteEntities, controller, &ModelController::remove);
   
    this->setController(controller);
}

ViewController::~ViewController()
{
    delete rootItem;
}

JenkinsManager *ViewController::getJenkinsManager()
{
    return jenkins_manager;
}

ExecutionManager *ViewController::getExecutionManager()
{
    return execution_manager;
}


SelectionController *ViewController::getSelectionController()
{
    return selectionController;
}

ActionController *ViewController::getActionController()
{
    return actionController;
}

ToolbarController *ViewController::getToolbarController()
{
    return toolbarController;
}

QList<ViewItem *> ViewController::getWorkerFunctions()
{
    QList<ViewItem*> workers;

    foreach(ViewItem* item, getItemsOfKind(NODE_KIND::WORKER_PROCESS)){
        NodeViewItem* node = (NodeViewItem*)item;
        if(item && node->getViewAspect() == VIEW_ASPECT::WORKERS){
            workers << item;
        }
    }
    return workers;
}

QList<ViewItem *> ViewController::getConstructableNodeDefinitions(NODE_KIND node_kind, EDGE_KIND edge_kind)
{
    QList<ViewItem*> items;
    if(controller  && selectionController && selectionController->getSelectionCount() == 1){
        int parentID = selectionController->getFirstSelectedItem()->getID();
        QList<int> IDs = controller->getConstructableConnectableNodes(parentID, node_kind, edge_kind);
        items = getViewItems(IDs);
    }
    return items;
}

QList<ViewItem*> ViewController::getValidEdges(EDGE_KIND kind)
{
    QList<ViewItem*> items;
    if(selectionController && controller){
        QList<int> selectedIDs = selectionController->getSelectionIDs();
        QList<int> IDs = controller->getConnectableNodeIDs(selectedIDs, kind);
        items = getViewItems(IDs);
    }
    return items;
}


QStringList ViewController::_getSearchSuggestions()
{
    QStringList suggestions;

    foreach(ViewItem* item, viewItems.values()){
        if(item->isInModel()){
            //ID's
            suggestions.append(QString::number(item->getID()));
            foreach(QString key, item->getKeys()){
                suggestions.append(item->getData(key).toString());
            }
        }
    }
    return suggestions;
}

QMap<QString, ViewItem *> ViewController::getSearchResults(QString query)
{
    QMap<QString, ViewItem*> results;

    foreach(ViewItem* item, viewItems.values()){
        if(item->isInModel()){
            QString ID = QString::number(item->getID());

            if(ID.contains(query)){
                results.insertMulti("ID", item);
            }

            foreach(QString key, item->getKeys()){
                QString data = item->getData(key).toString();
                if(data.contains(query, Qt::CaseInsensitive)){
                    results.insertMulti(key, item);
                }
            }
        }
    }
    return results;
}

NodeViewDockWidget *ViewController::constructNodeViewDockWidget(QString label)
{
    BaseDockWidget *dw = WindowManager::constructNodeViewDockWidget(label);
    NodeView* nodeView = new NodeView(dw);
    dw->setWidget(nodeView);

    if(actionController){
        //Add all actions which need focus!
        dw->addActions(actionController->getNodeViewActions());
    }

    //Setup NodeView
    nodeView->setViewController(this);

    return (NodeViewDockWidget*)dw;
}

QList<ViewItem *> ViewController::getExistingEdgeEndPointsForSelection(EDGE_KIND kind)
{
    QList<ViewItem *> list;

    if(selectionController){
        foreach(ViewItem* item, selectionController->getSelection()){
            if(item && item->isNode()){
                NodeViewItem* nodeItem = (NodeViewItem*) item;
                foreach(EdgeViewItem* edge, nodeItem->getEdges(kind)){
                    NodeViewItem* src = edge->getSource();
                    NodeViewItem* other = edge->getDestination();
                    if(src != item){
                        other = src;
                    }
                    if(!list.contains(other)){
                        list.append(other);
                    }
                }
            }
        }
    }
    return list;
}


QList<NODE_KIND> ViewController::getAdoptableNodeKinds2()
{
    QList<NODE_KIND> kinds;

    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        kinds = controller->getAdoptableNodeKinds(ID);
    }
    return kinds;
}

QList<NodeViewItem *> ViewController::getNodeKindItems()
{
    return nodeKindItems;
}

QList<EdgeViewItem *> ViewController::getEdgeKindItems()
{
    return edgeKindItems;
}

QList<EDGE_KIND> ViewController::getValidEdgeKindsForSelection()
{
    QList<EDGE_KIND> edgeKinds;
    if(selectionController && controller){
        edgeKinds = controller->getValidEdgeKindsForSelection(selectionController->getSelectionIDs());
    }
    return edgeKinds;
}

QList<EDGE_KIND> ViewController::getExistingEdgeKindsForSelection()
{
    QList<EDGE_KIND> edgeKinds;
    if(selectionController && controller){
        edgeKinds = controller->getExistingEdgeKindsForSelection(selectionController->getSelectionIDs());
    }
    return edgeKinds;
}

QList<QVariant> ViewController::getValidValuesForKey(int ID, QString keyName)
{
    QList<QVariant> valid_values;
    if(controller){
        valid_values = controller->getValidKeyValues(ID, keyName);
    }
    return valid_values;
}


void ViewController::setDefaultIcon(ViewItem *viewItem)
{
    if(viewItem){
        bool isNode = viewItem->isNode();
        bool isEdge = viewItem->isEdge();

        NodeViewItem* nodeViewItem = (NodeViewItem*)viewItem;
        EdgeViewItem* edgeViewItem = (EdgeViewItem*)viewItem;

        QString alias = "EntityIcons";
        QString image = viewItem->getData("kind").toString();

        if(isNode){
            auto node_kind = nodeViewItem->getNodeKind();

            auto icon = viewItem->getData("icon").toString();
            auto icon_prefix = viewItem->getData("icon_prefix").toString();


            switch(node_kind){
            case NODE_KIND::HARDWARE_NODE:{
                QString os = viewItem->getData("os").toString();
                QString arch = viewItem->getData("architecture").toString();
                QString spec_image = (os % "_" % arch);
                if(Theme::theme()->gotImage(alias, spec_image)){
                    image = spec_image;
                }
                break;
            }
            case NODE_KIND::WORKLOAD:{
                if(nodeViewItem->getViewAspect() == VIEW_ASPECT::WORKERS){
                    //If we are
                    alias = icon_prefix;
                    image = icon;
                }
                break;
            }
            case NODE_KIND::VARIADIC_PARAMETER:{
                alias = "Icons";
                image = "label";
                break;
            }
            case NODE_KIND::VARIABLE_PARAMETER:{
                alias = "EntityIcons";
                image = "Variable";
                break;
            }
            case NODE_KIND::WORKER_DEFINITIONS:{
                alias = "Icons";
                image = "medeaLogo";
                break;
            }
            case NODE_KIND::WORKER_PROCESS:
            case NODE_KIND::INPUT_PARAMETER:
            case NODE_KIND::RETURN_PARAMETER:{
                if(icon_prefix.length() > 0 && icon.length() > 0){
                    alias = icon_prefix;
                    image = icon;
                }
                break;
            }
            case NODE_KIND::VECTOR:
            case NODE_KIND::VECTOR_INSTANCE:{
                //Check children
                foreach(ViewItem* child, viewItem->getDirectChildren()){
                    auto* node = (NodeViewItem*) child;
                    if(child->isNode()){
                        auto child_kind = node->getNodeKind();

                        if(child_kind == NODE_KIND::MEMBER || child_kind == NODE_KIND::MEMBER_INSTANCE){
                            image += "_Member";
                        }else if(child_kind == NODE_KIND::AGGREGATE_INSTANCE){
                            image += "_AggregateInstance";
                        }
                    }
                    break;
                }
                break;
            }
            case NODE_KIND::MODEL:
                alias = "Icons";
                image = "medeaLogo";
                break;
            default:
                break;

            }
        }else if(isEdge){
            switch(edgeViewItem->getEdgeKind()){
                case EDGE_KIND::DEFINITION:{
                    alias = "Icons";
                    image = "gears";
                }
                break;
            default:
                break;
            }
        }

        //Try and get the image
        //auto i = Theme::theme()->getIcon(alias, image);
        viewItem->setIcon(alias, image);
    }
}

ViewItem *ViewController::getModel()
{
    int ID = nodeKindLookups.value(NODE_KIND::MODEL, -1);
    return getViewItem(ID);
}


void ViewController::requestSearchSuggestions()
{
    emit vc_gotSearchSuggestions(_getSearchSuggestions());
}

void ViewController::setController(ModelController *c)
{
    controller = c;
}


void ViewController::modelValidated(QString reportPath)
{
}

void ViewController::importGraphMLFile(QString graphmlPath)
{
    _importProjectFiles(QStringList(graphmlPath));
}

void ViewController::importGraphMLExtract(QString data)
{
    if(!data.isEmpty()){
        // fit the contents in all the view aspects after import when no model has been imported yet?
        emit vc_importProjects(QStringList(data));
    }
}

void ViewController::showCodeViewer(QString tabName, QString content)
{
    if(!codeViewer){
        codeViewer = WindowManager::constructViewDockWidget("Code Browser");
        codeViewer->setCloseVisible(false);
        CodeBrowser* codeBrowser = new CodeBrowser(codeViewer);
        codeViewer->setWidget(codeBrowser);
        codeViewer->setIcon("Icons", "bracketsAngled");
        codeViewer->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
        BaseWindow* window = WindowManager::manager()->getActiveWindow();
        if(window){
            window->addDockWidget(codeViewer);
        }
    }
    if(codeViewer){
        CodeBrowser* codeBrowser = qobject_cast<CodeBrowser*>(codeViewer->widget());
        if(codeBrowser){
            codeBrowser->showCode(tabName, content, false);
        }
        codeViewer->show();
    }
}

void ViewController::jenkinsManager_SettingsValidated(bool success, QString errorString)
{
    emit vc_JenkinsReady(success);
    QString message = success ? "Settings validated successfully" : errorString;
    NOTIFICATION_SEVERITY severity = success ? NOTIFICATION_SEVERITY::INFO : NOTIFICATION_SEVERITY::ERROR;
    NotificationManager::manager()->displayNotification(message, "Icons", "jenkins", -1, severity, NOTIFICATION_TYPE::APPLICATION, NOTIFICATION_CATEGORY::JENKINS);
}

void ViewController::jenkinsManager_GotJava(bool java, QString javaVersion)
{
    emit vc_JavaReady(java);
    NOTIFICATION_SEVERITY severity = java ? NOTIFICATION_SEVERITY::INFO : NOTIFICATION_SEVERITY::ERROR;
    QString message = java ? "Found java: " + javaVersion: "Cannot find java";
    NotificationManager::manager()->displayNotification(message, "Icons", "java", -1, severity, NOTIFICATION_TYPE::APPLICATION);
}

void ViewController::jenkinsManager_GotJenkinsNodesList(QString graphmlData)
{
    if(!graphmlData.isEmpty()){
        emit vc_triggerAction("Loading Jenkins Nodes");
        QStringList fileData;
        fileData << graphmlData;
        emit vc_importProjects(fileData);
    }
}

void ViewController::getCodeForComponent()
{
    ViewItem* item = getActiveSelectedItem();
    if(item && item->isNode()){
        auto def = (NodeViewItem*)item;
        if(def->getNodeKind() == NODE_KIND::COMPONENT_INSTANCE || def->getNodeKind() == NODE_KIND::COMPONENT_IMPL){
            int id = controller->getDefinition(item->getID());
            def = getNodeViewItem(id);
        }
        if(def){
            QString componentName = def->getData("label").toString();
            QString filePath = getTempFileForModel();
            if(!componentName.isEmpty() && !filePath.isEmpty()){
                execution_manager->GenerateCodeForComponent(filePath, componentName);
            }
        }
    }
}

void ViewController::validateModel()
{
    if(controller){
        QString filePath = getTempFileForModel();
        execution_manager->ValidateModel(filePath);
    }
}

void ViewController::selectModel()
{
    emit selectionController->itemActiveSelectionChanged(getModel(), true);
}

void ViewController::launchLocalDeployment()
{
    if(controller){
        QString filePath = getTempFileForModel();
        if(!filePath.isEmpty()){
            emit vc_launchLocalDeployment(filePath);
        }
    }
}


void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit vc_triggerAction("Table Changed");
    emit vc_setData(ID, key, data);
}

void ViewController::setupEntityKindItems()
{
    //Prune out the kinds we don't need.
    auto constructableNodes = EntityFactory::getNodeKinds();
    constructableNodes.removeAll(NODE_KIND::ATTRIBUTE_IMPL);
    constructableNodes.removeAll(NODE_KIND::ASSEMBLY_DEFINITIONS);
    constructableNodes.removeAll(NODE_KIND::DEPLOYMENT_DEFINITIONS);
    constructableNodes.removeAll(NODE_KIND::HARDWARE_DEFINITIONS);
    constructableNodes.removeAll(NODE_KIND::HARDWARE_CLUSTER);
    constructableNodes.removeAll(NODE_KIND::HARDWARE_NODE);
    constructableNodes.removeAll(NODE_KIND::INEVENTPORT_IMPL);
    constructableNodes.removeAll(NODE_KIND::INTERFACE_DEFINITIONS);
    constructableNodes.removeAll(NODE_KIND::MANAGEMENT_COMPONENT);
    constructableNodes.removeAll(NODE_KIND::OUTEVENTPORT_INSTANCE);
    constructableNodes.removeAll(NODE_KIND::PROCESS);
    constructableNodes.removeAll(NODE_KIND::RETURN_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::INPUT_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::WORKLOAD);
    constructableNodes.removeAll(NODE_KIND::VECTOR_INSTANCE);
    constructableNodes.removeAll(NODE_KIND::VARIABLE_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::QOS_DDS_PROFILE);

    for(auto kind : constructableNodes){
        QString label = EntityFactory::getNodeKindString(kind);

        auto item = new NodeViewItem(this, kind, label);
        //qCritical() << label;
        setDefaultIcon(item);
        nodeKindItems.append(item);
    }

    for(auto kind : EntityFactory::getEdgeKinds()){
        auto item = new EdgeViewItem(this, kind);
        setDefaultIcon(item);
        edgeKindItems.append(item);
    }
}


void ViewController::_showGitHubPage(QString relURL)
{
    QString URL = APP_URL() % relURL;
    _showWebpage(URL);
}

void ViewController::_showWebpage(QString URL)
{
    QDesktopServices::openUrl(QUrl(URL));
}

void ViewController::_showWiki(ViewItem *item)
{
    QString wikiURL = SettingsController::settings()->getSetting(SK_GENERAL_MEDEA_WIKI_URL).toString();
    QString url = wikiURL;

    bool isGitWiki = wikiURL.contains("github.com", Qt::CaseInsensitive);

    if(!isGitWiki){
        url += "/SEM/MEDEA";
    }

    if(item){
        QString kind = item->getData("kind").toString();
        if(isGitWiki){
            //GIT USES FLAT STRUCTURE
            url += "/ModelEntities-" + kind;
        }else{
            url += "/ModelEntities/" + kind;
        }
    }
    _showWebpage(url);
}

QString ViewController::getTempFileForModel()
{
    QString filePath;
    if(controller){
        QString data = controller->getProjectAsGraphML();
        if(!data.isEmpty()){
            filePath = FileHandler::writeTempTextFile(data, ".graphml");
        }
    }
    return filePath;
}

void ViewController::spawnSubView(ViewItem * item)
{
    if(item && item->isNode()){
        auto dockWidget = WindowManager::manager()->getNodeViewDockWidget(item);
        if(dockWidget){
            dockWidget->setVisible(true);
        }else{
            //Construct a dockWidget
            dockWidget = constructNodeViewDockWidget();
            //Setup Dock Widget
            dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
            dockWidget->setIcon(item->getIcon());
            dockWidget->setTitle(item->getData("label").toString());

            //Set the NodeView to be contained on this NodeViewItem
            dockWidget->getNodeView()->setContainedNodeViewItem((NodeViewItem*)item);

            //Show the reparent DockWidget Widget
            WindowManager::manager()->reparentDockWidget(dockWidget);

            //Fit the contents of the dockwidget to screen
            dockWidget->getNodeView()->fitToScreen();
        }


    }
}

bool ViewController::isControllerReady()
{
    return _controllerReady;
}

bool ViewController::canUndo()
{
    if(controller){
        return controller->canUndo();
    }
    return false;
}

bool ViewController::canRedo()
{
    if(controller){
        return controller->canRedo();
    }
    return false;
}


bool ViewController::destructViewItem(ViewItem *item)
{
    if(!item){
        return false;
    }
    QList<ViewItem*> children;
    children.append(item);
    children.append(item->getNestedChildren());
    QListIterator<ViewItem*> it(children);
    it.toBack();
    while(it.hasPrevious()){
        ViewItem* viewItem = it.previous();
        if(!viewItem || viewItem == rootItem){
            continue;
        }
        int ID = viewItem->getID();

        if(viewItem->isNode()){
            //Remove node from nodeKind Map
            NodeViewItem* nodeItem = (NodeViewItem*)viewItem;
            nodeKindLookups.remove(nodeItem->getNodeKind(), ID);
        }else if(viewItem->isEdge()){
            //Remove Edge from edgeKind map
            EdgeViewItem* edgeItem = (EdgeViewItem*)viewItem;
            edgeItem->disconnectEdge();
            edgeKindLookups.remove(edgeItem->getEdgeKind(), ID);
        }

        ViewItem* parentItem = viewItem->getParentItem();
        if(parentItem){
            parentItem->removeChild(viewItem);

            NodeViewItem* parentNodeItem = (NodeViewItem*) parentItem;

            if(parentItem->isNode()){
                //Update the Icon of Vectors!
                switch(parentNodeItem->getNodeKind()){
                case NODE_KIND::VECTOR:
                case NODE_KIND::VECTOR_INSTANCE:
                    setDefaultIcon(parentItem);
                    break;
                default:
                    break;
                }
            }
        }

        //Remove the item from the Hash/TopLevel Hash
        viewItems.remove(ID);
        topLevelItems.removeAll(ID);

        //Tell Views we are destructing!
        emit vc_viewItemDestructing(ID, viewItem);
        viewItem->destruct();
    }
    return true;
}

QList<ViewItem *> ViewController::getViewItems(QList<int> IDs)
{
    QList<ViewItem*> items;

    foreach(int ID, IDs){
        ViewItem* item = getViewItem(ID);
        if(item){
            items.append(item);
        }
    }
    return items;
}

ViewItem *ViewController::getActiveSelectedItem() const
{
    if(selectionController){
        return selectionController->getActiveSelectedItem();
    }
    return 0;
}

QList<NodeView *> ViewController::getNodeViewsContainingID(int ID)
{
    QList<NodeView *> nodeViews;

    foreach(NodeViewDockWidget* dock, WindowManager::manager()->getNodeViewDockWidgets()){
        if(dock){
            NodeView* view = dock->getNodeView();
            if(view && view->getIDsInView().contains(ID)){
                nodeViews.append(view);
            }
        }
    }
    return nodeViews;
}


NodeViewItem *ViewController::getNodeViewItem(int ID)
{
    ViewItem* item = getViewItem(ID);
    if(item && item->isNode()){
        return (NodeViewItem*) item;
    }
    return 0;
}

NodeViewItem *ViewController::getNodesImpl(int ID)
{
    if(controller){
        int implID = controller->getImplementation(ID);
        return getNodeViewItem(implID);
    }
    return 0;
}

NodeViewItem *ViewController::getNodesDefinition(int ID)
{
    if(controller){
        int defID = controller->getDefinition(ID);
        return getNodeViewItem(defID);
    }
    return 0;
}

NodeViewItem *ViewController::getSharedParent(NodeViewItem *node1, NodeViewItem *node2)
{
    NodeViewItem* parent = 0;
    if(controller && node1 && node2){
        auto ID = controller->getSharedParent(node1->getID(), node2->getID());
        parent = getNodeViewItem(ID);
    }
    return parent;
}

NodeView *ViewController::getActiveNodeView()
{
    ViewDockWidget* dock = WindowManager::manager()->getActiveViewDockWidget();
    if(dock && dock->isNodeViewDock()){
        NodeViewDockWidget* viewDock = (NodeViewDockWidget*) dock;
        NodeView* nodeView = viewDock->getNodeView();
        return nodeView;
    }
    return 0;
}

void ViewController::modelNotification(MODEL_SEVERITY severity, QString description, int ID){
    NOTIFICATION_SEVERITY ns = NOTIFICATION_SEVERITY::INFO;
    switch(severity){
        case MODEL_SEVERITY::ERROR:
            ns = NOTIFICATION_SEVERITY::ERROR;
            break;
        case MODEL_SEVERITY::WARNING:
            ns = NOTIFICATION_SEVERITY::WARNING;
            break;
        case MODEL_SEVERITY::INFO:
            ns = NOTIFICATION_SEVERITY::INFO;
            break;
        default:
            break;
    }

    NotificationManager::manager()->displayNotification(description, "Icons", "medeaLogo", ID, ns, NOTIFICATION_TYPE::MODEL, NOTIFICATION_CATEGORY::NOCATEGORY);
}

void ViewController::setControllerReady(bool ready)
{
    if(_controllerReady != ready){
        _controllerReady = ready;
        emit vc_controllerReady(ready);
        emit vc_ActionFinished();
    }
}

void ViewController::ModelControllerReady(bool ready)
{
    setControllerReady(ready);
    if(ready){
        emit vc_showWelcomeScreen(false);
        fitAllViews();
    }
}

void ViewController::openURL(QString url)
{
    _showWebpage(url);
}

void ViewController::deleteSelection()
{
    if(selectionController){
        emit vc_deleteEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::expandSelection()
{
    if(selectionController && selectionController->getSelectionCount() > 0){
        emit vc_triggerAction("Expand Selection");
        foreach(int ID, selectionController->getSelectionIDs()){
            emit vc_setData(ID, "isExpanded", true);
        }
    }
}

void ViewController::contractSelection()
{
    if(selectionController && selectionController->getSelectionCount() > 0){
        emit vc_triggerAction("Expand Selection");
        foreach(int ID, selectionController->getSelectionIDs()){
            emit vc_setData(ID, "isExpanded", false);
        }
    }
}

void ViewController::editLabel()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        emit vc_editTableCell(item->getID(), "label");
    }
}

void ViewController::editReplicationCount()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        emit vc_editTableCell(item->getID(), "replicate_count");
    }
}


void ViewController::constructDDSQOSProfile()
{
    foreach(ViewItem* item, getItemsOfKind(NODE_KIND::ASSEMBLY_DEFINITIONS)){
        if(item){
            emit vc_constructNode(item->getID(), NODE_KIND::QOS_DDS_PROFILE);
        }
    }
}


void ViewController::TeardownController()
{   
    if (controller) {
        setControllerReady(false);
        emit selectionController->clearSelection();

        emit vc_projectPathChanged("");
        emit mc_projectModified(false);
        destructViewItem(rootItem);
        nodeKindLookups.clear();
        edgeKindLookups.clear();

        //This will destruct!
        disconnect(controller);
        controller->disconnect(this);
        emit controller->InitiateTeardown();
        controller = 0;
        // reset the notification manager
        NotificationManager::manager()->clearModelNotifications();
    }
}


bool ViewController::_newProject(QString file_path)
{
    if(_closeProject()){
        if(!controller){
            initializeController();
            emit vc_SetupModelController(file_path);
            return true;
        }
    }
    return false;
}

bool ViewController::_saveProject()
{
    if(controller){
        QString filePath = controller->getProjectPath();

        if(filePath.isEmpty()){
            return _saveAsProject();
        }else{
            QString data = controller->getProjectAsGraphML();

            if(FileHandler::writeTextFile(filePath, data)){
                emit vc_projectSaved(filePath);
                emit vc_addProjectToRecentProjects(filePath);
            }
            return true;
        }
    }
    return false;
}

bool ViewController::_saveAsProject()
{
    if(controller){
        QString fileName = FileHandler::selectFile("Select a *.graphml file to save project as.", QFileDialog::AnyFile, true, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
        if(!fileName.isEmpty()){
            controller->setProjectPath(fileName);
            return _saveProject();
        }
    }
    return false;
}

bool ViewController::_closeProject(bool show_welcome)
{
    if(controller && controller->isProjectSaved()){
        auto file_path = controller->getProjectPath();

        if(file_path == ""){
            file_path = "Untitled Project";
        }

        //Ask User to confirm save?
        QMessageBox msgBox(QMessageBox::Question, "Save Changes",
                            "Do you want to save the changes made to '" + file_path + "'?",
                            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                            WindowManager::manager()->getMainWindow());
        msgBox.setIconPixmap(Theme::theme()->getImage("Icons", "floppyDisk", QSize(50,50), Theme::theme()->getMenuIconColor()));
        msgBox.setButtonText(QMessageBox::Yes, "Save");
        msgBox.setButtonText(QMessageBox::No, "Ignore");

        int buttonPressed = msgBox.exec();

        if(buttonPressed & QMessageBox::Yes){
            if(!_saveProject()){
                // if failed to save, don't exit!
                return false;
            }
        }else if(buttonPressed & QMessageBox::Cancel){
            return false;
        }
    }
    if(show_welcome){
        emit vc_showWelcomeScreen(true);
    }
    TeardownController();
    return true;
}
QList<ViewItem *> ViewController::getItemsOfKind(EDGE_KIND kind)
{
    QList<ViewItem*> items;
    foreach(int ID, edgeKindLookups.values(kind)){
        ViewItem* item = getViewItem(ID);
        if(item && item->isEdge()){
            items.append(item);
        }
    }
    return items;
}

QList<ViewItem *> ViewController::getItemsOfKind(NODE_KIND kind)
{
    QList<ViewItem*> items;
    foreach(int ID, nodeKindLookups.values(kind)){
        ViewItem* item = getViewItem(ID);
        if(item && item->isNode()){
            items.append(item);
        }
    }
    return items;
}

VIEW_ASPECT ViewController::getNodeViewAspect(int ID){
    VIEW_ASPECT aspect;
    if(controller){
        aspect = controller->getNodeViewAspect(ID);
    }
    return aspect;
}

bool ViewController::isNodeAncestor(int ID, int ID2){
    bool is_ancestor = false;
    if(controller){
        is_ancestor = controller->isNodeAncestor(ID, ID2);
    }
    return is_ancestor;
}


ModelController* ViewController::getModelController(){
    return controller;    
}

QStringList ViewController::getEntityKeys(int ID){
    QStringList keys;
    if(controller){
        keys = controller->getEntityKeys(ID);
    }
    return keys;
}

QVariant ViewController::getEntityDataValue(int ID, QString key_name){
    QVariant data;
    if(controller){
        data = controller->getEntityDataValue(ID, key_name);
    }
    //qCritical() << "Data for: " << ID << " KEY : " << key_name << " = " << data;
    return data;
}

void ViewController::model_EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id){
    
    auto src = getNodeViewItem(src_id);
    auto dst = getNodeViewItem(dst_id);
    auto parent = getSharedParent(src, dst);
    if(src && dst && parent){
        auto edge = new EdgeViewItem(this, id, src, dst, kind);
        
        //Fill with Data
        foreach(QString key, getEntityKeys(id)){
            //qCritical() << "Updating Data: " << key;
            edge->changeData(key, getEntityDataValue(id, key));
        }

        edgeKindLookups.insertMulti(kind, id);

        if(parent){
            parent->addChild(edge);
        }else{
            rootItem->addChild(edge);
            topLevelItems.append(id);
        }

        src->addEdgeItem(edge);
        dst->addEdgeItem(edge);

         //Insert into map
        viewItems[id] = edge;
        setDefaultIcon(edge);

        connect(edge->getTableModel(), &DataTableModel::req_dataChanged, this, &ViewController::table_dataChanged);
        //Tell Views
        emit vc_viewItemConstructed(edge);
    }
}

void ViewController::highlight(QList<int> ids){
    for(auto id : ids){
        emit vc_highlightItem(id, true);
    }
}
void ViewController::model_NodeConstructed(int parent_id, int id, NODE_KIND kind){
    //Construct a basic item
    NodeViewItem* item = new NodeViewItem(this, id, kind);

    //Fill with Data
    foreach(QString key, getEntityKeys(id)){
        //qCritical() << "Updating Data: " << key;
        item->changeData(key, getEntityDataValue(id, key));
    }

    //Get our parent
    auto parent = getNodeViewItem(parent_id);
    
    
    if(parent){
        parent->addChild(item);
    }else{
        rootItem->addChild(item);
        topLevelItems.append(id);
    }
    nodeKindLookups.insertMulti(kind, id);

    //Insert into map
    viewItems[id] = item;
    setDefaultIcon(item);
    connect(item->getTableModel(), &DataTableModel::req_dataChanged, this, &ViewController::table_dataChanged);
    //Tell Views
    emit vc_viewItemConstructed(item);
}

void ViewController::controller_entityDestructed(int ID, GRAPHML_KIND)
{
    auto view_item = getViewItem(ID);
    destructViewItem(view_item);
}

void ViewController::controller_dataChanged(int ID, QString key, QVariant data)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->changeData(key, data);
    }
}

void ViewController::controller_dataRemoved(int ID, QString key)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        viewItem->removeData(key);
    }
}


bool ViewController::isNodeOfType(int ID, NODE_TYPE type){
    bool is_type = false;
    if(controller){
        is_type = controller->isNodeOfType(ID, type); 
    }
    return is_type;
}
int ViewController::getNodeParentID(int ID){
    int parent_id = -1;
    if(controller){
        parent_id = controller->getNodeParentID(ID);
    }
    return parent_id;

}


void ViewController::setClipboardData(QString data)
{
    QApplication::clipboard()->setText(data);
}

void ViewController::newProject()
{
    _newProject();
}

bool ViewController::OpenProject()
{
    auto file_path = FileHandler::selectFile("Select Project to Open", QFileDialog::ExistingFile, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);    
    return OpenExistingProject(file_path);
}

bool ViewController::OpenExistingProject(QString file_path)
{
    if(file_path != ""){
        return _newProject(file_path);
    }
    return false;
}


void ViewController::importProjects()
{
    _importProjects();
}

void ViewController::importXMEProject()
{
    QStringList files = FileHandler::selectFiles("Select an XME File to import.", QFileDialog::ExistingFile, false, GME_FILE_EXT, GME_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmePath = files.first();
        QFile file(xmePath);
        QFileInfo fileInfo = QFileInfo(file);
        QString tempFile = FileHandler::getTempFileName(fileInfo.baseName() + "_FromXME.graphml");
        emit mc_showProgress(true, "Transforming XME Project");
        emit mc_progressChanged(-1);
        emit vc_importXMEProject(xmePath, tempFile);
    }
}

void ViewController::importXMIProject()
{
    QStringList files = FileHandler::selectFiles("Select an XMI File to import.", QFileDialog::ExistingFile, false, XMI_FILE_EXT, XMI_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmiPath = files.first();
        emit vc_importXMIProject(xmiPath);
    }
}

void ViewController::importIdlFile()
{
    QStringList files = FileHandler::selectFiles("Select an IDL File to import.", QFileDialog::ExistingFile, false, IDL_FILE_EXT, IDL_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmiPath = files.first();

        QtConcurrent::run([this, xmiPath]{
            QString data = QString::fromStdString(IdlParser::IdlParser::ParseIdl(xmiPath.toStdString(), true));
            emit vc_importProjects(QStringList(data));
        });
    }
}


void ViewController::closeProject()
{
    _closeProject(true);
}

void ViewController::closeMEDEA()
{
    if(_closeProject(true)){
        //Destruct main window
        WindowManager::teardown();
    }
}

void ViewController::generateWorkspace()
{
    QString file_path = getTempFileForModel();
    QString output_dir = FileHandler::selectFile("Select an folder to create workspace in.", QFileDialog::Directory, false);
    execution_manager->GenerateWorkspace(file_path, output_dir);
}


void ViewController::executeJenkinsJob()
{
    if(controller){
        QString data = controller->getProjectAsGraphML();
        if(!data.isEmpty()){
            QString filePath = FileHandler::writeTempTextFile(data, ".graphml");
            emit vc_executeJenkinsJob(filePath);
        }
    }
}

void ViewController::saveProject()
{
    _saveProject();
}

void ViewController::saveAsProject()
{
    _saveAsProject();
}

void ViewController::_importProjects()
{
    QStringList files = FileHandler::selectFiles("Select graphML File(s) to import.", QFileDialog::ExistingFiles, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
    _importProjectFiles(files);
}

void ViewController::_importProjectFiles(QStringList files)
{

    QStringList fileData;
    foreach (QString file, files) {
        QString data = FileHandler::readTextFile(file);
        if(data != ""){
            fileData.append(data);
        }
    }
    if(!fileData.isEmpty()){
        // fit the contents in all the view aspects after import when no model has been imported yet?
        emit vc_importProjects(fileData);
    }
}

void ViewController::fitView()
{
    NodeView* view = getActiveNodeView();
    if(view){
        view->fitToScreen();
    }
}

void ViewController::fitAllViews()
{

    emit vc_fitToScreen();
}

void ViewController::centerSelection()
{
    NodeView* view = getActiveNodeView();
    if(view){
        view->centerSelection();
    }
}

void ViewController::alignSelectionVertical()
{
    NodeView* view = getActiveNodeView();
    if(view){
        view->alignVertical();
    }
}

void ViewController::alignSelectionHorizontal()
{
    NodeView* view = getActiveNodeView();
    if(view){
        view->alignHorizontal();
    }
}

void ViewController::selectAndCenterConnectedEntities()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        emit vc_selectAndCenterConnectedEntities(item);
    }
}


void ViewController::centerOnID(int ID)
{
    emit vc_centerItem(ID);
}

void ViewController::showWiki()
{
    _showWiki(0);
}

void ViewController::reportBug()
{
    _showGitHubPage("issues/");
}

void ViewController::showWikiForSelectedItem()
{
    _showWiki(getActiveSelectedItem());

}

void ViewController::centerImpl()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        NodeViewItem* impl = getNodesImpl(item->getID());
        if(impl){
            emit vc_centerItem(impl->getID());
        }
    }
}

void ViewController::centerDefinition()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        NodeViewItem* def = getNodesDefinition(item->getID());
        if(def){
            emit vc_centerItem(def->getID());
        }
    }
}

void ViewController::popupImpl()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        NodeViewItem* impl = getNodesImpl(item->getID());
        if(impl){
            spawnSubView(impl);
        }
    }
}

void ViewController::popupDefinition()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        NodeViewItem* def = getNodesDefinition(item->getID());
        if(def){
            spawnSubView(def);
        }
    }
}

void ViewController::popupSelection()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        spawnSubView(item);
    }
}

void ViewController::popupItem(int ID)
{
    ViewItem* item = getViewItem(ID);
    if(item){
        spawnSubView(item);
    }
}

void ViewController::aboutQt()
{
    BaseWindow* window = WindowManager::manager()->getActiveWindow();
    QMessageBox::aboutQt(window);
}

void ViewController::aboutMEDEA()
{
    QString aboutString =
    "<h3>MEDEA " % APP_VERSION() % "</h3>"
    "<a href=\"" % APP_URL() % "\" style=\"color:" % Theme::theme()->getHighlightColorHex() %";\">Center for Distributed and Intelligent Systems - Model Analysis</a><br />"
    "The University of Adelaide<hr /><br />"
    "Team:"
    "<ul>"
    "<li>Dan Fraser (Lead Programmer)</li>"
    "<li>Cathlyn Aston (UX Programmer)</li>"
    "<li>Mitchell Conrad (Programmer)</li>"
    "<li>Jackson Michael (Workers)</li>"
    "</ul>"
    "Past Members:"
    "<ul>"
    "<li>Marianne Rieckmann</li>"
    "<li>Matthew Hart</li>"
    "</ul>";
    BaseWindow* window = WindowManager::manager()->getActiveWindow();
    QMessageBox::about(window, "About MEDEA " % APP_VERSION(), aboutString);
}


void ViewController::cut()
{
    if(selectionController){
        emit vc_cutEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::copy()
{
    if(selectionController){
        emit vc_copyEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::paste()
{
    if(selectionController && selectionController->getSelectionCount() == 1){
        emit vc_paste(selectionController->getSelectionIDs(), QApplication::clipboard()->text());
    }
}

void ViewController::replicate()
{
    if(selectionController && selectionController->getSelectionCount() > 0){
        emit vc_replicateEntities(selectionController->getSelectionIDs());
    }
}

void ViewController::initializeController()
{
    if(!controller){
        setControllerReady(false);
        controller = new ModelController();
        connectModelController(controller);
    }
}


bool ViewController::destructChildItems(ViewItem *parent)
{
    QVectorIterator<ViewItem*> it(parent->getDirectChildren());
    it.toBack();
    while(it.hasPrevious()){
        ViewItem* item = it.previous();
        destructViewItem(item);
    }
    return true;
}

bool ViewController::clearVisualItems()
{
    destructViewItem(rootItem);
    return true;
}

ViewItem *ViewController::getViewItem(int ID)
{
    if(viewItems.contains(ID)){
        return viewItems[ID];
    }
    return 0;
}
