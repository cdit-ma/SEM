#ifndef MEDEA_SERVERPORTIMPL_H
#define MEDEA_SERVERPORTIMPL_H
#include "../containernode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ServerPortImpl : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ServerPortImpl(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTIMPL_H

