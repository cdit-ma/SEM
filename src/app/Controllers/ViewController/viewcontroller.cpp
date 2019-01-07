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
//#include "../../Views/Table/popupdataeditor.h"

#include "../../Controllers/ExecutionManager/executionmanager.h"
#include "../../Controllers/JenkinsManager/jenkinsmanager.h"
#include "../../Controllers/SearchManager/searchmanager.h"
#include "../../Controllers/NotificationManager/notificationmanager.h"
#include "../../Controllers/NotificationManager/notificationobject.h"


#include "../../../modelcontroller/modelcontroller.h"
#include "../../../modelcontroller/entityfactory.h"
#include "../../../modelcontroller/version.h"

#include "../../Utils/filehandler.h"


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

#define IDL_FILE_EXT "IDL Documents (*.idl)"
#define IDL_FILE_SUFFIX ".idl"


ViewController::ViewController(){
    root_item = new ViewItem(this, GRAPHML_KIND::NONE);

    //Setup nodes
    setupEntityKindItems();

    //Initialize Settings
    SettingsController::initializeSettings();
    auto settings_controller = SettingsController::settings();
    connect(settings_controller, &SettingsController::settingChanged, this, &ViewController::SettingChanged);

    NotificationManager::construct_singleton(this);
    SearchManager::construct_singleton(this);
    selectionController = new SelectionController(this);
    actionController = new ActionController(this);
    menu = new ContextMenu(this);
    

    jenkins_manager = new JenkinsManager(this);
    execution_manager = new ExecutionManager(this);

    connect(NotificationManager::manager(), &NotificationManager::notificationAdded, this, &ViewController::notification_Added);
    connect(NotificationManager::manager(), &NotificationManager::notificationDeleted, this, &ViewController::notification_Destructed);


    
    connect(jenkins_manager, &JenkinsManager::JobQueued, this, &ViewController::RefreshExecutionMonitor);
    
    

    connect(execution_manager, &ExecutionManager::GotWorkloadCode, this, &ViewController::showCodeViewer);
    connect(this, &ViewController::vc_showToolbar, menu, &ContextMenu::popup);

    auto search_manager = SearchManager::manager();

    connect(actionController->edit_search, &QAction::triggered, search_manager, &SearchManager::PopupSearch);
    connect(actionController->edit_goto, &QAction::triggered, search_manager, &SearchManager::PopupGoto);
    connect(search_manager, &SearchManager::GotoID, this, &ViewController::centerOnID);

    //connect(this, &ViewController::vc_editTableCell, this, &ViewController::EditDataValue);
    
    autosave_timer_.setSingleShot(true);
    connect(&autosave_timer_, &QTimer::timeout, this, &ViewController::autoSaveProject);

    //Update the settings we need
    for(auto key : {SETTINGS::GENERAL_AUTOSAVE_DURATION}){
        SettingChanged(key, settings_controller->getSetting(key));
    }
}

void ViewController::SettingChanged(SETTINGS key, QVariant value){
    switch(key){
        case SETTINGS::GENERAL_AUTOSAVE_DURATION:{
            AutosaveDurationChanged(value.toInt());
            break;
        }
        default:
            break;
    }
}

void ViewController::AutosaveDurationChanged(int duration_minutes){
    is_autosave_enabled_ = duration_minutes > 0;
    if(is_autosave_enabled_){
        //Set new timer interval in milliseconds
        autosave_timer_.setInterval(duration_minutes * 60000);
        connect(this, &ViewController::ActionFinished, this, &ViewController::StartAutosaveCountdown, Qt::UniqueConnection);
    }else{
        autosave_timer_.stop();
        disconnect(this, &ViewController::ActionFinished, this, &ViewController::StartAutosaveCountdown);
    }
}

