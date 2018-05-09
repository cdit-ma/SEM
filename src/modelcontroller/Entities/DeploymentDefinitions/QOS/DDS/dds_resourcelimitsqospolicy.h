#ifndef DDS_RESOURCELIMITSQOSPOLICY_H
#define DDS_RESOURCELIMITSQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_ResourceLimitsQosPolicy: public Node 
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_ResourceLimitsQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_RESOURCELIMITSQOSPOLICY_H


