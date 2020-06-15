#ifndef BEHAVIOURDEFINITIONC_H
#define BEHAVIOURDEFINITIONC_H

#include "../node.h"

class EntityFactoryRegistryBroker;
class BehaviourDefinitions: public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    BehaviourDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // BEHAVIOURDEFINITIONC_H
