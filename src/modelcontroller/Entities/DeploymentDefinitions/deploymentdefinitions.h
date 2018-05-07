#ifndef DEPLOYMENTDEFINITIONC_H
#define DEPLOYMENTDEFINITIONC_H
#include "../node.h"

class EntityFactory;
class DeploymentDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	DeploymentDefinitions(EntityFactory* factory);
	static Node* ConstructDeploymentDefinitions(EntityFactory* factory);
	DeploymentDefinitions();
public:
    bool canAdoptChild(Node* child);
};
#endif // DEPLOYMENTDEFINITIONC_H
