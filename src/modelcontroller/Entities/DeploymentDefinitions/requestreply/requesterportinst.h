#ifndef MEDEA_PORT_REQUESTER_INST_H
#define MEDEA_PORT_REQUESTER_INST_H
#include "../../node.h"
#include "../eventportassembly.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class RequesterPortInst : public EventPortAssembly{

   
    friend class ::EntityFactory;
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        RequesterPortInst(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_PORT_REQUESTER_INST_H

