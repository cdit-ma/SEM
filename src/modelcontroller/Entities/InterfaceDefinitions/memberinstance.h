#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class MemberInstance : public DataNode
{

    Q_OBJECT

protected:
public:
	static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    MemberInstance(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // MEMBERINSTANCE_H
