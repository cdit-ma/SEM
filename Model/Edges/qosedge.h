#ifndef QOSEDGE_H
#define QOSEDGE_H
#include "../edge.h"

class QOSEdge : public Edge
{
public:
    QOSEdge(Node *src, Node *dst);
    ~QOSEdge();
    QString toString();
};

#endif // QOSEDGE_H
