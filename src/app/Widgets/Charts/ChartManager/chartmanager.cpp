#include "chartmanager.h"
#include "requestbuilder.h"
#include "../../../Controllers/WindowManager/windowmanager.h"

#include <QFutureWatcher>

ChartManager* ChartManager::manager_ = 0;

/**
 * @brief ChartManager::ChartManager
 * @param vc
 */
ChartManager::ChartManager(ViewController *vc)
{
    viewController_ = vc;

    chartDialog_ = new ChartDialog();
    chartPopup_ = new ChartInputPopup(chartDialog_);
    chartView_ = new TimelineChartView(chartDialog_);

    chartDialog_->setChartView(chartView_);

    if (vc) {
        connect(vc, &ViewController::vc_displayChartPopup, this, &ChartManager::displayChartPopup);
        connect(vc, &ViewController::vc_viewItemsInChart, this, &ChartManager::filterRequestsBySelectedEntities);
        connect(vc, &ViewController::modelClosed, chartDialog_, &ChartDialog::clear);
    }

    connect(this, &ChartManager::showChartsPanel, chartDialog_, &ChartDialog::showChartsDockWidget);
    connect(chartPopup_, &ChartInputPopup::selectedExperimentRun, this, &ChartManager::experimentRunSelected);
}


/**
 * @brief ChartManager::~ChartManager
 */
ChartManager::~ChartManager()
{
    if (chartDialog_)
        chartDialog_->deleteLater();
    if (chartPopup_)
        chartPopup_->deleteLater();
    if (chartView_)
        chartView_->deleteLater();
}


/**
 * @brief ChartManager::requestExperimentRuns
 */
