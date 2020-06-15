#ifndef ENUMMEMBER_H
#define ENUMMEMBER_H
#include "datanode.h"

class EntityFactoryRegistryBroker;
class EnumMember : public DataNode
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    EnumMember(EntityFactoryBroker& factory, bool is_temp_node);
};

#endif //ENUMMEMBER_H
