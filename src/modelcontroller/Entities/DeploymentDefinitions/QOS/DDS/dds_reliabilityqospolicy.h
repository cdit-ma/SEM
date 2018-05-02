#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ReliabilityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_ReliabilityQosPolicy(EntityFactory* factory);
	DDS_ReliabilityQosPolicy();
};
#endif // DDS_RELIABILITYQOSPOLICY_H


