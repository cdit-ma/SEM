#ifndef VARIABLE_H
#define VARIABLE_H
#include "../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
class Variable: public DataNode
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
	Variable(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);

protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif // VARIABLE_H
