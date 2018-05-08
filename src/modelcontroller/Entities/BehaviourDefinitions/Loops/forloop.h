#ifndef MEDEA_FORLOOP_H
#define MEDEA_FORLOOP_H

#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ForLoop : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ForLoop(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node *node);
    };
};

#endif // MEDEA_FORLOOP_H
