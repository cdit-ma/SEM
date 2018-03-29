#include "condition.h"
#include "termination.h"
#include "branch.h"


Condition::Condition(EntityFactory* factory, NODE_KIND kind, QString kind_str) : BehaviourNode(factory, kind, kind_str){
};

Condition::Condition(EntityFactory* factory) : BehaviourNode(factory, NODE_KIND::CONDITION, "Condition"){
	auto node_kind = NODE_KIND::CONDITION;
	QString kind_string = "Condition";
	RegisterNodeKind(factory, node_kind, kind_string, [](){return new Condition();});


    //Register DefaultData
    RegisterDefaultData(factory, node_kind, "value", QVariant::String);
};

Condition::Condition(NODE_KIND kind):BehaviourNode(kind){
    setNodeType(NODE_TYPE::CONDITION);
    //setAcceptsEdgeKind(EDGE_KIND::DATA);
    setWorkflowProducer(true);
    setWorkflowReciever(false);
}

Branch *Condition::getBranch()
{
    BehaviourNode* parent = getParentBehaviourNode();

    if(parent && parent->isNodeOfType(NODE_TYPE::BRANCH)){
        return (Branch*) parent;
    }
    return 0;
}

Termination *Condition::getRequiredTermination()
{
    Branch* branch = getBranch();
    if(branch){
        return branch->getTermination();
    }
    return 0;
}

bool Condition::gotTermination()
{
    Termination* t = getRequiredTermination();

    if(t && t->isNodeInBehaviourChain(this)){
        return true;
    }
    return false;
}

bool Condition::canAdoptChild(Node* child)
{
    if(child->isNodeOfType(NODE_TYPE::BEHAVIOUR)){
        BehaviourNode* behaviour_node = (BehaviourNode*)child;

        if(behaviour_node->isTopLevel()){
            return false;
        }
    }else{
        return false;
    }
    return BehaviourNode::canAdoptChild(child);
}

bool Condition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}



