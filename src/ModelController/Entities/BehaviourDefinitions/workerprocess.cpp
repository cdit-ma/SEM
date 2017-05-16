#include "workerprocess.h"



WorkerProcess::WorkerProcess():Process(NODE_KIND::WORKER_PROCESS)
{
    
}

WorkerProcess::WorkerProcess(EntityFactory* factory) : Process(factory, NODE_KIND::WORKER_PROCESS, "WorkerProcess"){
	auto node_kind = NODE_KIND::WORKER_PROCESS;
	QString kind_string = "WorkerProcess";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerProcess();});

    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "file", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "folder", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "icon_prefix", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "operation", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "worker", QVariant::String, true);
    RegisterDefaultData(factory, node_kind, "workerID", QVariant::String, false);
    RegisterDefaultData(factory, node_kind, "description", QVariant::String, true);
};

