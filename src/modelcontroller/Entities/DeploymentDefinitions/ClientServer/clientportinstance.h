#ifndef MEDEA_CLIENTPORTINSTANCE_H
#define MEDEA_CLIENTPORTINSTANCE_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ClientPortInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ClientPortInstance(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLIENTPORTINSTANCE_H

