#ifndef ENUM_INSTANCE_H
#define ENUM_INSTANCE_H
#include "datanode.h"

class EntityFactory;
class EnumInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    EnumInstance(EntityFactory& factory, bool is_temp_node);
public:
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //ENUM_INSTANCE_H
