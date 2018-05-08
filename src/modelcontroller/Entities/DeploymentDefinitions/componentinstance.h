#ifndef COMPONENTINSTANCE_H
#define COMPONENTINSTANCE_H
#include "../node.h"

class EntityFactory;
class ComponentInstance : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    ComponentInstance(EntityFactory& factory, bool is_temp_node);
    QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
};

#endif // COMPONENTINSTANCE_H
