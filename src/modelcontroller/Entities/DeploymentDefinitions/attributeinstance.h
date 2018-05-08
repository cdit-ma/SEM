#ifndef ATTRIBUTEINSTANCE_H
#define ATTRIBUTEINSTANCE_H
#include "../InterfaceDefinitions/datanode.h"

class EntityFactory;
class AttributeInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    AttributeInstance(EntityFactory& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // ATTRIBUTEINSTANCE_H
