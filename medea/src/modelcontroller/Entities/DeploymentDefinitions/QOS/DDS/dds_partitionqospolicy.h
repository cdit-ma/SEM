#ifndef DDS_PARTITIONQOSPOLICY_H
#define DDS_PARTITIONQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_PartitionQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_PartitionQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_PARTITIONQOSPOLICY_H


