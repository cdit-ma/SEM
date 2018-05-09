#ifndef DDS_PARTITIONQOSPOLICY_H
#define DDS_PARTITIONQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_PartitionQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_PartitionQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_PARTITIONQOSPOLICY_H


