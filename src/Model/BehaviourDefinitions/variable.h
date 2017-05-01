#ifndef VARIABLE_H
#define VARIABLE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class Variable: public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Variable(EntityFactory* factory);
	Variable();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);

};

#endif // VARIABLE_H
