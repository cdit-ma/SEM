#ifndef VECTORINSTANCE_H
#define VECTORINSTANCE_H
#include "../node.h"
#include "eventport.h"

class VectorInstance : public Node
{
    Q_OBJECT
public:
    VectorInstance();
    ~VectorInstance();

    // GraphML interface
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};


#endif // AGGREGATE_H

