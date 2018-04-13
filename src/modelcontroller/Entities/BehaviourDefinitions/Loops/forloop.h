#ifndef MEDEA_FORLOOP_H
#define MEDEA_FORLOOP_H

#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class ForLoop: public Node, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ForLoop(EntityFactory* factory);
        ForLoop();
    public:
        bool canAdoptChild(Node *node);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_FORLOOP_H
