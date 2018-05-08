#ifndef MEDEA_SERVERREQUEST_H
#define MEDEA_SERVERREQUEST_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerRequest : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        static void RegisterWithEntityFactory(EntityFactory& factory);
        ServerRequest(EntityFactory& factory, bool is_temp_node);
        
        QSet<Node*> getParentNodesForValidDefinition();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    private:
        Node* getTopBehaviourContainer();
        bool top_behaviour_calculated = false;
        Node* top_behaviour_container = 0;
    };
};

#endif // MEDEA_SERVERREQUEST_H

