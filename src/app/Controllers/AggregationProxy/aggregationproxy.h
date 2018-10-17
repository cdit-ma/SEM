
#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <QFuture>
#include <comms/aggregationrequester/aggregationrequester.h>

class AggregationProxy : public QObject
{
    Q_OBJECT

public:
    AggregationProxy();

    void RequestRunningExperiments();

signals:
    void resultsReady();

private:
    QFuture<std::unique_ptr<AggServer::PortLifecycleResponse>> getPortLifecycle(const AggServer::PortLifecycleRequest request);

    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
