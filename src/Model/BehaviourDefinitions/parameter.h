#ifndef PARAMETER_H
#define PARAMETER_H
#include "../InterfaceDefinitions/datanode.h"

class Parameter: public DataNode
{
    Q_OBJECT
public:
    Parameter(NODE_KIND kind);

    bool isInputParameter() const;
    bool isVariadicParameter() const;
    bool isReturnParameter() const;


    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
};

#endif // PARAMETER_H
