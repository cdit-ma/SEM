#ifndef MEDEA_SERVERPORT_H
#define MEDEA_SERVERPORT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerPort : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerPort(EntityFactory* factory);
        ServerPort();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORT_H
