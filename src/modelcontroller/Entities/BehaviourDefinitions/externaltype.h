
#ifndef MEDEA_EXTERNALTYPE_H
#define MEDEA_EXTERNALTYPE_H

#include "../node.h"


class EntityFactoryRegistryBroker;
namespace MEDEA{
    class ExternalType: public Node
    {
    friend class ::EntityFactory;
    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        ExternalType(EntityFactoryBroker& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

    private:
        Node* getTopBehaviourContainer();
        bool top_behaviour_calculated = false;
        Node* top_behaviour_container = 0;
    };
}

#endif // MEDEA_EXTERNALTYPE_H
