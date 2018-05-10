#ifndef MEDEA_ELSEIF_CONDITION_H
#define MEDEA_ELSEIF_CONDITION_H
#include "../containernode.h"
#include "../../InterfaceDefinitions/datanode.h"



class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ElseIfCondition: public DataNode{

    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ElseIfCondition(EntityFactoryBroker& factory, bool is_temp_node);
    };
};

#endif // MEDEA_ELSEIF_CONDITION_H
