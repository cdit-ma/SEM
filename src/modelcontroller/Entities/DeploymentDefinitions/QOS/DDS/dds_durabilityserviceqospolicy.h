#ifndef DDS_DURABILITYSERVICEQOSPOLICY_H
#define DDS_DURABILITYSERVICEQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_DurabilityServiceQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_DurabilityServiceQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_DURABILITYSERVICEQOSPOLICY_H


