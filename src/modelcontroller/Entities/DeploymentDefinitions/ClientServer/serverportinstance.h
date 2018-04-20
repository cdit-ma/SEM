#ifndef MEDEA_SERVERPORTINSTANCE_H
#define MEDEA_SERVERPORTINSTANCE_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class ServerPortInstance : public Node{
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        ServerPortInstance(EntityFactory* factory);
        ServerPortInstance();
        QList<Node*> getAdoptableNodes(Node* definition);
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};

#endif // MEDEA_SERVERPORTINSTANCE_H

