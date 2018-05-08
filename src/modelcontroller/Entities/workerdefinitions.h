#ifndef WORKERDEFINITIONC_H
#define WORKERDEFINITIONC_H
#include "node.h"

class EntityFactory;
class WorkerDefinitions : public Node
{
    friend class EntityFactory;
    Q_OBJECT

protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    WorkerDefinitions(EntityFactory& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};

#endif // WORKERDEFINITIONC_H
