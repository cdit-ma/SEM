#ifndef VECTOR_H
#define VECTOR_H
#include "datanode.h"

class Vector : public DataNode
{
    Q_OBJECT
public:
    Vector();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
private:
    QString getVectorType();
private slots:
    void updateType();
    void childrenChanged();

};


#endif // AGGREGATE_H

