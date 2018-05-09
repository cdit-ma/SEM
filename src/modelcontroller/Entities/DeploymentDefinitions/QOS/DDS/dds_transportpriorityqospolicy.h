#ifndef DDS_TRANSPORTPRIORITYQOSPOLICY_H
#define DDS_TRANSPORTPRIORITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TransportPriorityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_TransportPriorityQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_TRANSPORTPRIORITYQOSPOLICY_H


