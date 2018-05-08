#ifndef MEDEA_SERVERPORTIMPL_H
#define MEDEA_SERVERPORTIMPL_H
#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class ServerPortImpl : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ServerPortImpl(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTIMPL_H

