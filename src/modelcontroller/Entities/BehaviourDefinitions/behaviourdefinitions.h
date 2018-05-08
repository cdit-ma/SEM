#ifndef BEHAVIOURDEFINITIONC_H
#define BEHAVIOURDEFINITIONC_H

#include "../node.h"

class EntityFactory;
class BehaviourDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    BehaviourDefinitions(EntityFactory& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // BEHAVIOURDEFINITIONC_H
