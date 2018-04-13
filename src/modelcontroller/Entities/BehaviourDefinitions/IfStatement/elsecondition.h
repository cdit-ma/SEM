#ifndef MEDEA_ELSE_CONDITION_H
#define MEDEA_ELSE_CONDITION_H
#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class ElseCondition: public Node, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ElseCondition(EntityFactory* factory);
        ElseCondition();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_ELSE_CONDITION_H
