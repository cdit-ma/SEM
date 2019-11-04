#include "experimentdatamanager.h"
#include "requestbuilder.h"
#include "../../../Controllers/WindowManager/windowmanager.h"

#include <QFutureWatcher>

const int invalid_experiment_id = -1;
ExperimentDataManager* ExperimentDataManager::manager_ = nullptr;

/**
 * @brief ExperimentDataManager::ExperimentDataManager
 * @param vc
 */
ExperimentDataManager::ExperimentDataManager(const ViewController &vc)
    : viewController_(vc)
{
    chartDialog_ = new ChartDialog();
    dataflowDialog_ = new DataflowDialog();

    connect(&vc, &ViewController::vc_displayChartPopup, this, &ExperimentDataManager::displayChartPopup);
    connect(&vc, &ViewController::vc_viewItemsInChart, this, &ExperimentDataManager::filterRequestsBySelectedEntities);

    connect(&vc, &ViewController::modelClosed, chartDialog_, &ChartDialog::clear);
    connect(&vc, &ViewController::modelClosed, dataflowDialog_, &DataflowDialog::clearScene);

    connect(this, &ExperimentDataManager::showChartsPanel, &ExperimentDataManager::showDataflowPanel);
    connect(&chartPopup_, &ChartInputPopup::selectedExperimentRun, this, &ExperimentDataManager::experimentRunSelected);
}


/**
 * @brief ExperimentDataManager::aggregationProxy
 * @return
 */
AggregationProxy& ExperimentDataManager::aggregationProxy()
{
    try {
        return AggregationProxy::singleton();
    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        throw;
    }
}


/**
 * @brief ExperimentDataManager::timelineChartView
 * @return
 */
TimelineChartView& ExperimentDataManager::timelineChartView()
{
    return getChartDialog().getChartView();
}


/**
 * @brief ExperimentDataManager::requestExperimentRuns
 */
