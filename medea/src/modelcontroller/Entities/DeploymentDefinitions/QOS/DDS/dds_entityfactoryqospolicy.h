#ifndef DDS_ENTITYFACTORYQOSPOLICY_H
#define DDS_ENTITYFACTORYQOSPOLICY_H
#include "../../../node.h"


class EntityFactoryRegistryBroker;
class DDS_EntityFactoryQosPolicy: public Node 
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DDS_EntityFactoryQosPolicy(EntityFactoryBroker& factory, bool is_temp_node);
};
#endif // DDS_ENTITYFACTORYQOSPOLICY_H


