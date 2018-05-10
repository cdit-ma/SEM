#ifndef MEDEA_CLIENTPORT_H
#define MEDEA_CLIENTPORT_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ClientPort : public Node{

   
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ClientPort(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_CLIENTPORT_H
