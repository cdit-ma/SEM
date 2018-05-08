#ifndef DEPLOYMENTDEFINITIONC_H
#define DEPLOYMENTDEFINITIONC_H
#include "../node.h"

class EntityFactory;
class DeploymentDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory* factory);
	DeploymentDefinitions(EntityFactory* factory, bool is_temp);
public:
    bool canAdoptChild(Node* child);
};
#endif // DEPLOYMENTDEFINITIONC_H
