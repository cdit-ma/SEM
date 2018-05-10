#ifndef MEDEA_CLIENTPORTINSTANCE_H
#define MEDEA_CLIENTPORTINSTANCE_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ClientPortInstance : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ClientPortInstance(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLIENTPORTINSTANCE_H

