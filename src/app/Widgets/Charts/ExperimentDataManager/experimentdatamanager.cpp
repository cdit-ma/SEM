#include "experimentdatamanager.h"
#include "requestbuilder.h"
#include "../../../Controllers/WindowManager/windowmanager.h"

#include <QFutureWatcher>

const int default_playback_interval_ms = 33;
const int invalid_experiment_id = -1;
ExperimentDataManager* ExperimentDataManager::manager_ = nullptr;

/**
 * @brief ExperimentDataManager::ExperimentDataManager
 * @param vc
 */
ExperimentDataManager::ExperimentDataManager(const ViewController& vc)
    : viewController_(vc)
{
    chartDialog_ = new ChartDialog();
    dataflowDialog_ = new DataflowDialog();

    connect(&vc, &ViewController::modelClosed, this, &ExperimentDataManager::clear);
    connect(&vc, &ViewController::vc_displayChartPopup, this, &ExperimentDataManager::showChartInputPopup);
    connect(&vc, &ViewController::vc_viewItemsInChart, this, &ExperimentDataManager::filterRequestsBySelectedEntities);

    connect(&chartPopup_, &ChartInputPopup::visualiseExperimentRunData, this, &ExperimentDataManager::visualiseSelectedExperimentRun);
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
 * @brief ExperimentDataManager::requestExperimentData
 * @param request_type
 * @throws std::invalid_argument
 */
void ExperimentDataManager::requestExperimentData(ExperimentDataRequestType request_type, const QVariant& request_param, QObject* sender_obj)
{
    if (request_param.isNull() || !request_param.isValid()) {
        throw std::invalid_argument("ExperimentDataManager::requestExperimentData - Provided request parameter is invalid.");
    }

    try {

        bool valid = true;

        switch (request_type) {
        case ExperimentDataRequestType::ExperimentRun: {
            const auto& exp_name = request_param.toString();
            requestExperimentRuns(exp_name, qobject_cast<MEDEA::ExperimentData*>(sender_obj));
            break;
        }
        case ExperimentDataRequestType::ExperimentState: {
            valid = request_param.canConvert<quint32>();
            if (valid) {
                quint32 exp_run_id = request_param.toUInt();
                requestExperimentState(exp_run_id, qobject_cast<MEDEA::ExperimentData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::PortLifecycleEvent: {
            valid = request_param.canConvert<PortLifecycleRequest>();
            if (valid) {
                auto request = request_param.value<PortLifecycleRequest>();
                requestPortLifecycleEvents(request, selectedExperimentRun_, qobject_cast<PortInstanceData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::WorkloadEvent: {
            valid = request_param.canConvert<WorkloadRequest>();
            if (valid) {
                auto request = request_param.value<WorkloadRequest>();
                requestWorkloadEvents(request, selectedExperimentRun_, qobject_cast<WorkerInstanceData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::CPUUtilisationEvent: {
            valid = request_param.canConvert<CPUUtilisationRequest>();
            if (valid) {
                auto request = request_param.value<CPUUtilisationRequest>();
                requestCPUUtilisationEvents(request, selectedExperimentRun_, qobject_cast<NodeData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::MemoryUtilisationEvent: {
            valid = request_param.canConvert<MemoryUtilisationRequest>();
            if (valid) {
                auto request = request_param.value<MemoryUtilisationRequest>();
                requestMemoryUtilisationEvents(request, selectedExperimentRun_, qobject_cast<NodeData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::MarkerEvent: {
            valid = request_param.canConvert<MarkerRequest>();
            if (valid) {
                auto request = request_param.value<MarkerRequest>();
                requestMarkerEvents(request, selectedExperimentRun_, qobject_cast<MarkerSetData*>(sender_obj));
            }
            break;
        }
        case ExperimentDataRequestType::PortEvent: {
            valid = request_param.canConvert<PortEventRequest>();
            if (valid) {
                auto request = request_param.value<PortEventRequest>();
                requestPortEvents(request, selectedExperimentRun_, qobject_cast<PortInstanceData*>(sender_obj));
            }
            break;
        }
        }

        if (!valid) {
            auto&& request_str = QString::number(static_cast<int>(request_type)).toStdString() + request_param.toString().toStdString();
            throw std::invalid_argument("Failed to request data (" + request_str + ") - Invalid request parameter.");
        }

    } catch (const NoRequesterException& ex) {
        toastNotification("ExperimentDataManager::requestExperimentData - Failed to set up the aggregation server: " + ex.What(), "chart", Notification::Severity::ERROR);
    } catch (const RequestException& ex) {
        toastNotification("ExperimentDataManager::requestExperimentData - Failed to request experiment data: " + ex.What(), "chart", Notification::Severity::ERROR);
    } catch (const std::exception& ex) {
        toastNotification("ExperimentDataManager::requestExperimentData: " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
    }
}


/**
 * @brief ExperimentDataManager::requestExperimentRuns
 * @param experimentName
 * @param exp_data_requester
 */
void ExperimentDataManager::requestExperimentRuns(const QString& experimentName, MEDEA::ExperimentData* exp_data_requester)
{
    auto future = aggregationProxy().RequestExperimentRuns(experimentName);
    auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [this, experimentName, futureWatcher, exp_data_requester]() {
        try {
            auto&& exp_runs = futureWatcher->result();
            if (exp_data_requester != nullptr) {
                for (const auto& run : exp_runs) {
                    exp_data_requester->addExperimentRun(run);
                }
            } else {
                processExperimentRuns(experimentName, futureWatcher->result());
            }
        } catch(const std::exception& ex) {
            toastNotification("Failed to get experiment runs - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestExperimentState
 * @param experimentRunID
 * @param exp_data_requester
 */
void ExperimentDataManager::requestExperimentState(const quint32 experimentRunID, MEDEA::ExperimentData* exp_data_requester)
{
    auto future = aggregationProxy().RequestExperimentState(experimentRunID);
    auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

    connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [this, experimentRunID, futureWatcher, exp_data_requester]() {
        try {
            auto&& exp_state = futureWatcher->result();
            if (exp_data_requester != nullptr) {
                // Setup the sender experiment run's state
                exp_data_requester->updateData(experimentRunID, exp_state);
            } else {
                // Once the state is received, request the events for the selected experiment run
                processExperimentState(selectedExperimentRun_.experiment_name, experimentRunID, exp_state);
                // NOTE: This needs to be called last because it clears the selectedExperimentRun_
                setupRequestsForExperimentRun(experimentRunID);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestEvents
 * This is the last function that is called, from when a valid experiment run is selected from the ChartInputPopup
 * This is where the chart data request chain ends
 * @param builder
 */
void ExperimentDataManager::requestEvents(const RequestBuilder& builder)
{
    // Check if there is a valid selected experiment run
    if (selectedExperimentRun_.experiment_run_id == invalid_experiment_id) {
        toastNotification("ExperimentDataManager::requestEvents - Aborted requests; there is no valid selected experiment run", "chart", Notification::Severity::ERROR);
        return;
    }

    try {
        auto&& request_param = QVariant::fromValue<PortLifecycleRequest>(builder.getPortLifecycleRequest());
        requestExperimentData(ExperimentDataRequestType::PortLifecycleEvent, request_param);
    } catch (const std::exception&) {
        qInfo("No PortLifecycleRequest");
    }
    try {
        auto&& request_param = QVariant::fromValue<WorkloadRequest>(builder.getWorkloadRequest());
        requestExperimentData(ExperimentDataRequestType::WorkloadEvent, request_param);
    } catch (const std::exception&) {
        qInfo("No WorkloadRequest");
    }
    try {
        auto&& request_param = QVariant::fromValue<CPUUtilisationRequest>(builder.getCPUUtilisationRequest());
        requestExperimentData(ExperimentDataRequestType::CPUUtilisationEvent, request_param);
    } catch (const std::exception&) {
        qInfo("No CPUUtilisationRequest");
    }
    try {
        auto&& request_param = QVariant::fromValue<MemoryUtilisationRequest>(builder.getMemoryUtilisationRequest());
        requestExperimentData(ExperimentDataRequestType::MemoryUtilisationEvent, request_param);
    } catch (const std::exception&) {
        qInfo("No MemoryUtilisationRequest");
    }
    try {
        auto&& request_param = QVariant::fromValue<MarkerRequest>(builder.getMarkerRequest());
        requestExperimentData(ExperimentDataRequestType::MarkerEvent, request_param);
    } catch (const std::exception&) {
        qInfo("No MarkerRequest");
    }
    try {
        auto&& request_param = QVariant::fromValue<PortEventRequest>(builder.getPortEventRequest());
        requestExperimentData(ExperimentDataRequestType::PortEvent, request_param);
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
 * @param port_data_requester
 */
void ExperimentDataManager::requestPortLifecycleEvents(const PortLifecycleRequest& request, const AggServerResponse::ExperimentRun& experimentRun, PortInstanceData* port_data_requester)
{
    auto future = aggregationProxy().RequestPortLifecycleEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [this, futureWatcher, experimentRun, port_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (port_data_requester != nullptr) {
                port_data_requester->addPortLifecycleEvents(events);
            } else {
                processPortLifecycleEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get port lifecycle events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestWorkloadEvents
 * @param request
 * @param experimentRun
 * @param worker_inst_data_requester
 */
void ExperimentDataManager::requestWorkloadEvents(const WorkloadRequest& request, const AggServerResponse::ExperimentRun& experimentRun, WorkerInstanceData* worker_inst_data_requester)
{
    auto future = aggregationProxy().RequestWorkloadEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [this, futureWatcher, experimentRun, worker_inst_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (worker_inst_data_requester != nullptr) {
                worker_inst_data_requester->addWorkloadEvents(events);
            } else {
                processWorkloadEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get workload events - " + QString::fromStdString(ex.what()), "spanner", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestCPUUtilisationEvents
 * @param request
 * @param experimentRun
 * @param node_data_requester
 */
void ExperimentDataManager::requestCPUUtilisationEvents(const CPUUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun, NodeData* node_data_requester)
{
    auto future = aggregationProxy().RequestCPUUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun, node_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (node_data_requester != nullptr) {
                node_data_requester->addCPUUtilisationEvents(events);
            } else {
                processCPUUtilisationEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get cpu utilisation events - " + QString::fromStdString(ex.what()), "cpu", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestMemoryUtilisationEvents
 * @param request
 * @param experimentRun
 * @param node_data_requester
 */
void ExperimentDataManager::requestMemoryUtilisationEvents(const MemoryUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun, NodeData* node_data_requester)
{
    auto future = aggregationProxy().RequestMemoryUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [this, futureWatcher, experimentRun, node_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (node_data_requester != nullptr) {
                node_data_requester->addMemoryUtilisationEvents(events);
            } else {
                processMemoryUtilisationEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get memory utilisation events - " + QString::fromStdString(ex.what()), "memoryCard", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestMarkerEvents
 * @param request
 * @param experimentRun
 * @param marker_data_requester
 */
void ExperimentDataManager::requestMarkerEvents(const MarkerRequest& request, const AggServerResponse::ExperimentRun& experimentRun, MarkerSetData* marker_data_requester)
{
    auto future = aggregationProxy().RequestMarkerEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [this, futureWatcher, experimentRun, marker_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (marker_data_requester != nullptr) {
                marker_data_requester->addMarkerEvents(events);
            } else {
                processMarkerEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get marker events - " + QString::fromStdString(ex.what()), "bookmark", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}


/**
 * @brief ExperimentDataManager::requestPortEvents
 * @param request
 * @param experimentRun
 * @param port_data_requester
 */
void ExperimentDataManager::requestPortEvents(const PortEventRequest& request, const AggServerResponse::ExperimentRun &experimentRun, PortInstanceData* port_data_requester)
{
    auto future = aggregationProxy().RequestPortEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<PortEvent*>>::finished, [this, futureWatcher, experimentRun, port_data_requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (port_data_requester != nullptr) {
                port_data_requester->addPortEvents(events);
            } else {
                processPortEvents(experimentRun, events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get port events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
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
                exp_data = constructExperimentData(exp_run_name);
            }
            exp_data->addExperimentRun(exp_run);
            exp_run_names_.insert(static_cast<quint32>(exp_run.experiment_run_id), exp_run_name);
        }
    } else {
        MEDEA::ExperimentData* exp_data = experiment_data_hash_.value(experiment_name, nullptr);
        if (exp_data == nullptr) {
            exp_data = constructExperimentData(experiment_name);
        }
        for (const auto& exp_run : experiment_runs) {
            exp_data->addExperimentRun(exp_run);
            exp_run_names_.insert(static_cast<quint32>(exp_run.experiment_run_id), experiment_name);
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
    if (selectedExperimentRun_.experiment_run_id != invalid_experiment_id) {
        auto exp_data = experiment_data_hash_.value(selectedExperimentRun_.experiment_name, nullptr);
        if (exp_data == nullptr) {
            throw std::invalid_argument("ExperimentDataManager::processExperimentState - There is no ExperimentData with experiment name: " + experiment_name.toStdString());
        }
        if (show_in_pulse_) {
            // Setup/display experiment data for PULSE
            auto& exp_run_data = exp_data->getExperimentRun(experiment_run_id);
            exp_run_data.updateData(experiment_state);
            getDataflowDialog().constructGraphicsItemsForExperimentRun(experiment_name, exp_run_data);
            emit showDataflowPanel();
        }
        if (show_in_charts_) {
            // Updated the selected experiment run's last updated time - the selected run is passed on to the chartview
            auto&& last_updated_time = experiment_state.last_updated_time;
            selectedExperimentRun_.last_updated_time = last_updated_time;
            // Let the chartview know to update the time-range for the charts associated with the experiment_run_id
            timelineChartView().updateExperimentRunLastUpdatedTime(experiment_run_id, last_updated_time);
        }
    }
}


/**
 * @brief ExperimentDataManager::processPortLifecycleEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processPortLifecycleEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortLifecycleEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No port lifecycle events received for selection", "plug");
        } else {
            emit showChartsPanel();
            timelineChartView().addPortLifecycleEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::processWorkloadEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processWorkloadEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<WorkloadEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No workload events received for selection", "spanner");
        } else {
            emit showChartsPanel();
            timelineChartView().addWorkloadEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::processCPUUtilisationEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processCPUUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<CPUUtilisationEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No cpu utilisation events received for selection", "cpu");
        } else {
            emit showChartsPanel();
            timelineChartView().addCPUUtilisationEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::processMemoryUtilisationEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processMemoryUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MemoryUtilisationEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No memory utilisation events received for selection", "memoryCard");
        } else {
            emit showChartsPanel();
            timelineChartView().addMemoryUtilisationEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::processMarkerEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processMarkerEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MarkerEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No marker events received for selection", "bookmark");
        } else {
            emit showChartsPanel();
            timelineChartView().addMarkerEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::processPortEvents
 * @param exp_run
 * @param events
 */
void ExperimentDataManager::processPortEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortEvent*>& events)
{
    if (show_in_charts_) {
        if (events.isEmpty()) {
            toastNotification("No port events received for selection", "plug");
        } else {
            emit showChartsPanel();
            timelineChartView().addPortEvents(exp_run, events);
        }
    }
}


/**
 * @brief ExperimentDataManager::constructExperimentData
 * @param experiment_name
 * @return
 */
MEDEA::ExperimentData* ExperimentDataManager::constructExperimentData(const QString& experiment_name)
{
    auto exp_data = new MEDEA::ExperimentData(experiment_name);
    experiment_data_hash_.insert(experiment_name, exp_data);
    connect(exp_data, &MEDEA::ExperimentData::requestData, [this, exp_data] (quint32 exp_run_id) {
        requestExperimentData(ExperimentDataRequestType::ExperimentState, exp_run_id, exp_data);
    });
    return exp_data;
}


/**
 * @brief ExperimentDataManager::getExperimentData
 * @param exp_run_id
 * @return
 */
MEDEA::ExperimentData* ExperimentDataManager::getExperimentData(quint32 exp_run_id) const
{
    auto&& exp_name = exp_run_names_.value(exp_run_id, "");
    return experiment_data_hash_.value(exp_name, nullptr);
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
QString ExperimentDataManager::getItemLabel(const ViewItem *item) const
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
    if (chartDialog_ == nullptr) {
        throw std::runtime_error("Could not get ChartDialog reference; chartDialog_ is null.");
    }
    return *chartDialog_;
}


/**
 * @brief ExperimentDataManager::getDataflowDialog
 * @throws std::runtime_error
 * @return
 */
DataflowDialog& ExperimentDataManager::getDataflowDialog()
{
    if (dataflowDialog_ == nullptr) {
        throw std::runtime_error("Could not get DataflowDialog referece; dataflowDialog_ is null.");
    }
    return *dataflowDialog_;
}


/**
 * @brief ExperimentDataManager::startTimerLoop
 * @param exp_run_id
 */
void ExperimentDataManager::startTimerLoop(quint32 exp_run_id)
{
    if (!exp_run_timers_.contains(exp_run_id)) {
        auto timer_id = startTimer(default_playback_interval_ms);
        exp_run_timers_.insert(exp_run_id, timer_id);
        qDebug() << "-----------------------------------------------------------------------";
        qDebug() << "STARTED timer for [" << exp_run_id << "]";
    }
}


/**
 * @brief ExperimentDataManager::stopTimerLoop
 * @param timer_id
 */
void ExperimentDataManager::stopTimerLoop(quint32 exp_run_id)
{
    if (exp_run_timers_.contains(exp_run_id)) {
        const auto& timer_id = exp_run_timers_.take(exp_run_id);
        killTimer(timer_id);
        qDebug() << "KILLED timer for [" << exp_run_id << "]";
        qDebug() << "-----------------------------------------------------------------------";
    }
}


/**
 * @brief ExperimentDataManager::timerEvent
 * @param event
 */
void ExperimentDataManager::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    if (live_exp_run_id_ != invalid_experiment_id) {
        quint32 exp_run_id = static_cast<quint32>(live_exp_run_id_);
        const auto& exp_data = getExperimentData(exp_run_id);
        if (exp_data) {
            const auto& exp_run_data = exp_data->getExperimentRun(exp_run_id);
            // When the experiment run has an end-time, kill its timer
            if (exp_run_data.end_time() != 0) {
                stopTimerLoop(exp_run_id);
            } else {
                requestExperimentData(ExperimentDataRequestType::ExperimentState, exp_run_id, exp_data);
            }
        } else {
            throw std::invalid_argument("ExperimentDataManager::timerEvent - There is no experiment data for the current experiment run");
        }
    }

    // TODO: Use the code below when we allow multiple Pulse windows
    // Request the displayed experiment run(s)' state every tick interval
    /*for (const auto& exp_run_id : exp_run_timers_.keys()) {
        const auto& exp_data = getExperimentData(exp_run_id);
        if (exp_data) {
            const auto& exp_run_data = exp_data->getExperimentRun(exp_run_id);
            // When the experiment run has an end-time, kill its timer
            if (exp_run_data.end_time() != 0) {
                stopTimerLoop(exp_run_id);
            } else {
                requestExperimentData(ExperimentDataRequestType::ExperimentState, exp_run_id, exp_data);
            }
        } else {
            throw std::invalid_argument("ExperimentDataManager::timerEvent - There is no experiment data for the current experiment run");
        }
    }*/
}


/**
 * @brief ExperimentDataManager::showChartInputPopup
 */
void ExperimentDataManager::showChartInputPopup()
{
    // NOTE: All experiments are requested to fill the context menu's model all the time, the filtering based on input happens in the ChartInputPopup
    requestExperimentData(ExperimentDataRequestType::ExperimentRun, "");
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
    showChartInputPopup();
}


/**
 * @brief ExperimentDataManager::requestPortInstanceData
 * @param port
 */
void ExperimentDataManager::requestPortInstanceEvents(PortInstanceData& port)
{
    requestExperimentData(ExperimentDataRequestType::PortLifecycleEvent, QVariant::fromValue(port.getPortLifecycleRequest()), &port);
    requestExperimentData(ExperimentDataRequestType::PortEvent, QVariant::fromValue(port.getPortEventRequest()), &port);
}


/**
 * @brief ExperimentDataManager::requestWorkerInstanceData
 * @param worker_inst
 */
void ExperimentDataManager::requestWorkerInstanceEvents(WorkerInstanceData& worker_inst)
{
    requestExperimentData(ExperimentDataRequestType::WorkloadEvent, QVariant::fromValue(worker_inst.getWorkloadRequest()), &worker_inst);
}


/**
 * @brief ExperimentDataManager::requestNodeEvents
 * @param node
 */
void ExperimentDataManager::requestNodeEvents(NodeData& node)
{
    requestExperimentData(ExperimentDataRequestType::CPUUtilisationEvent, QVariant::fromValue(node.getCPUUtilisationRequest()), &node);
    requestExperimentData(ExperimentDataRequestType::MemoryUtilisationEvent, QVariant::fromValue(node.getMemoryUtilisationRequest()), &node);
}


/**
 * @brief ExperimentDataManager::requestMarkerSetEvents
 * @param marker_set
 */
void ExperimentDataManager::requestMarkerSetEvents(MarkerSetData& marker_set)
{
    requestExperimentData(ExperimentDataRequestType::MarkerEvent, QVariant::fromValue(marker_set.getMarkerRequest()), &marker_set);
}


/**
 * @brief ExperimentDataManager::clear
 */
void ExperimentDataManager::clear()
{
    // Clear Charts and Pulse
    getChartDialog().clear();
    getDataflowDialog().clear();

    // Clear data structures and stored states
    selectedViewItems_.clear();
    selectedDataKinds_.clear();
    selectedExperimentRun_.experiment_run_id = invalid_experiment_id;

    // Kill any existing timers
    for (const auto& exp_run_id : exp_run_timers_.keys()) {
        stopTimerLoop(exp_run_id);
    }

    experiment_data_hash_.clear();
    exp_run_names_.clear();
    exp_run_timers_.clear();
    live_exp_run_id_ = invalid_experiment_id;
}


/**
 * @brief ExperimentDataManager::visualiseSelectedExperimentRun
 * This slot is called when an experiment run was selected from the ChartInputPopup and "OK" was clicked
 * This is where the chart data request chain starts
 * @param experimentRun
 * @param charts
 * @param pulse
 */
void ExperimentDataManager::visualiseSelectedExperimentRun(const AggServerResponse::ExperimentRun& experimentRun, bool charts, bool pulse)
{
    if (experimentRun.experiment_run_id != invalid_experiment_id) {

        show_in_charts_ = charts;
        show_in_pulse_ = pulse;

        // Stop the previous timer if there is one
        stopTimerLoop(static_cast<quint32>(live_exp_run_id_));
        selectedExperimentRun_ = experimentRun;

        auto expRunID = static_cast<quint32>(experimentRun.experiment_run_id);
        if (experimentRun.end_time == 0) {
            live_exp_run_id_ = experimentRun.experiment_run_id;
            startTimerLoop(expRunID);
        } else {
            live_exp_run_id_ = invalid_experiment_id;
        }

        requestExperimentData(ExperimentDataRequestType::ExperimentState, expRunID);
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
