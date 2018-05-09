#ifndef DDS_DURABILITYQOSPOLICY_H
#define DDS_DURABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_DurabilityQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_DURABILITYQOSPOLICY_H


