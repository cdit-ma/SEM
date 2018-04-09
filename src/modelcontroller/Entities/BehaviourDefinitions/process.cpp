#include "process.h"
#include "parameter.h"
#include <QDebug>


const NODE_KIND node_kind = NODE_KIND::PROCESS;
const QString kind_string = "Process";

Process::Process(EntityFactory* factory, NODE_KIND kind, QString kind_str) : Node(factory, kind, kind_str){
};

Process::Process(EntityFactory* factory) : Node(factory, node_kind, kind_string){
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Process();});
};

Process::Process(NODE_KIND kind):Node(kind){
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
    return Node::canAdoptChild(node);
}

bool Process::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return Node::canAcceptEdge(edgeKind, dst);
}
