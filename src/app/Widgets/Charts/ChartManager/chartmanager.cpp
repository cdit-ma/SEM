#include "chartmanager.h"
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
        connect(vc, &ViewController::vc_showChartsPopup, this, &ChartManager::showChartsPopup);
        connect(vc, &ViewController::vc_viewItemsInChart, this, &ChartManager::filterRequestsBySelection);
    }

    connect(this, &ChartManager::showChartsPanel, chartDialog_, &ChartDialog::showChartsDockWidget);
    connect(chartPopup_, &ChartInputPopup::requestEventsForExperimentRun, this, &ChartManager::requestEventsForExperimentRun);
    connect(chartPopup_, &ChartInputPopup::rejected, [=]() { resetFilters(); });
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
    auto futureWatcher = new QFutureWatcher<QVector<ExperimentRun>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<ExperimentRun>>::finished, [=]() {
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
 * @param experiment_run_id
 */
void ChartManager::requestExperimentState(const quint32 experiment_run_id)
{
    auto future = viewController_->getAggregationProxy().RequestExperimentState(experiment_run_id);
    auto futureWatcher = new QFutureWatcher<ExperimentState>(this);

    connect(futureWatcher, &QFutureWatcher<ExperimentState>::finished, [=]() {
        try {
            auto state = futureWatcher->result();
            auto lastUpdatedTime = state.last_updated_time;
            if (selectedExperimentRun_.experiment_run_id == experiment_run_id)
                selectedExperimentRun_.last_updated_time = lastUpdatedTime;
            if (chartView_)
                chartView_->updateExperimentRunLastUpdatedTime(experiment_run_id, lastUpdatedTime);
            qDebug() << "[Experiment State Request]";
            qDebug() << "last-updated-time: " << QDateTime::fromMSecsSinceEpoch(state.last_updated_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "end-time: " << QDateTime::fromMSecsSinceEpoch(state.end_time).toString("MMM d, hh:mm:ss.zzz");
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestPortLifecycleEvents
 * @param experiment_run_id
 * @param time_intervals
 * @param component_instance_ids
 * @param port_ids
 */
void ChartManager::requestPortLifecycleEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &component_instance_ids, const QVector<QString> &port_ids)
{
    auto future = viewController_->getAggregationProxy().RequestPortLifecycleEvents(experiment_run_id, time_intervals, component_instance_ids, port_ids);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No workload events received for selection", "spanner");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addPortLifecycleEvents(selectedExperimentRun_, events);
            }
            qDebug() << "[PortLifecycle Request] Result size#: " << events.size();
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request port lifecycle events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestWorkloadEvents
 * @param experiment_run_id
 * @param time_intervals
 * @param component_instance_ids
 * @param worker_ids
 */
void ChartManager::requestWorkloadEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &component_instance_ids, const QVector<QString> &worker_ids)
{
    auto future = viewController_->getAggregationProxy().RequestWorkloadEvents(experiment_run_id, time_intervals, component_instance_ids, worker_ids);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No workload events received for selection", "spanner");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addWorkloadEvents(selectedExperimentRun_, events);
            }
            qDebug() << "[Workload Request] Result size#: " << events.size();
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request workload events - " + QString::fromStdString(ex.what()), "spanner", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestCPUUtilisationEvents
 * @param experiment_run_id
 * @param time_intervals
 * @param graphml_ids
 */
void ChartManager::requestCPUUtilisationEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &graphml_ids)
{
    auto future = viewController_->getAggregationProxy().RequestCPUUtilisationEvents(experiment_run_id, time_intervals, graphml_ids);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No cpu utilisation events received for selection", "cpu");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addCPUUtilisationEvents(selectedExperimentRun_, events);
            }
            qDebug() << "[CPUUtilisation Request] Result size#: " << events.size();
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request cpu utilisation events - " + QString::fromStdString(ex.what()), "cpu", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ChartManager::requestMemoryUtilisationEvents
 * @param experiment_run_id
 * @param time_intervals
 * @param graphml_ids
 */
void ChartManager::requestMemoryUtilisationEvents(const quint32 experiment_run_id, const QVector<qint64> &time_intervals, const QVector<QString> &graphml_ids)
{
    auto future = viewController_->getAggregationProxy().RequestMemoryUtilisationEvents(experiment_run_id, time_intervals, graphml_ids);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [=]() {
        try {
            auto events = futureWatcher->result();
            if (events.isEmpty()) {
                toastNotification("No memory utilisation events received for selection", "memoryCard");
            } else {
                emit showChartsPanel();
                if (chartView_ )
                    chartView_->addMemoryUtilisationEvents(selectedExperimentRun_, events);
            }
            qDebug() << "[MemoryUtilisation Request] Result size#: " << events.size();
            qDebug() << "-----------------------------------------------------------";
        } catch (const std::exception& ex) {
            toastNotification("Failed to request memory utilisation events - " + QString::fromStdString(ex.what()), "memoryCard", Notification::Severity::ERROR);
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
 * @brief ChartManager::resetFilters
 */
void ChartManager::resetFilters()
{
    selectedExperimentRun_.experiment_run_id = -1;
    hasEntitySelection_ = false;
    eventKinds_.clear();
    compNames_.clear();
    compInstPaths_.clear();
    compInstIDs_.clear();
    portPaths_.clear();
    portIDs_.clear();
    workerInstPaths_.clear();
    workerInstIDs_.clear();
    nodeHostnames_.clear();
    nodeIDs_.clear();
}


/**
 * @brief ChartManager::getItemLabel
 * @param item
 * @return
 */
QString ChartManager::getItemLabel(ViewItem *item)
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
 * @brief ChartManager::showChartsPopup
 */
void ChartManager::showChartsPopup()
{
    if (chartPopup_) {
        chartPopup_->setPopupVisible(true);
    }
    requestExperimentRuns("");
}


/**
 * @brief ChartManager::filterRequestsBySelection
 * This slot is called when the user has selected an entity(s) to view chart data for
 * @param selectedItems
 * @param selectedDataKinds
 */
void ChartManager::filterRequestsBySelection(const QVector<ViewItem *> &selectedItems, const QList<TIMELINE_DATA_KIND> &selectedDataKinds)
{
    // at this point, all selected node items should have a valid chart data kind to show
    // all selected items are from a single aspect - the active aspect

    if (!viewController_ || selectedDataKinds.isEmpty())
        return;

    // clear request filters
    resetFilters();

    hasEntitySelection_ = !selectedItems.isEmpty();
    eventKinds_ = selectedDataKinds;

    // send a separate filtered request per selected node item
    for (auto item : selectedItems) {

        if (!item || !item->isNode())
            continue;

        auto nodeItem = (NodeViewItem*) item;
        auto nodeItemID = nodeItem->getID();
        auto label = getItemLabel(nodeItem);

        switch (nodeItem->getNodeKind()) {
        case NODE_KIND::COMPONENT:
        case NODE_KIND::COMPONENT_IMPL:
            // can send port/workload requests
            compNames_.append(label);
            break;
        case NODE_KIND::COMPONENT_INST:
            // can send port/workload requests
            compInstIDs_.append(QString::number(nodeItem->getParentID()));
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
            if (selectedDataKinds.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                    portIDs_.append(QString::number(instID) + "_0");
                }
            }
            break;
        case NODE_KIND::PORT_REPLIER_INST:
        case NODE_KIND::PORT_REQUESTER_INST:
        case NODE_KIND::PORT_PUBLISHER_INST:
        case NODE_KIND::PORT_SUBSCRIBER_INST:
        case NODE_KIND::PORT_PERIODIC_INST:
            // can send port requests
            if (selectedDataKinds.contains(TIMELINE_DATA_KIND::PORT_LIFECYCLE)) {
                auto compInstItem = nodeItem->getParentItem();
                if (compInstItem)
                    portPaths_.append(getItemLabel(compInstItem->getParentItem()) + ".%/" + getItemLabel(compInstItem) + "/" + label);
            }
            break;
        case NODE_KIND::CLASS_INST:
            // can send workload requests
            if (selectedDataKinds.contains(TIMELINE_DATA_KIND::WORKLOAD)) {
                // a ClassInstance can be a child of either a CompImpl or CompInst
                auto parentNodeKind = nodeItem->getParentNodeKind();
                if (parentNodeKind == NODE_KIND::COMPONENT_IMPL) {
                    for (auto instID : viewController_->getNodeInstanceIDs(nodeItemID)) {
                        workerInstIDs_.append(QString::number(instID));
                    }
                } else if (parentNodeKind == NODE_KIND::COMPONENT_INST) {
                    workerInstIDs_.append(QString::number(nodeItemID));
                }
            }
            break;
        case NODE_KIND::HARDWARE_NODE:
            // can send cpu/mem requests
            nodeIDs_.append(label);
            break;
        default:
            break;
        }
    }

    // show the popup so that the user can select which experiment run they want to display data from
    if (chartPopup_)
        chartPopup_->setPopupVisible(true);
}


/**
 * @brief ChartManager::requestEventsForExperimentRun
 * @param experimentRun
 */
void ChartManager::requestEventsForExperimentRun(const ExperimentRun& experimentRun)
{
    auto experimentRunID = experimentRun.experiment_run_id;
    if (experimentRunID == -1)
        return;

    selectedExperimentRun_ = experimentRun;
    requestExperimentState(experimentRunID);

    qDebug() << "-----------------------------------------------------------";

    if (hasEntitySelection_) {
        for (auto kind : eventKinds_) {
            switch (kind) {
            case TIMELINE_DATA_KIND::PORT_LIFECYCLE:
                requestPortLifecycleEvents(experimentRunID, {}, compInstIDs_, portIDs_);
                break;
            case TIMELINE_DATA_KIND::WORKLOAD:
                requestWorkloadEvents(experimentRunID, {}, compInstIDs_, workerInstIDs_);
                break;
            case TIMELINE_DATA_KIND::CPU_UTILISATION:
                requestCPUUtilisationEvents(experimentRunID, {}, nodeIDs_);
                break;
            case TIMELINE_DATA_KIND::MEMORY_UTILISATION:
                requestMemoryUtilisationEvents(experimentRunID, {}, nodeIDs_);
                break;
            default:
                toastNotification("No valid chart data for selection", "chart", Notification::Severity::ERROR);
                break;
            }
        }
    } else {
        // if there is no selection, request all events for the selected experiment run
        requestPortLifecycleEvents(experimentRunID, {}, compInstIDs_, portIDs_);
        requestWorkloadEvents(experimentRunID, {}, compInstIDs_, workerInstIDs_);
        requestCPUUtilisationEvents(experimentRunID, {}, nodeIDs_);
        requestMemoryUtilisationEvents(experimentRunID, {}, nodeIDs_);
    }
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
