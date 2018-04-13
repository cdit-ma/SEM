#include "workerfunction.h"
#include <QDebug>
#include "BehaviourDefinitions/parameter.h"

WorkerFunction::WorkerFunction():Node(NODE_KIND::WORKER_FUNCTION)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setNodeType(NODE_TYPE::DEFINITION);

    setInstanceKind(NODE_KIND::WORKER_FUNCTION);
    setImplKind(NODE_KIND::WORKER_FUNCTION);
    setDefinitionKind(NODE_KIND::WORKER_FUNCTION);
}

WorkerFunction::WorkerFunction(EntityFactory* factory) : Node(factory, NODE_KIND::WORKER_FUNCTION, "WorkerFunction"){
	auto node_kind = NODE_KIND::WORKER_FUNCTION;
	QString kind_string = "WorkerFunction";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new WorkerFunction();});

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


bool WorkerFunction::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
        case EDGE_KIND::DEFINITION:{
            if(dst->getNodeKind() != NODE_KIND::WORKER_FUNCTION){
                return false;
            }

            
            if(getParentNode()->getNodeKind() == NODE_KIND::COMPONENT_IMPL){

                if(!dst->getDefinition()){
                    return false;
                }
            }
            
            //return true;
            break;
        }
        default:
            break;
    }

    return Node::canAcceptEdge(edgeKind, dst);
}

bool WorkerFunction::canAdoptChild(Node* node)
{
    if(!node->isNodeOfType(NODE_TYPE::PARAMETER)){
        return false;
    }

    Parameter* parameter = (Parameter*)node;

    if(parameter->isReturnParameter()){
        if(!getChildrenOfKind(NODE_KIND::RETURN_PARAMETER, 0).isEmpty()){
            return false;
        }
    }

    if(parameter->isVariadicParameter()){
        //Check to see if worker function is variadic
        auto d = gotData("is_variadic");
        if(!d){
            return false;
        }
    }
    
    return Node::canAdoptChild(node);
}