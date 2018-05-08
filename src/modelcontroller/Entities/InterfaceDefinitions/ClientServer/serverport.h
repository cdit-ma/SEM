#ifndef MEDEA_SERVERPORT_H
#define MEDEA_SERVERPORT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerPort : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ServerPort(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_SERVERPORT_H
