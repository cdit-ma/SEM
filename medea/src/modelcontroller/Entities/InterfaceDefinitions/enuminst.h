#ifndef ENUM_INST_H
#define ENUM_INST_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class EnumInst : public DataNode
{
    Q_OBJECT
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        EnumInst(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif //ENUM_INST_H
