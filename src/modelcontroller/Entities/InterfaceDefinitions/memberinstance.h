#ifndef MEMBERINSTANCE_H
#define MEMBERINSTANCE_H
#include "datanode.h"

class EntityFactory;
class MemberInstance : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	MemberInstance(EntityFactory* factory);
	MemberInstance();
    void parentSet(Node* parent);
public:
    bool canAdoptChild(Node* child);
};

#endif // MEMBERINSTANCE_H