void ViewController::StartAutosaveCountdown(){
    if(!autosave_timer_.isActive()){
        autosave_timer_.start();
    }
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
    delete root_item;
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
        QList<int> IDs = controller->getConstructablesConnectableNodes(parentID, node_kind, edge_kind);
        items = getViewItems(IDs);
    }
    return items;
}

QHash<EDGE_DIRECTION, ViewItem*> ViewController::getValidEdges(EDGE_KIND kind){
    QHash<EDGE_DIRECTION, ViewItem*>  items;
    if(selectionController && controller){
        auto selection = selectionController->getSelectionIDs();
        auto id_map = controller->getConnectableNodes(selection, kind);
        
        for(const auto& direction : id_map.uniqueKeys()){
            for(const auto& id : id_map.values(direction)){
                auto view_item = getViewItem(id);
                if(view_item){
                    items.insertMulti(direction, view_item);
                }
            }
        }
    }
    return items;
}

QMultiMap<EDGE_DIRECTION, ViewItem*> ViewController::getExistingEndPointsOfSelection(EDGE_KIND kind){
    QMultiMap<EDGE_DIRECTION, ViewItem*>  items;

    for(auto view_item : selectionController->getSelection()){
        if(view_item->isNode()){
            auto node_item = (NodeViewItem*) view_item;

            for(auto edge_item : node_item->getEdges(kind)){
                auto src = edge_item->getSource();
                auto dst = edge_item->getDestination();
                auto is_source = src == view_item;

                auto key = is_source ? EDGE_DIRECTION::SOURCE : EDGE_DIRECTION::TARGET;
                auto value = is_source ? dst : src;

                if(!items.contains(key, value)){
                    items.insert(key, value);
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

    for(auto item : view_items_.values()){
        auto node_item = (NodeViewItem*) item;
        auto edge_item = (EdgeViewItem*) item;

        if(item->isNode()){
            if(node_item->isNodeOfType(NODE_TYPE::ASPECT)){
                continue;
            }
        }
        items.append(item);
    }
    return items;
}

QStringList ViewController::GetIDs(){
    QStringList ids;
    for(auto id : view_items_.keys()){
        ids += QString::number(id);
    }
    return ids;
}

QStringList ViewController::_getSearchSuggestions()
{
    auto keys = getSearchableKeys();
    QSet<QString> suggestions;

    for(auto item : getSearchableEntities()){
        for(const auto& key : keys){
            if(item->hasData(key)){
                suggestions.insert(item->getData(key).toString());
            }
        }
    }
    return suggestions.toList();
}

QHash<QString, ViewItem *> ViewController::getSearchResults(QString query, QList<ViewItem*> view_items)
{
    auto keys = getSearchableKeys();
    QHash<QString, ViewItem*> results;
    
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

QSet<NODE_KIND> ViewController::getValidNodeKinds()
{
    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getValidNodeKinds(ID);
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
        emit ConstructEdges(src_ids, dst_ids, edge_kind);
    }
}

QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > ViewController::getValidEdgeKinds(QList<int> ids){
    QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > edge_kinds;
    if(controller){
        edge_kinds = controller->getValidEdgeKinds(ids);
    }
    return edge_kinds;
}

QSet<EDGE_KIND> ViewController::getCurrentEdgeKinds()
{
    QSet<EDGE_KIND> edgeKinds;
    if(selectionController && controller){
        edgeKinds = controller->getCurrentEdgeKinds(selectionController->getSelectionIDs());
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


void ViewController::SetDefaultIcon(ViewItem& view_item)
{
    QString default_icon_prefix = "EntityIcons";
    QString default_icon_name = view_item.getData("kind").toString();
    
    QString custom_icon_prefix = view_item.getData("icon_prefix").toString();
    QString custom_icon_name = view_item.getData("icon").toString();
        
    if(view_item.isNode()){
        auto& node_item = (NodeViewItem&)view_item;
        const auto& node_kind = node_item.getNodeKind();


        switch(node_kind){
        case NODE_KIND::HARDWARE_NODE:{
            auto os = view_item.getData("os").toString();
            auto arch = view_item.getData("architecture").toString();
            custom_icon_prefix = "EntityIcons";
            custom_icon_name = (os % "_" % arch);
            break;
        }
        case NODE_KIND::OPENCL_PLATFORM:{
            auto vendor = view_item.getData("vendor").toString();
            
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
            auto type = view_item.getData("type").toString();
            
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
            default_icon_name = "IfCondition";
            break;
        }
        case NODE_KIND::DEPLOYMENT_ATTRIBUTE:
            default_icon_prefix = "EntityIcons";
            default_icon_name = "Attribute";
            break;
        case NODE_KIND::PORT_REPLIER:
        case NODE_KIND::PORT_REPLIER_IMPL:
        case NODE_KIND::PORT_REPLIER_INST:
        {
            default_icon_prefix = "EntityIcons";
            default_icon_name = "ReplierPort";
            break;
        }
        case NODE_KIND::PORT_REQUESTER:
        case NODE_KIND::PORT_REQUESTER_IMPL:
        case NODE_KIND::PORT_REQUESTER_INST:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "RequesterPort";
            break;
        }
        case NODE_KIND::PORT_PUBSUB_DELEGATE:
        case NODE_KIND::PORT_REQUEST_DELEGATE:{
            default_icon_prefix = "Icons";
            default_icon_name = "arrowsLeftRightDark";
            break;
        }

        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::INPUT_PARAMETER_GROUP_INSTANCE:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "InputParameterGroup";
            break;
        }
        case NODE_KIND::RETURN_PARAMETER:
        case NODE_KIND::RETURN_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INSTANCE:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "ReturnParameterGroup";
            break;
        }
        case NODE_KIND::VARIABLE_PARAMETER:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "Variable";
            break;
        }
        case NODE_KIND::WORKER_DEFINITIONS:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "Workload";
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
        case NODE_KIND::BOOLEAN_EXPRESSION:
            default_icon_prefix = "EntityIcons";
            default_icon_name = "Condition";
            break;
        case NODE_KIND::FUNCTION:
        case NODE_KIND::FUNCTION_CALL:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "Workload";
            break;
        }
        case NODE_KIND::ENUM_INSTANCE:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "EnumMember";
            break;
        }
        case NODE_KIND::EXTERNAL_PUBSUB_DELEGATE:
        case NODE_KIND::EXTERNAL_SERVER_DELEGATE:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "ExternalAssembly";
            break;
        }
        case NODE_KIND::PORT_PERIODIC_INST:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "PeriodicPort";
            break;
        }
        case NODE_KIND::TRANSITION_FUNCTION:{
            default_icon_prefix = "Icons";
            default_icon_name = "loop";
            break;
        }
        case NODE_KIND::SHARED_DATATYPES:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "SharedDatatypes";
            if(view_item.isReadOnly()){
                custom_icon_prefix = "EntityIcons";
                custom_icon_name = "SharedDatatypesLocked";
            }
            break;
        }
        default:
            break;
        }
    }
        
    if(Theme::theme()->gotImage(default_icon_prefix, default_icon_name)){
        view_item.setDefaultIcon(default_icon_prefix, default_icon_name);
    }
    
    if(custom_icon_prefix.size() && custom_icon_name.size()){
        view_item.setIcon(custom_icon_prefix, custom_icon_name);
    }
}

