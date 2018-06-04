#ifndef MEDEA_PORT_REPLIER_INST_H
#define MEDEA_PORT_REPLIER_INST_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReplierPortInst : public Node{
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ReplierPortInst(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_PORT_REPLIER_INST_H

