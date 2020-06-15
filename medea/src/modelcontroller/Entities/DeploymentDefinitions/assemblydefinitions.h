#ifndef ASSEMBLYDEFINITIONC_H
#define ASSEMBLYDEFINITIONC_H
#include "../node.h"

#include "../../edgekinds.h"

class EntityFactoryRegistryBroker;
class AssemblyDefinitions: public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    AssemblyDefinitions(EntityFactoryBroker& factory, bool is_temp_node);
	void updateViewAspect(VIEW_ASPECT){};
public:
    VIEW_ASPECT getViewAspect() const;
};
#endif // AssemblyDefinitions_H
