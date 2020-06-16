#ifndef INTERFACEDEFINITIONC_H
#define INTERFACEDEFINITIONC_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class InterfaceDefinitions: public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    InterfaceDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // INTERFACEDEFINITIONC_H
