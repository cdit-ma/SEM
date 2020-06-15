#ifndef SHARED_DATATYPES_H
#define SHARED_DATATYPES_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class SharedDatatypes : public Node
{

    Q_OBJECT

    protected:
public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
        SharedDatatypes(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // SHARED_DATATYPES_H
