#ifndef COMPONENTBEHAVIOUR_H
#define COMPONENTBEHAVIOUR_H
#include "../node.h"

class EntityFactory;
class ComponentImpl: public Node
{
	friend class EntityFactory;
     Q_OBJECT
protected:
	ComponentImpl(EntityFactory* factory);
	ComponentImpl();
    QSet<Node*> getDependants() const;
};

#endif // COMPONENTBEHAVIOUR_H


