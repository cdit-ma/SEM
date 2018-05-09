#ifndef DDS_LIFESPANQOSPOLICY_H
#define DDS_LIFESPANQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_LifespanQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_LifespanQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_HISTORYQOSPOLICY_H


