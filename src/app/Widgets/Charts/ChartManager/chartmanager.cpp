#include "chartmanager.h"
#include "requestbuilder.h"
#include "../../../Controllers/WindowManager/windowmanager.h"

#include <QFutureWatcher>

const int invalid_experiment_id = -1;

ChartManager* ChartManager::manager_ = nullptr;

/**
 * @brief ChartManager::ChartManager
 * @param vc
 */
ChartManager::ChartManager(const ViewController &vc)
    : viewController_(vc)
{
    chartDialog_ = new ChartDialog();
    dataflowDialog_ = new DataflowDialog();

    connect(&vc, &ViewController::vc_displayChartPopup, this, &ChartManager::displayChartPopup);
    connect(&vc, &ViewController::vc_viewItemsInChart, this, &ChartManager::filterRequestsBySelectedEntities);

    connect(&vc, &ViewController::modelClosed, chartDialog_, &ChartDialog::clear);
    connect(&vc, &ViewController::modelClosed, dataflowDialog_, &DataflowDialog::clear);

    connect(this, &ChartManager::showChartsPanel, &ChartManager::showDataflowPanel);
    connect(&chartPopup_, &ChartInputPopup::selectedExperimentRun, this, &ChartManager::experimentRunSelected);
}


/**
 * @brief ChartManager::aggregationProxy
 * @return
 */
AggregationProxy& ChartManager::aggregationProxy()
{
    try {
        return AggregationProxy::singleton();
    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        throw;
    }
}


/**
 * @brief ChartManager::timelineChartView
 * @return
 */
TimelineChartView& ChartManager::timelineChartView()
{
    return getChartDialog().getChartView();

}


/**
 * @brief ChartManager::requestExperimentRuns
 */
