#ifndef ENUMMEMBER_H
#define ENUMMEMBER_H
#include "datanode.h"
class EntityFactory;
class EnumMember : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
	EnumMember(EntityFactory* factory);
	EnumMember();
};

#endif //ENUMMEMBER_H
