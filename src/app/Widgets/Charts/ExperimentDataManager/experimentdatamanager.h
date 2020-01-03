#ifndef EXPERIMENTDATAMANAGER_H
#define EXPERIMENTDATAMANAGER_H

#include <QObject>

#include "../../../Controllers/ViewController/viewcontroller.h"
#include "../../Dataflow/dataflowdialog.h"
#include "../Timeline/chartdialog.h"
#include "../Timeline/chartinputpopup.h"
#include "../Timeline/Chart/timelinechartview.h"
#include "../Data/experimentdata.h"

#include "../Data/Requests/portlifecyclerequest.h"
#include "../Data/Requests/workloadrequest.h"
#include "../Data/Requests/cpuutilisationrequest.h"
#include "../Data/Requests/memoryutilisationrequest.h"
#include "../Data/Requests/markerrequest.h"
#include "../Data/Requests/porteventrequest.h"
#include "../Data/Requests/networkutilisationrequest.h"

enum class ExperimentDataRequestType
{
    ExperimentRun,
    ExperimentState,
    PortLifecycleEvent,
    WorkloadEvent,
    CPUUtilisationEvent,
    MemoryUtilisationEvent,
    MarkerEvent,
    PortEvent,
    NetworkUtilisationEvent
};

class RequestBuilder;
class ExperimentDataManager : public QObject
{
    Q_OBJECT
    friend class ViewController;

public:
    static ExperimentDataManager* manager();

    ChartDialog& getChartDialog();
    DataflowDialog& getDataflowDialog();

    void startTimerLoop(quint32 exp_run_id);
    void stopTimerLoop(quint32 exp_run_id);
    void timerEvent(QTimerEvent* event);

    static void toastNotification(const QString& description, const QString& iconName, Notification::Severity severity = Notification::Severity::INFO);

signals:
    void showChartsPanel();
    void showDataflowPanel();

public slots:
    void showChartInputPopup();

    void filterRequestsBySelectedEntities(const QVector<ViewItem*>& selectedItems, const QList<MEDEA::ChartDataKind>& selectedDataKinds);

    void requestPortInstanceEvents(PortInstanceData& port);
    void requestWorkerInstanceEvents(WorkerInstanceData& worker_inst);
    void requestNodeEvents(NodeData& node);
    void requestMarkerSetEvents(MarkerSetData& marker_set);

    void clear();

private slots:
    void visualiseSelectedExperimentRun(const AggServerResponse::ExperimentRun& experimentRun, bool charts, bool pulse);

protected:
    static void constructSingleton(ViewController* vc);

private:
    ExperimentDataManager(const ViewController &vc);
    AggregationProxy& aggregationProxy();
    TimelineChartView& timelineChartView();

    void requestExperimentData(ExperimentDataRequestType request_type, const QVariant& request_param, QObject* sender_obj = nullptr);


    void requestExperimentRuns(const QString& experimentName, MEDEA::ExperimentData* exp_data_requester = nullptr);
    void requestExperimentState(const quint32 experimentRunID, MEDEA::ExperimentData* exp_data_requester = nullptr);

    void setupRequestsForExperimentRun(const quint32 experimentRunID);

    void requestEvents(const RequestBuilder &builder);
    void requestPortLifecycleEvents(const PortLifecycleRequest& request, const AggServerResponse::ExperimentRun& experimentRun, PortInstanceData* port_data_requester = nullptr);
    void requestWorkloadEvents(const WorkloadRequest& request, const AggServerResponse::ExperimentRun& experimentRun, WorkerInstanceData* worker_inst_data_requester = nullptr);
    void requestCPUUtilisationEvents(const CPUUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun, NodeData* node_data_requester = nullptr);
    void requestMemoryUtilisationEvents(const MemoryUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun, NodeData* node_data_requester = nullptr);
    void requestMarkerEvents(const MarkerRequest& request, const AggServerResponse::ExperimentRun& experimentRun, MarkerSetData* marker_data_requester = nullptr);
    void requestPortEvents(const PortEventRequest& request, const AggServerResponse::ExperimentRun& experimentRun, PortInstanceData* port_data_requester = nullptr);
    void requestNetworkUtilisationEvents(const NetworkUtilisationRequest& request, const AggServerResponse::ExperimentRun& experimentRun, NodeData* node_data_requester = nullptr);

    void processExperimentRuns(const QString& experiment_name, const QVector<AggServerResponse::ExperimentRun>& experiment_runs);
    void processExperimentState(const QString &experiment_name, quint32 experiment_run_id, const AggServerResponse::ExperimentState& experiment_state);

    void processPortLifecycleEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortLifecycleEvent*>& events);
    void processWorkloadEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<WorkloadEvent*>& events);
    void processCPUUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<CPUUtilisationEvent*>& events);
    void processMemoryUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MemoryUtilisationEvent*>& events);
    void processMarkerEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<MarkerEvent*>& events);
    void processPortEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<PortEvent*>& events);
    void processNetworkUtilisationEvents(const AggServerResponse::ExperimentRun& exp_run, const QVector<NetworkUtilisationEvent*>& events);

    MEDEA::ExperimentData* constructExperimentData(const QString& experiment_name);
    MEDEA::ExperimentData* getExperimentData(quint32 exp_run_id) const;

    QString getItemLabel(const ViewItem* item) const;

    DataflowDialog* dataflowDialog_ = nullptr;
    ChartDialog* chartDialog_ = nullptr;
    ChartInputPopup chartPopup_;

    QVector<ViewItem*> selectedViewItems_;
    QList<MEDEA::ChartDataKind> selectedDataKinds_;
    AggServerResponse::ExperimentRun selectedExperimentRun_;

    bool show_in_charts_ = false;
    bool show_in_pulse_ = false;

    QHash<QString, MEDEA::ExperimentData*> experiment_data_hash_;
    QHash<quint32, QString> exp_run_names_;
    QHash<quint32, int> exp_run_timers_;
    qint32 live_exp_run_id_;

    const ViewController& viewController_;
    static ExperimentDataManager* manager_;
};

Q_DECLARE_METATYPE(PortLifecycleRequest);
Q_DECLARE_METATYPE(WorkloadRequest);
Q_DECLARE_METATYPE(CPUUtilisationRequest);
Q_DECLARE_METATYPE(MemoryUtilisationRequest);
Q_DECLARE_METATYPE(MarkerRequest);
Q_DECLARE_METATYPE(PortEventRequest);
Q_DECLARE_METATYPE(NetworkUtilisationRequest);

#endif // EXPERIMENTDATAMANAGER_H
