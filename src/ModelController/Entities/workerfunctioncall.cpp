#include "WorkerFunctionCall.h"
#include <QDebug>

WorkerFunctionCall::WorkerFunctionCall():Node(NODE_KIND::WORKER_FUNCTIONCALL)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setDefinitionKind(NODE_KIND::WORKER_FUNCTION);
}

WorkerFunctionCall::WorkerFunctionCall(EntityFactory* factory) : Node(factory, NODE_KIND::WORKER_FUNCTIONCALL, "WorkerFunctionCall"){
	auto node_kind = NODE_KIND::WORKER_FUNCTIONCALL;
	QString kind_string = "WorkerFunctionCall";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerFunctionCall();});

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


bool WorkerFunctionCall::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            // Definition edge must link to a WorkerFunction
            if(dst->getNodeKind() != NODE_KIND::WORKER_FUNCTION){
                return false;
            }
            // The WorkerFunctionCall must exist within a ComponentImpl
            auto parent_node = dst->getParentNode();
            if(parent_node){
                if(parent_node->getNodeKind() != NODE_KIND::WORKER_INSTANCE){
                    return false;
                }
            }
            
            break;
        }
        default:
            break;
    }
    return Node::canAcceptEdge(edgeKind, dst);
}
