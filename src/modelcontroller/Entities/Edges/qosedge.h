#ifndef QOSEDGE_H
#define QOSEDGE_H
#include "../edge.h"


class EntityFactoryRegistryBroker;
class QosEdge : public Edge
{
    Q_OBJECT


protected:
public:
    static void RegisterWithEntityFactory(EntityFactoryRegistryBroker& broker);
protected:
    QosEdge(EntityFactoryBroker& factory, Node *src, Node *dst);
};
#endif // QosEDGE_H