#ifndef ATTRIBUTE_INST_H
#define ATTRIBUTE_INST_H
#include "../InterfaceDefinitions/datanode.h"


class EntityFactoryRegistryBroker;
class AttributeInst : public DataNode
{
    Q_OBJECT
    public:
	    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        AttributeInst(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
};

#endif // ATTRIBUTE_INST_H
