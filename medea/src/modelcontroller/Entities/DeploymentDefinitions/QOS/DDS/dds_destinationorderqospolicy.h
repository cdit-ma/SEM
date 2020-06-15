#ifndef DDS_DESTINATIONORDERQOSPOLICY_H
#define DDS_DESTINATIONORDERQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_DestinationOrderQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_DestinationOrderQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_DESTINATIONORDERQOSPOLICY_H


