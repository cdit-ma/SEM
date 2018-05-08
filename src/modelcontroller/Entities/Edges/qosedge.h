#ifndef QOSEDGE_H
#define QOSEDGE_H
#include "../edge.h"

class EntityFactory;
class QosEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    static void RegisterWithEntityFactory(EntityFactory& factory);
    QosEdge(EntityFactory& factory, Node *src, Node *dst);
};
#endif // QosEDGE_H