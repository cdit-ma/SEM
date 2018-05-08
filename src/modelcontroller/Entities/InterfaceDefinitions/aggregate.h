#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"

class EntityFactory;
class Aggregate : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    Aggregate(EntityFactory& factory, bool is_temp_node);
    void DataAdded(Data* data);
};


#endif // AGGREGATE_H
