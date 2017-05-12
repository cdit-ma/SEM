#ifndef PARAMETER_H
#define PARAMETER_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class Parameter: public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Parameter(EntityFactory* factory, NODE_KIND kind, QString kind_str);
    Parameter(NODE_KIND kind);
public:

    bool isInputParameter() const;
    bool isVariadicParameter() const;
    bool isReturnParameter() const;
    bool isVariableParameter() const;


    virtual bool canAdoptChild(Node *node);
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // PARAMETER_H
