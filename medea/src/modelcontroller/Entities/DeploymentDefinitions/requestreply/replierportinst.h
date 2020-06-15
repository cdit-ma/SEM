#ifndef MEDEA_PORT_REPLIER_INST_H
#define MEDEA_PORT_REPLIER_INST_H
#include "../eventportassembly.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReplierPortInst : public EventPortAssembly{
    friend class ::EntityFactory;
    
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        ReplierPortInst(EntityFactoryBroker& factory, bool is_temp_node);
    };
};

#endif // MEDEA_PORT_REPLIER_INST_H

