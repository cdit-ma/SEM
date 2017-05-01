#ifndef MODEL_H
#define MODEL_H

#include "node.h"
class EntityFactory;
class Model: public Node
{
    friend class EntityFactory;
    Q_OBJECT
protected:
	Model(EntityFactory* factory);
    Model();
public:

    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};
#endif // MODEL_H
