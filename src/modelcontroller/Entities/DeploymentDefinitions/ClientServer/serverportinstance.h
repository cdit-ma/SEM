#ifndef MEDEA_SERVERPORTINSTANCE_H
#define MEDEA_SERVERPORTINSTANCE_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerPortInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ServerPortInstance(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTINSTANCE_H

