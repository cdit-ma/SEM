#ifndef DDS_RESOURCELIMITSQOSPOLICY_H
#define DDS_RESOURCELIMITSQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_ResourceLimitsQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_ResourceLimitsQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_RESOURCELIMITSQOSPOLICY_H


