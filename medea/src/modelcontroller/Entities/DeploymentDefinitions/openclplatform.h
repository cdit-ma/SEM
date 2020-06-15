#ifndef OPENCLPLATFORM_H
#define OPENCLPLATFORM_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class OpenCLPlatform : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    OpenCLPlatform(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif // OPENCLPLATFORM_H
