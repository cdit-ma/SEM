#ifndef QOSEDGE_H
#define QOSEDGE_H
#include "../edge.h"

class QOSEdge : public Edge
{
    Q_OBJECT
protected:
    QOSEdge(Node *src, Node *dst);
public:
    static QOSEdge* createQOSEdge(Node* src, Node* dst);
};

#endif // QOSEDGE_H
