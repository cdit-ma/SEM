#ifndef DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#define DDS_OWNERSHIPSTRENGTHQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_OwnershipStrengthQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_OwnershipStrengthQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_OWNERSHIPSTRENGTHQOSPOLICY_H


