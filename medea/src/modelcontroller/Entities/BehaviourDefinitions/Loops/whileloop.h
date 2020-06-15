#ifndef MEDEA_WHILELOOP_H
#define MEDEA_WHILELOOP_H
#include "../../node.h"

class EntityFactoryRegistryBroker;
namespace MEDEA{
    class WhileLoop : public Node
    {
        friend class ::EntityFactory;
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        WhileLoop(EntityFactoryBroker& factory, bool is_temp_node);
    private:
        void updateLabel();
        Node* expression_ = 0;
    };
};

#endif // MEDEA_WHILELOOP_H
