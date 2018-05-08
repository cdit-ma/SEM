#ifndef HARDWAREDEFINITIONC_H
#define HARDWAREDEFINITIONC_H

#include "../node.h"

class EntityFactory;
class HardwareDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    HardwareDefinitions(EntityFactory& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};
#endif // HARDWAREDEFINITIONC_H
