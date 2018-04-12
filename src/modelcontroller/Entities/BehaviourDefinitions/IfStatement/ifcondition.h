#ifndef MEDEA_IF_CONDITION_H
#define MEDEA_IF_CONDITION_H
#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class IfCondition: public Node, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        IfCondition(EntityFactory* factory);
        IfCondition();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_IF_CONDITION_H
