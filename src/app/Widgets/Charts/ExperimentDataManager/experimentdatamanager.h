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
#include "../Data/Requests/markerrequest.h"
#include "../Data/Requests/porteventrequest.h"

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
    void timerEvent(QTimerEvent* event) override;

    void visualiseSelectedExperimentRun(const AggServerResponse::ExperimentRun& experimentRun, bool charts, bool pulse);
    void experimentRunDataUpdated(quint32 exp_run_id, qint64 last_updated_time);

protected:
    static void constructSingleton(ViewController* vc);

private:
    explicit ExperimentDataManager(const ViewController &vc);
    
    static AggregationProxy& aggregationProxy();
    TimelineChartView& timelineChartView();

    void requestExperimentData(ExperimentDataRequestType request_type, const QVariant& request_param, QObject* requester = nullptr);
    void requestExperimentRuns(const QString& experimentName, MEDEA::ExperimentData* requester = nullptr);
    void requestExperimentState(quint32 experimentRunID, MEDEA::ExperimentRunData* requester = nullptr);

    void requestPortLifecycleEvents(const PortLifecycleRequest& request, PortInstanceData* requester);
    void requestWorkloadEvents(const WorkloadRequest& request, WorkerInstanceData* requester);
    void requestCPUUtilisationEvents(const UtilisationRequest& request, NodeData* requester);
    void requestMemoryUtilisationEvents(const UtilisationRequest& request, NodeData* requester);
    void requestMarkerEvents(const MarkerRequest& request, MarkerSetData* requester);
    void requestPortEvents(const PortEventRequest& request, PortInstanceData* requester);
    void requestNetworkUtilisationEvents(const UtilisationRequest& request, NodeData* requester);

    void processExperimentRuns(MEDEA::ExperimentData* requester, const QString& exp_name, const QVector<AggServerResponse::ExperimentRun>& exp_runs);
    void processExperimentState(MEDEA::ExperimentRunData* requester, const AggServerResponse::ExperimentState& exp_state);
    void processPortLifecycleEvents(PortInstanceData* requester, const QVector<PortLifecycleEvent*>& events);
    void processPortEvents(PortInstanceData* requester, const QVector<PortEvent*>& events);
    void processWorkloadEvents(WorkerInstanceData* requester, const QVector<WorkloadEvent*>& events);
    void processMarkerEvents(MarkerSetData* requester, const QString& exp_name, quint32 exp_run_id, const QVector<MarkerEvent*>& events);
    void processCPUUtilisationEvents(NodeData* requester, const QVector<CPUUtilisationEvent*>& events);
    void processMemoryUtilisationEvents(NodeData* requester, const QVector<MemoryUtilisationEvent*>& events);
    void processNetworkUtilisationEvents(NodeData* requester, const QVector<NetworkUtilisationEvent*>& events);

    MEDEA::ExperimentData* constructExperimentData(const QString& exp_name);
    MEDEA::ExperimentData* getExperimentData(const QString& exp_name) const;
    MEDEA::ExperimentRunData& getExperimentRunData(const QString& exp_name, quint32 exp_run_id) const;

    void showDataForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data);
    void showPulseForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data);
    void showChartsForExperimentRun(const MEDEA::ExperimentRunData& exp_run_data);
    void showChartForSeries(const QPointer<const MEDEA::EventSeries>& series, const MEDEA::ExperimentRunData& exp_run_data);

    DataflowDialog* dataflowDialog_ = nullptr;
    ChartDialog* chartDialog_ = nullptr;
    ChartInputPopup chartPopup_;

    QVector<ViewItem*> selectedViewItems_;
    QList<MEDEA::ChartDataKind> selectedDataKinds_;
    AggServerResponse::ExperimentRun selectedExperimentRun_;

    bool show_in_charts_ = false;
    bool show_in_pulse_ = false;

    QHash<QString, MEDEA::ExperimentData*> experiment_data_hash_;
    QHash<quint32, int> exp_run_timers_;
    QString live_exp_name_;
    qint32 live_exp_run_id_;

    const ViewController& viewController_;
    static ExperimentDataManager* manager_;
};

Q_DECLARE_METATYPE(PortLifecycleRequest);
Q_DECLARE_METATYPE(WorkloadRequest);
Q_DECLARE_METATYPE(UtilisationRequest);
Q_DECLARE_METATYPE(MarkerRequest);
Q_DECLARE_METATYPE(PortEventRequest);

#endif // EXPERIMENTDATAMANAGER_H