#ifndef MEMBER_H
#define MEMBER_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class Member : public DataNode
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    Member(EntityFactoryBroker& factory, bool is_temp_node);
    void parentSet(Node* parent);
};

#endif // MEMBER_H
