
#ifndef MEDEA_AGGREGATIONPROXY_H
#define MEDEA_AGGREGATIONPROXY_H

#include <QObject>
#include <comms/environmentcontroller/environmentcontroller.h>

class AggregationProxy : public QObject
{
    Q_OBJECT
public:
    AggregationProxy();
private:
    EnvironmentManager::EnvironmentController controller_;
};

#endif // MEDEA_AGGREGATIONPROXY_H
