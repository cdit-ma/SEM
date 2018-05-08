#ifndef OUTEVENTPORTIMPL_H
#define OUTEVENTPORTIMPL_H
#include "../node.h"

class EntityFactory;
class OutEventPortImpl : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	OutEventPortImpl(EntityFactory& factory, bool is_temp_node);

    QSet<Node*> getParentNodesForValidDefinition();
public:
    bool canAdoptChild(Node* child);
private:
    Node* getTopBehaviourContainer();
    bool top_behaviour_calculated = false;
    Node* top_behaviour_container = 0;
};

#endif // OUTEVENTPORTIMPL_H
