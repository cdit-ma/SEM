#ifndef WORKERDEFINITIONS_H
#define WORKERDEFINITIONS_H
#include "node.h"

class WorkerDefinitions : public Node
{
    Q_OBJECT
public:
    WorkerDefinitions();
public:
    bool canAdoptChild(Node *node);
};

#endif // WORKERDEFINITIONS_H