ViewItem *ViewController::getModel()
{
    return getViewItem(model_id_);
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
    auto selected_items = selectionController->getSelection();
    if(selected_items.count()){
        emit TriggerAction(key_name + " Changed");
        for(auto item : selected_items){
            auto data = item->getData(key_name).toInt();
            emit SetData(item->getID(), key_name, data + 1);
        }
    }
}
void ViewController::decrementSelectedKey(QString key_name){
    auto selected_items = selectionController->getSelection();
    if(selected_items.count()){
        emit TriggerAction(key_name + " Changed");
        for(auto item : selected_items){
            auto data = item->getData(key_name).toInt();
            if(data > 0){
                emit SetData(item->getID(), key_name, data - 1);
            }
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
        emit TriggerAction("Loading Jenkins Nodes");
        QStringList fileData;
        fileData << graphmlData;
        emit ImportProjects(fileData);
    }
}

void ViewController::getCodeForComponent()
{
    auto item = getActiveSelectedItem();
    if(item && item->isNode()){
        execution_manager->GenerateCodeForWorkload(getTempFileForModel(), item);
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



void ViewController::table_dataChanged(int ID, QString key, QVariant data)
{
    emit TriggerAction("Table Changed");
    emit SetData(ID, key, data);
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
    constructableNodes.removeAll(NODE_KIND::PORT_SUBSCRIBER_IMPL);
    constructableNodes.removeAll(NODE_KIND::PORT_REPLIER_IMPL);
    constructableNodes.removeAll(NODE_KIND::INTERFACE_DEFINITIONS);
    constructableNodes.removeAll(NODE_KIND::MANAGEMENT_COMPONENT);
    constructableNodes.removeAll(NODE_KIND::PORT_PUBLISHER_INST);
    constructableNodes.removeAll(NODE_KIND::RETURN_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::INPUT_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::VECTOR_INSTANCE);
    constructableNodes.removeAll(NODE_KIND::VARIABLE_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::QOS_DDS_PROFILE);


    for(auto kind : constructableNodes){
        QString label = EntityFactory::getPrettyNodeKindString(kind);
        if(kind == NODE_KIND::VARIADIC_PARAMETER){
            label = "Optional Parameter";
        }

        auto item = new NodeViewItem(this, kind, label);
        if(item){
            ViewController::SetDefaultIcon(*item);
        }
        nodeKindItems[kind] = item;
    }

    

    for(auto kind : EntityFactory::getEdgeKinds()){
        auto label = EntityFactory::getEdgeKindString(kind);
        //Trim the Edge_ from the label
        label.remove(0, 5);
        auto item = new EdgeViewItem(this, kind, label);
        if(item){
            ViewController::SetDefaultIcon(*item);
        }

        edgeKindItems[kind] = item;
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
                dockWidget->getNodeView()->FitToScreen();
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

void ViewController::ResetViewItems(){
    //Destruct the view items
    DestructViewItem(root_item);
    //Unset the model id
    model_id_ = -1;
}

void ViewController::DestructViewItem(ViewItem *item)
{
    if(item){
        QList<ViewItem*> children;
        children += item;
        children += item->getNestedChildren();

        while(children.size()){
            auto view_item = children.back();
            children.pop_back();

            if(view_item != root_item){
                int ID = view_item->getID();
                //Remove the item from the Hash/TopLevel Hash
                view_items_.remove(ID);
                //Tell Views we are destructing!
                emit vc_viewItemDestructing(ID, view_item);
                view_item->destruct();
            }
        }
    }
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

void ViewController::AddNotification(MODEL_SEVERITY severity, QString title, QString description, int id){
    auto notification_severity = Notification::Severity::INFO;
    switch(severity){
        case MODEL_SEVERITY::ERROR:
            notification_severity = Notification::Severity::ERROR;
            break;
        case MODEL_SEVERITY::WARNING:
            notification_severity = Notification::Severity::WARNING;
            break;
        case MODEL_SEVERITY::INFO:
            notification_severity = Notification::Severity::INFO;
            break;
        default:
            break;
    }
    auto notification = NotificationManager::manager()->AddNotification(title, "Icons", "dotsInRectangle", notification_severity, Notification::Type::MODEL, Notification::Category::NONE, true, id);
    notification->setDescription(description);
}

void ViewController::setControllerReady(bool ready)
{
    if(_controllerReady != ready){
        _controllerReady = ready;
        emit vc_controllerReady(ready);
        emit ActionFinished();
    }

    if(ready){
        //Reset the autosave id
        autosave_id_ = 0;
    }else{
        autosave_timer_.stop();
    }
}

void ViewController::ModelReady(bool ready)
{
    setControllerReady(ready);
    if(ready){
        emit vc_showWelcomeScreen(false);
        //Call this after everything has loaded
        QMetaObject::invokeMethod(this, "vc_fitToScreen", Qt::QueuedConnection, Q_ARG(bool, false));
    }
}

void ViewController::openURL(QString url)
{
    _showWebpage(url);
}

void ViewController::deleteSelection()
{
    if(selectionController){
        emit Delete(selectionController->getSelectionIDs());
    }
}

void ViewController::expandSelection()
{
    auto selection = selectionController->getSelectionIDs();
    if(selection.size()){
        emit TriggerAction("Expand Selection");
        for(auto id : selection){
            emit SetData(id, "isExpanded", true);
        }
    }
}

void ViewController::contractSelection()
{
    auto selection = selectionController->getSelectionIDs();
    if(selection.size()){
        emit TriggerAction("Expand Selection");
        for(auto id : selection){
            emit SetData(id, "isExpanded", false);
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



void ViewController::TeardownController()
{   
    if (controller) {
        setControllerReady(false);
        emit selectionController->clearSelection();
        
        emit ProjectFileChanged("");
        emit ProjectModified(false);
        
        ResetViewItems();
        

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
}

bool ViewController::_newProject(QString file_path)
{
    if(_closeProject()){
        if(!controller){
            initializeController();
            emit SetupModelController(file_path);

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
                controller->setProjectSaved(filePath);
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
            msgBox.setIconPixmap(Theme::theme()->getImage("Icons", "floppyDisk", Theme::theme()->getLargeIconSize()));
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

VIEW_ASPECT ViewController::getNodeViewAspect(int ID){
    if(controller){
        return controller->getNodeViewAspect(ID);
    }
    return VIEW_ASPECT::NONE;
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


QVariant ViewController::getEntityDataValue(int ID, QString key_name){
    QVariant data;
    if(controller){
        data = controller->getEntityDataValue(ID, key_name);
    }
    // () << "Data for: " << ID << " KEY : " << key_name << " = " << data;
    return data;
}

void ViewController::EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id){
    auto src = getNodeViewItem(src_id);
    auto dst = getNodeViewItem(dst_id);
    auto parent = getSharedParent(src, dst);
    if(src && dst && parent){
        auto edge_item = new EdgeViewItem(this, id, src, dst, kind);
        
        SetParentNode(parent, edge_item);
        src->addEdgeItem(edge_item);
        dst->addEdgeItem(edge_item);

        StoreViewItem(edge_item);
    }
}

void ViewController::StoreViewItem(ViewItem* view_item){
    if(view_item){
        auto id = view_item->getID();
        if(!view_items_.contains(id)){
            view_items_[id] = view_item;
        }

        if(controller){
            for(const auto& data : controller->getEntityDataList(id)){
                view_item->changeData(data.key_name, data.value, data.is_protected);
            }
        }
        if(view_item->isNode()){
            auto node_item = (NodeViewItem*) view_item;
            if(node_item->getNodeKind() == NODE_KIND::MODEL){
                model_id_ = id;
            }
            NodeTypesChanged(id);
            NodeEdgeKindsChanged(id);
        }
        ViewController::SetDefaultIcon(*view_item);
        connect(view_item->getTableModel(), &DataTableModel::req_dataChanged, this, &ViewController::table_dataChanged);
        connect(view_item, &ViewItem::showNotifications, NotificationManager::manager(), &NotificationManager::ShowNotificationPanel);
        emit vc_viewItemConstructed(view_item);
    }
}

void ViewController::SetParentNode(ViewItem* parent, ViewItem* child){
    if(!parent){
        parent = root_item;
    }

    if(parent){
        parent->addChild(child);
    }
}

void ViewController::NodeConstructed(int parent_id, int id, NODE_KIND node_kind){
    auto node_item = new NodeViewItem(this, id, node_kind);
    auto parent_item = getNodeViewItem(parent_id);
    SetParentNode(parent_item, node_item);
    StoreViewItem(node_item);
}

void ViewController::EntityDestructed(int id, GRAPHML_KIND)
{
    DestructViewItem(getViewItem(id));
}

void ViewController::DataChanged(int id, DataUpdate data)
{
    auto view_item = getViewItem(id);
    if(view_item){
        view_item->changeData(data.key_name, data.value, data.is_protected);
    }
}

void ViewController::DataRemoved(int id, QString key_name)
{
    auto view_item = getViewItem(id);
    if(view_item){
        view_item->removeData(key_name);
    }
}

void ViewController::NodeEdgeKindsChanged(int id)
{
    auto node_item = getNodeViewItem(id);
    if(node_item){
        if(controller){

            const auto& edge_kinds = getValidEdgeKinds({id});

            QHash<EDGE_KIND, QSet<EDGE_DIRECTION> > valid_edge_kinds;

            //Sources
            for(auto edge_kind : edge_kinds.first){
                valid_edge_kinds[edge_kind].insert(EDGE_DIRECTION::SOURCE);
            }

            for(auto edge_kind : edge_kinds.second){
                valid_edge_kinds[edge_kind].insert(EDGE_DIRECTION::TARGET);
            }

            //Set the required edges
            node_item->setValidEdgeKinds(valid_edge_kinds);
        }
    }
}

void ViewController::NodeTypesChanged(int id)
{
    auto node_item = getNodeViewItem(id);
    if(node_item){
        if(controller){
            const auto& node_types = controller->getNodesTypes(id);
            node_item->setNodeTypes(node_types);
        }
    }
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

void ViewController::generateProjectWorkspace()
{
    auto file_path = getTempFileForModel();
    auto output_dir = FileHandler::selectFile(WindowManager::manager()->getMainWindow(), "Select an folder to generate project in.", QFileDialog::Directory, false);
    if(!output_dir.isEmpty()){
        execution_manager->GenerateProject(file_path, output_dir);
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
        emit ImportProjects(fileData);
    }
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
    "<li>Jackson Michael (Programmer)</li>"
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
        auto data = controller->cut(selectionController->getSelectionIDs());
        setClipboardData(data);
    }
}

void ViewController::copy()
{
    if(selectionController){
        auto data = controller->copy(selectionController->getSelectionIDs());
        setClipboardData(data);
    }
}

void ViewController::paste()
{
    if(selectionController && selectionController->getSelectionCount() == 1){
        emit Paste(selectionController->getSelectionIDs(), QApplication::clipboard()->text());
    }
}

void ViewController::replicate()
{
    if(selectionController && selectionController->getSelectionCount() > 0){
        emit Replicate(selectionController->getSelectionIDs());
    }
}

void ViewController::initializeController()
{
    if(!controller){
        setControllerReady(false);
        controller = new ModelController(QApplication::applicationDirPath());
        ConnectModelController(controller);
    }
}


ViewItem *ViewController::getViewItem(int ID)
{
    if(view_items_.contains(ID)){
        return view_items_[ID];
    }
    return 0;
}



void ViewController::notification_Added(QSharedPointer<NotificationObject> notification){
    //Check for IDs
    auto entity = getViewItem(notification->getEntityID());
    if(entity){
        entity->addNotification(notification);
    }
}

void ViewController::notification_Destructed(QSharedPointer<NotificationObject> notification){
    //Check for IDs
    auto entity = getViewItem(notification->getEntityID());
    if(entity){
        entity->removeNotification(notification);
    }
}
