#ifndef DDS_OWNERSHIPQOSPOLICY_H
#define DDS_OWNERSHIPQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_OwnershipQosPolicy: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_OwnershipQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_OWNERSHIPQOSPOLICY_H


