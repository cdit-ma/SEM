#ifndef AGGREGATEEDGE_H
#define AGGREGATEEDGE_H
#include "../edge.h"

class EntityFactory;
class AggregateEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    AggregateEdge(EntityFactory& factory, Node *src, Node *dst);
};
#endif // AGGREGATEEDGE_H
