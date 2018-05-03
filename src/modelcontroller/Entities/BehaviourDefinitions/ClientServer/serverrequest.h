#ifndef MEDEA_SERVERREQUEST_H
#define MEDEA_SERVERREQUEST_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerRequest : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerRequest(EntityFactory* factory);
        ServerRequest();
        QSet<Node*> getParentNodesForValidDefinition() override;
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

