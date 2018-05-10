#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class ComponentAssembly : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    ComponentAssembly(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // COMPONENTASSEMBLY_H
