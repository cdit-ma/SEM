#ifndef MEDEA_PORT_REPLIER_H
#define MEDEA_PORT_REPLIER_H
#include "../eventport.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ReplierPort : public EventPort{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ReplierPort(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_PORT_REPLIER_H
