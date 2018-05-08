#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

class EntityFactory;
class ComponentImpl: public Node
{
	friend class EntityFactory;
     Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    ComponentImpl(EntityFactory& factory, bool is_temp_node);
    QSet<Node*> getDependants() const;
};

#endif // COMPONENTBEHAVIOUR_H


