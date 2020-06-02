#include "viewcontroller.h"

#include "../WindowManager/windowmanager.h"
#include "../../Utils/filehandler.h"
#include "../../Widgets/Windows/basewindow.h"
#include "../../Widgets/DockWidgets/viewdockwidget.h"
#include "../../Widgets/Dialogs/variabledialog.h"
#include "../../Widgets/CodeEditor/codebrowser.h"
#include "../../Widgets/Monitors/jobmonitor.h"
#include "../../Widgets/Monitors/consolemonitor.h"
#include "../../Widgets/Monitors/jenkinsmonitor.h"
#include "../../Widgets/Charts/ExperimentDataManager/experimentdatamanager.h"
#include "../../Views/ContextMenu/contextmenu.h"
#include "../../Controllers/SearchManager/searchmanager.h"
#include "../../../modelcontroller/modelcontroller.h"
#include "../../../modelcontroller/entityfactory.h"
#include "../../../modelcontroller/version.h"

#include <QtConcurrent/QtConcurrentRun>
#include <QMessageBox>
#include <QDateTime>
#include <QApplication>
#include <QClipboard>
#include <QThreadPool>
#include <QListIterator>
#include <QStringBuilder>
#include <QDesktopServices>
#include <QFutureWatcher>
#include <QJsonArray>

#include <iostream>

#define GRAPHML_FILE_EXT "GraphML Documents (*.graphml)"
#define GRAPHML_FILE_SUFFIX ".graphml"

ViewController::ViewController()
{
    root_item = new ViewItem(this, GRAPHML_KIND::NONE);

    //Setup nodes
    setupEntityKindItems();

    //Initialize Settings
    auto settings_controller = SettingsController::settings();
    connect(settings_controller, &SettingsController::settingChanged, this, &ViewController::SettingChanged);

    NotificationManager::construct_singleton(this);
    SearchManager::construct_singleton(this);
    ExperimentDataManager::constructSingleton(this);

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
    
    autosave_timer_.setSingleShot(true);
    connect(&autosave_timer_, &QTimer::timeout, this, &ViewController::autoSaveProject);

    //Update the settings we need
    for(auto key : {SETTINGS::GENERAL_AUTOSAVE_DURATION}){
        SettingChanged(key, settings_controller->getSetting(key));
    }
}

void ViewController::QueryRunningExperiments()
{
    emit vc_displayChartPopup();
}

void ViewController::SettingChanged(SETTINGS key, const QVariant& value)
{
    switch(key){
        case SETTINGS::GENERAL_AUTOSAVE_DURATION:{
            AutosaveDurationChanged(value.toInt());
            break;
        }
        default:
            break;
    }
}

