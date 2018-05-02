#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"

class EntityFactory;
class DDS_OwnershipQosPolicy: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DDS_OwnershipQosPolicy(EntityFactory* factory);
	DDS_OwnershipQosPolicy();
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


