#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "../node.h"

class EntityFactory;
class Setter : public Node
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	Setter(EntityFactory& factory, bool is_temp_node);
    void childAdded(Node* child);
public:
    bool canAdoptChild(Node* child);
};

#endif //SETFUNCTION_H
