#ifndef DATAEDGE_H
#define DATAEDGE_H
#include "../edge.h"

class DataEdge : public Edge
{
    Q_OBJECT
public:
    DataEdge(Node *src, Node *dst);
    ~DataEdge();
    QString toString();
};

#endif // DATAEDGE_H
