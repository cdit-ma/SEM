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
};


#endif // AGGREGATE_H
