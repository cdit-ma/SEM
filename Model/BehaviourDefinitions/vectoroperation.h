#ifndef VECTOROPERATION_H
#define VECTOROPERATION_H
#include "behaviournode.h"

class VectorOperation: public BehaviourNode
{
    Q_OBJECT
public:
    VectorOperation();
    ~VectorOperation();

public:
    bool canConnect(Node* attachableObject);
    bool canAdoptChild(Node* child);
};

#endif // VECTOROPERATION_H
