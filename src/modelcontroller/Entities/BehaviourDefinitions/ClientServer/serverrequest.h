#ifndef MEDEA_SERVERREQUEST_H
#define MEDEA_SERVERREQUEST_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerRequest : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerRequest(EntityFactory* factory);
        ServerRequest();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERREQUEST_H