void ChartManager::requestExperimentRuns(const QString& experimentName)
{
    auto future = viewController_->getAggregationProxy().RequestExperimentRuns(experimentName);
    auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [=]() {
        try {
            if (chartPopup_)
                chartPopup_->setExperimentRuns(futureWatcher->result().toList());
        } catch(const std::exception& ex) {
            toastNotification("Failed to request experiment runs - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestExperimentState
 * @param experimentRunID
 */
void ChartManager::requestExperimentState(const quint32 experimentRunID)
{
    auto future = viewController_->getAggregationProxy().RequestExperimentState(experimentRunID);
    auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

    connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [=]() {
        try {
            auto state = futureWatcher->result();
            if (chartView_) {
                chartView_->updateExperimentRunLastUpdatedTime(experimentRunID, state.last_updated_time);
            }
            /*
            qDebug() << "[Experiment State Request]";
            qDebug() << "last-updated-time: " << QDateTime::fromMSecsSinceEpoch(state.last_updated_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "end-time: " << QDateTime::fromMSecsSinceEpoch(state.end_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "-----------------------------------------------------------------";
            */
        } catch (const std::exception& ex) {
            toastNotification("Failed to request experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestEvents
 * @param builder
 * @param experimentRun
 */
void ChartManager::requestEvents(const RequestBuilder& builder, const AggServerResponse::ExperimentRun& experimentRun)
{  
    try {
        requestPortLifecycleEvents(builder.getPortLifecycleRequest(), experimentRun);
    } catch (const std::exception&) {
        qInfo("No PortLifecycleRequest");
    }

    try {
        requestWorkloadEvents(builder.getWorkloadRequest(), experimentRun);
    } catch (const std::exception&) {
        qInfo("No WorkloadRequest");
    }

    try {
        requestCPUUtilisationEvents(builder.getCPUUtilisationRequest(), experimentRun);
    } catch (const std::exception&) {
        qInfo("No CPUUtilisationRequest");
    }

    try {
        requestMemoryUtilisationEvents(builder.getMemoryUtilisationRequest(), experimentRun);
    } catch (const std::exception&) {
        qInfo("No MemoryUtilisationRequest");
    }

    try {
        requestMarkerEvents(builder.getMarkerRequest(), experimentRun);
    } catch (const std::exception&) {
        qInfo("No MarkerRequest");
    }
}


/**
 * @brief ChartManager::requestPortLifecycleEvents
 * @param request
 * @param experimentRun
 */
void ChartManager::requestPortLifecycleEvents(const PortLifecycleRequest &request, const AggServerResponse::ExperimentRun& experimentRun)
{
    auto future = viewController_->getAggregationProxy().RequestPortLifecycleEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No port lifecycle events received for selection", "plug");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addPortLifecycleEvents(experimentRun, events);
            }
            //qDebug() << "[PortLifecycle Request] Result size#: " << events.size();
            //qDebug() << "-----------------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request port lifecycle events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });
    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestWorkloadEvents
 * @param request
 */
void ChartManager::requestWorkloadEvents(const WorkloadRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    auto future = viewController_->getAggregationProxy().RequestWorkloadEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No port lifecycle events received for selection", "plug");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addWorkloadEvents(experimentRun, events);
            }
            //qDebug() << "[Workload Request] Result size#: " << events.size();
            //qDebug() << "-----------------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request port lifecycle events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });
    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestCPUUtilisationEvents
 * @param request
 */
void ChartManager::requestCPUUtilisationEvents(const CPUUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    auto future = viewController_->getAggregationProxy().RequestCPUUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No cpu utilisation events received for selection", "cpu");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addCPUUtilisationEvents(experimentRun, events);
            }
            //qDebug() << "[CPUUtilisation Request] Result size#: " << events.size();
            //qDebug() << "-----------------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request cpu utilisation events - " + QString::fromStdString(ex.what()), "cpu", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestMemoryUtilisationEvents
 * @param request
 */
void ChartManager::requestMemoryUtilisationEvents(const MemoryUtilisationRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    auto future = viewController_->getAggregationProxy().RequestMemoryUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No memory utilisation events received for selection", "memoryCard");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addMemoryUtilisationEvents(experimentRun, events);
            }
            //qDebug() << "[MemoryUtilisation Request] Result size#: " << events.size();
            //qDebug() << "-----------------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request memory utilisation events - " + QString::fromStdString(ex.what()), "memoryCard", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestMarkerEvents
 * @param request
 */
void ChartManager::requestMarkerEvents(const MarkerRequest &request, const AggServerResponse::ExperimentRun &experimentRun)
{
    auto future = viewController_->getAggregationProxy().RequestMarkerEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No marker events received for selection", "bookmark");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addMarkerEvents(experimentRun, events);
            }
            //qDebug() << "[Marker Request] Result size#: " << events.size();
            //qDebug() << "-----------------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request marker events - " + QString::fromStdString(ex.what()), "bookmark", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
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
 * @brief ChartManager::getChartDialog
 * @return
 */
ChartManager* ChartManager::manager()
{
    return manager_;
}


/**
 * @brief ChartManager::getChartDialog
 * @return
 */
ChartDialog* ChartManager::getChartDialog()
{
    return chartDialog_;
}


/**
 * @brief ChartManager::displayChartPopup
 */
void ChartManager::displayChartPopup()
{
    requestExperimentRuns("");
    if (chartPopup_) {
        chartPopup_->setPopupVisible(true);
    }
}


/**
 * @brief ChartManager::filterRequestsBySelectedEntities
 * @param selectedItems
 * @param selectedDataKinds
 */
void ChartManager::filterRequestsBySelectedEntities(const QVector<ViewItem*> &selectedItems, const QList<TIMELINE_DATA_KIND> &selectedDataKinds)
{
    selectedViewItems_ = selectedItems;
    selectedDataKinds_ = selectedDataKinds;
    displayChartPopup();
}


/**
 * @brief ChartManager::experimentRunSelected
 * @param experimentRun
 */
void ChartManager::experimentRunSelected(const AggServerResponse::ExperimentRun& experimentRun)
{
    // request the experiment state to get the experiment run's last updated time
    auto experimentRunID = experimentRun.experiment_run_id;
    auto future = viewController_->getAggregationProxy().RequestExperimentState(experimentRunID);
    auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

    connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [=]() {
        try {
            // once the state is received, request the events for the selected experiment run
            experimentRunStateReceived(experimentRun, futureWatcher->result());
        } catch (const std::exception& ex) {
            toastNotification("Failed to request experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::experimentRunStateReceived
 * @param experimentRun
 * @param experimentState
 */
void ChartManager::experimentRunStateReceived(AggServerResponse::ExperimentRun experimentRun, AggServerResponse::ExperimentState experimentState)
{
    experimentRun.last_updated_time = experimentState.last_updated_time;
    requestEventsForExperimentRun(experimentRun);
}


/**
 * @brief ChartManager::requestEventsForExperimentRun
 * @param experimentRun
 */
void ChartManager::requestEventsForExperimentRun(const AggServerResponse::ExperimentRun& experimentRun)
{
    auto experimentRunID = experimentRun.experiment_run_id;
    if (experimentRunID == -1)
        return;

    if (selectedDataKinds_.isEmpty()) {
        selectedDataKinds_ = GET_TIMELINE_DATA_KINDS();
        selectedDataKinds_.removeAll(TIMELINE_DATA_KIND::DATA);
    }

    auto builder = RequestBuilder::build();
    builder.buildRequests(selectedDataKinds_);
    builder.setExperimentID(experimentRunID);

    //qDebug() << "-----------------------------FILTERS-----------------------------";

    if (!selectedViewItems_.isEmpty()) {

        QVector<QString> compNames;
        QVector<QString> compInstPaths;
        QVector<QString> portPaths;
        QVector<QString> workerInstPaths;
        QVector<QString> nodeHostnames;

        for (auto item : selectedViewItems_) {

            if (!item || !item->isNode())
                continue;

            auto nodeItem = (NodeViewItem*) item;
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
                nodeItemID = viewController_->getNodeDefinitionID(nodeItemID);
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_SUBSCRIBER:
            case NODE_KIND::PORT_PERIODIC:
                // can send port requests
                if (selectedDataKinds_.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                    /*for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                        portIDs_.append(QString::number(instID) + "_0");
                    }*/
                    for (auto instItem : viewController_->getNodesInstances(item->getID())) {
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
                if (selectedDataKinds_.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                    auto compInstItem = nodeItem->getParentItem();
                    if (compInstItem)
                        portPaths.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
                }
                break;
            case NODE_KIND::CLASS_INST:
                // can send workload requests
                if (selectedDataKinds_.contains(TIMELINE_DATA_KIND::WORKLOAD)) {
                    // a ClassInstance can be a child of either a CompImpl or CompInst
                    /*auto parentNodeKind = nodeItem->getParentNodeKind();
                    if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                        for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                            workerInstIDs_.append(QString::number(instID) + "_0");
                        }
                    } else if (parentNodeKind == NODE_KIND::COMPONENT_INST) {
                        workerInstIDs_.append(QString::number(nodeItemID) + "_0");
                    }*/
                    auto parentNodeKind = nodeItem->getParentNodeKind();
                    if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                        for (auto instItem : viewController_->getNodesInstances(item->getID())) {
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

    //qDebug() << "-----------------------------------------------------------------";

    requestEvents(builder, experimentRun);
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
        manager_ = new ChartManager(vc);
    }
}
