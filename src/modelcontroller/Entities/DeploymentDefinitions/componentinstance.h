#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class ComponentInstance : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    ComponentInstance(EntityFactoryBroker& factory, bool is_temp_node);
    QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
};

#endif // COMPONENTINSTANCE_H
