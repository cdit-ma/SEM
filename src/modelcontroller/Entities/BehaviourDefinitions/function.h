#ifndef MEDEA_FUNCTION_H
#define MEDEA_FUNCTION_H
#include "containernode.h"

class EntityFactory;
namespace MEDEA{
    class Function :  public Node, public ContainerNode{
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
