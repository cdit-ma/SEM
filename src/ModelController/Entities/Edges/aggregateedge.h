#ifndef AGGREGATEEDGE_H
#define AGGREGATEEDGE_H
#include "../edge.h"

class EntityFactory;
class AggregateEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    AggregateEdge(EntityFactory* factory);
    AggregateEdge(Node *src, Node *dst);
private:
    static AggregateEdge* ConstructEdge(Node* src, Node* dst);
};
#endif // AGGREGATEEDGE_H
