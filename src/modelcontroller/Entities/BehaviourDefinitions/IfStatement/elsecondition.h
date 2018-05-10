#ifndef MEDEA_ELSE_CONDITION_H
#define MEDEA_ELSE_CONDITION_H
#include "../../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ElseCondition: public DataNode{
    
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ElseCondition(EntityFactoryBroker& factory, bool is_temp_node);
    };
};

#endif // MEDEA_ELSE_CONDITION_H