void ChartManager::requestExperimentRuns(const QString& experimentName)
{
    try {
        auto future = aggregationProxy().RequestExperimentRuns(experimentName);
        auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [&cp = chartPopup_, futureWatcher]() {
            try {
                cp.setExperimentRuns(futureWatcher->result().toList());
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
 * @brief ChartManager::requestExperimentState
 * @param experimentRunID
 */
void ChartManager::requestExperimentState(const quint32 experimentRunID)
{
    try {
        auto future = aggregationProxy().RequestExperimentState(experimentRunID);
        auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

        connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [this, futureWatcher]() {
            try {
                // once the state is received, request the events for the selected experiment run
                experimentRunStateReceived(futureWatcher->result());
            } catch (const std::exception& ex) {
                toastNotification("Failed to get experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
            }
        });

        futureWatcher->setFuture(future);

    } catch (const NoRequesterException& ex) {
        toastNotification("Failed to set up the Aggregation Server when requesting experiment runs: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("Failed to request experiment state: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ChartManager::requestEvents
 * This is the last function that is called, from when a valid experiment run is selected from the ChartInputPopup
 * This is where the chart data request chain ends
 * @param builder
 */
void ChartManager::requestEvents(const RequestBuilder& builder)
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

    // reset the selected run
    selectedExperimentRun_.experiment_run_id = invalid_experiment_id;
}


/**
 * @brief ChartManager::requestPortLifecycleEvents
 * @param request
 * @param experimentRun
 */
void ChartManager::requestPortLifecycleEvents(const PortLifecycleRequest &request, const AggServerResponse::ExperimentRun& experimentRun)
{
    try {
        auto future = aggregationProxy().RequestPortLifecycleEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                auto events = futureWatcher->result();
                if (events.isEmpty()) {
                    toastNotification("No port lifecycle events received for selection", "plug");
                } else {
                    emit showChartsPanel();
                    timelineChartView().addPortLifecycleEvents(experimentRun, events);
                }
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
 * @brief ChartManager::requestWorkloadEvents
 * @param request
 */
void ChartManager::requestWorkloadEvents(const WorkloadRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestWorkloadEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                auto events = futureWatcher->result();
                if (events.isEmpty()) {
                    toastNotification("No workload events received for selection", "plug");
                } else {
                    emit showChartsPanel();
                    timelineChartView().addWorkloadEvents(experimentRun, events);
                }
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
 * @brief ChartManager::requestCPUUtilisationEvents
 * @param request
 */
void ChartManager::requestCPUUtilisationEvents(const CPUUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestCPUUtilisationEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                auto events = futureWatcher->result();
                if (events.isEmpty()) {
                    toastNotification("No cpu utilisation events received for selection", "cpu");
                } else {
                    emit showChartsPanel();
                    timelineChartView().addCPUUtilisationEvents(experimentRun, events);
                }
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
 * @brief ChartManager::requestMemoryUtilisationEvents
 * @param request
 */
void ChartManager::requestMemoryUtilisationEvents(const MemoryUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestMemoryUtilisationEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                auto events = futureWatcher->result();
                if (events.isEmpty()) {
                    toastNotification("No memory utilisation events received for selection", "memoryCard");
                } else {
                    emit showChartsPanel();
                    timelineChartView().addMemoryUtilisationEvents(experimentRun, events);
                }
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
 * @brief ChartManager::requestMarkerEvents
 * @param request
 */
void ChartManager::requestMarkerEvents(const MarkerRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    try {
        auto future = aggregationProxy().RequestMarkerEvents(request);
        auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);

        connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [this, futureWatcher, experimentRun]() {
            try {
                auto events = futureWatcher->result();
                if (events.isEmpty()) {
                    toastNotification("No marker events received for selection", "bookmark");
                } else {
                    emit showChartsPanel();
                    timelineChartView().addMarkerEvents(experimentRun, events);
                }
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
 * @brief ChartManager::toastNotification
 * @param description
 * @param iconName
 * @param severity
 */
void ChartManager::toastNotification(const QString &description, const QString &iconName, Notification::Severity severity)
{
    NotificationManager::manager()->AddNotification(description, "Icons", iconName, severity, Notification::Type::APPLICATION, Notification::Category::NONE);
}


/**
 * @brief ChartManager::getItemLabel
 * @param item
 * @return
 */
QString ChartManager::getItemLabel(const ViewItem *item)
{
    if (item) {
        return item->getData("label").toString();
    }
    return "";
}


/**
 * @brief ChartManager::manager
 * @return
 */
ChartManager* ChartManager::manager()
{
    return manager_;
}


/**
 * @brief ChartManager::getChartDialog
 * @throws std::runtime_error
 * @return
 */
ChartDialog& ChartManager::getChartDialog()
{
    if (chartDialog_ != nullptr) {
        return *chartDialog_;
    } else {
        throw std::runtime_error("Could not get ChartDialog reference; chartDialog_ is null.");
    }
}


/**
 * @brief ChartManager::getDataflowDialog
 * @return
 */
DataflowDialog &ChartManager::getDataflowDialog()
{
    if (dataflowDialog_ != nullptr) {
        return *dataflowDialog_;
    } else {
        throw std::runtime_error("Could not get DataflowDialog referece; dataflowDialog_ is null.");
    }
}


/**
 * @brief ChartManager::displayChartPopup
 */
void ChartManager::displayChartPopup()
{
    requestExperimentRuns("");
    chartPopup_.setPopupVisible(true);
}


/**
 * @brief ChartManager::filterRequestsBySelectedEntities
 * @param selectedItems
 * @param selectedDataKinds
 */
void ChartManager::filterRequestsBySelectedEntities(const QVector<ViewItem*> &selectedItems, const QList<MEDEA::ChartDataKind> &selectedDataKinds)
{
    selectedViewItems_ = selectedItems;
    selectedDataKinds_ = selectedDataKinds;
    displayChartPopup();
}


/**
 * @brief ChartManager::experimentRunSelected
 * This slot is called when an experiment run was selected from the ChartInputPopup and "OK" was clicked
 * This is where the chart data request chain starts
 * @param experimentRun
 */
void ChartManager::experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun)
{
    if (experimentRun.experiment_run_id != invalid_experiment_id) {
        auto expRunID = static_cast<quint32>(experimentRun.experiment_run_id);
        selectedExperimentRun_ = experimentRun;
        requestExperimentState(expRunID);
    }
}


/**
 * @brief ChartManager::experimentRunStateReceived
 * @param experimentState
 */
void ChartManager::experimentRunStateReceived(AggServerResponse::ExperimentState experimentState)
{
    // check if there is a valid selected experiment run
    if (selectedExperimentRun_.experiment_run_id == invalid_experiment_id) {
        return;
    }

    auto expRunID = static_cast<quint32>(selectedExperimentRun_.experiment_run_id);
    auto stateID = static_cast<quint32>(experimentState.experiment_run_id);

    if (expRunID == stateID) {
        // updated the selected experiment run's last updated time
        auto expLastUpdatedTime = experimentState.last_updated_time;
        selectedExperimentRun_.last_updated_time = expLastUpdatedTime;
        // let the view know to update the time-range for the charts with the same expRunID
        timelineChartView().updateExperimentRunLastUpdatedTime(expRunID, expLastUpdatedTime);
        // setup/build the requests
        setupRequestsForExperimentRun(expRunID);
        // setup/display experiment data for PULSE
        getDataflowDialog().setExperimentInfo(selectedExperimentRun_.experiment_name, expRunID);
        getDataflowDialog().displayExperimentState(experimentState);
    }
}


/**
 * @brief ChartManager::setupRequestsForExperimentRun
 * @param experimentRunID
 */
void ChartManager::setupRequestsForExperimentRun(const quint32 experimentRunID)
{
    if (selectedDataKinds_.isEmpty()) {
        selectedDataKinds_ = MEDEA::Event::GetChartDataKinds();
        selectedDataKinds_.removeAll(MEDEA::ChartDataKind::DATA);
    }

    auto builder = RequestBuilder::build();
    builder.buildRequests(selectedDataKinds_);
    builder.setExperimentID(experimentRunID);

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
 * @brief ChartManager::constructSingleton
 * @param vc
 */
void ChartManager::constructSingleton(ViewController *vc)
{
    if (!manager_) {
        manager_ = new ChartManager(*vc);
    }
}
