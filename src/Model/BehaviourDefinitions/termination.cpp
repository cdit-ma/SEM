#include "termination.h"
#include "branch.h"
#include <QDebug>
#include "../entityfactory.h"

Termination::Termination(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::TERMINATION, "Termination"){
	auto node_kind = NODE_KIND::TERMINATION;
	QString kind_string = "Termination";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Termination();});
};

Termination::Termination():BehaviourNode(NODE_KIND::TERMINATION){
    setWorkflowProducer(true);
    setWorkflowReciever(true);
}

Branch *Termination::getBranch()
{
    foreach(Edge* edge, getEdges(0, Edge::EC_WORKFLOW)){
        BehaviourNode* source = (BehaviourNode*) edge->getSource();
        if(source->isNodeOfType(NODE_TYPE::BRANCH)){
            Branch* branch = (Branch*) source;
            if(branch->getTermination() == this){
                return branch;
            }
        }
    }
    return 0;
}

bool Termination::canAdoptChild(Node*)
{
    return false;
}

bool Termination::canAcceptEdge(Edge::EDGE_KIND edgeKind, Node *dst)
{
    if(!acceptsEdgeKind(edgeKind)){
        return false;
    }

    switch(edgeKind){
    case Edge::EC_WORKFLOW:{
        if(!getBranch()){
            return false;
        }
        break;
    }
    default:
        break;
    }
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}

