#ifndef COMPONENTASSEMBLY_H
#define COMPONENTASSEMBLY_H
#include "../node.h"

class EntityFactory;
class ComponentAssembly : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	ComponentAssembly(EntityFactory* factory);
	ComponentAssembly();
};

#endif // COMPONENTASSEMBLY_H
