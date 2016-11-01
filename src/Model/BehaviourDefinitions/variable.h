#ifndef VARIABLE_H
#define VARIABLE_H
#include "../InterfaceDefinitions/datanode.h"

class Variable: public DataNode
{
    Q_OBJECT
public:
    Variable();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};

#endif // VARIABLE_H
