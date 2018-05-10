#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class OutEventPortImpl : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	OutEventPortImpl(EntityFactoryBroker& factory, bool is_temp_node);

    QSet<Node*> getParentNodesForValidDefinition();
public:
    bool canAdoptChild(Node* child);
private:
    Node* getTopBehaviourContainer();
    bool top_behaviour_calculated = false;
    Node* top_behaviour_container = 0;
};

#endif // OUTEVENTPORTIMPL_H
