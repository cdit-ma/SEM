#ifndef MEDEA_CLIENTPORTINSTANCE_H
#define MEDEA_CLIENTPORTINSTANCE_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ClientPortInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ClientPortInstance(EntityFactory* factory);
        ClientPortInstance();
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLIENTPORTINSTANCE_H

