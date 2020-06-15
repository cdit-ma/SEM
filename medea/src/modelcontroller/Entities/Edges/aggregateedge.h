#ifndef AGGREGATEEDGE_H
#define AGGREGATEEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class AggregateEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AggregateEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};
#endif // AGGREGATEEDGE_H
