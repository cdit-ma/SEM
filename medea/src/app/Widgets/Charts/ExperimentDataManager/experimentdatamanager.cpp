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

    connect(&aggregationProxy(), &AggregationProxy::toastNotification, this, &ExperimentDataManager::toastNotification);
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
				valid = request_param.canConvert<HardwareMetricRequest>();
				if (valid) {
					auto request = request_param.value<HardwareMetricRequest>();
					requestCPUUtilisationEvents(request, qobject_cast<NodeData*>(requester));
				}
				break;
			case ExperimentDataRequestType::MemoryUtilisationEvent:
				valid = request_param.canConvert<HardwareMetricRequest>();
				if (valid) {
					auto request = request_param.value<HardwareMetricRequest>();
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
                valid = request_param.canConvert<HardwareMetricRequest>();
                if (valid) {
                    auto request = request_param.value<HardwareMetricRequest>();
                    requestNetworkUtilisationEvents(request, qobject_cast<NodeData*>(requester));
                }
                break;
		    case ExperimentDataRequestType::GPUMetrics:
                valid = request_param.canConvert<HardwareMetricRequest>();
                if (valid) {
                    auto request = request_param.value<HardwareMetricRequest>();
                    requestGPUMetrics(request, qobject_cast<NodeData*>(requester));
                }
                break;
		}

		if (!valid) {
			auto&& request_str = QString::number(static_cast<int>(request_type)).toStdString() + request_param.toString().toStdString();
			throw std::invalid_argument("Failed to request data (" + request_str + ") - Invalid request parameter.");
		}

	} catch (const NoRequesterException& ex) {
		toastNotification("ExperimentDataManager::requestExperimentData - Failed to set up the aggregation server: " + ex.toString(), "chart", Notification::Severity::ERROR);
	} catch (const RequestException& ex) {
		toastNotification("ExperimentDataManager::requestExperimentData - Failed to request experiment data: " + ex.toString(), "chart", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestExperimentRuns(experimentName);
    auto futureWatcher = new QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<AggServerResponse::ExperimentRun>>::finished, [this, experimentName, futureWatcher, requester]() {
        try {
            processExperimentRuns(requester, experimentName, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get experiment runs - " + ex.toString(), "chart", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestExperimentState(experimentRunID);
    auto futureWatcher = new QFutureWatcher<AggServerResponse::ExperimentState>(this);
    connect(futureWatcher, &QFutureWatcher<AggServerResponse::ExperimentState>::finished, [this, futureWatcher, requester]() {
        try {
            processExperimentState(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get experiment state - " + ex.toString(), "chart", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestPortLifecycleEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortLifecycleEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<PortLifecycleEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processPortLifecycleEvents(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get port lifecycle events - " + ex.toString(), "plug", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestWorkloadEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<WorkloadEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<WorkloadEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processWorkloadEvents(request_filters_, requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get workload events - " + ex.toString(), "spanner", Notification::Severity::ERROR);
        } catch (const std::exception& ex) {
            toastNotification("Failed to get workload events - " + QString::fromStdString(ex.what()), "spanner", Notification::Severity::ERROR);
            // TODO: Ask Jackson why this causes a crash
            //throw;
        }
    });
    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::requestCPUUtilisationEvents
 * @param request
 * @param requester
 */
void ExperimentDataManager::requestCPUUtilisationEvents(const HardwareMetricRequest& request, NodeData* requester)
{
    const auto& future = aggregationProxy().RequestCPUUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<CPUUtilisationEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<CPUUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processCPUUtilisationEvents(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get cpu utilisation events - " + ex.toString(), "cpu", Notification::Severity::ERROR);
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
void ExperimentDataManager::requestMemoryUtilisationEvents(const HardwareMetricRequest& request, NodeData* requester)
{
    const auto& future = aggregationProxy().RequestMemoryUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MemoryUtilisationEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<MemoryUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processMemoryUtilisationEvents(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get memory utilisation events - " + ex.toString(), "memoryCard", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestMarkerEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<MarkerEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<MarkerEvent*>>::finished, [this, futureWatcher, request, requester]() {
        try {
            processMarkerEvents(requester, request.experiment_name(), request.experiment_run_id(), futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get marker events - " + ex.toString(), "bookmark", Notification::Severity::ERROR);
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
    const auto& future = aggregationProxy().RequestPortEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<PortEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<PortEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processPortEvents(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get port events - " + ex.toString(), "plug", Notification::Severity::ERROR);
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
void ExperimentDataManager::requestNetworkUtilisationEvents(const HardwareMetricRequest& request, NodeData* requester)
{
    const auto& future = aggregationProxy().RequestNetworkUtilisationEvents(request);
    auto futureWatcher = new QFutureWatcher<QVector<NetworkUtilisationEvent*>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<NetworkUtilisationEvent*>>::finished, [this, futureWatcher, requester]() {
        try {
            processNetworkUtilisationEvents(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get network utilisation events - " + ex.toString(), "waveEmit", Notification::Severity::ERROR);
        } catch (const std::exception& ex) {
            toastNotification("Failed to get network utilisation events - " + QString::fromStdString(ex.what()), "waveEmit", Notification::Severity::ERROR);
        }
    });
    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::requestGPUMetrics
 * @param request
 * @param requester
 */
void ExperimentDataManager::requestGPUMetrics(const HardwareMetricRequest& request, NodeData* requester)
{
    const auto& future = aggregationProxy().RequestGPUMetrics(request);
    auto futureWatcher = new QFutureWatcher<QVector<AggregationProxy::GPUMetricSample>>(this);
    connect(futureWatcher, &QFutureWatcher<QVector<AggregationProxy::GPUMetricSample>>::finished, [this, futureWatcher, requester]() {
        try {
            processGPUMetrics(requester, futureWatcher->result());
        } catch (const RequestException& ex) {
            toastNotification("Failed to get gpu metrics - " + ex.toString(), "gpu", Notification::Severity::ERROR);
        } catch (const std::exception& ex) {
            toastNotification("Failed to get gpu metrics - " + QString::fromStdString(ex.what()), "gpu", Notification::Severity::ERROR);
        }
    });
    futureWatcher->setFuture(future);
}

/**
 * @brief ExperimentDataManager::processExperimentRuns
 * This is called when the experiment runs for a given experiment name has successfully been received
 * It stores the experiment runs' data with its associated experiment
 * When there is no specific requester, it also sets the ChartInputPopup's stringlist model for the auto-complete menu
 * @param requester - nullptr if the request wasn't made by an instance of ExperimentData
 * @param exp_name - name of the experiment that the experiment runs belong to
 * @param exp_runs - received experiment runs
 * @throws std::logic_error
 * @throws MalformedProtoException
 */
void ExperimentDataManager::processExperimentRuns(MEDEA::ExperimentData* requester, const QString& exp_name, const QVector<AggServerResponse::ExperimentRun>& exp_runs)
{
    if (requester != nullptr) {
        for (const auto& run : exp_runs) {
            requester->addExperimentRun(run);
        }
        return;
    }

    if (exp_runs.isEmpty()) {
        throw std::logic_error("ExperimentDataManager::processExperimentRuns - Experiment " + exp_name.toStdString() + " doesn't have any runs.");
    }

    // This is the case where all the experiment runs for all experiments has been requested
    if (exp_name.isEmpty()) {
        for (const auto& exp_run : exp_runs) {
            const auto& exp_run_name = exp_run.experiment_name;
            if (exp_run_name.isEmpty()) {
                throw MalformedProtoException("ExperimentDataManager::processExperimentRuns - Experiment run name cannot be empty");
            }
            auto exp_data = getExperimentData(exp_run_name);
            if (exp_data == nullptr) {
                exp_data = constructExperimentData(exp_run_name);
            }
            exp_data->addExperimentRun(exp_run);
        }
    } else {
        auto exp_data = getExperimentData(exp_name);
        if (exp_data == nullptr) {
            exp_data = constructExperimentData(exp_name);
        }
        for (const auto &exp_run : exp_runs) {
            exp_data->addExperimentRun(exp_run);
        }
    }

    // Set the experiment runs stored in the chartPopup_
    chartPopup_.setExperimentRuns(exp_name, exp_runs.toList());
}

/**
 * @brief ExperimentDataManager::processExperimentState
 * This is called when the experiment state for a given experiment run has successfully been received
 * It will update the corresponding  ExperimentRunData with the received ExperimentState, which inherently triggers an update for all the contained data within it
 * If the experiment state wasn't requested by an ExperimentRunData instance, the experiment data will be displayed in the selected visualisation tool(s)
 * @param requester - nullptr if the request wasn't made by an instance of ExperimentRunData
 * @param exp_state - received experiment state
 */
void ExperimentDataManager::processExperimentState(ExperimentRunData* requester, const AggServerResponse::ExperimentState& exp_state)
{
    if (requester != nullptr) {
        requester->updateData(exp_state);
    } else {
        auto& exp_run_data = getExperimentRunData(request_filters_.experiment_name, request_filters_.experiment_run_id);
        exp_run_data.updateData(exp_state);
        showDataForExperimentRun(exp_run_data);
    }
}

/**
 * @brief ExperimentDataManager::processPortLifecycleEvents
 * This is called when the Port lifecycle events for a given experiment run has successfully been received
 * @param requester - nullptr if the request wasn't made by an instance of PortInstanceData
 * @param events - received Port lifecycle events
 */
void ExperimentDataManager::processPortLifecycleEvents(PortInstanceData* requester, const QVector<PortLifecycleEvent*>& events)
{
    if (requester != nullptr) {
        requester->addPortLifecycleEvents(events);
    }
}

/**
 * @brief ExperimentDataManager::processPortEvents
 * This is called when the Port events for a given experiment run has successfully been received
 * @param requester - nullptr if the request wasn't made by an instance of PortInstanceData
 * @param events - received Port events
 */
void ExperimentDataManager::processPortEvents(PortInstanceData* requester, const QVector<PortEvent*>& events)
{
    if (requester != nullptr) {
        requester->addPortEvents(events);
    }
}

/**
 * @brief ExperimentDataManager::processWorkloadEvents
 * This is called when the Workload events for a given experiment run has successfully been received
 * @param request_filters - the request filters used when these workload events were requested
 * @param requester - nullptr if the request wasn't made by an instance of WorkerInstanceData
 * @param events - received Workload events
 */
void ExperimentDataManager::processWorkloadEvents(RequestFilters request_filters, WorkerInstanceData* requester, const QVector<WorkloadEvent*>& events)
{
    // NOTE: The WorkloadEventSeries aren't constructed until the Workload events are received
    //  This means that the events need to have been added to its requester WorkerInstanceData so that the respective
    //  WorkloadEventSeries(s) can be constructed, and hence, the workload charts can be added and shown
    if (requester != nullptr) {
        requester->addWorkloadEvents(events);
        if (request_filters.show_charts && request_filters.requested_data_kinds.contains(ChartDataKind::WORKLOAD)) {
            const auto& exp_run_data = getExperimentRunData(request_filters.experiment_name, request_filters.experiment_run_id);
            if (request_filters.filter_by_selection) {
                const auto& filtered_series_list = exp_run_data.getWorkloadEventSeries(request_filters.component_names,
                                                                                       request_filters.component_inst_paths,
                                                                                       request_filters.worker_inst_paths);
                for (const auto& series : requester->getWorkloadEventSeries()) {
                    if (filtered_series_list.contains(series)) {
                        showChartForSeries(series, exp_run_data);
                    }
                }
            } else {
                for (const auto& series : requester->getWorkloadEventSeries()) {
                    showChartForSeries(series, exp_run_data);
                }
            }
        }
    }
}

/**
 * @brief ExperimentDataManager::processMarkerEvents
 * This is called when the Marker events for a given experiment run has successfully been received
 * It constructs a corresponding MarkerSetData for each Marker event (if one doesn't already exist), adds the event
 * to the MarkerEventSeries within the MarkerSetData, and then adds and shows the chart(s) for each MarkerEventSeries
 * @param requester - nullptr if the request wasn't made by an instance of MarkerSetData
 * @param exp_name - name of the experiment that the events belong to
 * @param exp_run_id - the experiment run id that the events belong to
 * @param events - received Marker events
 */
void ExperimentDataManager::processMarkerEvents(MarkerSetData* requester, const QString& exp_name, quint32 exp_run_id, const QVector<MarkerEvent*>& events)
{
    if (requester != nullptr) {
        requester->addMarkerEvents(events);
        return;
    }
    auto& exp_run_data = getExperimentRunData(exp_name, exp_run_id);
    for (const auto& event : events) {
        const auto& marker_name = event->getMarkerName();
        auto marker_set_data = exp_run_data.getMarkerSetData(marker_name);
        if (marker_set_data == nullptr) {
            try {
                // Add a new MarkerSetData to the ExperimentRunData
                marker_set_data = &exp_run_data.addMarkerSetData(marker_name);
            } catch (const std::logic_error& ex) {
                toastNotification(ex.what(), "charts", Notification::Severity::ERROR);
            }
        }
        if (marker_set_data != nullptr) {
            marker_set_data->addMarkerEvents({event});
        }
    }
    for (const auto& series : exp_run_data.getMarkerEventSeries()) {
        showChartForSeries(series, exp_run_data);
    }
}

/**
 * @brief ExperimentDataManager::processCPUUtilisationEvents
 * This is called when the CPU utilisation events for a given experiment run has successfully been received
 * @param requester - nullptr if the request wasn't made by an instance of NodeData
 * @param events - received CPU utilisation events
 */
void ExperimentDataManager::processCPUUtilisationEvents(NodeData* requester, const QVector<CPUUtilisationEvent*>& events)
{
    if (requester != nullptr) {
        requester->addCPUUtilisationEvents(events);
    }
}

/**
 * @brief ExperimentDataManager::processMemoryUtilisationEvents
 * This is called when the memory utilisation events for a given experiment run has successfully been received
 * @param requester - nullptr if the request wasn't made by an instance of NodeData
 * @param events - received memory utilisation events
 */
void ExperimentDataManager::processMemoryUtilisationEvents(NodeData* requester, const QVector<MemoryUtilisationEvent*>& events)
{
    if (requester != nullptr) {
        requester->addMemoryUtilisationEvents(events);
    }
}

/**
 * @brief ExperimentDataManager::processNetworkUtilisationEvents
 * This is called when the network utilisation events for a given experiment run has successfully been received
 * @param requester - nullptr if the request wasn't made by an instance of NodeData
 * @param events - received network utilisation events
 */
void ExperimentDataManager::processNetworkUtilisationEvents(NodeData* requester, const QVector<NetworkUtilisationEvent*>& events)
{
    if (requester != nullptr) {
        requester->addNetworkUtilisationEvents(events);
    }
}

/**
 * @brief ExperimentDataManager::processGPUMetrics
 * @param requester
 * @param samples
 */
void ExperimentDataManager::processGPUMetrics(NodeData* requester, const QVector<AggregationProxy::GPUMetricSample>& samples)
{
    if (requester != nullptr) {
        for (const auto& sample : samples) {
            requester->addGPUComputeUtilisationEvents({sample.compute_utilisation});
            requester->addGPUMemoryUtilisationEvents({sample.memory_utilisation});
            requester->addGPUTemperatureEvents({sample.temperature});
        }
    }
}

/**
 * @brief ExperimentDataManager::constructExperimentData
 * @param exp_name
 * @return
 */
ExperimentData* ExperimentDataManager::constructExperimentData(const QString& exp_name)
{
    auto exp_data = new ExperimentData(exp_name);
    experiment_data_hash_.insert(exp_name, exp_data);
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
    throw std::invalid_argument("ExperimentDataManager::getExperimentRunData - There is no ExperimentData named " + exp_name.toStdString());
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
    // Clear previous filters
    request_filters_.clear();

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
    // NOTE: This should be the only place where this is called
    chartPopup_.enforceChartsOnly();
    showChartInputPopup();

    // NOTE: Call this last; showChartInputPopup() clears the request filters
    setupRequestFilters(selectedItems, selectedDataKinds);
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
    requestExperimentData(ExperimentDataRequestType::GPUMetrics, QVariant::fromValue(node.getGPUMetricsRequest()), &node);
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

    // Clear request filters
    request_filters_.clear();

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

        request_filters_.experiment_name = experimentRun.experiment_name;
        request_filters_.experiment_run_id = experimentRun.experiment_run_id;
        request_filters_.show_charts = charts;
        request_filters_.show_pulse = pulse;

        // NOTE: We no longer want to stop it in case the charts are still displaying the live experiment
        // TODO: Refactor timer work
        // Stop the previous timer if there is one
        stopTimerLoop(static_cast<quint32>(live_exp_run_id_));

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
    if (!request_filters_.show_charts) {
        return;
    }

    auto& selected_data_kinds = request_filters_.requested_data_kinds;
    if (selected_data_kinds.isEmpty()) {
        selected_data_kinds.append(MEDEA::Event::GetChartDataKinds());
        selected_data_kinds.removeAll(MEDEA::ChartDataKind::DATA);
    }

    const auto& comp_names = request_filters_.component_names;
    const auto& comp_inst_paths = request_filters_.component_inst_paths;
    const auto& port_paths = request_filters_.port_paths;
    const auto& worker_inst_paths = request_filters_.worker_inst_paths;
    const auto& node_hostnames = request_filters_.node_hostnames;
    bool filter_charts = request_filters_.filter_by_selection;

    QList<QPointer<const EventSeries>> series_to_show;

    if (selected_data_kinds.contains(ChartDataKind::PORT_LIFECYCLE)) {
        if (filter_charts) {
            series_to_show += exp_run_data.getPortLifecycleSeries(comp_names, comp_inst_paths, port_paths);
        } else {
            series_to_show += exp_run_data.getPortLifecycleSeries();
        }
    }
    if (selected_data_kinds.contains(ChartDataKind::PORT_EVENT)) {
        if (filter_charts) {
            series_to_show += exp_run_data.getPortEventSeries(comp_names, comp_inst_paths, port_paths);
        } else {
            series_to_show += exp_run_data.getPortEventSeries();
        }
    }
    if (selected_data_kinds.contains(ChartDataKind::WORKLOAD)) {
        if (filter_charts) {
            series_to_show += exp_run_data.getWorkloadEventSeries(comp_names, comp_inst_paths, worker_inst_paths);
        } else {
            series_to_show += exp_run_data.getWorkloadEventSeries();
        }
    }
    if (selected_data_kinds.contains(ChartDataKind::CPU_UTILISATION)) {
        series_to_show += exp_run_data.getCPUUtilisationSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::MEMORY_UTILISATION)) {
        series_to_show += exp_run_data.getMemoryUtilisationSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::NETWORK_UTILISATION)) {
        series_to_show += exp_run_data.getNetworkUtilisationSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::GPU_COMPUTE_UTILISATION)) {
        series_to_show += exp_run_data.getGPUComputeUtilisationSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::GPU_MEMORY_UTILISATION)) {
        series_to_show += exp_run_data.getGPUMemoryUtilisationSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::GPU_TEMPERATURE)) {
        series_to_show += exp_run_data.getGPUTemperatureSeries(node_hostnames);
    }
    if (selected_data_kinds.contains(ChartDataKind::MARKER)) {
        // Request all Marker events for the exp_run_data - Marker events can't be filtered by view items at the moment
        // NOTE: MarkerSetData/MarkerEventSeries are constructed/updated upon receiving Marker events
        //  This happens in processMarkerEvents where the respective charts are also added/updated
        MarkerRequest marker_request;
        marker_request.setExperimentName(exp_run_data.experiment_name());
        marker_request.setExperimentRunID(exp_run_data.experiment_run_id());
        requestExperimentData(ExperimentDataRequestType::MarkerEvent, QVariant::fromValue(marker_request));
    }

    // Show all/filtered charts
    for (const auto& series : series_to_show) {
        showChartForSeries(series, exp_run_data);
    }
}

/**
 * @brief ExperimentDataManager::showChartForSeries
 * @param series
 * @param exp_run_data
 */
void ExperimentDataManager::showChartForSeries(const QPointer<const EventSeries>& series, const ExperimentRunData& exp_run_data)
{
    if (request_filters_.show_charts) {
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
    if (request_filters_.show_pulse) {
        //getDataflowDialog().constructGraphicsItemsForExperimentRun(exp_run_data);
        // NOTE: Commenting out the new feature until it is complete
        // TODO: Create a setting to flag/unflag this new feature at runtime
        getDataflowDialog().constructPulseViewItemsForExperimentRun(exp_run_data);
        emit showDataflowPanel();
    }
}

/**
 * @brief getItemLabel
 * @param item
 * @return the ViewItem's label
 */
QString getItemLabel(ViewItem* item)
{
    if (item) {
        return item->getData(KeyName::Label).toString();
    }
    return "";
}

/**
 * @brief getPath
 * @param item
 * @return
 */
QString getPath(ViewItem* item)
{
    if (item == nullptr || !item->isNode()) {
        return "";
    }
    auto node_item = qobject_cast<NodeViewItem*>(item);
    if (!node_item->isNodeOfType(NODE_TYPE::INSTANCE)) {
        return "";
    }
    const auto& node_item_label = getItemLabel(node_item);
    switch (node_item->getNodeKind()) {
        case NODE_KIND::COMPONENT_INST:
            // The parent item is either the AssemblyDefinitions or a ComponentAssembly
            if (node_item->getParentNodeKind() == NODE_KIND::ASSEMBLY_DEFINITIONS) {
                return node_item_label;
            } else {
                return getItemLabel(node_item->getParentNodeViewItem()) + ".%/" + node_item_label;
            }
        case NODE_KIND::PORT_REPLIER_INST:
        case NODE_KIND::PORT_REQUESTER_INST:
        case NODE_KIND::PORT_PUBLISHER_INST:
        case NODE_KIND::PORT_SUBSCRIBER_INST:
        case NODE_KIND::PORT_PERIODIC_INST:
            // The parent item is a ComponentInst
            return getPath(node_item->getParentNodeViewItem()) + "/" + node_item_label;
        case NODE_KIND::CLASS_INST:
            if (node_item->getData(KeyName::IsWorker).toBool()) {
                // The parent item is either a ComponentImpl or a ComponentInst
                if (node_item->getParentNodeKind() == NODE_KIND::COMPONENT_INST) {
                    return getPath(node_item->getParentNodeViewItem()) + "/" + node_item_label;
                }
            }
            break;
        default:
            break;
    }
    return "";
}

/**
 * @brief ExperimentDataManager::setupRequestFilters
 * @param view_items
 * @param data_kinds
 */
void ExperimentDataManager::setupRequestFilters(const QVector<ViewItem*>& view_items, const QList<MEDEA::ChartDataKind>& data_kinds)
{
    if (view_items.isEmpty()) {
        return;
    }

    auto& selected_data = request_filters_.requested_data_kinds;
    selected_data.append(data_kinds);

    auto& comp_names = request_filters_.component_names;
    auto& comp_inst_paths = request_filters_.component_inst_paths;
    auto& port_paths = request_filters_.port_paths;
    auto& worker_inst_paths = request_filters_.worker_inst_paths;
    auto& node_hostnames = request_filters_.node_hostnames;
    auto& marker_names = request_filters_.marker_names;

    for (auto item : view_items) {
        if (!item || !item->isNode()) {
            continue;
        }
        const auto& label = getItemLabel(item);
        auto node_item = qobject_cast<NodeViewItem*>(item);
        if (node_item->isNodeOfType(NODE_TYPE::ASPECT)) {
            continue;
        }
        switch (node_item->getNodeKind()) {
            case NODE_KIND::COMPONENT:
            case NODE_KIND::COMPONENT_IMPL:
                comp_names.append(label);
                break;
            case NODE_KIND::COMPONENT_INST:
                comp_inst_paths.append(getPath(node_item));
                break;
            case NODE_KIND::PORT_REPLIER_IMPL:
            case NODE_KIND::PORT_REQUESTER_IMPL:
            case NODE_KIND::PORT_PUBLISHER_IMPL:
            case NODE_KIND::PORT_SUBSCRIBER_IMPL:
            case NODE_KIND::PORT_REPLIER:
            case NODE_KIND::PORT_REQUESTER:
            case NODE_KIND::PORT_PUBLISHER:
            case NODE_KIND::PORT_SUBSCRIBER:
            case NODE_KIND::PORT_PERIODIC:
                if (selected_data.contains(ChartDataKind::PORT_LIFECYCLE) || selected_data.contains(ChartDataKind::PORT_EVENT)) {
                    for (auto port_inst_item : viewController_.getNodesInstances(item->getID())) {
                        if (port_inst_item) {
                            port_paths.append(getPath(port_inst_item));
                        }
                    }
                }
                break;
            case NODE_KIND::PORT_REPLIER_INST:
            case NODE_KIND::PORT_REQUESTER_INST:
            case NODE_KIND::PORT_PUBLISHER_INST:
            case NODE_KIND::PORT_SUBSCRIBER_INST:
            case NODE_KIND::PORT_PERIODIC_INST:
                port_paths.append(getPath(node_item));
                break;
            case NODE_KIND::CLASS_INST:
                // Only ClassInstances that have isWorker set to true can enter this case
                // The "View In Chart" menu wouldn't have been visible otherwise
                if (selected_data.contains(MEDEA::ChartDataKind::WORKLOAD)) {
                    const auto parent_node_kind = node_item->getParentNodeKind();
                    if (parent_node_kind == NODE_KIND::COMPONENT_IMPL) {
                        for (auto worker_inst_item : viewController_.getNodesInstances(item->getID())) {
                            if (worker_inst_item) {
                                worker_inst_paths.append(getPath(worker_inst_item));
                            }
                        }
                    } else if (parent_node_kind == NODE_KIND::COMPONENT_INST) {
                        worker_inst_paths.append(getPath(node_item));
                    }
                }
                break;
            case NODE_KIND::HARDWARE_NODE:
                node_hostnames.append(label);
                break;
            case NODE_KIND::FUNCTION_CALL: {
                // Only "Marker" function calls can enter this case
                // The "View In Chart" menu wouldn't have been visible otherwise
                for (auto child : node_item->getNestedChildren()) {
                    if (child->isEdge()) {
                        continue;
                    }
                    auto child_item = qobject_cast<NodeViewItem*>(child);
                    if (child_item->getNodeKind() == NODE_KIND::MEMBER_INST) {
                        if (getItemLabel(child_item) == "Label") {
                            auto&& marker_name = child_item->getData(KeyName::Value).toString();
                            marker_names.append(marker_name.remove('"'));
                            break;
                        }
                    }
                }
                break;
            }
            default:
                // We don't filter by any other entity kind at the moment
                break;
        }

        // Set this to true if at least one of the selected node items is not an aspect
        request_filters_.filter_by_selection = true;
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
