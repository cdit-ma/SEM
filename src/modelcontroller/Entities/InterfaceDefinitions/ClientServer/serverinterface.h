#ifndef MEDEA_SERVER_INTERFACE_H
#define MEDEA_SERVER_INTERFACE_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerInterface : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerInterface(EntityFactory* factory);
        ServerInterface();
    public:
        bool canAdoptChild(Node* child);
    };
};

#endif // MEDEA_SERVER_INTERFACE_H
