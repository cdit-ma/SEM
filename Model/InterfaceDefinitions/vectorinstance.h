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

    bool canAdoptChild(Node* child);
    bool canConnect_DefinitionEdge(Node *definition);
    bool canConnect_DataEdge(Node *node);
};


#endif // AGGREGATE_H

