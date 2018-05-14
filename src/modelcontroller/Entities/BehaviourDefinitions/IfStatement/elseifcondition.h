#ifndef MEDEA_ELSEIF_CONDITION_H
#define MEDEA_ELSEIF_CONDITION_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ElseIfCondition: public Node{

    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ElseIfCondition(EntityFactoryBroker& factory, bool is_temp_node);
private:
        void updateLabel();
        Node* expression_ = 0;
    };
};

#endif // MEDEA_ELSEIF_CONDITION_H
