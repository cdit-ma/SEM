#ifndef ENUMMEMBER_H
#define ENUMMEMBER_H
#include "datanode.h"
class EntityFactory;
class EnumMember : public DataNode
{
	friend class EntityFactory;
    Q_OBJECT
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    EnumMember(EntityFactory& factory, bool is_temp_node);
};

#endif //ENUMMEMBER_H
