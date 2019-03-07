#ifndef MEDEA_CLASS_INSTANCE_H
#define MEDEA_CLASS_INSTANCE_H
#include "../node.h"

class EntityFactoryRegistryBroker;
class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ClassInstance : public Node{
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ClassInstance(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_CLASS_INSTANCE_H
