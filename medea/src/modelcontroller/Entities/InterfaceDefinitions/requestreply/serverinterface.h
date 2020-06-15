#ifndef MEDEA_SERVER_INTERFACE_H
#define MEDEA_SERVER_INTERFACE_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ServerInterface : public Node{
    friend class ::EntityFactory;
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        ServerInterface(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    private:
        void updateLabel();
    };
};

#endif // MEDEA_SERVER_INTERFACE_H
