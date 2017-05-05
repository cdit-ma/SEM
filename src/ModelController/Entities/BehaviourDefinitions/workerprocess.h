#ifndef WORKERPROCESS_H
#define WORKERPROCESS_H

#include "process.h"

class EntityFactory;
class WorkerProcess : public Process
{
public:
    WorkerProcess();
    WorkerProcess(EntityFactory* factory);
};

#endif // WORKERPROCESS_H
