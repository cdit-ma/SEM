#ifndef COMPONENT_H
#define COMPONENT_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Component : public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Component(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // COMPONENT_H
