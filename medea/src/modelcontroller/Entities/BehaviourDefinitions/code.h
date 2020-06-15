#ifndef MODEL_IMPL_CODE_H
#define MODEL_IMPL_CODE_H

#include "../node.h"


class EntityFactoryRegistryBroker;
class Code: public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Code(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // MODEL_IMPL_CODE_H
