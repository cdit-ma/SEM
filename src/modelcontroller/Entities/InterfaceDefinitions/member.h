#ifndef MEMBER_H
#define MEMBER_H
#include "datanode.h"

class EntityFactory;
class Member : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	Member(EntityFactory* factory);
	Member();
    void parentSet(Node* parent);
};

#endif // MEMBER_H
