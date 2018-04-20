#ifndef MEDEA_SERVERPORTIMPL_H
#define MEDEA_SERVERPORTIMPL_H
#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class ServerPortImpl : public Node, public ContainerNode{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerPortImpl(EntityFactory* factory);
        ServerPortImpl();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTIMPL_H