void ViewController::AutosaveDurationChanged(int duration_minutes)
{
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

void ViewController::StartAutosaveCountdown()
{
    if(!autosave_timer_.isActive()){
        autosave_timer_.start();
    }
}

QList<ViewItem*> ViewController::ToViewItemList(const QList<NodeViewItem*>& node_items)
{
    return *(QList<ViewItem*>*)(&node_items);
}

QList<ViewItem*> ViewController::ToViewItemList(const QList<EdgeViewItem*>& edge_items)
{
    return *(QList<ViewItem*>*)(&edge_items);
}

ContextMenu* ViewController::getContextMenu()
{
    return menu;
}

void ViewController::RequestJenkinsNodes()
{
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

void ViewController::ListJenkinsJobs()
{
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

void ViewController::ShowJenkinsBuildDialog(const QStringList& job_names)
{
    VariableDialog dialog("Launch Jenkins Job");
    dialog.addOption("Job", SETTING_TYPE::STRINGLIST, job_names);

    auto options = dialog.getOptions();
    if(!options.isEmpty()){
        auto job_name = dialog.getOptionValue("Job").toString();
        RequestJenkinsBuildJobName(job_name);
    }
}

void ViewController::ShowJenkinsBuildDialog(const QString& job_name, QList<Jenkins_Job_Parameter> parameters)
{
    VariableDialog dialog("Jenkins: " + job_name + " Parameters");

    for(const auto& parameter : parameters){
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

void ViewController::RequestJenkinsBuildJob()
{
    RequestJenkinsBuildJobName(jenkins_manager->GetJobName());
}

void ViewController::RequestJenkinsBuildJobName(const QString& job_name)
{
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

bool ViewController::isWelcomeScreenShowing()
{
    return showingWelcomeScreen;
}

void ViewController::welcomeScreenToggled(bool visible)
{
    showingWelcomeScreen = visible;
    auto notification_manager = NotificationManager::manager();
    if(visible){
        notification_manager->hideToast();
    }else{
        //Toast the notification
        QTimer::singleShot(100, notification_manager, &NotificationManager::toastLatestNotification);
    }
}

QList<ViewItem *> ViewController::getViewItemParents(const QList<ViewItem*>& entities)
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

int ViewController::getNodeDefinitionID(int ID) const
{
    auto definition = getNodesDefinition(ID);
    if (definition) {
        return definition->getID();
    }
    return -1;
}

QList<int> ViewController::getNodeInstanceIDs(int ID) const
{
    QList<int> instIDs;
    auto instances = getNodesInstances(ID);
    for (auto inst : instances) {
        if (inst) {
            instIDs.append(inst->getID());
        }
    }
    return instIDs;
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

QHash<EDGE_DIRECTION, ViewItem*> ViewController::getValidEdges(EDGE_KIND kind)
{
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

QMultiMap<EDGE_DIRECTION, ViewItem*> ViewController::getExistingEndPointsOfSelection(EDGE_KIND kind)
{
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

QStringList getSearchableKeys()
{
    return {KeyName::Label, KeyName::Description, KeyName::Kind, KeyName::Namespace, KeyName::Type, KeyName::Value, KeyName::ID};
};

QList<ViewItem*> ViewController::getSearchableEntities()
{
    QList<ViewItem*> items;
    for (auto item : view_items_.values()) {
        if (item->isNode()) {
            auto node_item = qobject_cast<NodeViewItem*>(item);
            if(node_item->isNodeOfType(NODE_TYPE::ASPECT)){
                continue;
            }
        }
        items.append(item);
    }
    return items;
}

QStringList ViewController::GetIDs()
{
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

QHash<QString, ViewItem *> ViewController::getSearchResults(const QString& query, QList<ViewItem*> view_items)
{
    auto keys = getSearchableKeys();
    QHash<QString, ViewItem*> results;
    
    QRegExp rx("*" + query + "*");
    rx.setPatternSyntax(QRegExp::WildcardUnix);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if(view_items.isEmpty()){
        view_items = getSearchableEntities();
    }
    for(const auto& item : view_items){
        for(const auto& key : keys){
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

QList<ViewItem*> ViewController::filterList(const QString& query, const QList<ViewItem*>& view_items)
{
    if(view_items.isEmpty()){
        return view_items;
    }else{
        return getSearchResults(query, view_items).values().toSet().toList();
    }
}

ViewDockWidget* ViewController::constructViewDockWidget(const QString& label, QWidget* parent)
{
    auto dock_widget = WindowManager::manager()->constructViewDockWidget(label, parent);
    auto node_view = new NodeView(*this, dock_widget);
    
    dock_widget->setWidget(node_view);
    connect(node_view, &NodeView::itemSelectionChanged, SearchManager::manager(), &SearchManager::ItemSelectionChanged);

    return dock_widget;
}

QSet<NODE_KIND> ViewController::getValidNodeKinds()
{
    if(selectionController && controller && selectionController->getSelectionCount() == 1){
        int ID = selectionController->getFirstSelectedItem()->getID();
        return controller->getValidNodeKinds(ID);
    }
    return {};
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

void ViewController::constructEdges(int id, EDGE_KIND edge_kind, EDGE_DIRECTION edge_direction)
{
    auto selection = getSelectionController()->getSelectionIDs();
    if(!selection.isEmpty()){
        auto id_list = {id};
        auto src_ids = edge_direction == EDGE_DIRECTION::SOURCE ? id_list : selection;
        auto dst_ids = edge_direction == EDGE_DIRECTION::TARGET ? id_list : selection;
        emit ConstructEdges(src_ids, dst_ids, edge_kind);
    }
}

QPair<QSet<EDGE_KIND>, QSet<EDGE_KIND> > ViewController::getValidEdgeKinds(const QList<int>& ids)
{
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

QSet<NODE_KIND> ViewController::getValidChartNodeKinds()
{
    QSet<NODE_KIND> chart_valid_node_kinds;
    chart_valid_node_kinds.insert(NODE_KIND::COMPONENT);
    chart_valid_node_kinds.insert(NODE_KIND::COMPONENT_IMPL);
    chart_valid_node_kinds.insert(NODE_KIND::COMPONENT_INST);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REPLIER);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REPLIER_IMPL);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REPLIER_INST);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REQUESTER);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REQUESTER_IMPL);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_REQUESTER_INST);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_PERIODIC);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_PERIODIC_INST);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_PUBLISHER);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_PUBLISHER_IMPL);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_PUBLISHER_INST);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_SUBSCRIBER);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_SUBSCRIBER_IMPL);
    chart_valid_node_kinds.insert(NODE_KIND::PORT_SUBSCRIBER_INST);
    chart_valid_node_kinds.insert(NODE_KIND::CLASS_INST);
    chart_valid_node_kinds.insert(NODE_KIND::HARDWARE_NODE);
    chart_valid_node_kinds.insert(NODE_KIND::INTERFACE_DEFINITIONS);
    chart_valid_node_kinds.insert(NODE_KIND::BEHAVIOUR_DEFINITIONS);
    chart_valid_node_kinds.insert(NODE_KIND::ASSEMBLY_DEFINITIONS);
    chart_valid_node_kinds.insert(NODE_KIND::HARDWARE_DEFINITIONS);
    return chart_valid_node_kinds;
}

QSet<MEDEA::ChartDataKind> ViewController::getValidChartDataKindsForSelection()
{
    QSet<MEDEA::ChartDataKind> validDataKinds;

    if (selectionController) {

        QSet<NODE_KIND> selectedKinds;
        auto validChartNodeKinds = getValidChartNodeKinds();

        for (auto item : selectionController->getSelection()) {
            if (!item->isNode())
                continue;
            auto nodeItem = (NodeViewItem*) item;
            auto nodeKind = nodeItem->getNodeKind();
            if (!validChartNodeKinds.contains(nodeKind)) {
                return validDataKinds;
            }
            if ((nodeKind == NODE_KIND::CLASS_INST) && !nodeItem->getData(KeyName::IsWorker).toBool()) {
                return validDataKinds;
            }
            selectedKinds.insert(nodeKind);
        }

        for (auto kind : selectedKinds) {
            switch (kind) {
            // Added cases for the aspects
            case NODE_KIND::BEHAVIOUR_DEFINITIONS:
                validDataKinds.insert(MEDEA::ChartDataKind::MARKER);
            case NODE_KIND::INTERFACE_DEFINITIONS:
            case NODE_KIND::ASSEMBLY_DEFINITIONS:
            case NODE_KIND::COMPONENT:
            case NODE_KIND::COMPONENT_IMPL:
            case NODE_KIND::COMPONENT_INST: {
                validDataKinds.insert(MEDEA::ChartDataKind::PORT_LIFECYCLE);
                validDataKinds.insert(MEDEA::ChartDataKind::PORT_EVENT);
                validDataKinds.insert(MEDEA::ChartDataKind::WORKLOAD);
                break;
            }
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REPLIER_IMPL:
            case NODE_KIND::PORT_REPLIER_INST:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::PORT_REQUESTER_IMPL:
            case NODE_KIND::PORT_REQUESTER_INST:
            case NODE_KIND::PORT_PERIODIC:
            case NODE_KIND::PORT_PERIODIC_INST:
            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_PUBLISHER_IMPL:
            case NODE_KIND::PORT_PUBLISHER_INST:
            case NODE_KIND::PORT_SUBSCRIBER:
            case NODE_KIND::PORT_SUBSCRIBER_IMPL:
            case NODE_KIND::PORT_SUBSCRIBER_INST:
                validDataKinds.insert(MEDEA::ChartDataKind::PORT_LIFECYCLE);
                validDataKinds.insert(MEDEA::ChartDataKind::PORT_EVENT);
                break;
            case NODE_KIND::CLASS_INST:
                validDataKinds.insert(MEDEA::ChartDataKind::WORKLOAD);
                break;
            case NODE_KIND::HARDWARE_DEFINITIONS:
            case NODE_KIND::HARDWARE_NODE: {
                validDataKinds.insert(MEDEA::ChartDataKind::CPU_UTILISATION);
                validDataKinds.insert(MEDEA::ChartDataKind::MEMORY_UTILISATION);
                validDataKinds.insert(MEDEA::ChartDataKind::NETWORK_UTILISATION);
                break;
            }
            default:
                break;
            }
        }
    }

    return validDataKinds;
}

QList<QVariant> ViewController::getValidValuesForKey(int ID, const QString& keyName)
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
    QString default_icon_name = view_item.getData(KeyName::Kind).toString();
    
    QString custom_icon_prefix = view_item.getData(KeyName::IconPrefix).toString();
    QString custom_icon_name = view_item.getData(KeyName::Icon).toString();
        
    if(view_item.isNode()){

        auto& node_item = (NodeViewItem&)view_item;
        const auto& node_kind = node_item.getNodeKind();

        switch(node_kind){
        case NODE_KIND::HARDWARE_NODE:{
            auto os = view_item.getData(KeyName::OS).toString();
            auto arch = view_item.getData(KeyName::Architecture).toString();
            custom_icon_prefix = "EntityIcons";
            custom_icon_name = (os % "_" % arch);
            break;
        }
        case NODE_KIND::OPENCL_PLATFORM:{
            auto vendor = view_item.getData(KeyName::Vendor).toString();
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
            auto type = view_item.getData(KeyName::Type).toString();
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
        case NODE_KIND::CALLBACK_FNC:
        case NODE_KIND::CALLBACK_FNC_INST:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "CallbackFunction";
            break;
        }
        case NODE_KIND::INPUT_PARAMETER:
        case NODE_KIND::INPUT_PARAMETER_GROUP:
        case NODE_KIND::INPUT_PARAMETER_GROUP_INST:{
            default_icon_prefix = "EntityIcons";
            default_icon_name = "InputParameterGroup";
            break;
        }
        case NODE_KIND::RETURN_PARAMETER:
        case NODE_KIND::RETURN_PARAMETER_GROUP:
        case NODE_KIND::RETURN_PARAMETER_GROUP_INST:{
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
        case NODE_KIND::ENUM_INST:{
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
        case NODE_KIND::TRIGGER_DEFN:
        case NODE_KIND::TRIGGER_INST: {
            default_icon_prefix = "Icons";
            default_icon_name = "circleBoltDark";
            break;
        }
        case NODE_KIND::STRATEGY_INST: {
            default_icon_prefix = "Icons";
            default_icon_name = "squares";
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

DefaultDockWidget* ViewController::constructDockWidget(const QString& title, const QString& icon_path, const QString& icon_name, QWidget* widget, BaseWindow* window)
{
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

void ViewController::showCodeViewer(const QString& tabName, const QString& content)
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

JobMonitor* ViewController::getExecutionMonitor()
{
    if(!job_monitor){
        job_monitor = new JobMonitor(jenkins_manager);
        auto toolbar = job_monitor->getToolbar();
        toolbar->addAction(actionController->model_executeLocalJob);
        toolbar->addAction(actionController->jenkins_executeJob);
    }
    return job_monitor;
}

void ViewController::incrementSelectedKey(const QString& key_name)
{
    auto selected_items = selectionController->getSelection();
    if(selected_items.count()){
        emit TriggerAction(key_name + " Changed");
        for(auto item : selected_items){
            auto data = item->getData(key_name).toInt();
            emit SetData(item->getID(), key_name, data + 1);
        }
    }
}

void ViewController::decrementSelectedKey(const QString& key_name)
{
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

void ViewController::RefreshExecutionMonitor(const QString& job_name)
{
    showExecutionMonitor();
    job_monitor->refreshRecentBuildsByName(job_name);
}

void ViewController::showExecutionMonitor()
{
    auto window_manager = WindowManager::manager();

    //First time 
    if(!execution_monitor){
        auto monitor = getExecutionMonitor();
        execution_monitor = constructDockWidget("Execution Monitor", "Icons", "bracketsAngled", monitor);
    }
    WindowManager::ShowDockWidget(execution_monitor);
}

void ViewController::jenkinsManager_GotJenkinsNodesList(const QString& graphmlData)
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

void ViewController::table_dataChanged(int ID, const QString& key, const QVariant& data)
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
    constructableNodes.removeAll(NODE_KIND::VECTOR_INST);
    constructableNodes.removeAll(NODE_KIND::VARIABLE_PARAMETER);
    constructableNodes.removeAll(NODE_KIND::QOS_DDS_PROFILE);
    constructableNodes.removeAll(NODE_KIND::TRIGGER_DEFN);

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

void ViewController::_showGitHubPage(const QString& relURL)
{
    QString URL = APP_URL() % relURL;
    _showWebpage(URL);
}

void ViewController::_showWebpage(const QString& URL)
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
        QString kind = item->getData(KeyName::Kind).toString();
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
            auto label = item->getData(KeyName::Label).toString();
            auto parent = window_manager->getMainWindow();
            dockWidget = constructViewDockWidget(label, parent);
            dockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);

            // Need to connect the ViewItem's labelChanged signal so that the title of the DockWidget that contains it,
            // updates with the ViewItem
            connect(item, &ViewItem::labelChanged, [dockWidget] (const QString& new_label) {
                dockWidget->setTitle(new_label);
            });

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

void ViewController::ResetViewItems()
{
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

        while(!children.isEmpty()){
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

QList<ViewItem *> ViewController::getViewItems(const QList<int>& IDs)
{
    QList<ViewItem*> items;
    for(int ID : IDs) {
        ViewItem* item = getViewItem(ID);
        if (item) {
            items.append(item);
        }
    }
    return items;
}

ViewItem* ViewController::getActiveSelectedItem() const
{
    if(selectionController){
        return selectionController->getActiveSelectedItem();
    }
    return nullptr;
}

NodeViewItem* ViewController::getNodeViewItem(int ID) const
{
    ViewItem* item = getViewItem(ID);
    if(item && item->isNode()){
        return (NodeViewItem*) item;
    }
    return nullptr;
}

NodeViewItem *ViewController::getNodesImpl(int ID) const
{
    if(controller){
        int implID = controller->getImplementation(ID);
        return getNodeViewItem(implID);
    }
    return nullptr;
}

NodeViewItem *ViewController::getNodesDefinition(int ID) const
{
    if(controller){
        int defID = controller->getDefinition(ID);
        return getNodeViewItem(defID);
    }
    return nullptr;
}

QList<ViewItem *> ViewController::getNodesInstances(int ID) const
{
    QList<ViewItem*> instances;
    if (controller) {
        for (auto instID : controller->getInstances(ID)) {
            instances.append(getViewItem(instID));
        }
    }
    return instances;
}

NodeViewItem* ViewController::getSharedParent(NodeViewItem *node1, NodeViewItem *node2)
{
    NodeViewItem* parent = nullptr;
    if(controller && node1 && node2){
        auto ID = controller->getSharedParent(node1->getID(), node2->getID());
        parent = getNodeViewItem(ID);
    }
    return parent;
}

NodeView* ViewController::getActiveNodeView()
{
    auto dock = WindowManager::manager()->getActiveViewDockWidget();
    if(dock){
        return dock->getNodeView();
    }
    return nullptr;
}

void ViewController::AddNotification(MODEL_SEVERITY severity, const QString& title, const QString& description, int id)
{
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

void ViewController::openURL(const QString& url)
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
    if(!selection.isEmpty()){
        emit TriggerAction("Expand Selection");
        for(auto id : selection){
            emit SetData(id, KeyName::IsExpanded, true);
        }
    }
}

void ViewController::contractSelection()
{
    auto selection = selectionController->getSelectionIDs();
    if(!selection.isEmpty()){
        emit TriggerAction("Expand Selection");
        for(auto id : selection){
            emit SetData(id, KeyName::IsExpanded, false);
        }
    }
}

void ViewController::editLabel()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        emit vc_editTableCell(item->getID(), KeyName::Label);
    }
}

void ViewController::editReplicationCount()
{
    ViewItem* item = getActiveSelectedItem();
    if(item){
        emit vc_editTableCell(item->getID(), KeyName::ReplicateValue);
    }
}

void ViewController::viewSelectionChart(const QList<MEDEA::ChartDataKind>& dataKinds)
{
    if (selectionController && !dataKinds.isEmpty()) {
        emit vc_viewItemsInChart(selectionController->getSelection(), dataKinds);
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
        controller = nullptr;
        
        auto manager = NotificationManager::manager();
        
        // Clear previous validation notification items
        for (const auto& notification : manager->getNotificationsOfType(Notification::Type::MODEL)) {
            manager->deleteNotification(notification->getID());
        }

        emit modelClosed();
    }
}

void ViewController::autoSaveProject()
{
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

bool ViewController::_newProject(const QString& file_path)
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

bool ViewController::_saveAsProject(const QString& file_path)
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

            int user_clicked_button = msgBox.exec();
            if (user_clicked_button & QMessageBox::Yes) {
                // This is the case where the user has clicked "Save"
                // It attempts to save the project and only exits if it was successful
                if (!_saveProject()) {
                    // INSPECT: This may cause issues; returning false from this function blocks other functions such as newProject and closeMedea
                    // TODO: Investigate what other functions are affected by this case and find a fix for them
                    return false;
                }
            } else if (user_clicked_button & QMessageBox::Cancel) {
                // This is the case where the user has clicked "Cancel" - abort exiting
                return false;
            }
        }
    }

    // This should be true if the closeProject was triggered by the user from the "File" menu
    if (show_welcome) {
        emit vc_showWelcomeScreen(true);
    }

    // If saveProject was successful or the user has clicked "Ignore", close the project
    TeardownController();
    return true;
}

VIEW_ASPECT ViewController::getNodeViewAspect(int ID)
{
    if(controller){
        return controller->getNodeViewAspect(ID);
    }
    return VIEW_ASPECT::NONE;
}

bool ViewController::isNodeAncestor(int ID, int ID2)
{
    bool is_ancestor = false;
    if(controller){
        is_ancestor = controller->isNodeAncestor(ID, ID2);
    }
    return is_ancestor;
}

ModelController* ViewController::getModelController()
{
    return controller;    
}

QVariant ViewController::getEntityDataValue(int ID, QString key_name)
{
    QVariant data;
    if (controller) {
        data = controller->getEntityDataValue(ID, key_name);
    }
    return data;
}

void ViewController::EdgeConstructed(int id, EDGE_KIND kind, int src_id, int dst_id)
{
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

void ViewController::StoreViewItem(ViewItem* view_item)
{
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

void ViewController::SetParentNode(ViewItem* parent, ViewItem* child)
{
    if(!parent){
        parent = root_item;
    }
    if(parent){
        parent->addChild(child);
    }
}

void ViewController::NodeConstructed(int parent_id, int id, NODE_KIND node_kind)
{
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

void ViewController::DataRemoved(int id, const QString& key_name)
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

void ViewController::setClipboardData(const QString& data)
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
            auto exec_monitor = getExecutionMonitor();
            auto local_monitor = exec_monitor->getConsoleMonitor("Local Deployment");
            if(!local_monitor){
                local_monitor = exec_monitor->constructConsoleMonitor();
                // NOTE: If the function above returned the existing monitor if we already have one, we don't need null checks here and below
                if (local_monitor) {
                    connect(execution_manager, &ExecutionManager::GotProcessStdOutLine, local_monitor, &Monitor::AppendLine);
                    connect(execution_manager, &ExecutionManager::GotProcessStdErrLine, local_monitor, &Monitor::AppendLine);
                    connect(execution_manager, &ExecutionManager::ModelExecutionStateChanged, local_monitor, &Monitor::StateChanged);
                    connect(local_monitor, &Monitor::Abort, execution_manager, &ExecutionManager::CancelModelExecution);
                }
            }
            if (local_monitor) {
                local_monitor->Clear();
            }
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

void ViewController::_importProjectFiles(const QStringList& files)
{
    QStringList fileData;
    for (const QString& file : files) {
        QString data = FileHandler::readTextFile(file);
        if (data != "") {
            fileData.append(data);
        }
    }
    if (!fileData.isEmpty()) {
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

/**
 * @brief ViewController::selectAndCenterConnectedEntities
 * Select and center on the selection's connected entities
 */
void ViewController::selectAndCenterConnectedEntities()
{
    if (selectionController) {
        emit vc_selectAndCenterConnectedEntities(selectionController->getSelection());
    }
}

/**
 * @brief ViewController::selectAndCenterInstances
 * Select, highlight and center on the selection's instances
 */
void ViewController::selectAndCenterInstances()
{
    if (selectionController) {
        QList<int> instanceIDs;
        for (const auto& item : selectionController->getSelection()) {
            if (item) {
                instanceIDs.append(getNodeInstanceIDs(item->getID()));
            }
        }
        emit vc_selectItems(instanceIDs);
        emit vc_centerOnItems(instanceIDs);
        HighlightItems(instanceIDs);
    }
}

/**
 * @brief ViewController::HighlightItems
 * Flash the items with the provided ids
 * @param ids
 */
void ViewController::HighlightItems(const QList<int>& ids)
{
    QtConcurrent::run([this, ids]() {
        // highlight and flash the items
        int flash_count = 5;
        while (--flash_count > 0) {
            for (auto id : ids) {
                emit vc_highlightItem(id, flash_count % 2 == 0);
            }
            QThread::currentThread()->msleep(500);
        }
    });
}

void ViewController::centerOnID(int ID)
{
    emit vc_centerItem(ID);
}

void ViewController::showWiki()
{
    _showWiki(nullptr);
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
        try{
            auto new_controller = new ModelController(QApplication::applicationDirPath());
            controller = new_controller;
            ConnectModelController(controller);
        }catch(const std::exception& ex){
            auto parent = WindowManager::manager()->getMainWindow();
            QMessageBox::critical(parent, "MEDEA Model Exception", ex.what());
            QCoreApplication::exit(1);
        }
    }
}

ViewItem *ViewController::getViewItem(int ID) const
{
    if (view_items_.contains(ID)) {
        return view_items_[ID];
    }
    return nullptr;
}

void ViewController::notification_Added(QSharedPointer<NotificationObject> notification)
{
    //Check for IDs
    auto entity = getViewItem(notification->getEntityID());
    if(entity){
        entity->addNotification(notification);
    }
}

void ViewController::notification_Destructed( QSharedPointer<NotificationObject> notification)
{
    //Check for IDs
    auto entity = getViewItem(notification->getEntityID());
    if(entity){
        entity->removeNotification(notification);
    }
}
