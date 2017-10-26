#ifndef ENUM_INSTANCE_H
#define ENUM_INSTANCE_H
#include "datanode.h"

class EntityFactory;
class EnumInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	EnumInstance(EntityFactory* factory);
	EnumInstance();
public:
    bool canAdoptChild(Node* child);
    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //ENUM_INSTANCE_H
