#ifndef PARAMETER_H
#define PARAMETER_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class Parameter: public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    Parameter(EntityFactory& factory, NODE_KIND node_kind, bool is_temp_node);
public:
    bool isInputParameter() const;
    bool isVariadicParameter() const;
    bool isReturnParameter() const;
    bool isVariableParameter() const;

    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // PARAMETER_H
