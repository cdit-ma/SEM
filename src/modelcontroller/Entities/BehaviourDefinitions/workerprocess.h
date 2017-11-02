#ifndef WORKERPROCESS_H
#define WORKERPROCESS_H

#include "process.h"

class EntityFactory;
class WorkerProcess : public Process
{
public:
    WorkerProcess();
    WorkerProcess(EntityFactory* factory);

    bool canAcceptEdge(EDGE_KIND edgeKind, Node *dst);
};

#endif // WORKERPROCESS_H
