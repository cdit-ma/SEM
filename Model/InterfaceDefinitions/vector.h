#ifndef VECTOR_H
#define VECTOR_H
#include "../node.h"
#include "eventport.h"

class Vector : public Node
{
    Q_OBJECT
public:
    Vector();
    ~Vector();

    // GraphML interface
    Edge::EDGE_CLASS canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};


#endif // AGGREGATE_H

