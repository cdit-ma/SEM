#ifndef DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#define DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_OwnershipStrengthQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    DDS_OwnershipStrengthQosPolicy(EntityFactory& factory, bool is_temp_node);
};
#endif // DDS_OWNERSHIPSTRENGTHQOSPOLICY_H


