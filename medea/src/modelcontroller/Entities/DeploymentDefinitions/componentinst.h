#ifndef COMPONENTINST_H
#define COMPONENTINST_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class ComponentInst : public Node
{
    Q_OBJECT
    public:
	    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        ComponentInst(EntityFactoryBroker& factory, bool is_temp_node);
        QSet<Node*> getListOfValidAncestorsForChildrenDefinitions();
};

#endif // COMPONENTINST_H
