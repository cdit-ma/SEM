#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>

#include <google/protobuf/util/time_util.h>
#include <comms/aggregationrequester/aggregationrequester.h>
#include "../../Widgets/Charts/Data/Events/protoMessageStructs.h"

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    explicit AggregationProxy();

    void RequestRunningExperiments();
    void RequestExperimentRun(QString experimentName = "");

    static std::unique_ptr<google::protobuf::Timestamp> constructTimestampFromMS(qint64 milliseconds);
    static const QDateTime getQDateTime(const google::protobuf::Timestamp &time);
    static const QString getQString(const std::string &string);

signals:
    void showChartUserInputDialog();
    void experimentRuns(QList<ExperimentRun> runs);

    void clearPreviousEvents();
    void receivedAllEvents();

private:
    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
