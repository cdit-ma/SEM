#ifndef DDS_DEADLINEQOSPOLICY_H
#define DDS_DEADLINEQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_DeadlineQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_DeadlineQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_DEADLINEQOSPOLICY_H


