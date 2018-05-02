#ifndef DDS_PARTITIONQOSPOLICY_H
#define DDS_PARTITIONQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_PartitionQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_PartitionQosPolicy(EntityFactory* factory);
	DDS_PartitionQosPolicy();
};
#endif // DDS_PARTITIONQOSPOLICY_H


