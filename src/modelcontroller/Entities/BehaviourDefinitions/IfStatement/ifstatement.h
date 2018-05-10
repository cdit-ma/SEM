#ifndef MEDEA_IFSTATEMENT_H
#define MEDEA_IFSTATEMENT_H
#include "../../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class IfStatement: public Node
    {
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        IfStatement(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAdoptChild(Node* child);
    };
};
#endif // MEDEA_IFSTATEMENT_H
