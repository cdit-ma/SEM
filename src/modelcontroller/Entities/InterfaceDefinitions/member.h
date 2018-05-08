#ifndef MEMBER_H
#define MEMBER_H
#include "datanode.h"

class EntityFactory;
class Member : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    Member(EntityFactory& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // MEMBER_H
