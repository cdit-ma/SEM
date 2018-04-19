#include "viewcontroller.h"

#include "../WindowManager/windowmanager.h"
#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/DockWidgets/basedockwidget.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "../../Widgets/Dialogs/variabledialog.h"
#include "../../Views/ContextMenu/contextmenu.h"

#include "../../Views/NodeView/nodeview.h"
#include "../../Widgets/CodeEditor/codebrowser.h"

#include "../../Widgets/Monitors/jobmonitor.h"
#include "../../Widgets/Monitors/consolemonitor.h"
#include "../../Widgets/Monitors/jenkinsmonitor.h"

#include "../../Controllers/ExecutionManager/executionmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"
#include "../../Controllers/SearchManager/searchmanager.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"


#include "../../../modelcontroller/modelcontroller.h"
#include "../../../modelcontroller/entityfactory.h"
#include "../../../modelcontroller/version.h"

#include "../../Utils/filehandler.h"
#include "../../../idlparser/idlparser.h"

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
#include <QTextBrowser>
#include <QFile>
#include <QFutureWatcher>
#include <iostream>
#include <sstream>
#include <QJsonArray>

#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"
#define GME_FILE_EXT "GME Documents (*.xme)"
#define GME_FILE_SUFFIX ".xme"

#define XMI_FILE_EXT "UML XMI Documents (*.xml)"
#define XMI_FILE_SUFFIX ".xml"

#define IDL_FILE_EXT "IDL Documents (*.idl)"
#define IDL_FILE_SUFFIX ".idl"


ViewController::ViewController() : QObject(){
    rootItem = new ViewItem(this, GRAPHML_KIND::NONE);

    //Setup nodes
    setupEntityKindItems();

    //Initialize Settings
    SettingsController::initializeSettings();

    NotificationManager::construct_singleton(this);
    SearchManager::construct_singleton(this);
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);
    menu = new ContextMenu(this);
    

    jenkins_manager = new JenkinsManager(this);
    execution_manager = new ExecutionManager(this);



    
    connect(jenkins_manager, &JenkinsManager::JobQueued, this, &ViewController::RefreshExecutionMonitor);
    
    

    connect(execution_manager, &ExecutionManager::GotCodeForComponent, this, &ViewController::showCodeViewer);
    connect(this, &ViewController::vc_showToolbar, menu, &ContextMenu::popup);

    connect(actionController->edit_search, &QAction::triggered, SearchManager::manager(), &SearchManager::PopupSearch);

    //setup auto save
    autosave_timer_ = new QTimer(this);
    autosave_timer_->setInterval(60000);
    autosave_timer_->start();
    connect(autosave_timer_, &QTimer::timeout, this, &ViewController::autoSaveProject);
}

QList<ViewItem*> ViewController::ToViewItemList(QList<NodeViewItem*> &items){
    return *(QList<ViewItem*>*)(&items);
}
QList<ViewItem*> ViewController::ToViewItemList(QList<EdgeViewItem*> &items){
    return *(QList<ViewItem*>*)(&items);
}

ContextMenu* ViewController::getContextMenu(){
    return menu;
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
    connect(this, &ViewController::vc_constructEdges, controller, &ModelController::constructEdges);
    
    connect(this, &ViewController::vc_destructEdges, controller, &ModelController::destructEdges);
    connect(this, &ViewController::vc_destructAllEdges, controller, &ModelController::destructAllEdges);
    connect(this, &ViewController::vc_constructConnectedNode, controller, &ModelController::constructConnectedNode);
    connect(this, &ViewController::vc_projectSaved, controller, &ModelController::setProjectSaved);
    connect(this, &ViewController::vc_undo, controller, &ModelController::undo);
    connect(this, &ViewController::vc_redo, controller, &ModelController::redo);
    connect(this, &ViewController::vc_triggerAction, controller, &ModelController::triggerAction);
    connect(this, &ViewController::vc_cutEntities, controller, &ModelController::cut);
    connect(this, &ViewController::vc_copyEntities, controller, &ModelController::copy);
    connect(this, &ViewController::vc_paste, controller, &ModelController::paste);
    connect(this, &ViewController::vc_replicateEntities, controller, &ModelController::replicate);
    connect(this, &ViewController::vc_deleteEntities, controller, &ModelController::remove);
   
    controller = c;
}

