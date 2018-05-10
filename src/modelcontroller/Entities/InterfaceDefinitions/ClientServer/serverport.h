#ifndef MEDEA_SERVERPORT_H
#define MEDEA_SERVERPORT_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ServerPort : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ServerPort(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_SERVERPORT_H
