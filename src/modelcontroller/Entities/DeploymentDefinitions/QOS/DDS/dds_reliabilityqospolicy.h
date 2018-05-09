#ifndef DDS_RELIABILITYQOSPOLICY_H
#define DDS_RELIABILITYQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ReliabilityQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_ReliabilityQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_RELIABILITYQOSPOLICY_H


