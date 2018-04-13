#ifndef MEDEA_CLASS_H
#define MEDEA_CLASS_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class Class : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        Class(EntityFactory* factory);
        Class();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLASS_H
