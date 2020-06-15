#ifndef MEDEA_PORT_REQUESTER_H
#define MEDEA_PORT_REQUESTER_H
#include "../eventport.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class RequesterPort : public EventPort{

   
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
