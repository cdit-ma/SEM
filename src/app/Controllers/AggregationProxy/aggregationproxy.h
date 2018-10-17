
#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
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
    AggServer::Requester requester_;

};

#endif // MEDEA_AGGREGATIONPROXY_H
