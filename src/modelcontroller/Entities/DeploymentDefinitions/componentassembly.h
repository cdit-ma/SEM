#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"

class EntityFactory;
class ComponentAssembly : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    ComponentAssembly(EntityFactory& factory, bool is_temp_node);
};

#endif // COMPONENTASSEMBLY_H
