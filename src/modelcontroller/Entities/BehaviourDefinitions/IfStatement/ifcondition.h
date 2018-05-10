#ifndef MEDEA_IF_CONDITION_H
#define MEDEA_IF_CONDITION_H
#include "../containernode.h"
#include "../../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class IfCondition: public DataNode
    {
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        IfCondition(EntityFactoryBroker& factory, bool is_temp_node);
    };
};

#endif // MEDEA_IF_CONDITION_H
