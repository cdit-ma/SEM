#ifndef MODEL_H
#define MODEL_H

#include "node.h"

class Model: public Node
{
    Q_OBJECT
public:
    Model();
    bool canAdoptChild(Node*);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};
#endif // MODEL_H
