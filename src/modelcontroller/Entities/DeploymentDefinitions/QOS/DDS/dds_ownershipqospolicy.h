#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_OwnershipQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_OwnershipQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


