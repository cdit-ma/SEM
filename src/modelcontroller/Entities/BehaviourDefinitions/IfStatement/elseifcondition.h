#ifndef MEDEA_ELSEIF_CONDITION_H
#define MEDEA_ELSEIF_CONDITION_H
#include "../containernode.h"


class EntityFactory;
namespace MEDEA{
    class ElseIfCondition: public Node, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ElseIfCondition(EntityFactory* factory);
        ElseIfCondition();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_ELSEIF_CONDITION_H