void ViewController::RequestJenkinsNodes(){
    auto request = jenkins_manager->getJenkinsNodes();

    if(request.first){
        auto future_watcher = new QFutureWatcher<QString>(this);
        connect(future_watcher, &QFutureWatcher<QString>::finished, [=](){
            auto graphml = future_watcher->result();
            jenkinsManager_GotJenkinsNodesList(graphml);
        });
        future_watcher->setFuture(request.second);
    }
}

void ViewController::ListJenkinsJobs(){
    auto request = jenkins_manager->GetJenkinsConfiguration();

    auto list_jobs_future = new QFutureWatcher<QJsonDocument>(this);
    connect(list_jobs_future, &QFutureWatcher<QJsonDocument>::finished, [=](){
        auto json = list_jobs_future->result();
        QList<QString> jobs;

        if(!json.isNull()){
            auto config = json.object();
            
            //Check through the list of jobs to 
            for(auto job : config["jobs"].toArray()){
                auto job_name = job.toObject()["name"].toString();
                jobs += job_name;
            }
        }
        ShowJenkinsBuildDialog(jobs);
    });

    list_jobs_future->setFuture(request.second);
}

void ViewController::ShowJenkinsBuildDialog(QStringList job_names){
    VariableDialog dialog("Launch Jenkins Job");
    dialog.addOption("Job", SETTING_TYPE::STRINGLIST, job_names);

    auto options = dialog.getOptions();
    if(options.size()){
        auto job_name = dialog.getOptionValue("Job").toString();
        RequestJenkinsBuildJobName(job_name);
    }
}

void ViewController::ShowJenkinsBuildDialog(QString job_name, QList<Jenkins_Job_Parameter> parameters){
    VariableDialog dialog("Jenkins: " + job_name + " Parameters");

    for(auto parameter : parameters){
        auto default_value = parameter.defaultValue;
        bool is_file_model = parameter.name == "model" && (parameter.type == SETTING_TYPE::FILE);
        
        if(is_file_model){
            default_value = getTempFileForModel();
        }

        dialog.addOption(parameter.name, parameter.type, default_value);
        dialog.setOptionIcon(parameter.name, "Icons", "label");
        //Disable model upload
        dialog.setOptionEnabled(parameter.name, !is_file_model);
    }

    auto options = dialog.getOptions();
    auto got_options = options.size() == parameters.size();

    if(got_options){
        //Update the parameters
        for(auto& parameter : parameters){
            parameter.value = dialog.getOptionValue(parameter.name).toString();
        }
        jenkins_manager->BuildJob(job_name, parameters);
    }
}
void ViewController::RequestJenkinsBuildJob(){
    RequestJenkinsBuildJobName(jenkins_manager->GetJobName());
}


