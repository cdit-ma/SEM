#ifndef PARAMETER_H
#define PARAMETER_H
#include "../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
class Parameter: public DataNode
{

    Q_OBJECT

protected:
    Parameter(EntityFactoryBroker& factory, NODE_KIND node_kind, bool is_temp_node);
public:
    virtual bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // PARAMETER_H
