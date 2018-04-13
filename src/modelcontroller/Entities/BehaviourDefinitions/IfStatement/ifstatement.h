#ifndef MEDEA_IFSTATEMENT_H
#define MEDEA_IFSTATEMENT_H
#include "../../node.h"

class EntityFactory;
namespace MEDEA{
    class IfStatement: public Node
    {
        friend class ::EntityFactory;
        Q_OBJECT
    protected:
        IfStatement(EntityFactory* factory);
        IfStatement();
    public:
        bool canAdoptChild(Node* child);
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
    };
};
#endif // MEDEA_IFSTATEMENT_H
