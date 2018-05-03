#ifndef MEDEA_WHILELOOP_H
#define MEDEA_WHILELOOP_H

#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class WhileLoop: public Node
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
