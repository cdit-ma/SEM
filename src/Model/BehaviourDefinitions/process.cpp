#include "process.h"
#include "parameter.h"

#include "../entityfactory.h"
Process::Process(EntityFactory* factory, NODE_KIND kind, QString kind_str) : BehaviourNode(factory, kind, kind_str){
};

Process::Process(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::PROCESS, "Process"){
	auto node_kind = NODE_KIND::PROCESS;
	QString kind_string = "Process";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Process();});
};

Process::Process(NODE_KIND kind):BehaviourNode(kind){
    setWorkflowReciever(true);
    setWorkflowProducer(true);
}

bool Process::canAdoptChild(Node* node)
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

    return BehaviourNode::canAdoptChild(node);
}

bool Process::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
    //Q_UNUSED(edgeKind);
    //Q_UNUSED(dst);
    //return false;
}
