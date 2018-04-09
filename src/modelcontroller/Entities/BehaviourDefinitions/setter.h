#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "../node.h"

class EntityFactory;
class Setter: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Setter(EntityFactory* factory);
	Setter();
    void childAdded(Node* child);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //SETFUNCTION_H