void ViewController::RequestJenkinsBuildJobName(QString job_name){
    auto request = jenkins_manager->GetJobParameters(job_name);

    auto future_watcher = new QFutureWatcher<QList<Jenkins_Job_Parameter> >(this);
    connect(future_watcher, &QFutureWatcher<QList<Jenkins_Job_Parameter> >::finished, [=](){
        auto parameters = future_watcher->result();
        ShowJenkinsBuildDialog(job_name, parameters);
    });
    future_watcher->setFuture(request.second);
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


bool ViewController::isWelcomeScreenShowing(){
    return showingWelcomeScreen;
}


void ViewController::welcomeScreenToggled(bool visible){
    showingWelcomeScreen = visible;
    auto notification_manager = NotificationManager::manager();
    if(visible){
        notification_manager->hideToast();
    }else{
        //Toast the notification
        QTimer::singleShot(100, notification_manager, &NotificationManager::toastLatestNotification);
    }
}

QList<ViewItem *> ViewController::getViewItemParents(QList<ViewItem*> entities)
{
    QSet<ViewItem*> parent_set;

    for(auto view_item : entities){
        if(view_item){
            parent_set.insert(view_item->getParentItem());
        }
    }
    auto parent_list = parent_set.toList();
    std::sort(parent_list.begin(), parent_list.end(), ViewItem::SortByLabel);
    return parent_list;
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

QHash<EDGE_DIRECTION, ViewItem*> ViewController::getValidEdges2(EDGE_KIND kind){
    QHash<EDGE_DIRECTION, ViewItem*>  items;
    if(selectionController && controller){
        auto selection = selectionController->getSelectionIDs();
        auto id_map = controller->getConnectableNodeIds2(selection, kind);
        
        for(auto direction : id_map.uniqueKeys()){
            for(auto id : id_map.values(direction)){
                auto view_item = getViewItem(id);
                if(view_item){
                    items.insertMulti(direction, view_item);
                }
            }
        }
    }
    return items;
}

QStringList getSearchableKeys(){
    return {"label", "description", "kind", "namespace", "type", "value", "ID"};
};

QList<ViewItem*> ViewController::getSearchableEntities(){
    QList<ViewItem*> items;

    for(ViewItem* item : viewItems.values()){
        auto node_item = (NodeViewItem*) item;
        auto edge_item = (EdgeViewItem*) item;

        if(item->isNode()){
            if(node_item->isNodeOfType(NODE_TYPE::ASPECT)){
                continue;
            }
        }
        if(item->isInModel()){
            items.append(item);
        }
    }
    return items;
}

QStringList ViewController::_getSearchSuggestions()
{
    auto keys = getSearchableKeys();
    QSet<QString> suggestions;

    for(auto item : getSearchableEntities()){
        for(auto key : keys){
            if(item->hasData(key)){
                suggestions.insert(item->getData(key).toString());
            }
        }
    }
    return suggestions.toList();
}

QMap<QString, ViewItem *> ViewController::getSearchResults(QString query, QList<ViewItem*> view_items)
{
    auto keys = getSearchableKeys();
    QMap<QString, ViewItem*> results;
    
    QRegExp rx("*" + query + "*");
    rx.setPatternSyntax(QRegExp::WildcardUnix);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if(view_items.isEmpty()){
        view_items = getSearchableEntities();
    }
    for(auto item : view_items){
        for(auto key : keys){
            if(item->hasData(key)){
                auto data = item->getData(key).toString();
                if(rx.exactMatch(data)){
                    results.insertMulti(key, item);
                }
            }
        }
    }
    return results;
}

QList<ViewItem*> ViewController::filterList(QString query, QList<ViewItem*> view_items){
    if(view_items.isEmpty()){
        return view_items;
    }else{
        return getSearchResults(query, view_items).values().toSet().toList();
    }
}

ViewDockWidget *ViewController::constructViewDockWidget(QString label, QWidget* parent)
{
    auto dock_widget = WindowManager::manager()->constructViewDockWidget(label, parent);
    auto node_view = new NodeView(dock_widget);
    dock_widget->setWidget(node_view);
    
    //Setup NodeView
    node_view->setViewController(this);

    return (ViewDockWidget*)dock_widget;
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
NodeViewItem* ViewController::getNodeItem(NODE_KIND kind){
    return nodeKindItems.value(kind, 0);
}

QSet<NODE_KIND> ViewController::getAdoptableNodeKinds()
{
    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getAdoptableNodeKinds(ID);
    }
    return {NODE_KIND::NONE};
}

QList<NodeViewItem *> ViewController::getNodeKindItems()
{
    auto list = nodeKindItems.values();
    std::sort(list.begin(), list.end(), ViewItem::SortByLabel);
    return list;
}

QList<EdgeViewItem *> ViewController::getEdgeKindItems()
{
    auto list = edgeKindItems.values();
    std::sort(list.begin(), list.end(), ViewItem::SortByLabel);
    return list;
}

void ViewController::constructEdges(int id, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction){

    auto selection = getSelectionController()->getSelectionIDs();
    
    if(!selection.isEmpty()){
        auto id_list = {id};
        auto src_ids = edge_direction == EDGE_DIRECTION::SOURCE ? id_list : selection;
        auto dst_ids = edge_direction == EDGE_DIRECTION::TARGET ? id_list : selection;
        emit vc_constructEdges(src_ids, dst_ids, edge_kind);
    }
}

QList<EDGE_KIND> ViewController::getValidEdgeKindsForSelection()
{
    QList<EDGE_KIND> edge_kinds;
    if(selectionController && controller){
        edge_kinds = controller->getValidEdgeKindsForSelection(selectionController->getSelectionIDs());
    }
    return edge_kinds;
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

        QString default_icon_prefix = "EntityIcons";
        QString default_icon_name = viewItem->getData("kind").toString();
        
        QString custom_icon_prefix = viewItem->getData("icon_prefix").toString();
        QString custom_icon_name = viewItem->getData("icon").toString();
        
        if(isNode){
            auto node_kind = nodeViewItem->getNodeKind();


            switch(node_kind){
            case NODE_KIND::HARDWARE_NODE:{
                auto os = viewItem->getData("os").toString();
                auto arch = viewItem->getData("architecture").toString();
                custom_icon_prefix = "EntityIcons";
                custom_icon_name = (os % "_" % arch);
                break;
            }
            case NODE_KIND::OPENCL_PLATFORM:{
                auto vendor = viewItem->getData("vendor").toString();
                
                custom_icon_prefix = "Icons";
                
                if(vendor == "Advanced Micro Devices, Inc."){
                    custom_icon_name = "amd";
                }else if(vendor == "Altera Corporation"){
                    custom_icon_name = "intel";
                }else if(vendor == "NVIDIA Corporation"){
                    custom_icon_name = "nvidia";
                }
                
                break;
            }
            case NODE_KIND::OPENCL_DEVICE:{
                auto type = viewItem->getData("type").toString();
                
                custom_icon_prefix = "Icons";
                if(type == "CL_DEVICE_TYPE_CPU"){
                    custom_icon_name = "cpu";
                }else if(type == "CL_DEVICE_TYPE_GPU"){
                    custom_icon_name = "gpu";
                }
                break;
            }
            case NODE_KIND::VARIADIC_PARAMETER:{
                default_icon_prefix = "Icons";
                default_icon_name = "label";
                break;
            }
            case NODE_KIND::FOR_LOOP:{
                default_icon_prefix = "EntityIcons";
                default_icon_name = "WhileLoop";
                break;
            }
            case NODE_KIND::IF_CONDITION:
            case NODE_KIND::ELSEIF_CONDITION:
            case NODE_KIND::ELSE_CONDITION:{
                default_icon_prefix = "EntityIcons";
                default_icon_name = "Condition";
                break;
            }
            case NODE_KIND::DEPLOYMENT_ATTRIBUTE:
            case NODE_KIND::VARIABLE_PARAMETER:{
                default_icon_prefix = "EntityIcons";
                default_icon_name = "Variable";
                break;
            }
            case NODE_KIND::WORKER_DEFINITIONS:{
                default_icon_prefix = "Icons";
                default_icon_name = "medeaLogo";
                break;
            }
            case NODE_KIND::MODEL:
                default_icon_prefix = "Icons";
                default_icon_name = "medeaLogo";
                break;
            case NODE_KIND::EXTERNAL_TYPE:
                default_icon_prefix = "Icons";
                default_icon_name = "translate";
                break;
            case NODE_KIND::FUNCTION:
            case NODE_KIND::WORKER_FUNCTIONCALL:{
                default_icon_prefix = "EntityIcons";
                default_icon_name = "WorkerProcess";
                break;
            }
            case NODE_KIND::WORKER_INSTANCE:{
                default_icon_prefix = "EntityIcons";
                default_icon_name = "ManagementComponent";
                break;
            }
            case NODE_KIND::CLASS:
            case NODE_KIND::CLASS_INSTANCE:{
                default_icon_prefix = "Icons";
                default_icon_name = "circleStarDark";
                break;
            }
            case NODE_KIND::ENUM_INSTANCE:
            case NODE_KIND::ENUM:{
                default_icon_prefix = "Icons";
                default_icon_name = "circleCirclesDark";
                break;
            }
            case NODE_KIND::ENUM_MEMBER:{
                default_icon_prefix = "Icons";
                default_icon_name = "circleDark";
                break;
            }
            default:
                break;
            }
        }else if(isEdge){
            switch(edgeViewItem->getEdgeKind()){
                case EDGE_KIND::DEFINITION:{
                    default_icon_prefix = "Icons";
                    default_icon_name = "gears";
                }
                break;
            default:
                break;
            }
        }

        ;
        auto got_icon = false;
        if(Theme::theme()->gotImage(default_icon_prefix, default_icon_name)){
            Theme::theme()->getIcon(default_icon_prefix, default_icon_name);
            viewItem->setIcon(default_icon_prefix, default_icon_name);
            got_icon = true;
        }

        if(Theme::theme()->gotImage(custom_icon_prefix, custom_icon_name)){
            Theme::theme()->getIcon(custom_icon_prefix, custom_icon_name);
            viewItem->setIcon(custom_icon_prefix, custom_icon_name);
            got_icon = true;
        }

        if(!got_icon){
            viewItem->setIcon("Icons", "circleQuestion");
        }
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

DefaultDockWidget* ViewController::constructDockWidget(QString title, QString icon_path, QString icon_name, QWidget* widget, BaseWindow* window){
    auto window_manager = WindowManager::manager();
    if(!window){
        window = window_manager->getActiveWindow();
    }
    auto dock_widget = window_manager->constructDockWidget(title, window);
    dock_widget->setCloseVisible(false);

    

    Theme::theme()->setWindowIcon(title, icon_path, icon_name);

    dock_widget->setWidget(widget);
    dock_widget->setIcon("WindowIcon", title);
    dock_widget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

    
    window->addDockWidget(dock_widget);

    return dock_widget;
}

void ViewController::showCodeViewer(QString tabName, QString content)
{   
    if(!codeViewer){
        codeBrowser = new CodeBrowser();
        codeViewer = constructDockWidget("Code Browser", "Icons", "bracketsAngled", codeBrowser);
    }
    if(codeViewer && codeBrowser){
        codeBrowser->showCode(tabName, content, false);
        WindowManager::ShowDockWidget(codeViewer);
    }
}

JobMonitor* ViewController::getExecutionMonitor(){
    if(!job_monitor){
        job_monitor = new JobMonitor(jenkins_manager);
        auto toolbar = job_monitor->getToolbar();
        toolbar->addAction(actionController->model_executeLocalJob);
        toolbar->addAction(actionController->jenkins_executeJob);
    }
    return job_monitor;
}

void ViewController::incrementSelectedKey(QString key_name){
    ViewItem* item = getActiveSelectedItem();
    if(item && item->hasData(key_name) && !item->isDataProtected(key_name)){
        auto data = item->getData(key_name).toInt();
        emit vc_triggerAction(key_name + " Changed");
        emit vc_setData(item->getID(), key_name, data + 1);
    }
}
void ViewController::decrementSelectedKey(QString key_name){
    ViewItem* item = getActiveSelectedItem();
    if(item && item->hasData(key_name) && !item->isDataProtected(key_name)){
        auto data = item->getData(key_name).toInt();
        if(data > 0){
            emit vc_triggerAction(key_name + " Changed");
            emit vc_setData(item->getID(), key_name, data - 1);
        }
    }
}

void ViewController::RefreshExecutionMonitor(QString job_name){
    showExecutionMonitor();
    job_monitor->refreshRecentBuildsByName(job_name);
}

void ViewController::showExecutionMonitor(){
    auto window_manager = WindowManager::manager();

    //First time 
    if(!execution_monitor){
        auto monitor = getExecutionMonitor();
        execution_monitor = constructDockWidget("Execution Monitor", "Icons", "bracketsAngled", monitor);
    }
    WindowManager::ShowDockWidget(execution_monitor);
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

    constructableNodes.removeAll(NODE_KIND::BLACKBOX);
    constructableNodes.removeAll(NODE_KIND::BLACKBOX_INSTANCE);

    for(auto kind : constructableNodes){
        QString label = EntityFactory::getNodeKindString(kind);

        auto item = new NodeViewItem(this, kind, label);
        setDefaultIcon(item);
        nodeKindItems[kind] = item;
    }

    

    for(auto kind : EntityFactory::getEdgeKinds()){
        auto label = EntityFactory::getEdgeKindString(kind);
        //Trim the Edge_ from the label
        label.remove(0, 5);
        auto item = new EdgeViewItem(this, kind, label);
        setDefaultIcon(item);

        edgeKindItems[kind] = item;
    }

    //Sort the lists to be alphabetical
    //std::sort(nodeKindItems.begin(), nodeKindItems.end(), ViewItem::SortByLabel);
    //std::sort(edgeKindItems.begin(), edgeKindItems.end(), ViewItem::SortByLabel);
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
    QString wikiURL = SettingsController::settings()->getSetting(SETTINGS::GENERAL_MEDEA_WIKI_URL).toString();
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
        auto window_manager = WindowManager::manager();
        auto dockWidget = window_manager->getViewDockWidget(item);
        if(dockWidget){
            dockWidget->setVisible(true);
        }else{
            auto label = item->getData("label").toString();
            auto parent = window_manager->getMainWindow();
            dockWidget = constructViewDockWidget(label, parent);
            dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

            //Try and reparent it
            if(window_manager->reparentDockWidget(dockWidget)){
                //Set the icon
                dockWidget->setIcon(item->getIcon());
                //Set the NodeView to be contained on this NodeViewItem
                dockWidget->getNodeView()->setContainedNodeViewItem((NodeViewItem*)item);
                //Fit the contents of the dockwidget to screen
                dockWidget->getNodeView()->fitToScreen();
            }else{
                WindowManager::manager()->destructDockWidget(dockWidget);
            }
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

    for(auto dock : WindowManager::manager()->getViewDockWidgets()){
        if(dock){
            auto node_view = dock->getNodeView();
            if(node_view && node_view->getIDsInView().contains(ID)){
                nodeViews.append(node_view);
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
    auto dock = WindowManager::manager()->getActiveViewDockWidget();
    if(dock){
        return dock->getNodeView();
    }
    return 0;
}

void ViewController::modelNotification(MODEL_SEVERITY severity, QString title, QString description, int ID){
    Notification::Severity ns = Notification::Severity::INFO;
    switch(severity){
        case MODEL_SEVERITY::ERROR:
            ns = Notification::Severity::ERROR;
            break;
        case MODEL_SEVERITY::WARNING:
            ns = Notification::Severity::WARNING;
            break;
        case MODEL_SEVERITY::INFO:
            ns = Notification::Severity::INFO;
            break;
        default:
            break;
    }
    auto notification = NotificationManager::manager()->AddNotification(title, "Icons", "dotsInRectangle", ns, Notification::Type::MODEL, Notification::Category::NONE, true, ID);
    notification->setDescription(description);
}

void ViewController::setControllerReady(bool ready)
{
    if(_controllerReady != ready){
        _controllerReady = ready;
        emit vc_controllerReady(ready);
        emit vc_ActionFinished();
    }

    if(ready){
        //Reset the autosave id
        autosave_id_ = 0;
        autosave_timer_->start();
    }else{
        autosave_timer_->stop();
    }
}

void ViewController::ModelControllerReady(bool ready)
{
    setControllerReady(ready);
    if(ready){
        emit vc_showWelcomeScreen(false);
        //Call this after everything has loaded
        QMetaObject::invokeMethod(this, "fitAllViews", Qt::QueuedConnection);
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
    QMutexLocker locker(&mutex);
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


        auto manager = NotificationManager::manager();
        
        // Clear previous validation notification items
        for (auto notification : manager->getNotificationsOfType(Notification::Type::MODEL)) {
            manager->deleteNotification(notification->getID());
        }
    }
}

void ViewController::autoSaveProject(){
    //Try and lock the mutex, if we can't lock
    if(mutex.tryLock()){
        if(controller && !controller->isProjectSaved()){
            auto project_action_count = controller->getProjectActionCount();
            auto project_path = controller->getProjectPath();
            auto is_project_an_autosave = FileHandler::isAutosaveFilePath(project_path);
            
            if((project_action_count > autosave_id_) && !is_project_an_autosave &&  !project_path.isEmpty()){
                //Get the data from the controller
                auto autosave_data = controller->getProjectAsGraphML();
                auto autosave_path = FileHandler::getAutosaveFilePath(project_path);
                if(FileHandler::writeTextFile(autosave_path, autosave_data, false)){
                    //Display a notification of the autosave
                    NotificationManager::manager()->AddNotification("Auto-saved '" + autosave_path + "'", "Icons", "clockCycle", Notification::Severity::INFO, Notification::Type::APPLICATION, Notification::Category::FILE);
                    //update the autosave id
                    autosave_id_ = project_action_count;
                    emit vc_addProjectToRecentProjects(autosave_path);
                }
            }
        }
        mutex.unlock();
    }else{
        //Try to autosave in 1000 ms
        QTimer::singleShot(1000, this, &ViewController::autoSaveProject);
    }
}

bool ViewController::_newProject(QString file_path)
{
    if(_closeProject()){
        if(!controller){
            initializeController();
            emit vc_SetupModelController(file_path);

            if(!file_path.isEmpty()){
                emit vc_addProjectToRecentProjects(file_path); 
            }
            return true;
        }
    }
    return false;
}

bool ViewController::_saveProject()
{
    if(controller){
        QString filePath = controller->getProjectPath();
        bool is_autosaved = FileHandler::isAutosaveFilePath(filePath);
        
        if(is_autosaved){
            return _saveAsProject(FileHandler::getFileFromAutosavePath(filePath));
        }else if(filePath.isEmpty()){
            return _saveAsProject();
        }else{
            QString data = controller->getProjectAsGraphML();

            if(FileHandler::writeTextFile(filePath, data)){
                emit vc_projectSaved(filePath);
                emit vc_addProjectToRecentProjects(filePath);
            }

            auto autosave_path = FileHandler::getAutosaveFilePath(filePath);
            //Remove the autosave
            if(FileHandler::removeFile(autosave_path)){
                emit vc_removeProjectFromRecentProjects(autosave_path);
            }
            return true;
        }
    }
    return false;
}

bool ViewController::_saveAsProject(QString file_path)
{
    if(controller){
        QString fileName = FileHandler::selectFile(WindowManager::manager()->getMainWindow(), "Select a *.graphml file to save project as.", QFileDialog::AnyFile, true, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX, file_path);
        if(!fileName.isEmpty()){
            controller->setProjectPath(fileName);
            return _saveProject();
        }
    }
    return false;
}

bool ViewController::_closeProject(bool show_welcome)
{
    {
        //Mutex lock things
        QMutexLocker locker(&mutex);
        if(controller && !controller->isProjectSaved()){
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
    // () << "Data for: " << ID << " KEY : " << key_name << " = " << data;
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

        SetParentNode(parent, edge);

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

void ViewController::SetParentNode(ViewItem* parent, ViewItem* child){
    if(!parent){
        parent = rootItem;
        topLevelItems.append(child->getID());
    }

    if(parent){
        parent->addChild(child);
    }
}

void ViewController::model_NodeConstructed(int parent_id, int id, NODE_KIND kind){
    //Construct a basic item
    NodeViewItem* item = new NodeViewItem(this, id, kind);


    for(const auto& key : getEntityKeys(id)){
        item->changeData(key, getEntityDataValue(id, key));
    }

    //Get our parent
    auto parent = getNodeViewItem(parent_id);


    nodeKindLookups.insertMulti(kind, id);
    //Insert into map
    viewItems[id] = item;
    setDefaultIcon(item);
    connect(item->getTableModel(), &DataTableModel::req_dataChanged, this, &ViewController::table_dataChanged);
    

    SetParentNode(parent, item);

    //Tell Views
    emit vc_viewItemConstructed(item);
}

void ViewController::controller_entityDestructed(int ID, GRAPHML_KIND)
{
    auto view_item = getViewItem(ID);
    //qCritical() << "DESTRUCT #" << ID << " " <<view_item->getData("kind").toString() << " = " << view_item->getData("label").toString();
    destructViewItem(view_item);
}

void ViewController::controller_dataChanged(int ID, QString key, QVariant data)
{
    ViewItem* viewItem = getViewItem(ID);

    if(viewItem){
        //qCritical() << "== REPLY: " << ID << " KEY: " << key << " = " << data;
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
    auto file_path = FileHandler::selectFile(WindowManager::manager()->getMainWindow(), "Select Project to Open", QFileDialog::ExistingFile, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);    
    return OpenExistingProject(file_path);
}

bool ViewController::OpenExistingProject(QString file_path)
{
    if(file_path != ""){
        //Handle the Autosave life.
        auto auto_save = FileHandler::getAutosaveFilePath(file_path);

        QFileInfo file_info(file_path);
        QFileInfo autosave_file_info(auto_save);

        if(autosave_file_info.exists() && autosave_file_info.lastModified() > file_info.lastModified()){
            //Ask User to confirm save?
            QMessageBox msgBox(QMessageBox::Question, "Open AutoSaved Project",
                                "An more recent autosaved backup of the selected project exists. '" + auto_save + "'",
                                QMessageBox::Yes | QMessageBox::No,
                                WindowManager::manager()->getMainWindow());

            msgBox.setIconPixmap(Theme::theme()->getImage("Icons", "clockCycle", QSize(50,50), Theme::theme()->getMenuIconColor()));
            msgBox.setButtonText(QMessageBox::Yes, "Open autosaved backup");
            msgBox.setButtonText(QMessageBox::No, "Open selected project");

            auto button_pressed = msgBox.exec();

            if(button_pressed == QMessageBox::Yes){
                file_path = auto_save;
            }
        }

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
    QStringList files = FileHandler::selectFiles(WindowManager::manager()->getMainWindow(), "Select an XME File to import.", QFileDialog::ExistingFile, false, GME_FILE_EXT, GME_FILE_SUFFIX);
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
    QStringList files = FileHandler::selectFiles(WindowManager::manager()->getMainWindow(), "Select an XMI File to import.", QFileDialog::ExistingFile, false, XMI_FILE_EXT, XMI_FILE_SUFFIX);
    if(files.length() == 1){
        QString xmiPath = files.first();
        emit vc_importXMIProject(xmiPath);
    }
}

void ViewController::importIdlFiles()
{
    auto idl_file_paths = FileHandler::selectFiles(WindowManager::manager()->getMainWindow(), "Select an IDL File to import.", QFileDialog::ExistingFiles, false, IDL_FILE_EXT, IDL_FILE_SUFFIX);
    if(idl_file_paths.length()){
        QtConcurrent::run([=]{
            // Construct a notification item with a loading gif as its icon
            auto notification = NotificationManager::manager()->AddNotification("Importing IDLs files ...", "Icons", "bracketsAngled", Notification::Severity::RUNNING, Notification::Type::MODEL, Notification::Category::FILE);
            
            std::vector<std::string> std_idl_files;
            for(auto idl_file : idl_file_paths){
                std_idl_files.push_back(idl_file.toStdString());
            }

            std::stringstream ss;
            //Redirect standard error to our string stream
            auto old_buffer = std::cerr.rdbuf(ss.rdbuf()); 
            auto results = IdlParser::ParseIdls(std_idl_files, true);

            auto idl_qstr = QString::fromStdString(results.second);
            //reset the old buffer
            std::cerr.rdbuf(old_buffer);
            auto error = QString::fromStdString(ss.str());
            
            notification->setTitle(results.first ? "Successfully Parsed " + QString::number(idl_file_paths.size()) + " IDLs" : "Failed to import IDLs");
            notification->setDescription(error);
            notification->setSeverity(results.first ? Notification::Severity::SUCCESS : Notification::Severity::ERROR);
            if(idl_qstr.length()){
                emit vc_importProjects({idl_qstr});
            }
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
    auto file_path = getTempFileForModel();
    auto output_dir = FileHandler::selectFile(WindowManager::manager()->getMainWindow(), "Select an folder to create workspace in.", QFileDialog::Directory, false);
    if(!output_dir.isEmpty()){
        execution_manager->GenerateWorkspace(file_path, output_dir);
    }
}

void ViewController::executeModelLocal()
{
    auto file_path = getTempFileForModel();
    auto temp_dir = FileHandler::getTempFileName("/");

    VariableDialog dialog("Local Deployment Options");
    dialog.addOption("Duration", SETTING_TYPE::INT, 60);
    dialog.setOptionIcon("Duration", "Icons", "clock");
    dialog.addOption("Workspace", SETTING_TYPE::PATH, temp_dir);
    dialog.setOptionIcon("Workspace", "Icons", "folder");
    auto options = dialog.getOptions();

    if(!options.isEmpty()){
        auto duration = options.value("Duration").toInt();
        auto workspace = options.value("Workspace").toString();
        //If starting a job was valid, connect in the monitor
        if(execution_manager->ExecuteModel(file_path, workspace, duration)){
            showExecutionMonitor();
            auto job_monitor = getExecutionMonitor();
            auto local_monitor = job_monitor->getConsoleMonitor("Local Deployment");
            if(!local_monitor){
                local_monitor = job_monitor->constructConsoleMonitor();
                connect(execution_manager, &ExecutionManager::GotProcessStdOutLine, local_monitor, &Monitor::AppendLine);
                connect(execution_manager, &ExecutionManager::GotProcessStdErrLine, local_monitor, &Monitor::AppendLine);
                connect(execution_manager, &ExecutionManager::ModelExecutionStateChanged, local_monitor, &Monitor::StateChanged);
                connect(local_monitor, &Monitor::Abort, execution_manager, &ExecutionManager::CancelModelExecution);
            }
            local_monitor->Clear();
            showExecutionMonitor();
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
    QStringList files = FileHandler::selectFiles(WindowManager::manager()->getMainWindow(), "Select graphML File(s) to import.", QFileDialog::ExistingFiles, false, GRAPHML_FILE_EXT, GRAPHML_FILE_SUFFIX);
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
    QMutexLocker locker(&mutex);
    if(!controller){
        setControllerReady(false);
        controller = new ModelController();
        connectModelController(controller);
    }
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
