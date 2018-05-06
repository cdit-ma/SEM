#ifndef BEHAVIOURDEFINITIONC_H
#define BEHAVIOURDEFINITIONC_H

#include "../node.h"

class EntityFactory;
class BehaviourDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	BehaviourDefinitions(EntityFactory* factory);
	BehaviourDefinitions();
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // BEHAVIOURDEFINITIONC_H
