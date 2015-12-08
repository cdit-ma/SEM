#ifndef VECTOR_H
#define VECTOR_H
#include "../node.h"

class Vector : public Node
{
    Q_OBJECT
public:
    Vector();
    ~Vector();

    bool canAdoptChild(Node* child);
    bool canConnect_AggregateEdge(Node *aggregate);
};


#endif // AGGREGATE_H

