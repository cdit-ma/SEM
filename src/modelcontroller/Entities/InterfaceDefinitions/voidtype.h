#ifndef VOIDTYPE_H
#define VOIDTYPE_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class VoidType : public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    VoidType(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // VOIDTYPE_H
