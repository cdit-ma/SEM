#ifndef MEDEA_PORT_REPLIER_IMPL_H
#define MEDEA_PORT_REPLIER_IMPL_H
#include "../containernode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReplierPortImpl : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ReplierPortImpl(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTIMPL_H

