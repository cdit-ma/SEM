#include "variable.h"
#include <QDebug>
#include "../InterfaceDefinitions/aggregateinstance.h"
#include "../InterfaceDefinitions/member.h"
#include "../InterfaceDefinitions/aggregate.h"
#include "../InterfaceDefinitions/vectorinstance.h"
#include "inputparameter.h"

Variable::Variable():BehaviourNode()
{

}

Variable::~Variable()
{
}

Edge::EDGE_CLASS Variable::canConnect(Node* node)
{
    InputParameter* inputParameter = dynamic_cast<InputParameter*>(node);

    if(!inputParameter){
        return false;
    }

    if(!inputParameter->canConnect(this)){
        return false;
    }

    return Node::canConnect(node);
}

bool Variable::canAdoptChild(Node* child)
{
    AggregateInstance* aggregateInstance = dynamic_cast<AggregateInstance*>(child);
    VectorInstance* vectorInstance = dynamic_cast<VectorInstance*>(child);

    if(!aggregateInstance && !vectorInstance){
#ifdef DEBUG_MODE
        qWarning() << "Aggregate can only adopt Member/Instances";
#endif
        return false;
    }

    //Check for loops

    return Node::canAdoptChild(child);
}
