#ifndef VECTORINSTANCE_H
#define VECTORINSTANCE_H
#include "datanode.h"


class EntityFactoryRegistryBroker;
class VectorInstance : public DataNode
{

    Q_OBJECT

protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    VectorInstance(EntityFactoryBroker& factory, bool is_temp_node);
public:
    bool canAdoptChild(Node* child);
};


#endif // AGGREGATE_H

