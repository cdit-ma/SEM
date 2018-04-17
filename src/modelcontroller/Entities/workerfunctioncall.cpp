#include "WorkerFunctionCall.h"
#include <QDebug>
#include "BehaviourDefinitions/parameter.h"

WorkerFunctionCall::WorkerFunctionCall():Node(NODE_KIND::WORKER_FUNCTIONCALL)
{
    setAcceptsEdgeKind(EDGE_KIND::DEFINITION);
    setNodeType(NODE_TYPE::INSTANCE);
    setDefinitionKind(NODE_KIND::WORKER_FUNCTION);

    //Disable rules which break
    SetEdgeRuleActive(EdgeRule::REQUIRE_NO_DEFINITION, false);
    SetEdgeRuleActive(EdgeRule::MIRROR_PARENT_DEFINITION_HIERARCHY, false);
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

bool WorkerFunctionCall::canAdoptChild(Node* node)
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


QList<Node*> WorkerFunctionCall::getAdoptableNodes(Node* definition){
    //The only things we need to adopt in a WorkerFunctionCall is all of the Parameters from the top level definition of the WorkerFunction
    QList<Node*> adoptable_nodes;
    if(definition){
        auto top_definition = definition->getDefinition(true);
        if(top_definition){
            for(auto child : top_definition->getChildren(0)){
                if(child->isNodeOfType(NODE_TYPE::PARAMETER)){
                    adoptable_nodes << child;
                }
            }
        }
    }
    return adoptable_nodes;
}