#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "datanode.h"

class EntityFactory;
class MemberInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	static void RegisterWithEntityFactory(EntityFactory& factory);
    MemberInstance(EntityFactory& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // MEMBERINSTANCE_H
