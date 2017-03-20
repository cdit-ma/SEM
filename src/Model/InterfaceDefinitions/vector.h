#ifndef VECTOR_H
#define VECTOR_H
#include "../node.h"

class Vector : public Node
{
    Q_OBJECT
public:
    Vector();
    QString getVectorType();

    bool canAdoptChild(Node* child);
    bool canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst);
private slots:
    void updateType();
    void childrenChanged();

};


#endif // AGGREGATE_H

