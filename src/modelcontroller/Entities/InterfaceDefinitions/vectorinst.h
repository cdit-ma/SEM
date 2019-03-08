#ifndef VECTORINST_H
#define VECTORINST_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class VectorInst : public DataNode
{
    Q_OBJECT
    public:
        static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
    protected:
        VectorInst(EntityFactoryBroker& factory, bool is_temp_node);
        void parentSet(Node* parent);
    public:
        bool canAdoptChild(Node* child);
};


#endif // VECTORINST_H

