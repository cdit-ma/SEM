#ifndef MEDEA_PORT_REQUESTER_H
#define MEDEA_PORT_REQUESTER_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class RequesterPort : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        RequesterPort(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_PORT_REQUESTER_H
