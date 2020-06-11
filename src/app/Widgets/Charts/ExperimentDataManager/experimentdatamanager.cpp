#include "experimentdatamanager.h"
#include "../../../Controllers/WindowManager/windowmanager.h"

#include <QFutureWatcher>

const int default_playback_interval_ms = 33;
const int invalid_experiment_id = -1;
ExperimentDataManager* ExperimentDataManager::manager_ = nullptr;

using namespace MEDEA;

/**
 * @brief ExperimentDataManager::ExperimentDataManager
 * @param vc
 */
ExperimentDataManager::ExperimentDataManager(const ViewController& vc)
	: viewController_(vc)
{
    chartDialog_ = new ChartDialog();
    dataflowDialog_ = new DataflowDialog();

    live_exp_run_id_ = invalid_experiment_id;

    connect(&vc, &ViewController::modelClosed, this, &ExperimentDataManager::clear);
    connect(&vc, &ViewController::vc_displayChartPopup, this, &ExperimentDataManager::showChartInputPopup);
    connect(&vc, &ViewController::vc_viewItemsInChart, this, &ExperimentDataManager::filterRequestsBySelectedEntities);

    connect(&chartPopup_, &ChartInputPopup::visualiseExperimentRunData, this, &ExperimentDataManager::visualiseSelectedExperimentRun);
}

