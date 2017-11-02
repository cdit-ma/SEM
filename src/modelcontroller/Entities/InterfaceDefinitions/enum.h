#ifndef ENUM_H
#define ENUM_H
#include "../node.h"

class EntityFactory;
class Enum : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Enum(EntityFactory* factory);
	Enum();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //ENUM_H
