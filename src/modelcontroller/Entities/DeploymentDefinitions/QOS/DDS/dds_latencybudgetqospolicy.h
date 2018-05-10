#ifndef DDS_LATENCYBUDGETQOSPOLICY_H
#define DDS_LATENCYBUDGETQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_LatencyBudgetQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_LatencyBudgetQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_LATENCYBUDGETQOSPOLICY_H


