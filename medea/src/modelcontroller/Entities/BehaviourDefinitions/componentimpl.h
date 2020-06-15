#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class ComponentImpl: public Node
{

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    ComponentImpl(EntityFactoryBroker& factory, bool is_temp_node);
    QSet<Node*> getDependants() const;
};

#endif // COMPONENTBEHAVIOUR_H


