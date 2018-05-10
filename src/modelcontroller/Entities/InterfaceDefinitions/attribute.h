#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Attribute : public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Attribute(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // ATTRIBUTE_H
