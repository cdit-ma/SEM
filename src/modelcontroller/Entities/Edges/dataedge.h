#ifndef DATAEDGE_H
#define DATAEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class DataEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    DataEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};

#endif // DATAEDGE_H
