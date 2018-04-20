#ifndef MEDEA_CLIENTPORT_H
#define MEDEA_CLIENTPORT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ClientPort : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ClientPort(EntityFactory* factory);
        ClientPort();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLIENTPORT_H
