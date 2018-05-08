#ifndef ASSEMBLYDEFINITIONC_H
#define ASSEMBLYDEFINITIONC_H
#include "../node.h"

#include "../../edgekinds.h"
class EntityFactory;
class AssemblyDefinitions: public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    AssemblyDefinitions(EntityFactory& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};
#endif // AssemblyDefinitions_H
