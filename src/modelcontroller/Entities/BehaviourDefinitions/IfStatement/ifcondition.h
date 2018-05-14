#ifndef MEDEA_IF_CONDITION_H
#define MEDEA_IF_CONDITION_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class IfCondition: public Node
    {
    friend class ::EntityFactory;
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        IfCondition(EntityFactoryBroker& factory, bool is_temp_node);
    private:
        void updateLabel();
        Node* expression_ = 0;
    };
};

#endif // MEDEA_IF_CONDITION_H
