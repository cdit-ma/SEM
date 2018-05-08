
#ifndef MEDEA_EXTERNALTYPE_H
#define MEDEA_EXTERNALTYPE_H

#include "../node.h"
class EntityFactory;
namespace MEDEA{
    class ExternalType: public Node
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ExternalType(EntityFactory& factory, bool is_temp_node);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);

    private:
        Node* getTopBehaviourContainer();
        bool top_behaviour_calculated = false;
        Node* top_behaviour_container = 0;
    };
}

#endif // MEDEA_EXTERNALTYPE_H
