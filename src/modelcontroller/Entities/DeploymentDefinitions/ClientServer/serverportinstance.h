#ifndef MEDEA_SERVERPORTINSTANCE_H
#define MEDEA_SERVERPORTINSTANCE_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ServerPortInstance : public Node{
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ServerPortInstance(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTINSTANCE_H