/**
 * @brief ExperimentDataManager::aggregationProxy
 * @throws
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
 * @param request_param
 * @param requester
 * @throws std::invalid_argument
*/
void ExperimentDataManager::requestExperimentData(ExperimentDataRequestType request_type, const QVariant& request_param, QObject* requester)
{
	if (request_param.isNull() || !request_param.isValid()) {
		throw std::invalid_argument("ExperimentDataManager::requestExperimentData - Provided request parameter is invalid.");
	}

	try {

		bool valid = true;

		switch (request_type) {
			case ExperimentDataRequestType::ExperimentRun: {
				const auto& exp_name = request_param.toString();
				requestExperimentRuns(exp_name, qobject_cast<ExperimentData*>(requester));
				break;
			}
			case ExperimentDataRequestType::ExperimentState:
				valid = request_param.canConvert<quint32>();
				if (valid) {
					quint32 exp_run_id = request_param.toUInt();
					requestExperimentState(exp_run_id, qobject_cast<ExperimentRunData*>(requester));
				}
				break;
			case ExperimentDataRequestType::PortLifecycleEvent:
				valid = request_param.canConvert<PortLifecycleRequest>();
				if (valid) {
					auto request = request_param.value<PortLifecycleRequest>();
					requestPortLifecycleEvents(request, qobject_cast<PortInstanceData*>(requester));
				}
				break;
			case ExperimentDataRequestType::WorkloadEvent:
				valid = request_param.canConvert<WorkloadRequest>();
				if (valid) {
					auto request = request_param.value<WorkloadRequest>();
                    requestWorkloadEvents(request, qobject_cast<WorkerInstanceData*>(requester));
				}
				break;
			case ExperimentDataRequestType::CPUUtilisationEvent:
				valid = request_param.canConvert<UtilisationRequest>();
				if (valid) {
					auto request = request_param.value<UtilisationRequest>();
					requestCPUUtilisationEvents(request, qobject_cast<NodeData*>(requester));
				}
				break;
			case ExperimentDataRequestType::MemoryUtilisationEvent:
				valid = request_param.canConvert<UtilisationRequest>();
				if (valid) {
					auto request = request_param.value<UtilisationRequest>();
					requestMemoryUtilisationEvents(request, qobject_cast<NodeData*>(requester));
				}
				break;
			case ExperimentDataRequestType::MarkerEvent:
				valid = request_param.canConvert<MarkerRequest>();
				if (valid) {
					auto request = request_param.value<MarkerRequest>();
					requestMarkerEvents(request, qobject_cast<MarkerSetData*>(requester));
				}
				break;
			case ExperimentDataRequestType::PortEvent:
				valid = request_param.canConvert<PortEventRequest>();
				if (valid) {
					auto request = request_param.value<PortEventRequest>();
					requestPortEvents(request, qobject_cast<PortInstanceData*>(requester));
				}
				break;
            case ExperimentDataRequestType::NetworkUtilisationEvent:
                valid = request_param.canConvert<UtilisationRequest>();
                if (valid) {
                    auto request = request_param.value<UtilisationRequest>();
                    requestNetworkUtilisationEvents(request, qobject_cast<NodeData*>(requester));
                }
                break;
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
 * @param requester
 */
void ExperimentDataManager::requestExperimentRuns(const QString& experimentName, ExperimentData* requester)
{
    auto future = aggregationProxy().RequestExperimentRuns(experimentName);
    auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [this, experimentName, futureWatcher, requester]() {
        try {
            auto&& exp_runs = futureWatcher->result();
            if (requester != nullptr) {
                for (const auto& run : exp_runs) {
                    requester->addExperimentRun(run);
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
 * @param requester
 */
void ExperimentDataManager::requestExperimentState(const quint32 experimentRunID, ExperimentRunData* requester)
{
    auto future = aggregationProxy().RequestExperimentState(experimentRunID);
    auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);

    connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& exp_state = futureWatcher->result();
            if (requester != nullptr) {
                requester->updateData(exp_state);
            } else {
                auto& exp_run_data = getExperimentRunData(selectedExperimentRun_.experiment_name, selectedExperimentRun_.experiment_run_id);
                exp_run_data.updateData(exp_state);
                showDataForExperimentRun(exp_run_data);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get experiment state - " + QString::fromStdString(ex.what()), "chart", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::requestPortLifecycleEvents
 * @param request
 * @param requester
 */
void ExperimentDataManager::requestPortLifecycleEvents(const PortLifecycleRequest& request, PortInstanceData* requester)
{
    auto future = aggregationProxy().RequestPortLifecycleEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addPortLifecycleEvents(events);
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
 * @param requester
 */
void ExperimentDataManager::requestWorkloadEvents(const WorkloadRequest& request, WorkerInstanceData* requester)
{
    auto future = aggregationProxy().RequestWorkloadEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addWorkloadEvents(events);
                // NOTE: This needs to be here because the workload event series aren't constructed until the workload events are received
                //  This also means that the events need to have been added before showing the workload charts
                const auto& exp_run_data = getExperimentRunData(selectedExperimentRun_.experiment_name,
                                                                selectedExperimentRun_.experiment_run_id);
                for (const auto& series : requester->getWorkloadEventSeries()) {
                    showChartForSeries(series, exp_run_data);
                }
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
 * @param requester
 */
void ExperimentDataManager::requestCPUUtilisationEvents(const UtilisationRequest& request, NodeData* requester)
{
    auto future = aggregationProxy().RequestCPUUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addCPUUtilisationEvents(events);
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
 * @param requester
 */
void ExperimentDataManager::requestMemoryUtilisationEvents(const UtilisationRequest& request, NodeData* requester)
{
    auto future = aggregationProxy().RequestMemoryUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addMemoryUtilisationEvents(events);
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
 * @param requester
 */
void ExperimentDataManager::requestMarkerEvents(const MarkerRequest& request, MarkerSetData* requester)
{
    auto future = aggregationProxy().RequestMarkerEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addMarkerEvents(events);
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
 * @param requester
 */
void ExperimentDataManager::requestPortEvents(const PortEventRequest& request, PortInstanceData* requester)
{
    auto future = aggregationProxy().RequestPortEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<PortEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addPortEvents(events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get port events - " + QString::fromStdString(ex.what()), "plug", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::requestNetworkUtilisationEvents
 * @param request
 * @param requester
 */
void ExperimentDataManager::requestNetworkUtilisationEvents(const UtilisationRequest& request, NodeData* requester)
{
    auto future = aggregationProxy().RequestNetworkUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<NetworkUtilisationEvent*>>(this);

    connect(futureWatcher, &QFutureWatcher<QVector<NetworkUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            auto&& events = futureWatcher->result();
            if (requester != nullptr) {
                requester->addNetworkUtilisationEvents(events);
            }
        } catch (const std::exception& ex) {
            toastNotification("Failed to get network utilisation events - " + QString::fromStdString(ex.what()), "waveEmit", Notification::Severity::ERROR);
        }
    });

    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::processExperimentRuns
 * This is called when the experiment runs for a given experiment name has successfully been received
 * It stores the experiment runs' data with its associated experiment, and sets the ChartInputPopup's stringlist model for the menu
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
            if (exp_run_name.isEmpty()) {
                throw std::runtime_error("ExperimentDataManager::processExperimentRuns - Experiment run name cannot be empty");
            }
            auto exp_data = getExperimentData(exp_run_name);
            if (exp_data == nullptr) {
                exp_data = constructExperimentData(exp_run_name);
            }
            exp_data->addExperimentRun(exp_run);
        }
    } else {
        auto exp_data = getExperimentData(experiment_name);
        if (exp_data == nullptr) {
            exp_data = constructExperimentData(experiment_name);
        }
        for (const auto& exp_run : experiment_runs) {
            exp_data->addExperimentRun(exp_run);
        }
    }

    // Set the experiment runs stored in the chartPopup_
    chartPopup_.setExperimentRuns(experiment_name, experiment_runs.toList());
}

/**
 * @brief ExperimentDataManager::constructExperimentData
 * @param experiment_name
 * @return
 */
ExperimentData* ExperimentDataManager::constructExperimentData(const QString& experiment_name)
{
    auto exp_data = new ExperimentData(experiment_name);
    experiment_data_hash_.insert(experiment_name, exp_data);
    connect(exp_data, &ExperimentData::dataUpdated, this, &ExperimentDataManager::experimentRunDataUpdated);
    return exp_data;
}

/**
 * @brief ExperimentDataManager::getExperimentData
 * @param exp_name
 * @return
 */
ExperimentData* ExperimentDataManager::getExperimentData(const QString& exp_name) const
{
    return experiment_data_hash_.value(exp_name, nullptr);
}

/**
 * @brief ExperimentDataManager::getExperimentRunData
 * @param exp_name
 * @param exp_run_id
 * @return
 */
ExperimentRunData& ExperimentDataManager::getExperimentRunData(const QString& exp_name, quint32 exp_run_id) const
{
    auto exp_data = getExperimentData(exp_name);
    if (exp_data) {
        return exp_data->getExperimentRun(exp_run_id);
    }
    throw std::invalid_argument("ExperimentDataManager::getExperimentRunData - The is no ExperimentData named " + exp_name.toStdString());
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
    }
}

/**
 * @brief ExperimentDataManager::timerEvent
 * @param event
 * @throws std::invalid_argument
 */
void ExperimentDataManager::timerEvent(QTimerEvent* event)
{
    Q_UNUSED(event);

    // TODO: Need to discuss what we want happening in the charts when a live experiment(s) is being visualised
    //  Unlike Pulse, Charts can display data from multiple experiment runs from different experiments at one time
    //  Previously, the timer was only used for Pulse and was stopped the moment a finished experiment is selected

    if (live_exp_run_id_ != invalid_experiment_id) {
        const auto exp_run_id = static_cast<quint32>(live_exp_run_id_);
        try {
            auto& exp_run_data = getExperimentRunData(live_exp_name_, exp_run_id);
            // When the experiment run has an end-time, kill its timer
            if (exp_run_data.end_time() != 0) {
                stopTimerLoop(exp_run_id);
                live_exp_run_id_ = invalid_experiment_id;
            } else {
                requestExperimentData(ExperimentDataRequestType::ExperimentState, exp_run_id, &exp_run_data);
            }
        } catch (const std::exception& ex) {
            throw std::invalid_argument(ex.what());
        }
    }

    const auto& timer_id = event->timerId();


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
    // NOTE: All experiments are requested to fill the context menu's model all the time
    //  The filtering based on input happens in the ChartInputPopup
    requestExperimentData(ExperimentDataRequestType::ExperimentRun, "");
    chartPopup_.setPopupVisible(true);
}

/**
 * @brief ExperimentDataManager::filterRequestsBySelectedEntities
 * @param selectedItems
 * @param selectedDataKinds
 */
void ExperimentDataManager::filterRequestsBySelectedEntities(const QVector<ViewItem*> &selectedItems, const QList<ChartDataKind> &selectedDataKinds)
{
    selectedViewItems_ = selectedItems;
    selectedDataKinds_ = selectedDataKinds;

    // NOTE: This should be the only place where this is called
    chartPopup_.enforceChartsOnly();
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
    requestExperimentData(ExperimentDataRequestType::NetworkUtilisationEvent, QVariant::fromValue(node.getNetworkUtilisationRequest()), &node);
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
    exp_run_timers_.clear();
    live_exp_name_.clear();
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

        // NOTE: We no longer want to stop it in case the charts are still displaying the live experiment
        // TODO: Refactor timer work
        // Stop the previous timer if there is one
        stopTimerLoop(static_cast<quint32>(live_exp_run_id_));
        selectedExperimentRun_ = experimentRun;

        auto expRunID = static_cast<quint32>(experimentRun.experiment_run_id);
        if (experimentRun.end_time == 0) {
            live_exp_name_ = experimentRun.experiment_name;
            live_exp_run_id_ = experimentRun.experiment_run_id;
            startTimerLoop(expRunID);
        } else {
            live_exp_name_.clear();
            live_exp_run_id_ = invalid_experiment_id;
        }

        requestExperimentData(ExperimentDataRequestType::ExperimentState, expRunID);
    }
}

/**
 * @brief ExperimentDataManager::experimentRunDataUpdated
 * @param exp_run_id
 * @param last_updated_time
 */
void ExperimentDataManager::experimentRunDataUpdated(quint32 exp_run_id, qint64 last_updated_time)
{
    // This will update the time range of the charts whenever the experiment run's last updated time has changed
    // TODO (Ask Jackson): Do we want this to happen? Or should we require the user to re-request the charts?
    timelineChartView().updateExperimentRunLastUpdatedTime(exp_run_id, last_updated_time);
}

/**
 * @brief ExperimentDataManager::showDataForExperimentRun
 * @param exp_run_data
 */
void ExperimentDataManager::showDataForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data)
{
    showChartsForExperimentRun(exp_run_data);
    showPulseForExperimentRun(exp_run_data);
}

/**
 * @brief ExperimentDataManager::showChartsForExperimentRun
 * @param exp_run_data
 */
void ExperimentDataManager::showChartsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data)
{
    if (!show_in_charts_) {
        return;
    }

    // If there are no view items or data kinds to filter by, show all data kinds
    if (selectedViewItems_.isEmpty() || selectedDataKinds_.isEmpty()) {
        selectedDataKinds_ = Event::GetChartDataKinds();
        selectedDataKinds_.removeAll(ChartDataKind::DATA);
    }

    QList<QPointer<const EventSeries>> series_to_show;

    if (selectedDataKinds_.contains(ChartDataKind::PORT_LIFECYCLE)) {
        series_to_show += exp_run_data.getPortLifecycleSeries();
    }
    if (selectedDataKinds_.contains(ChartDataKind::PORT_EVENT)) {
        series_to_show += exp_run_data.getPortEventSeries();
    }
    if (selectedDataKinds_.contains(ChartDataKind::WORKLOAD)) {
        series_to_show += exp_run_data.getWorkloadEventSeries();
    }
    if (selectedDataKinds_.contains(ChartDataKind::CPU_UTILISATION)) {
        series_to_show += exp_run_data.getCPUUtilisationSeries();
    }
    if (selectedDataKinds_.contains(ChartDataKind::MEMORY_UTILISATION)) {
        series_to_show += exp_run_data.getMemoryUtilisationSeries();
    }
    if (selectedDataKinds_.contains(ChartDataKind::NETWORK_UTILISATION)) {
        series_to_show += exp_run_data.getNetworkUtilisationSeries();
    }

    for (const auto& series : series_to_show) {
        showChartForSeries(series, exp_run_data);
    }

    // TODO: Finalise MarkerSetData and handle it here!
    //  They can be added in ExperimentRunData, but they currently don't get added anywhere
    //  They don't live anywhere in the proto message, you have to explicitly request them

    selectedDataKinds_.clear();
    selectedViewItems_.clear();
}

/**
 * @brief ExperimentDataManager::showChartForSeries
 * @param series
 * @param exp_run_data
 */
void ExperimentDataManager::showChartForSeries(const QPointer<const EventSeries>& series, const ExperimentRunData& exp_run_data)
{
    if (show_in_charts_) {
        if (!series.isNull()) {
            timelineChartView().addChart(series, exp_run_data);
            emit showChartsPanel();
        }
    }
}

/**
 * @brief ExperimentDataManager::showPulseForExperimentRun
 * @param exp_run_data
 */
void ExperimentDataManager::showPulseForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data)
{
    if (show_in_pulse_) {
        getDataflowDialog().constructGraphicsItemsForExperimentRun(exp_run_data);
        emit showDataflowPanel();
    }
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