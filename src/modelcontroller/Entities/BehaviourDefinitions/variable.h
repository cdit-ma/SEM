#ifndef VARIABLE_H
#define VARIABLE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class Variable: public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Variable(EntityFactory* factory);
	Variable();
public:
    bool canAdoptChild(Node* child);
protected:
    void childAdded(Node* child);
    void childRemoved(Node* child);
};

#endif // VARIABLE_H
