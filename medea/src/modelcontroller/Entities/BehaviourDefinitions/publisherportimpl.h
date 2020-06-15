#ifndef PORT_PUBLISHER_PORT_IMPL_H
#define PORT_PUBLISHER_PORT_IMPL_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class PublisherPortImpl : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	PublisherPortImpl(EntityFactoryBroker& factory, bool is_temp_node);

    QSet<Node*> getParentNodesForValidDefinition();
public:
    bool canAdoptChild(Node* child);
private:
    Node* getTopBehaviourContainer();
    bool top_behaviour_calculated = false;
    Node* top_behaviour_container = 0;
};

#endif // PORT_PUBLISHER_PORT_IMPL_H
