#ifndef MEDEA_WHILELOOP_H
#define MEDEA_WHILELOOP_H

#include "../containernode.h"

class EntityFactory;
namespace MEDEA{
    class WhileLoop: public Node, public ContainerNode
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        WhileLoop(EntityFactory* factory);
        WhileLoop();
    public:
        bool canAdoptChild(Node *node);
    };
};

#endif // MEDEA_WHILELOOP_H
