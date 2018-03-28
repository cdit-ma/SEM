#ifndef MEDEA_FUNCTION_H
#define MEDEA_FUNCTION_H
#include "behaviournode.h"

class EntityFactory;
namespace MEDEA{
    class Function : public BehaviourNode{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        Function(EntityFactory* factory);
        Function();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_FUNCTION_H
