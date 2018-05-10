#ifndef SETFUNCTION_H
#define SETFUNCTION_H
#include "../node.h"


class EntityFactoryRegistryBroker;
class Setter : public Node
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	Setter(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
private:
	void updateLabel();
	Node* lhs_ = 0;
	Node* comparator_ = 0;
	Node* rhs_ = 0;

};

#endif //SETFUNCTION_H
