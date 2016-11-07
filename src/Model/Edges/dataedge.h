#ifndef DATAEDGE_H
#define DATAEDGE_H
#include "../edge.h"

class DataEdge : public Edge
{
    Q_OBJECT
protected:
    DataEdge(Node *src, Node *dst);
public:
    static DataEdge* createDataEdge(Node* src, Node* dst);
};

#endif // DATAEDGE_H
