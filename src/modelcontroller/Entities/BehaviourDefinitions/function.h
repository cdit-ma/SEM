#ifndef MEDEA_FUNCTION_H
#define MEDEA_FUNCTION_H
#include "../node.h"
#include "containernode.h"

class EntityFactory;
namespace MEDEA{
    class Function :  public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        Function(EntityFactory* factory);
        Function();
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_FUNCTION_H
