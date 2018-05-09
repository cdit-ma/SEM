#ifndef DDS_TIMEBASEDFILTERQOSPOLICY_H
#define DDS_TIMEBASEDFILTERQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_TimeBasedFilterQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_TimeBasedFilterQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_TIMEBASEDFILTERQOSPOLICY_H


