#ifndef MEDEA_FORLOOP_H
#define MEDEA_FORLOOP_H

#include "../../node.h"



class EntityFactoryRegistryBroker;
namespace MEDEA{
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
        Node* expression_ = 0;
        Node* iteration_ = 0;
    };
};

#endif // MEDEA_FORLOOP_H
