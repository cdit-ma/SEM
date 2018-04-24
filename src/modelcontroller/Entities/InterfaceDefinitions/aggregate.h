#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class EntityFactory;
class Aggregate : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Aggregate(EntityFactory* factory);
	Aggregate();
    void DataAdded(Data* data);
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};


#endif // AGGREGATE_H
