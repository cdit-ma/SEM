#ifndef QOSEDGE_H
#define QOSEDGE_H
#include "../edge.h"

class EntityFactory;
class QosEdge : public Edge
{
    Q_OBJECT
    friend class EntityFactory;
protected:
    QosEdge(EntityFactory* factory);
    QosEdge(Node *src, Node *dst);
private:
    static QosEdge* ConstructEdge(Node* src, Node* dst);
};
#endif // QosEDGE_H