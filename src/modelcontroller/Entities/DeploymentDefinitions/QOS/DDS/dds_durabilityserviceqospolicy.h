#ifndef DDS_DURABILITYSERVICEQOSPOLICY_H
#define DDS_DURABILITYSERVICEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DurabilityServiceQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_DurabilityServiceQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_DURABILITYSERVICEQOSPOLICY_H


