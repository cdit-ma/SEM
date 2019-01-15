#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>

#include "../../Widgets/Charts/Data/Events/protomessagestructs.h"
#include "../../Widgets/Charts/Data/Events/event.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();
    ~AggregationProxy();

    void SetServerEndpoint(QString endpoint);
    void SetRequestEventKinds(QList<TIMELINE_DATA_KIND> kinds);

    void RequestRunningExperiments();
    void RequestExperimentRuns(QString experimentName = "");
    void RequestExperimentState(quint32 experimentRunID);
    void RequestEvents(QString nodeHostname, QString componentName, QString workerName);
    void ReloadRunningExperiments();

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void setChartUserInputDialogVisible(bool visible);
    void requestedExperimentRuns(QList<ExperimentRun> runs);
    void requestedExperimentState(QStringList nodeHostname, QStringList componentName, QStringList workerName);

    void clearPreviousEvents();
    void receivedAllEvents();

public slots:
    void setSelectedExperimentRunID(quint32 ID);

private:
    bool GotRequester();

    bool hasSelectedExperimentID_ = false;
    quint32 experimentRunID_;

    QList<TIMELINE_DATA_KIND> requestEventKinds_;

    AggServer::Requester* requester_ = 0;

};

#endif // MEDEA_AGGREGATIONPROXY_H
