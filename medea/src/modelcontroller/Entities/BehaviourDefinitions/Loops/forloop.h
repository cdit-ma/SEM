#ifndef MEDEA_FORLOOP_H
#define MEDEA_FORLOOP_H

#include "../../node.h"



class EntityFactoryRegistryBroker;
class Setter;

namespace MEDEA{
    class BooleanExpression;
    class ForLoop : public Node{

    

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ForLoop(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node *node);
    private:
        void updateLabel();
        Node* variable_ = 0;
        BooleanExpression* expression_ = 0;
        Setter* iteration_ = 0;
    };
};

#endif // MEDEA_FORLOOP_H
