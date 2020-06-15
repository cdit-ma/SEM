#ifndef DEFINITIONEDGE_H
#define DEFINITIONEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class DefinitionEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DefinitionEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};

#endif // EDGE_H
