#ifndef AGGREGATE_H
#define AGGREGATE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Aggregate : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Aggregate(EntityFactoryBroker& factory, bool is_temp_node);
};


#endif // AGGREGATE_H
