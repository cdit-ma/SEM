#ifndef WORKERDEFINITIONC_H
#define WORKERDEFINITIONC_H
#include "node.h"


class EntityFactoryRegistryBroker;
class WorkerDefinitions : public Node
{

    Q_OBJECT


protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    WorkerDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // WORKERDEFINITIONC_H
