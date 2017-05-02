#include "condition.h"
#include "termination.h"
#include "branch.h"

//#include "../entityfactory.h"
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
    setWorkflowProducer(true);
    setWorkflowReciever(false);


    if(kind == NODE_KIND::CONDITION){
        //updateDefaultData("value", QVariant::String);
    }
    //setMoveEnabled(false);
    //setExpandEnabled(false);
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

bool Condition::canAdoptChild(Node*)
{
    return false;
}

bool Condition::canAcceptEdge(EDGE_KIND edgeKind, Node *dst)
{
    return BehaviourNode::canAcceptEdge(edgeKind, dst);
}



