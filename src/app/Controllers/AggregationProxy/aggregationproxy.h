#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/protoMessageStructs.h"
#include "../../Widgets/Charts/Data/Events/workloadevent.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();

    void RequestRunningExperiments();
    void RequestExperimentRuns(QString experimentName = "");
    void RequestExperimentState(quint32 experimentRunID);
    void RequestEvents(quint32 ID, QString componentName);
    void ReloadRunningExperiments();

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static std::string constructStdStringFromQString(QString s);

    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);
    void requstedExperimentRuns(QList<ExperimentRun> runs);
    void requstedComponentNames(QStringList names);

    void receivedWorkloadEvent(WorkloadEvent* event);

    void clearPreviousEvents();
    void receivedAllEvents();

public slots:
    void setSelectedExperimentRunID(quint32 ID);

private:    
    void SendWorkloadRequest(AggServer::WorkloadRequest& request);

    WorkerInstance convertWorkerInstance(const AggServer::WorkerInstance inst);
    WorkloadEvent::WorkloadEventType getWorkloadEventType(const AggServer::WorkloadEvent_WorkloadEventType type);

    bool hasSelctedExperimentID_ = false;
    quint32 experimentRunID_;
    QStringList componentNames_;

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
