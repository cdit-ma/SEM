#include "workerprocess.h"
#include <QDebug>

WorkerProcess::WorkerProcess():Process(NODE_KIND::WORKER_PROCESS)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
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
}


bool WorkerProcess::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(dst->getNodeKind() != NODE_KIND::WORKER_PROCESS){
                return false;
            }
            if(dst->isInModel()){
                return false;
            }
            if(!isInModel()){
                return false;
            }
            return true;
            break;
        }
        default:
            break;
    }

    return Process::canAcceptEdge(edgeKind, dst);
}


