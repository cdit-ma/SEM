#ifndef MEDEA_WHILELOOP_H
#define MEDEA_WHILELOOP_H

#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class WhileLoop : public Node
    {

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        WhileLoop(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node *node);
    };
};

#endif // MEDEA_WHILELOOP_H
