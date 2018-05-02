#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_DurabilityQosPolicy(EntityFactory* factory);
	DDS_DurabilityQosPolicy();
};
#endif // DDS_DURABILITYQOSPOLICY_H


