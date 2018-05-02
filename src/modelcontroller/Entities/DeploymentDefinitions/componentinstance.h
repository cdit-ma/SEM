#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"

class EntityFactory;
class ComponentInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ComponentInstance(EntityFactory* factory);
	ComponentInstance();
    QSet<Node*> getListOfValidAncestorsForChildrenDefinitions() override;
};

#endif // COMPONENTINSTANCE_H
