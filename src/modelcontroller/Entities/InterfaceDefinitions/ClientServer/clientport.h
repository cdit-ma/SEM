#ifndef MEDEA_CLIENTPORT_H
#define MEDEA_CLIENTPORT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ClientPort : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ClientPort(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_CLIENTPORT_H
