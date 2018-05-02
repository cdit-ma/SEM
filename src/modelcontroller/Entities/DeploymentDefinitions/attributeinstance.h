#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class AttributeInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	AttributeInstance(EntityFactory* factory);
	AttributeInstance();
    void parentSet(Node* parent);
};

#endif // ATTRIBUTEINSTANCE_H
