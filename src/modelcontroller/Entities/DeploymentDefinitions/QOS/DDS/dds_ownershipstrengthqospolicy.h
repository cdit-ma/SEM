#ifndef DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#define DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_OwnershipStrengthQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_OwnershipStrengthQosPolicy(EntityFactory* factory);
	DDS_OwnershipStrengthQosPolicy();
};
#endif // DDS_OWNERSHIPSTRENGTHQOSPOLICY_H


