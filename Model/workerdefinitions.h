#ifndef WORKERDEFINITIONS_H
#define WORKERDEFINITIONS_H
#include "node.h"


class WorkerDefinitions : public Node
{
public:
    WorkerDefinitions();
public:
    bool canAdoptChild(Node *node);
    Edge::EDGE_CLASS canConnect(Node *node);
};

#endif // WORKERDEFINITIONS_H
