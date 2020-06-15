#ifndef MEDEA_TRANSITION_FUNCTION_H
#define MEDEA_TRANSITION_FUNCTION_H
#include "../node.h"
#include "containernode.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class TransitionFunction :  public Node{

   
    friend class ::EntityFactory;
    protected:
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        TransitionFunction(EntityFactoryBroker& factory, bool is_temp_node);

        void parentSet(Node* parent);
    private:
        void updateLabel();
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_TRANSITION_FUNCTION_H
