#ifndef MODEL_IMPL_HEADER_H
#define MODEL_IMPL_HEADER_H

#include "../node.h"


class EntityFactoryRegistryBroker;
class Header: public Node
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Header(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // MODEL_IMPL_HEADER_H
