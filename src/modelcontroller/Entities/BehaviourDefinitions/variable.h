#ifndef VARIABLE_H
#define VARIABLE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class Variable: public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
	Variable(EntityFactory& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif // VARIABLE_H
