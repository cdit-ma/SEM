#ifndef HARDWAREDEFINITIONC_H
#define HARDWAREDEFINITIONC_H

#include "../node.h"


class EntityFactoryRegistryBroker;
class HardwareDefinitions: public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    HardwareDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};
#endif // HARDWAREDEFINITIONC_H
