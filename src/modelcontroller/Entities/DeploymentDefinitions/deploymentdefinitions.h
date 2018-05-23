#ifndef DEPLOYMENTDEFINITIONC_H
#define DEPLOYMENTDEFINITIONC_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class DeploymentDefinitions: public Node
{

    Q_OBJECT
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DeploymentDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};
#endif // DEPLOYMENTDEFINITIONC_H