void ExperimentDataManager::requestExperimentRuns(const QString& experimentName)
{
    try {
        auto future = aggregationProxy().RequestExperimentRuns(experimentName);
        auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [this, experimentName, futureWatcher]() {
            try {
                processExperimentRuns(experimentName, futureWatcher->result());
            } catch(const std::exception& ex) {
                toastNotification("Failed to get experiment runs - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestExperimentState
 * @param experimentRunID
 */
void ExperimentDataManager::requestExperimentState(const quint32 experimentRunID)
{
    try {
        auto future = aggregationProxy().RequestExperimentState(experimentRunID);
        auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

        connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [this, experimentRunID, futureWatcher]() {
            try {
                // once the state is received, request the events for the selected experiment run
                processExperimentState(selectedExperimentRun_.experiment_name, experimentRunID, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment state: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request experiment state: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestEvents
 * This is the last function that is called, from when a valid experiment run is selected from the ChartInputPopup
 * This is where the chart data request chain ends
 * @param builder
 */
void ExperimentDataManager::requestEvents(const RequestBuilder& builder)
{
    // check if there is a valid selected experiment run
    if (selectedExperimentRun_.experiment_run_id == invalid_experiment_id) {
        return;
    }

    try {
        requestPortLifecycleEvents(builder.getPortLifecycleRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No PortLifecycleRequest");
    }
    try {
        requestWorkloadEvents(builder.getWorkloadRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No WorkloadRequest");
    }
    try {
        requestCPUUtilisationEvents(builder.getCPUUtilisationRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No CPUUtilisationRequest");
    }
    try {
        requestMemoryUtilisationEvents(builder.getMemoryUtilisationRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No MemoryUtilisationRequest");
    }
    try {
        requestMarkerEvents(builder.getMarkerRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No MarkerRequest");
    }
    try {
        requestPortEvents(builder.getPortEventRequest(), selectedExperimentRun_);
    } catch (const std::exception&) {
        qInfo("No PortEventRequest");
    }

    // Reset the selected experiment run
    selectedExperimentRun_.experiment_run_id = invalid_experiment_id;
}


/**
 * @brief ExperimentDataManager::requestPortLifecycleEvents
 * @param request
 * @param experimentRun
 */
void ExperimentDataManager::requestPortLifecycleEvents(const PortLifecycleRequest &request, const AggServerResponse::ExperimentRun& experimentRun)
{
    try {
        auto future = aggregationProxy().RequestPortLifecycleEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processPortLifecycleEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get port lifecycle events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request port lifecycle events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestWorkloadEvents
 * @param request
 */
void ExperimentDataManager::requestWorkloadEvents(const WorkloadRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestWorkloadEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processWorkloadEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get workload events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request workload events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestCPUUtilisationEvents
 * @param request
 */
void ExperimentDataManager::requestCPUUtilisationEvents(const CPUUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestCPUUtilisationEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processCPUUtilisationEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get cpu utilisation events - " + QString::fromStdString(ex.what()), "cpu", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request cpu utilisation events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestMemoryUtilisationEvents
 * @param request
 */
void ExperimentDataManager::requestMemoryUtilisationEvents(const MemoryUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestMemoryUtilisationEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processMemoryUtilisationEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get memory utilisation events - " + QString::fromStdString(ex.what()), "memoryCard", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request memory utilisation events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestMarkerEvents
 * @param request
 */
void ExperimentDataManager::requestMarkerEvents(const MarkerRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestMarkerEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processMarkerEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get marker events - " + QString::fromStdString(ex.what()), "bookmark", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request marker events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ChartManager::requestPortEvents
 * @param request
 * @param experimentRun
 */
void ExperimentDataManager::requestPortEvents(const PortEventRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestPortEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<PortEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<PortEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                processPortEvents(experimentRun, futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get port events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request port events: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::processExperimentRuns
 * This is called when the experiment runs for a given experiment name has successfully been received
 * It stores the experiment runs' data with its associated experimnent, and sets the ChartInputPopup's stringlist model for the menu
 * @param experiment_name
 * @param experiment_runs
 * @throws std::runtime_error
 */
void ExperimentDataManager::processExperimentRuns(const QString& experiment_name, const QVector<AggServerResponse::ExperimentRun>& experiment_runs)
{
    if (experiment_runs.isEmpty()) {
        throw std::runtime_error("ExperimentDataManager::processExperimentRuns - Experiment " + experiment_name.toStdString() + " doesn't have any runs.");
    }

    // This is the case where all the experiment runs for all experiments has been requested
    if (experiment_name.isEmpty()) {
        for (const auto& exp_run : experiment_runs) {
            const auto& exp_run_name = exp_run.experiment_name;
            MEDEA::ExperimentData* exp_data = experiment_data_hash_.value(exp_run_name, nullptr);
            if (exp_data == nullptr) {
                exp_data = new MEDEA::ExperimentData(exp_run_name);
                experiment_data_hash_.insert(exp_run_name, exp_data);
            }
            exp_data->addExperimentRun(exp_run);
        }
    } else {
        MEDEA::ExperimentData* exp_data = experiment_data_hash_.value(experiment_name, nullptr);
        if (exp_data == nullptr) {
            exp_data = new MEDEA::ExperimentData(experiment_name);
            experiment_data_hash_.insert(experiment_name, exp_data);
        }
        for (const auto& exp_run : experiment_runs) {
            exp_data->addExperimentRun(exp_run);
        }
    }

    // Set the experiment runs stored in the chartPopup_
    chartPopup_.setExperimentRuns(experiment_name, experiment_runs.toList());
}


/**
 * @brief ExperimentDataManager::processExperimentState
 * @param experiment_name
 * @param experiment_run_id
 * @param experiment_state
 * @throws std::invalid_argument
 */
void ExperimentDataManager::processExperimentState(const QString& experiment_name, quint32 experiment_run_id, const AggServerResponse::ExperimentState& experiment_state)
{
    if (experiment_name.isEmpty()) {
        throw std::invalid_argument("ExperimentDataManager::processExperimentState - Experiment name cannot be empty.");
    }

    auto exp_data = experiment_data_hash_.value(experiment_name, nullptr);
    if (exp_data == nullptr) {
        throw std::invalid_argument("ExperimentDataManager::processExperimentState - There is no ExperimentData with experiment name: " + experiment_name.toStdString());
    }

    auto& exp_run_data = exp_data->getExperimentRun(experiment_run_id);
    exp_run_data.setExperimentState(experiment_state);

    // Updated the selected experiment run's last updated time - the selected run is passed on to the chartview
    auto last_updated_time = experiment_state.last_updated_time;
    selectedExperimentRun_.last_updated_time = last_updated_time;

    // Let the chartview know to update the time-range for the charts associated with the experiment_run_id
    timelineChartView().updateExperimentRunLastUpdatedTime(experiment_run_id, last_updated_time);

    // Setup/display experiment data for PULSE
    getDataflowDialog().constructGraphicsItemsForExperimentRun(experiment_name, exp_run_data);

    // Setup/build the event requests
    // NOTE: This needs to be called last because it clears the selected experiment run
    setupRequestsForExperimentRun(experiment_run_id);
}


/**
 * @brief ExperimentDataManager::processPortLifecycleEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processPortLifecycleEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortLifecycleEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No port lifecycle events received for selection", "plug");
    } else {
        emit showChartsPanel();
        getDataflowDialog().storePortLifecycleEvents(events);
        timelineChartView().addPortLifecycleEvents(exp_run, events);
    }
}


/**
 * @brief ExperimentDataManager::processWorkloadEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processWorkloadEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<WorkloadEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No workload events received for selection", "plug");
    } else {
        emit showChartsPanel();
        timelineChartView().addWorkloadEvents(exp_run, events);
    }
}


/**
 * @brief ExperimentDataManager::processCPUUtilisationEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processCPUUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<CPUUtilisationEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No cpu utilisation events received for selection", "cpu");
    } else {
        emit showChartsPanel();
        timelineChartView().addCPUUtilisationEvents(exp_run, events);
    }
}


/**
 * @brief ExperimentDataManager::processMemoryUtilisationEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processMemoryUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MemoryUtilisationEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No memory utilisation events received for selection", "memoryCard");
    } else {
        emit showChartsPanel();
        timelineChartView().addMemoryUtilisationEvents(exp_run, events);
    }
}


/**
 * @brief ExperimentDataManager::processMarkerEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processMarkerEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MarkerEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No marker events received for selection", "bookmark");
    } else {
        emit showChartsPanel();
        timelineChartView().addMarkerEvents(exp_run, events);
    }
}


/**
 * @brief ExperimentDataManager::processPortEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processPortEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortEvent*>& events)
{
    if (events.isEmpty()) {
        toastNotification("No port events received for selection", "plug");
    } else {
        qDebug() << "Requested Port Events for " << exp_run.experiment_name << "[" << exp_run.experiment_run_id << "]";
        qDebug() << "Received#: " << events.size();
        // TODO - Next step; GUI side implementation
        //emit showChartsPanel();
        //timelineChartView().addPortEvents(experimentRun, events);
    }
}


/**
 * @brief ExperimentDataManager::toastNotification
 * @param description
 * @param iconName
 * @param severity
 */
void ExperimentDataManager::toastNotification(const QString &description, const QString &iconName, Notification::Severity severity)
{
    NotificationManager::manager()->AddNotification(description, "Icons", iconName, severity, Notification::Type::APPLICATION, Notification::Category::NONE);
}


/**
 * @brief ExperimentDataManager::getItemLabel
 * @param item
 * @return
 */
QString ExperimentDataManager::getItemLabel(const ViewItem *item)
{
    if (item) {
        return item->getData("label").toString();
    }
    return "";
}


/**
 * @brief ExperimentDataManager::manager
 * @return
 */
ExperimentDataManager* ExperimentDataManager::manager()
{
    return manager_;
}


/**
 * @brief ExperimentDataManager::getChartDialog
 * @throws std::runtime_error
 * @return
 */
ChartDialog& ExperimentDataManager::getChartDialog()
{
    if (chartDialog_ != nullptr) {
        return *chartDialog_;
    } else {
        throw std::runtime_error("Could not get ChartDialog reference; chartDialog_ is null.");
    }
}


/**
 * @brief ExperimentDataManager::getDataflowDialog
 * @return
 */
DataflowDialog &ExperimentDataManager::getDataflowDialog()
{
    if (dataflowDialog_ != nullptr) {
        return *dataflowDialog_;
    } else {
        throw std::runtime_error("Could not get DataflowDialog referece; dataflowDialog_ is null.");
    }
}


/**
 * @brief ExperimentDataManager::displayChartPopup
 */
void ExperimentDataManager::displayChartPopup()
{
    // NOTE - All experiments are requested to fill the context menu's model all the time, the filtering based on input happens in the ChartInputPopup
    requestExperimentRuns("");
    chartPopup_.setPopupVisible(true);
}


/**
 * @brief ExperimentDataManager::filterRequestsBySelectedEntities
 * @param selectedItems
 * @param selectedDataKinds
 */
void ExperimentDataManager::filterRequestsBySelectedEntities(const QVector<ViewItem*> &selectedItems, const QList<MEDEA::ChartDataKind> &selectedDataKinds)
{
    selectedViewItems_ = selectedItems;
    selectedDataKinds_ = selectedDataKinds;
    displayChartPopup();
}


/**
 * @brief ExperimentDataManager::experimentRunSelected
 * This slot is called when an experiment run was selected from the ChartInputPopup and "OK" was clicked
 * This is where the chart data request chain starts
 * @param experimentRun
 */
void ExperimentDataManager::experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun)
{
    if (experimentRun.experiment_run_id != invalid_experiment_id) {
        auto expRunID = static_cast<quint32>(experimentRun.experiment_run_id);
        selectedExperimentRun_ = experimentRun;
        requestExperimentState(expRunID);
    }
}


/**
 * @brief ExperimentDataManager::setupRequestsForExperimentRun
 * @param experimentRunID
 */
void ExperimentDataManager::setupRequestsForExperimentRun(const quint32 experimentRunID)
{
    if (selectedDataKinds_.isEmpty()) {
        selectedDataKinds_ = MEDEA::Event::GetChartDataKinds();
        selectedDataKinds_.removeAll(MEDEA::ChartDataKind::DATA);
    }

    auto builder = RequestBuilder::build();
    builder.buildRequests(selectedDataKinds_);
    builder.setExperimentRunID(experimentRunID);

    if (!selectedViewItems_.isEmpty()) {

        QVector<QString> compNames;
        QVector<QString> compInstPaths;
        QVector<QString> portPaths;
        QVector<QString> workerInstPaths;
        QVector<QString> nodeHostnames;

        for (auto item : selectedViewItems_) {

            if (!item || !item->isNode())
                continue;

            auto nodeItem = qobject_cast<NodeViewItem*>(item);
            auto nodeItemID = nodeItem->getID();
            auto label = getItemLabel(nodeItem);

            switch (nodeItem->getNodeKind()) {
            case NODE_KIND::COMPONENT:
            case NODE_KIND::COMPONENT_IMPL:
                // can send port/workload requests
                compNames.append(label);
                break;
            case NODE_KIND::COMPONENT_INST:
                // can send port/workload requests
                //compInstIDs_.append(QString::number(nodeItem->getParentID() + "_0"));
                compInstPaths.append(getItemLabel(nodeItem->getParentItem()) + ".%/" + label);
                break;
            case NODE_KIND::PORT_REPLIER_IMPL:
            case NODE_KIND::PORT_REQUESTER_IMPL:
            case NODE_KIND::PORT_PUBLISHER_IMPL:
            case NODE_KIND::PORT_SUBSCRIBER_IMPL:
                nodeItemID = viewController_.getNodeDefinitionID(nodeItemID);
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_SUBSCRIBER:
            case NODE_KIND::PORT_PERIODIC:
                // can send port requests
                if (selectedDataKinds_.contains(MEDEA::ChartDataKind::PORT_LIFECYCLE)) {
                    /*for (auto instID : viewController_.getNodeInstanceIDs(nodeItemID)) {
                        portIDs_.append(QString::number(instID) + "_0");
                    }*/
                    for (auto instItem : viewController_.getNodesInstances(item->getID())) {
                        if (instItem && instItem->getParentItem()) {
                            auto compInstItem = instItem->getParentItem();
                            portPaths.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                        }
                    }
                }
                break;
            case NODE_KIND::PORT_REPLIER_INST:
            case NODE_KIND::PORT_REQUESTER_INST:
            case NODE_KIND::PORT_PUBLISHER_INST:
            case NODE_KIND::PORT_SUBSCRIBER_INST:
            case NODE_KIND::PORT_PERIODIC_INST:
                // can send port requests
                if (selectedDataKinds_.contains(MEDEA::ChartDataKind::PORT_LIFECYCLE)) {
                    auto compInstItem = nodeItem->getParentItem();
                    if (compInstItem)
                        portPaths.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                }
                break;
            case NODE_KIND::CLASS_INST:
                // can send workload requests
                if (selectedDataKinds_.contains(MEDEA::ChartDataKind::WORKLOAD)) {
                    // a ClassInstance can be a child of either a CompImpl or CompInst
                    /*auto parentNodeKind = nodeItem->getParentNodeKind();
                    if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                        for (auto instID : viewController_.getNodeInstanceIDs(nodeItemID)) {
                            workerInstIDs_.append(QString::number(instID) + "_0");
                        }
                    } else if (parentNodeKind == NODE_KIND::COMPONENT_INST) {
                        workerInstIDs_.append(QString::number(nodeItemID) + "_0");
                    }*/
                    auto parentNodeKind = nodeItem->getParentNodeKind();
                    if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                        for (auto instItem : viewController_.getNodesInstances(item->getID())) {
                            if (instItem && instItem->getParentItem()) {
                                auto compInstItem = instItem->getParentItem();
                                workerInstPaths.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                            }
                        }
                    } else if (parentNodeKind == NODE_KIND::COMPONENT_INST) {
                        auto compInstItem = nodeItem->getParentItem();
                        if (compInstItem)
                            workerInstPaths.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                    }
                }
                break;
            case NODE_KIND::HARDWARE_NODE:
                // can send cpu/mem requests
                //nodeIDs_.append(label);
                nodeHostnames.append(label);
                break;
            default:
                break;
            }
        }

        builder.setComponentNames(compNames);
        builder.setComponentInstancePaths(compInstPaths);
        builder.setPortPaths(portPaths);
        builder.setWorkerInstancePaths(workerInstPaths);
        builder.setNodeHostnames(nodeHostnames);
    }

    requestEvents(builder);
    selectedDataKinds_.clear();
    selectedViewItems_.clear();
}


/**
 * @brief ExperimentDataManager::constructSingleton
 * @param vc
 */
void ExperimentDataManager::constructSingleton(ViewController *vc)
{
    if (!manager_) {
        manager_ = new ExperimentDataManager(*vc);
    }
}
