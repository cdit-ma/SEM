#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_ReliabilityQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_ReliabilityQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_RELIABILITYQOSPOLICY_H


