#ifndef MEMBERINST_H
#define MEMBERINST_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class MemberInst : public DataNode
{
    Q_OBJECT
    public:
       static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        MemberInst(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
};

#endif // MEMBERINST_H
