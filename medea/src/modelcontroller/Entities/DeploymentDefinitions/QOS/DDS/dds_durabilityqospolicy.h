#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_DurabilityQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_DurabilityQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_DURABILITYQOSPOLICY_H


