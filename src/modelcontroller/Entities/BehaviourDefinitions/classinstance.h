#ifndef MEDEA_CLASS_INSTANCE_H
#define MEDEA_CLASS_INSTANCE_H
#include "../node.h"

class EntityFactory;
namespace MEDEA{
    class ClassInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ClassInstance(EntityFactory* factory);
        ClassInstance();
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLASS_INSTANCE_H
