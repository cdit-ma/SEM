#ifndef DDS_TRANSPORTPRIORITYQOSPOLICY_H
#define DDS_TRANSPORTPRIORITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TransportPriorityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_TransportPriorityQosPolicy(EntityFactory* factory);
	DDS_TransportPriorityQosPolicy();
};
#endif // DDS_TRANSPORTPRIORITYQOSPOLICY_H


