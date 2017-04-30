#include "workerprocess.h"

WorkerProcess::WorkerProcess():Process(NODE_KIND::WORKER_PROCESS)
{
    updateDefaultData("file", QVariant::String, true);
    updateDefaultData("folder", QVariant::String, true);
    updateDefaultData("icon", QVariant::String, true);
    updateDefaultData("icon_prefix", QVariant::String, true);
    updateDefaultData("operation", QVariant::String, true);
    updateDefaultData("worker", QVariant::String, true);
    updateDefaultData("workerID", QVariant::String, false);
    updateDefaultData("description", QVariant::String, true);
}

