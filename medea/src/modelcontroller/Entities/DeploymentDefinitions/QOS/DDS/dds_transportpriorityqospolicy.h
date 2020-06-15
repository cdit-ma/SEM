#ifndef DDS_TRANSPORTPRIORITYQOSPOLICY_H
#define DDS_TRANSPORTPRIORITYQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_TransportPriorityQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_TransportPriorityQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_TRANSPORTPRIORITYQOSPOLICY_H


